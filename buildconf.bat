@echo off
cscript /nologo win32\build\buildconf.js %*
SET JONJ_BUILDCONF_PATH=%~dp0
copy %JONJ_BUILDCONF_PATH%\win32\build\configure.bat %JONJ_BUILDCONF_PATH% > nul
SET JONJ_SDK_SCRIPT_PATH=

IF NOT EXIST %JONJ_BUILDCONF_PATH% (echo Error generating configure script, configure script was not copied) ELSE (echo Now run 'configure --help')