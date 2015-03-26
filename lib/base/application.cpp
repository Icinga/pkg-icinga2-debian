/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2015 Icinga Development Team (http://www.icinga.org)    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "base/application.hpp"
#include "base/stacktrace.hpp"
#include "base/timer.hpp"
#include "base/logger.hpp"
#include "base/exception.hpp"
#include "base/objectlock.hpp"
#include "base/utility.hpp"
#include "base/debug.hpp"
#include "base/type.hpp"
#include "base/convert.hpp"
#include "base/scriptglobal.hpp"
#include "base/process.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#ifdef __linux__
#include <sys/prctl.h>
#endif /* __linux__ */

using namespace icinga;

REGISTER_TYPE(Application);

boost::signals2::signal<void (void)> Application::OnReopenLogs;
Application::Ptr Application::m_Instance = NULL;
bool Application::m_ShuttingDown = false;
bool Application::m_RequestRestart = false;
bool Application::m_RequestReopenLogs = false;
pid_t Application::m_ReloadProcess = 0;
static bool l_Restarting = false;
static bool l_InExceptionHandler = false;
int Application::m_ArgC;
char **Application::m_ArgV;
double Application::m_StartTime;

/**
 * Constructor for the Application class.
 */
void Application::OnConfigLoaded(void)
{
	m_PidFile = NULL;

	ASSERT(m_Instance == NULL);
	m_Instance = this;
}

/**
 * Destructor for the application class.
 */
void Application::Stop(void)
{
	m_ShuttingDown = true;

#ifdef _WIN32
	WSACleanup();
#endif /* _WIN32 */

	// Getting a shutdown-signal when a restart is in progress usually
	// means that the restart succeeded and the new process wants to take
	// over. Write the PID of the new process to the pidfile before this
	// process exits to keep systemd happy.
	if (l_Restarting) {
		try {
			UpdatePidFile(GetPidPath(), m_ReloadProcess);
		} catch (const std::exception&) {
			/* abort restart */
			Log(LogCritical, "Application", "Cannot update PID file. Aborting restart operation.");
			return;
		}
		ClosePidFile(false);
	} else
		ClosePidFile(true);

	DynamicObject::Stop();
}

Application::~Application(void)
{
	m_Instance = NULL;
}

void Application::Exit(int rc)
{
	std::cout.flush();
	std::cerr.flush();

	BOOST_FOREACH(const Logger::Ptr& logger, Logger::GetLoggers()) {
		logger->Flush();
	}

	UninitializeBase();
#ifdef I2_DEBUG
	exit(rc);
#else /* I2_DEBUG */
	_exit(rc); // Yay, our static destructors are pretty much beyond repair at this point.
#endif /* I2_DEBUG */
}

void Application::InitializeBase(void)
{
#ifdef _WIN32
	/* disable GUI-based error messages for LoadLibrary() */
	SetErrorMode(SEM_FAILCRITICALERRORS);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		BOOST_THROW_EXCEPTION(win32_error()
			<< boost::errinfo_api_function("WSAStartup")
			<< errinfo_win32_error(WSAGetLastError()));
	}
#endif /* _WIN32 */

	Utility::ExecuteDeferredInitializers();

	/* make sure the thread pool gets initialized */
	GetTP();

	Timer::Initialize();
}

void Application::UninitializeBase(void)
{
	Timer::Uninitialize();

	GetTP().Stop();
}

/**
 * Retrieves a pointer to the application singleton object.
 *
 * @returns The application object.
 */
Application::Ptr Application::GetInstance(void)
{
	return m_Instance;
}

