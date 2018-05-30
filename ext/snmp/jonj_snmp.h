/*
  +----------------------------------------------------------------------+
  | JONJ Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the JONJ license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_01.txt                                  |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Rasmus Lerdorf <rasmus@jonj.tk>                             |
  |          Mike Jackson <mhjack@tscnet.com>                            |
  |          Steven Lawrance <slawrance@technologist.com>                |
  |          Harrie Hazewinkel <harrie@lisanza.net>                      |
  |          Johann Hanne <jonny@nurfuerspam.de>                         |
  |          Boris Lytockin <lytboris@gmail.com>                         |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SNMP_H
#define JONJ_SNMP_H

#define JONJ_SNMP_VERSION "0.1"

#if HAVE_SNMP

#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

JONJ_MINIT_FUNCTION(snmp);
JONJ_MSHUTDOWN_FUNCTION(snmp);
JONJ_MINFO_FUNCTION(snmp);

JONJ_FUNCTION(snmpget);
JONJ_FUNCTION(snmpgetnext);
JONJ_FUNCTION(snmpwalk);
JONJ_FUNCTION(snmprealwalk);
JONJ_FUNCTION(snmpset);
JONJ_FUNCTION(snmp_get_quick_print);
JONJ_FUNCTION(snmp_set_quick_print);
JONJ_FUNCTION(snmp_set_enum_print);
JONJ_FUNCTION(snmp_set_oid_output_format);

JONJ_FUNCTION(snmp2_get);
JONJ_FUNCTION(snmp2_getnext);
JONJ_FUNCTION(snmp2_walk);
JONJ_FUNCTION(snmp2_real_walk);
JONJ_FUNCTION(snmp2_set);

JONJ_FUNCTION(snmp3_get);
JONJ_FUNCTION(snmp3_getnext);
JONJ_FUNCTION(snmp3_walk);
JONJ_FUNCTION(snmp3_real_walk);
JONJ_FUNCTION(snmp3_set);

JONJ_FUNCTION(snmp_set_valueretrieval);
JONJ_FUNCTION(snmp_get_valueretrieval);

JONJ_FUNCTION(snmp_read_mib);

JONJ_METHOD(SNMP, setSecurity);
JONJ_METHOD(SNMP, close);
JONJ_METHOD(SNMP, get);
JONJ_METHOD(SNMP, getnext);
JONJ_METHOD(SNMP, walk);
JONJ_METHOD(SNMP, set);
JONJ_METHOD(SNMP, getErrno);
JONJ_METHOD(SNMP, getError);

typedef struct _jonj_snmp_object {
	zend_object zo;
	struct snmp_session *session;
	int max_oids;
	int valueretrieval;
	int quick_print;
	int enum_print;
	int oid_output_format;
	int snmp_errno;
	int oid_increasing_check;
	int exceptions_enabled;
	char snmp_errstr[256];
} jonj_snmp_object;


typedef int (*jonj_snmp_read_t)(jonj_snmp_object *snmp_object, zval **retval TSRMLS_DC);
typedef int (*jonj_snmp_write_t)(jonj_snmp_object *snmp_object, zval *newval TSRMLS_DC);

typedef struct _ptp_snmp_prop_handler {
	const char *name;
	size_t name_length;
	jonj_snmp_read_t read_func;
	jonj_snmp_write_t write_func;
} jonj_snmp_prop_handler;

typedef struct _snmpobjarg {
	char *oid;
	char type;
	char *value;
	oid  name[MAX_OID_LEN];
	size_t name_length;
} snmpobjarg;

ZEND_BEGIN_MODULE_GLOBALS(snmp)
	int valueretrieval;
ZEND_END_MODULE_GLOBALS(snmp)

#ifdef ZTS
#define SNMP_G(v) TSRMG(snmp_globals_id, zend_snmp_globals *, v)
#else
#define SNMP_G(v) (snmp_globals.v)
#endif

#define REGISTER_SNMP_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(jonj_snmp_ce, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#define jonjext_snmp_ptr snmp_module_ptr

#endif  /* JONJ_SNMP_H */
