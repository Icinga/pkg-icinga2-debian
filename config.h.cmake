#ifndef CONFIG_H
#define CONFIG_H

#cmakedefine HAVE_COUNTER_MACRO
#cmakedefine HAVE_BACKTRACE_SYMBOLS
#cmakedefine HAVE_PIPE2
#cmakedefine HAVE_VFORK
#cmakedefine HAVE_DLADDR
#cmakedefine HAVE_LIBEXECINFO
#cmakedefine HAVE_CXXABI_H
#cmakedefine HAVE_NICE
#cmakedefine HAVE_EDITLINE

#cmakedefine ICINGA2_UNITY_BUILD

#define ICINGA_PREFIX "${CMAKE_INSTALL_PREFIX}"
#define ICINGA_SYSCONFDIR "${CMAKE_INSTALL_FULL_SYSCONFDIR}"
#define ICINGA_RUNDIR "${ICINGA2_RUNDIR}"
#define ICINGA_LOCALSTATEDIR "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}"
#define ICINGA_PKGDATADIR "${CMAKE_INSTALL_FULL_DATADIR}/icinga2"
#define ICINGA_INCLUDECONFDIR "${CMAKE_INSTALL_FULL_DATADIR}/icinga2/include"
#define ICINGA_USER "${ICINGA2_USER}"
#define ICINGA_GROUP "${ICINGA2_GROUP}"

#define ICINGA_BUILD_HOST_NAME "${ICINGA2_BUILD_HOST_NAME}"
#define ICINGA_BUILD_COMPILER_NAME "${ICINGA2_BUILD_COMPILER_NAME}"
#define ICINGA_BUILD_COMPILER_VERSION "${ICINGA2_BUILD_COMPILER_VERSION}"

#endif /* CONFIG_H */