void Application::SetResourceLimits(void)
{
#ifdef __linux__
	rlimit rl;

#	ifdef RLIMIT_NOFILE
	rl.rlim_cur = 16 * 1024;
	rl.rlim_max = rl.rlim_cur;

	if (setrlimit(RLIMIT_NOFILE, &rl) < 0)
		Log(LogNotice, "Application", "Could not adjust resource limit for open file handles (RLIMIT_NOFILE)");
#	else /* RLIMIT_NOFILE */
	Log(LogNotice, "Application", "System does not support adjusting the resource limit for open file handles (RLIMIT_NOFILE)");
#	endif /* RLIMIT_NOFILE */

#	ifdef RLIMIT_NPROC
	rl.rlim_cur = 16 * 1024;
	rl.rlim_max = rl.rlim_cur;

	if (setrlimit(RLIMIT_NPROC, &rl) < 0)
		Log(LogNotice, "Application", "Could not adjust resource limit for number of processes (RLIMIT_NPROC)");
#	else /* RLIMIT_NPROC */
	Log(LogNotice, "Application", "System does not support adjusting the resource limit for number of processes (RLIMIT_NPROC)");
#	endif /* RLIMIT_NPROC */

#	ifdef RLIMIT_STACK
	int argc = Application::GetArgC();
	char **argv = Application::GetArgV();
	bool set_stack_rlimit = true;

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--no-stack-rlimit") == 0) {
			set_stack_rlimit = false;
			break;
		}
	}

	if (getrlimit(RLIMIT_STACK, &rl) < 0) {
		Log(LogWarning, "Application", "Could not determine resource limit for stack size (RLIMIT_STACK)");
		rl.rlim_max = RLIM_INFINITY;
	}

	if (set_stack_rlimit)
		rl.rlim_cur = 256 * 1024;
	else
		rl.rlim_cur = rl.rlim_max;

	if (setrlimit(RLIMIT_STACK, &rl) < 0)
		Log(LogNotice, "Application", "Could not adjust resource limit for stack size (RLIMIT_STACK)");
	else if (set_stack_rlimit) {
		char **new_argv = static_cast<char **>(malloc(sizeof(char *) * (argc + 2)));

		if (!new_argv) {
			perror("malloc");
			Exit(EXIT_FAILURE);
		}

		new_argv[0] = argv[0];
		new_argv[1] = strdup("--no-stack-rlimit");

		if (!new_argv[1]) {
			perror("strdup");
			exit(1);
		}

		for (int i = 1; i < argc; i++)
			new_argv[i + 1] = argv[i];

		new_argv[argc + 1] = NULL;

		(void) execvp(new_argv[0], new_argv);
		perror("execvp");
		_exit(EXIT_FAILURE);
	}
#	else /* RLIMIT_STACK */
	Log(LogNotice, "Application", "System does not support adjusting the resource limit for stack size (RLIMIT_STACK)");
#	endif /* RLIMIT_STACK */
#endif /* __linux__ */
}

int Application::GetArgC(void)
{
	return m_ArgC;
}

void Application::SetArgC(int argc)
{
	m_ArgC = argc;
}

char **Application::GetArgV(void)
{
	return m_ArgV;
}

void Application::SetArgV(char **argv)
{
	m_ArgV = argv;
}

/**
 * Processes events for registered sockets and timers and calls whatever
 * handlers have been set up for these events.
 */
void Application::RunEventLoop(void)
{
	Timer::Initialize();

	double lastLoop = Utility::GetTime();

mainloop:
	while (!m_ShuttingDown && !m_RequestRestart) {
		/* Watches for changes to the system time. Adjusts timers if necessary. */
		Utility::Sleep(2.5);

		if (m_RequestReopenLogs) {
			Log(LogNotice, "Application", "Reopening log files");
			m_RequestReopenLogs = false;
			OnReopenLogs();
		}

		double now = Utility::GetTime();
		double timeDiff = lastLoop - now;

		if (std::fabs(timeDiff) > 15) {
			/* We made a significant jump in time. */
			Log(LogInformation, "Application")
			    << "We jumped "
			    << (timeDiff < 0 ? "forward" : "backward")
			    << " in time: " << std::fabs(timeDiff) << " seconds";

			Timer::AdjustTimers(-timeDiff);
		}

		lastLoop = now;
	}

	if (m_RequestRestart) {
		m_RequestRestart = false;         // we are now handling the request, once is enough

		// are we already restarting? ignore request if we already are
		if (l_Restarting)
			goto mainloop;

		l_Restarting = true;
		m_ReloadProcess = StartReloadProcess();

		goto mainloop;
	}

	Log(LogInformation, "Application", "Shutting down...");

	DynamicObject::StopObjects();
	Application::GetInstance()->OnShutdown();

	UninitializeBase();
}

void Application::OnShutdown(void)
{
	/* Nothing to do here. */
}

