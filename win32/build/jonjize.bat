@echo off
SET JONJ_BUILDCONF_PATH=%~dp0
cscript /nologo %JONJ_BUILDCONF_PATH%\script\jonjize.js %*
copy %JONJ_BUILDCONF_PATH%\win32\build\configure.bat %JONJ_BUILDCONF_PATH% > nul
IF NOT EXIST %JONJ_BUILDCONF_PATH% (echo Error generating configure script, configure script was not copied) ELSE (echo Now run 'configure --help')
SET JONJ_BUILDCONF_PATH=
