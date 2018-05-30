#  +----------------------------------------------------------------------+
#  | JONJ Version 5                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2016 The JONJ Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 3.01 of the JONJ license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available through the world-wide-web at the following url:           |
#  | http://www.jonj.tk/license/3_01.txt                                  |
#  | If you did not receive a copy of the JONJ license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@jonj.tk so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Author: Wez Furlong <wez@thebrainroom.com>                           |
#  +----------------------------------------------------------------------+
#
# $Id$
# This is the makefile template for the win32 build

CC="$(JONJ_CL)"
LD="$(LINK)"
MC="$(MC)"
MT="$(MT)"
RE2C="$(RE2C)"
PGOMGR="$(PGOMGR)"
JONJ_BUILD=$(JONJ_BUILD)

MCFILE=$(BUILD_DIR)\wsyslog.rc
BUILD_DIR_DEV_NAME=jonj-$(JONJ_VERSION_STRING)-devel-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE)
BUILD_DIR_DEV=$(BUILD_DIR)\$(BUILD_DIR_DEV_NAME)

all: generated_files $(EXT_TARGETS) $(PECL_TARGETS) $(SAPI_TARGETS)

build_dirs: $(BUILD_DIR) $(BUILD_DIRS_SUB) $(BUILD_DIR_DEV)

!if $(RE2C) == ""
generated_files: build_dirs Zend\zend_ini_parser.c \
	Zend\zend_language_parser.c \
	$(JONJDEF) $(MCFILE)
!else
generated_files: build_dirs Zend\zend_ini_parser.c \
	Zend\zend_language_parser.c Zend\zend_ini_scanner.c \
	Zend\zend_language_scanner.c \
	$(JONJDEF) $(MCFILE)
!endif

$(BUILD_DIR)\$(JONJDLL).def: $(JONJ_DLL_DEF_SOURCES)
	type $(JONJ_DLL_DEF_SOURCES) > $(BUILD_DIR)\$(JONJDLL).def

Zend\zend_ini_parser.c Zend\zend_ini_parser.h: Zend\zend_ini_parser.y
	$(BISON) --output=Zend/zend_ini_parser.c -v -d -p ini_ Zend/zend_ini_parser.y

Zend\zend_language_parser.c Zend\zend_language_parser.h: Zend\zend_language_parser.y
	$(BISON) --output=Zend/zend_language_parser.c -v -d -p zend Zend/zend_language_parser.y