static void ReloadProcessCallback(const ProcessResult& pr)
{
	if (pr.ExitStatus != 0)
		Log(LogCritical, "Application", "Found error in config: reloading aborted");
	l_Restarting = false;
}

pid_t Application::StartReloadProcess(void)
{
	Log(LogInformation, "Application", "Got reload command: Starting new instance.");

	// prepare arguments
	Array::Ptr args = new Array();
	args->Add(GetExePath(m_ArgV[0]));

	for (int i=1; i < Application::GetArgC(); i++) {
		if (std::string(Application::GetArgV()[i]) != "--reload-internal")
			args->Add(Application::GetArgV()[i]);
		else
			i++;     // the next parameter after --reload-internal is the pid, remove that too
	}
	args->Add("--reload-internal");
	args->Add(Convert::ToString(Utility::GetPid()));

	Process::Ptr process = new Process(Process::PrepareCommand(args));
	process->SetTimeout(300);
	process->Run(&ReloadProcessCallback);

	return process->GetPID();
}

/**
 * Signals the application to shut down during the next
 * execution of the event loop.
 */
void Application::RequestShutdown(void)
{
	Log(LogInformation, "Application", "Received request to shut down.");

	m_ShuttingDown = true;
}

/**
 * Signals the application to restart during the next
 * execution of the event loop.
 */
void Application::RequestRestart(void)
{
	m_RequestRestart = true;
}

/**
 * Signals the application to reopen log files during the
 * next execution of the event loop.
 */
void Application::RequestReopenLogs(void)
{
	m_RequestReopenLogs = true;
}

/**
 * Retrieves the full path of the executable.
 *
 * @param argv0 The first command-line argument.
 * @returns The path.
 */
String Application::GetExePath(const String& argv0)
{
	String executablePath;

#ifndef _WIN32
	char buffer[MAXPATHLEN];
	if (getcwd(buffer, sizeof(buffer)) == NULL) {
		BOOST_THROW_EXCEPTION(posix_error()
		    << boost::errinfo_api_function("getcwd")
		    << boost::errinfo_errno(errno));
	}

	String workingDirectory = buffer;

	if (argv0[0] != '/')
		executablePath = workingDirectory + "/" + argv0;
	else
		executablePath = argv0;

	bool foundSlash = false;
	for (size_t i = 0; i < argv0.GetLength(); i++) {
		if (argv0[i] == '/') {
			foundSlash = true;
			break;
		}
	}

	if (!foundSlash) {
		const char *pathEnv = getenv("PATH");
		if (pathEnv != NULL) {
			std::vector<String> paths;
			boost::algorithm::split(paths, pathEnv, boost::is_any_of(":"));

			bool foundPath = false;
			BOOST_FOREACH(String& path, paths) {
				String pathTest = path + "/" + argv0;

				if (access(pathTest.CStr(), X_OK) == 0) {
					executablePath = pathTest;
					foundPath = true;
					break;
				}
			}

			if (!foundPath) {
				executablePath.Clear();
				BOOST_THROW_EXCEPTION(std::runtime_error("Could not determine executable path."));
			}
		}
	}

	if (realpath(executablePath.CStr(), buffer) == NULL) {
		BOOST_THROW_EXCEPTION(posix_error()
		    << boost::errinfo_api_function("realpath")
		    << boost::errinfo_errno(errno)
		    << boost::errinfo_file_name(executablePath));
	}

	return buffer;
#else /* _WIN32 */
	char FullExePath[MAXPATHLEN];

	if (!GetModuleFileName(NULL, FullExePath, sizeof(FullExePath)))
		BOOST_THROW_EXCEPTION(win32_error()
		    << boost::errinfo_api_function("GetModuleFileName")
		    << errinfo_win32_error(GetLastError()));

	return FullExePath;
#endif /* _WIN32 */
}

#ifndef _WIN32
static String UnameHelper(char type)
{
	/* Unfortunately the uname() system call doesn't support some of the
	 * query types we're interested in - so we're using popen() instead. */

	char cmd[] = "uname -X 2>&1";
	cmd[7] = type;

	FILE *fp = popen(cmd, "r");

	char line[1024];
	std::ostringstream msgbuf;

	while (fgets(line, sizeof(line), fp) != NULL)
		msgbuf << line;

	pclose(fp);

	String result = msgbuf.str();
	result.Trim();

	return result;
}

