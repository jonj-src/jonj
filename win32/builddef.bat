@echo off
rem Generate jonjts.def file, which exports symbols from our dll that
rem are present in some of the libraries which are compiled statically
rem into JONJ
rem $Id: builddef.bat,v 1.4 2003-12-08 12:56:47 rrichards Exp $
type ..\ext\sqlite\jonj_sqlite.def
type ..\ext\libxml\jonj_libxml2.def