!if $(RE2C) != ""
Zend\zend_ini_scanner.c: Zend\zend_ini_scanner.l
	$(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_ini_scanner_defs.h -oZend/zend_ini_scanner.c Zend/zend_ini_scanner.l

Zend\zend_language_scanner.c: Zend\zend_language_scanner.l
	$(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt Zend/zend_language_scanner_defs.h -oZend/zend_language_scanner.c Zend/zend_language_scanner.l
!endif

!if $(PGOMGR) != ""
JONJ5_PGD_OPTION=/PGD:$(PGOPGD_DIR)\jonj5.pgd
!else
JONJ5_PGD_OPTION=
!endif

JONJDLL_RES=$(BUILD_DIR)\$(JONJDLL).res

$(MCFILE): win32\build\wsyslog.mc
	$(MC) -h win32\ -r $(BUILD_DIR)\ -x $(BUILD_DIR)\ win32\build\wsyslog.mc

#	$(RC) /fo $(MCFILE) $(BUILD_DIR)\wsyslog.rc

!if $(MT) == ""
_VC_MANIFEST_EMBED_EXE=
_VC_MANIFEST_EMBED_DLL=
!else
_VC_MANIFEST_EMBED_EXE= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED_DLL= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;2
!endif

$(JONJDLL_RES): win32\build\template.rc
	$(RC) /fo $(JONJDLL_RES) /d FILE_DESCRIPTION="\"JONJ Script Interpreter\"" \
		/d FILE_NAME="\"$(JONJDLL)\"" /d PRODUCT_NAME="\"JONJ Script Interpreter\"" \
		/I$(BUILD_DIR) /d MC_INCLUDE="\"$(MCFILE)\"" \
		win32\build\template.rc

$(BUILD_DIR)\$(JONJDLL): generated_files $(JONJDEF) $(JONJ_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(JONJDLL_RES) $(MCFILE)
	@copy win32\build\default.manifest $(BUILD_DIR)\$(JONJDLL).manifest
	@$(CC) $(JONJ_GLOBAL_OBJS) $(STATIC_EXT_OBJS) $(STATIC_EXT_LIBS) $(LIBS) $(JONJDLL_RES) /link /out:$(BUILD_DIR)\$(JONJDLL) $(JONJ5_PGD_OPTION) $(JONJ_LDFLAGS) $(LDFLAGS) $(STATIC_EXT_LDFLAGS)
	-@$(_VC_MANIFEST_EMBED_DLL)

$(BUILD_DIR)\$(JONJLIB): $(BUILD_DIR)\$(JONJDLL)

$(BUILD_DIR) $(BUILD_DIRS_SUB) $(BUILD_DIR_DEV):
	@echo Recreating build dirs
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR)
	@for %D in ($(BUILD_DIRS_SUB)) do @if not exist %D @mkdir %D > NUL
	@if not exist $(BUILD_DIR_DEV) @mkdir $(BUILD_DIR_DEV) > NUL
	@cd "$(JONJ_SRC_DIR)"
	

clean-sapi:
	@echo Cleaning SAPI
	@for %D in (_x $(EXT_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	@for %D in (_x $(PECL_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	@for %D in (_x $(SAPI_TARGETS)) do @if exist $(BUILD_DIR)\%D @del /F /Q $(BUILD_DIR)\%D > NUL
	-@del /F /Q $(BUILD_DIR)\$(JONJDLL)

clean: clean-sapi
	@echo Cleaning distribution build dirs
	@cd $(BUILD_DIR)
	@for %D in (_x $(BUILD_DIRS_SUB)) do @if exist %D @del /F /Q %D\*.* > NUL
	@cd "$(JONJ_SRC_DIR)"
	-@del /F /Q $(BUILD_DIR)\*.res $(BUILD_DIR)\*.manifest $(BUILD_DIR)\*.lib $(BUILD_DIR)\*.ilk $(BUILD_DIR)\*.pdb $(BUILD_DIR)\*.exp $(JONJDEF) $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)-Win32.zip $(BUILD_DIR)\pecl-$(JONJ_VERSION_STRING)-Win32.zip > NUL
	-rd /s /q $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)

clean-pecl:
	@echo Cleaning PECL targets only
	-rd /s /q $(BUILD_DIR)\pecl

clean-all:
	@echo Cleaning standard build dirs
	@cd $(BUILD_DIR)
	@for %D in (_x $(BUILD_DIRS_SUB)) do @if exist %D @rd /s /q %D
	@cd "$(JONJ_SRC_DIR)"
	-@del /f /q $(BUILD_DIR)\*.res $(BUILD_DIR)\*.lib $(BUILD_DIR)\*.ilk $(BUILD_DIR)\*.pdb $(BUILD_DIR)\*.exp $(JONJDEF) $(BUILD_DIR)\*.rc $(BUILD_DIR)\*.manifest $(BUILD_DIR)\*.dbg $(BUILD_DIR)\*.bin $(BUILD_DIR)\jonj*.dll $(BUILD_DIR)\jonj*.exe > NUL

clean-pgo: clean-all
	-rd /s /q $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)
	-rd /s /q $(BUILD_DIR)\pecl-$(JONJ_VERSION_STRING)
	-rd /s /q $(BUILD_DIR)\jonj-test-pack-$(JONJ_VERSION_STRING)
	-del /f /q $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\jonj-debug-pack-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\pecl-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\jonj-test-pack-$(JONJ_VERSION_STRING).zip

test:
	"$(BUILD_DIR)\jonj.exe" -d open_basedir= -d output_buffering=0 run-tests.jonj $(TESTS) -p "$(BUILD_DIR)\jonj.exe"

build-snap: generated_files
	SET PATH=$(PATH);$(JONJ_BUILD)\bin
	@$(MAKE) "$(BUILD_DIR)\$(JONJDLL)"
	-for %T in ($(SAPI_TARGETS)) do $(MAKE) /I /nologo "%T"
	-for %T in ($(EXT_TARGETS)) do $(MAKE) /I /nologo "%T"
	-for %T in ($(PECL_TARGETS)) do $(MAKE) /I /nologo "%T"

build-dist: $(BUILD_DIR)\deplister.exe
	-rd /s /q $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)
	-rd /s /q $(BUILD_DIR)\pecl-$(JONJ_VERSION_STRING)
	-del /f /q $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\jonj-debug-pack-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip
	-del /f /q $(BUILD_DIR)\pecl-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip
	$(BUILD_DIR)\jonj.exe -d date.timezone=UTC -n -dphar.readonly=0 win32/build/mkdist.jonj "$(BUILD_DIR)" "$(JONJ_BUILD)" "$(JONJDLL)" "$(SAPI_TARGETS)" "$(EXT_TARGETS) $(JONJ_EXTRA_DIST_FILES)" "$(PECL_TARGETS) $(PECL_EXTRA_DIST_FILES)" "$(SNAPSHOT_TEMPLATE)"
	cd $(BUILD_DIR)\jonj-$(JONJ_VERSION_STRING)
	-$(ZIP) -9 -q -r ..\jonj-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip .
	cd ..\..
	cd $(BUILD_DIR)\pecl-$(JONJ_VERSION_STRING)
	-$(ZIP) -9 -q -r ..\pecl-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip .
	cd ..\..
	cd $(BUILD_DIR)\jonj-test-pack-$(JONJ_VERSION_STRING)
	-$(ZIP) -9 -q -r ..\jonj-test-pack-$(JONJ_VERSION_STRING).zip .
	cd ..\..
	cd $(BUILD_DIR)
	-$(ZIP) -9 -q jonj-debug-pack-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip *.pdb
	cd
	cd
	-$(ZIP) -9 -q -r jonj-devel-pack-$(JONJ_VERSION_STRING)$(JONJ_ZTS_ARCHIVE_POSTFIX)-Win32-$(JONJ_COMPILER_SHORT)-$(JONJ_ARCHITECTURE).zip $(BUILD_DIR_DEV_NAME)
	cd ..\..

dist: all build-dist
snap: build-snap build-devel build-dist

$(BUILD_DIR)\deplister.exe:	win32\build\deplister.c
	$(JONJ_CL) /Fo$(BUILD_DIR)\ /Fd$(BUILD_DIR)\ /Fp$(BUILD_DIR)\ /FR$(BUILD_DIR) /Fe$(BUILD_DIR)\deplister.exe win32\build\deplister.c imagehlp.lib

msi-installer: dist
	$(BUILD_DIR)\jonj.exe ..\jonj-installer\build-installer.jonj "$(BUILD_DIR)" "$(JONJDLL)" "$(SAPI_TARGETS)" "$(EXT_TARGETS)" "$(PECL_TARGETS)"

# need to redirect, since INSTALL is a file in the root...
install: really-install install-sdk

build-lib: build-ext-libs
	@if not exist $(BUILD_DIR_DEV)\lib mkdir $(BUILD_DIR_DEV)\lib >nul
	@copy $(BUILD_DIR)\$(JONJLIB) $(BUILD_DIR_DEV)\lib /y >nul

build-devel: build-headers build-lib
	@if not exist $(BUILD_DIR_DEV)\script mkdir $(BUILD_DIR_DEV)\script >nul
	@if not exist $(BUILD_DIR_DEV)\build mkdir $(BUILD_DIR_DEV)\build >nul
	@copy run-tests.jonj $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\confutils.js $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\configure.tail $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\config.w32.jonjize.in $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\Makefile.jonjize $(BUILD_DIR_DEV)\script\ /y >nul
	@copy win32\build\jonjize.bat $(BUILD_DIR_DEV)\ /y >nul
	@copy win32\build\template.rc $(BUILD_DIR_DEV)\build\ /y >nul
	@copy win32\build\default.manifest $(BUILD_DIR_DEV)\build\ /y >nul
	@copy $(BUILD_DIR)\devel\config.jonjize.js $(BUILD_DIR_DEV)\script\ /y >nul
	@copy $(BUILD_DIR)\devel\jonjize.js $(BUILD_DIR_DEV)\script\ /y >nul
	@copy $(BUILD_DIR)\devel\ext_deps.js $(BUILD_DIR_DEV)\script\ /y >nul

install-sdk: build-devel
	@xcopy /Q /Y /E /I $(BUILD_DIR_DEV)\* $(JONJ_PREFIX)\SDK

really-install:
	@if not exist $(JONJ_PREFIX) mkdir $(JONJ_PREFIX)
	@if not exist $(JONJ_PREFIX)\ext mkdir $(JONJ_PREFIX)\ext
	@echo Installing files under $(JONJ_PREFIX)
	@copy $(BUILD_DIR)\*.exe $(JONJ_PREFIX) /y >nul
	@copy $(BUILD_DIR)\jonj_*.dll $(JONJ_PREFIX)\ext /y >nul
	dir /b $(BUILD_DIR)\jonj_*.dll > $(BUILD_DIR)\extension_dlls.txt
	@xcopy $(BUILD_DIR)\*.dll /exclude:$(BUILD_DIR)\extension_dlls.txt $(JONJ_PREFIX) /y >nul
	@echo Registering event source with syslog (requires admin rights)
	@echo It's okay for this step to fail:
	-$(JONJ_PREFIX)\jonj.exe -n -dextension_dir=$(JONJ_PREFIX) win32/build/registersyslog.jonj $(JONJ_PREFIX)\$(JONJDLL)