static String LsbReleaseHelper(void)
{
	FILE *fp = popen("lsb_release -s -d 2>&1", "r");

	char line[1024];
	std::ostringstream msgbuf;

	while (fgets(line, sizeof(line), fp) != NULL)
		msgbuf << line;

	pclose(fp);

	String result = msgbuf.str();
	result.Trim();

	return result;
}
#endif /* _WIN32 */

/**
 * Display version and path information.
 */
void Application::DisplayInfoMessage(std::ostream& os, bool skipVersion)
{
	os << "Application information:" << "\n";

	if (!skipVersion)
		os << "  Application version: " << GetVersion() << "\n";

	os << "  Installation root: " << GetPrefixDir() << "\n"
	   << "  Sysconf directory: " << GetSysconfDir() << "\n"
	   << "  Run directory: " << GetRunDir() << "\n"
	   << "  Local state directory: " << GetLocalStateDir() << "\n"
	   << "  Package data directory: " << GetPkgDataDir() << "\n"
	   << "  State path: " << GetStatePath() << "\n"
	   << "  Objects path: " << GetObjectsPath() << "\n"
	   << "  Vars path: " << GetVarsPath() << "\n"
	   << "  PID path: " << GetPidPath() << "\n"
	   << "  Application type: " << GetApplicationType() << "\n";

#ifndef _WIN32
	os << "\n"
	   << "System information:" << "\n"
	   << "  Operating system: " << UnameHelper('s') << "\n"
	   << "  Operating system version: " << UnameHelper('r') << "\n"
	   << "  Architecture: " << UnameHelper('m') << "\n";
#endif /* _WIN32 */

#ifdef __linux__
	os << "  Distribution: " << LsbReleaseHelper() << "\n";
#endif /* __linux__ */
}

/**
 * Displays a message that tells users what to do when they encounter a bug.
 */
void Application::DisplayBugMessage(std::ostream& os)
{
	os << "***" << "\n"
	   << "* This would indicate a runtime problem or configuration error. If you believe this is a bug in Icinga 2" << "\n"
	   << "* please submit a bug report at https://dev.icinga.org/ and include this stack trace as well as any other" << "\n"
	   << "* information that might be useful in order to reproduce this problem." << "\n"
	   << "***" << "\n";
}

String Application::GetCrashReportFilename(void)
{
	return GetLocalStateDir() + "/log/icinga2/crash/report." + Convert::ToString(Utility::GetTime());
}


#ifndef _WIN32
void Application::GetDebuggerBacktrace(const String& filename)
{
#ifdef __linux__
	prctl(PR_SET_DUMPABLE, 1);
#endif /* __linux __ */

	String my_pid = Convert::ToString(Utility::GetPid());

	pid_t pid = fork();

	if (pid < 0) {
		BOOST_THROW_EXCEPTION(posix_error()
		    << boost::errinfo_api_function("fork")
		    << boost::errinfo_errno(errno));
	}

	if (pid == 0) {
		int fd = open(filename.CStr(), O_CREAT | O_RDWR | O_APPEND, 0600);

		if (fd < 0) {
			BOOST_THROW_EXCEPTION(posix_error()
			    << boost::errinfo_api_function("open")
			    << boost::errinfo_errno(errno)
			    << boost::errinfo_file_name(filename));
		}

		if (fd != 1) {
			/* redirect stdout to the file */
			dup2(fd, 1);
			close(fd);
		}

		/* redirect stderr to stdout */
		if (fd != 2)
			close(2);

		dup2(1, 2);

		char *my_pid_str = strdup(my_pid.CStr());
		const char *argv[] = {
			"gdb",
			"--batch",
			"-p",
			my_pid_str,
			"-ex",
			"thread apply all bt full",
			"-ex",
			"detach",
			"-ex",
			"quit",
			NULL
		};
		(void)execvp(argv[0], const_cast<char **>(argv));
		perror("Failed to launch GDB");
		free(my_pid_str);
		_exit(0);
	}

	int status;
	if (waitpid(pid, &status, 0) < 0) {
		BOOST_THROW_EXCEPTION(posix_error()
		    << boost::errinfo_api_function("waitpid")
		    << boost::errinfo_errno(errno));
	}

#ifdef __linux__
	prctl(PR_SET_DUMPABLE, 0);
#endif /* __linux __ */
}

