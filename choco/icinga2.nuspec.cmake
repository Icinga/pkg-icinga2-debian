<?xml version="1.0" encoding="utf-8"?>
<!-- Do not remove this test for UTF-8: if ??? doesn?t appear as greek uppercase omega letter enclosed in quotation marks, you should use an editor that supports UTF-8, not this one. -->
<!--package xmlns="http://schemas.microsoft.com/packaging/2010/07/nuspec.xsd"-->
<package xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <metadata>
    <!-- Read this before publishing packages to chocolatey.org: https://github.com/chocolatey/chocolatey/wiki/CreatePackages -->
    <id>icinga2</id>
    <title>Icinga2</title>
    <version>${SPEC_VERSION}</version>
    <authors>2016 - The Icinga Project</authors>
    <owners>Icinga Development Team</owners>
    <summary>icinga2 - Monitoring Agent for Windows</summary>
    <description>Icinga 2 is an open source monitoring platform which notifies users about host and service outages.</description>
    <projectUrl>https://www.icinga.com/</projectUrl>
    <tags>icinga2 agent monitoring</tags>
    <licenseUrl>https://www.icinga.com/resources/faq/</licenseUrl>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <iconUrl>https://www.icinga.com/wp-content/uploads/2015/05/icinga_icon_128x128.png</iconUrl>
  </metadata>
  <files>
    <file src="${CMAKE_CURRENT_BINARY_DIR}/chocolateyInstall.ps1" target="tools" />
    <file src="${CMAKE_CURRENT_SOURCE_DIR}/chocolateyUninstall.ps1" target="tools" />
  </files>
</package>