/**
 * Signal handler for SIGINT and SIGTERM. Prepares the application for cleanly
 * shutting down during the next execution of the event loop.
 *
 * @param - The signal number.
 */
void Application::SigIntTermHandler(int signum)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigaction(signum, &sa, NULL);

	Application::Ptr instance = Application::GetInstance();

	if (!instance)
		return;

	instance->RequestShutdown();
}

/**
 * Signal handler for SIGUSR1. This signal causes Icinga to re-open
 * its log files and is mainly for use by logrotate.
 *
 * @param - The signal number.
 */
void Application::SigUsr1Handler(int)
{
	RequestReopenLogs();
}

/**
 * Signal handler for SIGABRT. Helps with debugging ASSERT()s.
 *
 * @param - The signal number.
 */
void Application::SigAbrtHandler(int)
{
#ifndef _WIN32
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigaction(SIGABRT, &sa, NULL);
#endif /* _WIN32 */

	std::cerr << "Caught SIGABRT." << std::endl
		  << "Current time: " << Utility::FormatDateTime("%Y-%m-%d %H:%M:%S %z", Utility::GetTime()) << std::endl
		  << std::endl;

	String fname = GetCrashReportFilename();
	Utility::MkDir(Utility::DirName(fname), 0750);

	std::ofstream ofs;
	ofs.open(fname.CStr());

	Log(LogCritical, "Application")
	    << "Icinga 2 has terminated unexpectedly. Additional information can be found in '" << fname << "'" << "\n";

	DisplayInfoMessage(ofs);

	StackTrace trace;
	ofs << "Stacktrace:" << "\n";
	trace.Print(ofs, 1);

	DisplayBugMessage(ofs);

#ifndef _WIN32
	ofs << "\n";
	ofs.close();

	GetDebuggerBacktrace(fname);
#else /* _WIN32 */
	ofs.close();
#endif /* _WIN32 */
}
#else /* _WIN32 */
/**
 * Console control handler. Prepares the application for cleanly
 * shutting down during the next execution of the event loop.
 */
BOOL WINAPI Application::CtrlHandler(DWORD type)
{
	Application::Ptr instance = Application::GetInstance();

	if (!instance)
		return TRUE;

	instance->RequestShutdown();

	SetConsoleCtrlHandler(NULL, FALSE);
	return TRUE;
}
#endif /* _WIN32 */

/**
 * Handler for unhandled exceptions.
 */
void Application::ExceptionHandler(void)
{
	if (l_InExceptionHandler)
		for (;;)
			Utility::Sleep(5);

	l_InExceptionHandler = true;

#ifndef _WIN32
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigaction(SIGABRT, &sa, NULL);
#endif /* _WIN32 */

	String fname = GetCrashReportFilename();
	Utility::MkDir(Utility::DirName(fname), 0750);

	std::ofstream ofs;
	ofs.open(fname.CStr());

	ofs << "Caught unhandled exception." << "\n"
	    << "Current time: " << Utility::FormatDateTime("%Y-%m-%d %H:%M:%S %z", Utility::GetTime()) << "\n"
	    << "\n";

	DisplayInfoMessage(ofs);

	try {
		RethrowUncaughtException();
	} catch (const std::exception& ex) {
		Log(LogCritical, "Application")
		    << DiagnosticInformation(ex, false) << "\n"
		    << "\n"
		    << "Additional information is available in '" << fname << "'" << "\n";

		ofs << "\n"
		    << DiagnosticInformation(ex)
		    << "\n";
	}

	DisplayBugMessage(ofs);

	ofs.close();

#ifndef _WIN32
	GetDebuggerBacktrace(fname);
#endif /* _WIN32 */

	abort();
}

#ifdef _WIN32
LONG CALLBACK Application::SEHUnhandledExceptionFilter(PEXCEPTION_POINTERS exi)
{
	if (l_InExceptionHandler)
		return EXCEPTION_CONTINUE_SEARCH;

	l_InExceptionHandler = true;

	String fname = GetCrashReportFilename();
	Utility::MkDir(Utility::DirName(fname), 0750);

	std::ofstream ofs;
	ofs.open(fname.CStr());

	Log(LogCritical, "Application")
	    << "Icinga 2 has terminated unexpectedly. Additional information can be found in '" << fname << "'";

	DisplayInfoMessage(ofs);

	ofs << "Caught unhandled SEH exception." << "\n"
	    << "Current time: " << Utility::FormatDateTime("%Y-%m-%d %H:%M:%S %z", Utility::GetTime()) << "\n"
	    << "\n";

	StackTrace trace(exi);
	ofs << "Stacktrace:" << "\n";
	trace.Print(ofs, 1);

	DisplayBugMessage(ofs);

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif /* _WIN32 */

/**
 * Installs the exception handlers.
 */
void Application::InstallExceptionHandlers(void)
{
	std::set_terminate(&Application::ExceptionHandler);

#ifndef _WIN32
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &Application::SigAbrtHandler;
	sigaction(SIGABRT, &sa, NULL);
#else /* _WIN32 */
	SetUnhandledExceptionFilter(&Application::SEHUnhandledExceptionFilter);
#endif /* _WIN32 */
}

/**
 * Runs the application.
 *
 * @returns The application's exit code.
 */
int Application::Run(void)
{
#ifndef _WIN32
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &Application::SigIntTermHandler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	sa.sa_handler = &Application::SigUsr1Handler;
	sigaction(SIGUSR1, &sa, NULL);
#else /* _WIN32 */
	SetConsoleCtrlHandler(&Application::CtrlHandler, TRUE);
#endif /* _WIN32 */

	try {
		UpdatePidFile(GetPidPath());
	} catch (const std::exception&) {
		Log(LogCritical, "Application")
		    << "Cannot update PID file '" << GetPidPath() << "'. Aborting.";
		return EXIT_FAILURE;
	}

	return Main();
}

/**
 * Grabs the PID file lock and updates the PID. Terminates the application
 * if the PID file is already locked by another instance of the application.
 *
 * @param filename The name of the PID file.
 * @param pid The PID to write; default is the current PID
 */
void Application::UpdatePidFile(const String& filename, pid_t pid)
{
	ASSERT(!OwnsLock());
	ObjectLock olock(this);

	if (m_PidFile != NULL)
		fclose(m_PidFile);

	/* There's just no sane way of getting a file descriptor for a
	 * C++ ofstream which is why we're using FILEs here. */
	m_PidFile = fopen(filename.CStr(), "r+");

	if (m_PidFile == NULL)
		m_PidFile = fopen(filename.CStr(), "w");

	if (m_PidFile == NULL) {
		Log(LogCritical, "Application")
		    << "Could not open PID file '" << filename << "'.";
		BOOST_THROW_EXCEPTION(std::runtime_error("Could not open PID file '" + filename + "'"));
	}

#ifndef _WIN32
	int fd = fileno(m_PidFile);

	Utility::SetCloExec(fd);

	struct flock lock;

	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;

	if (fcntl(fd, F_SETLK, &lock) < 0) {
		Log(LogCritical, "Application", "Could not lock PID file. Make sure that only one instance of the application is running.");

		Application::Exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, 0) < 0) {
		Log(LogCritical, "Application")
		    << "ftruncate() failed with error code " << errno << ", \"" << Utility::FormatErrorNumber(errno) << "\"";

		BOOST_THROW_EXCEPTION(posix_error()
		    << boost::errinfo_api_function("ftruncate")
		    << boost::errinfo_errno(errno));
	}
#endif /* _WIN32 */

	fprintf(m_PidFile, "%lu\n", (unsigned long)pid);
	fflush(m_PidFile);
}

/**
 * Closes the PID file. Does nothing if the PID file is not currently open.
 */
void Application::ClosePidFile(bool unlink)
{
	ASSERT(!OwnsLock());
	ObjectLock olock(this);

	if (m_PidFile != NULL)
	{
		if (unlink) {
			String pidpath = GetPidPath();
			::unlink(pidpath.CStr());
		}

		fclose(m_PidFile);
	}

	m_PidFile = NULL;
}

/**
 * Checks if another process currently owns the pidfile and read it
 *
 * @param filename The name of the PID file.
 * @returns 0: no process owning the pidfile, pid of the process otherwise
 */
pid_t Application::ReadPidFile(const String& filename)
{
	FILE *pidfile = fopen(filename.CStr(), "r");

	if (pidfile == NULL)
		return 0;

#ifndef _WIN32
	int fd = fileno(pidfile);

	struct flock lock;

	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;

	if (fcntl(fd, F_GETLK, &lock) < 0) {
		int error = errno;
		fclose(pidfile);
		BOOST_THROW_EXCEPTION(posix_error()
		    << boost::errinfo_api_function("fcntl")
		    << boost::errinfo_errno(error));
	}

	if (lock.l_type == F_UNLCK) {
		// nobody has locked the file: no icinga running
		fclose(pidfile);
		return -1;
	}
#endif /* _WIN32 */

	pid_t runningpid;
	int res = fscanf(pidfile, "%d", &runningpid);
	fclose(pidfile);

	// bogus result?
	if (res != 1)
		return 0;

#ifdef _WIN32
	HANDLE hProcess = OpenProcess(0, FALSE, runningpid);

	if (!hProcess)
		return 0;

	CloseHandle(hProcess);
#endif /* _WIN32 */

	return runningpid;
}


/**
 * Retrieves the path of the installation prefix.
 *
 * @returns The path.
 */
String Application::GetPrefixDir(void)
{
	return ScriptGlobal::Get("PrefixDir");
}

/**
 * Sets the path for the installation prefix.
 *
 * @param path The new path.
 */
void Application::DeclarePrefixDir(const String& path)
{
	if (!ScriptGlobal::Exists("PrefixDir"))
		ScriptGlobal::Set("PrefixDir", path);
}

/**
 * Retrives the path of the sysconf dir.
 *
 * @returns The path.
 */
String Application::GetSysconfDir(void)
{
	return ScriptGlobal::Get("SysconfDir");
}

/**
 * Sets the path of the sysconf dir.
 *
 * @param path The new path.
 */
void Application::DeclareSysconfDir(const String& path)
{
	if (!ScriptGlobal::Exists("SysconfDir"))
		ScriptGlobal::Set("SysconfDir", path);
}

/**
 * Retrieves the path for the run dir.
 *
 * @returns The path.
 */
String Application::GetRunDir(void)
{
	return ScriptGlobal::Get("RunDir");
}

/**
 * Sets the path of the run dir.
 *
 * @param path The new path.
 */
void Application::DeclareRunDir(const String& path)
{
	if (!ScriptGlobal::Exists("RunDir"))
		ScriptGlobal::Set("RunDir", path);
}

/**
 * Retrieves the path for the local state dir.
 *
 * @returns The path.
 */
String Application::GetLocalStateDir(void)
{
	return ScriptGlobal::Get("LocalStateDir");
}

/**
 * Sets the path for the local state dir.
 *
 * @param path The new path.
 */
void Application::DeclareLocalStateDir(const String& path)
{
	if (!ScriptGlobal::Exists("LocalStateDir"))
		ScriptGlobal::Set("LocalStateDir", path);
}

/**
 * Retrieves the path for the local state dir.
 *
 * @returns The path.
 */
String Application::GetZonesDir(void)
{
	return ScriptGlobal::Get("ZonesDir", &Empty);
}

/**
 * Sets the path of the zones dir.
 *
 * @param path The new path.
 */
void Application::DeclareZonesDir(const String& path)
{
	if (!ScriptGlobal::Exists("ZonesDir"))
		ScriptGlobal::Set("ZonesDir", path);
}

/**
 * Retrieves the path for the package data dir.
 *
 * @returns The path.
 */
String Application::GetPkgDataDir(void)
{
	String defaultValue = "";
	return ScriptGlobal::Get("PkgDataDir", &Empty);
}

/**
 * Sets the path for the package data dir.
 *
 * @param path The new path.
 */
void Application::DeclarePkgDataDir(const String& path)
{
	if (!ScriptGlobal::Exists("PkgDataDir"))
		ScriptGlobal::Set("PkgDataDir", path);
}

/**
 * Retrieves the path for the include conf dir.
 *
 * @returns The path.
 */
String Application::GetIncludeConfDir(void)
{
	return ScriptGlobal::Get("IncludeConfDir", &Empty);
}

/**
 * Sets the path for the package data dir.
 *
 * @param path The new path.
 */
void Application::DeclareIncludeConfDir(const String& path)
{
	if (!ScriptGlobal::Exists("IncludeConfDir"))
		ScriptGlobal::Set("IncludeConfDir", path);
}

/**
 * Retrieves the path for the state file.
 *
 * @returns The path.
 */
String Application::GetStatePath(void)
{
	return ScriptGlobal::Get("StatePath", &Empty);
}

/**
 * Sets the path for the state file.
 *
 * @param path The new path.
 */
void Application::DeclareStatePath(const String& path)
{
	if (!ScriptGlobal::Exists("StatePath"))
		ScriptGlobal::Set("StatePath", path);
}

/**
 * Retrieves the path for the objects file.
 *
 * @returns The path.
 */
String Application::GetObjectsPath(void)
{
	return ScriptGlobal::Get("ObjectsPath", &Empty);
}

/**
 * Sets the path for the objects file.
 *
 * @param path The new path.
 */
void Application::DeclareObjectsPath(const String& path)
{
	if (!ScriptGlobal::Exists("ObjectsPath"))
		ScriptGlobal::Set("ObjectsPath", path);
}

/**
* Retrieves the path for the vars file.
*
* @returns The path.
*/
String Application::GetVarsPath(void)
{
	return ScriptGlobal::Get("VarsPath", &Empty);
}

/**
* Sets the path for the vars file.
*
* @param path The new path.
*/
void Application::DeclareVarsPath(const String& path)
{
	if (!ScriptGlobal::Exists("VarsPath"))
		ScriptGlobal::Set("VarsPath", path);
}

/**
 * Retrieves the path for the PID file.
 *
 * @returns The path.
 */
String Application::GetPidPath(void)
{
	return ScriptGlobal::Get("PidPath", &Empty);
}

/**
 * Sets the path for the PID file.
 *
 * @param path The new path.
 */
void Application::DeclarePidPath(const String& path)
{
	if (!ScriptGlobal::Exists("PidPath"))
		ScriptGlobal::Set("PidPath", path);
}

/**
 * Retrieves the name of the Application type.
 *
 * @returns The name.
 */
String Application::GetApplicationType(void)
{
	return ScriptGlobal::Get("ApplicationType");
}

/**
 * Sets the name of the Application type.
 *
 * @param path The new type name.
 */
void Application::DeclareApplicationType(const String& type)
{
	if (!ScriptGlobal::Exists("ApplicationType"))
		ScriptGlobal::Set("ApplicationType", type);
}

/**
 * Retrieves the name of the user.
 *
 * @returns The name.
 */
String Application::GetRunAsUser(void)
{
	return ScriptGlobal::Get("RunAsUser");
}

/**
 * Sets the name of the user.
 *
 * @param path The new user name.
 */
void Application::DeclareRunAsUser(const String& user)
{
	if (!ScriptGlobal::Exists("RunAsUser"))
		ScriptGlobal::Set("RunAsUser", user);
}

/**
 * Retrieves the name of the group.
 *
 * @returns The name.
 */
String Application::GetRunAsGroup(void)
{
	return ScriptGlobal::Get("RunAsGroup");
}

/**
 * Sets the concurrency level.
 *
 * @param path The new concurrency level.
 */
void Application::DeclareConcurrency(int ncpus)
{
	if (!ScriptGlobal::Exists("Concurrency"))
		ScriptGlobal::Set("Concurrency", ncpus);
}

/**
 * Retrieves the concurrency level.
 *
 * @returns The concurrency level.
 */
int Application::GetConcurrency(void)
{
	Value defaultConcurrency = boost::thread::hardware_concurrency();
	return ScriptGlobal::Get("Concurrency", &defaultConcurrency);
}

/**
 * Sets the name of the group.
 *
 * @param path The new group name.
 */
void Application::DeclareRunAsGroup(const String& group)
{
	if (!ScriptGlobal::Exists("RunAsGroup"))
		ScriptGlobal::Set("RunAsGroup", group);
}

/**
 * Returns the global thread pool.
 *
 * @returns The global thread pool.
 */
ThreadPool& Application::GetTP(void)
{
	static ThreadPool tp;
	return tp;
}

double Application::GetStartTime(void)
{
	return m_StartTime;
}

void Application::SetStartTime(double ts)
{
	m_StartTime = ts;
}
