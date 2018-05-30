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
   | Author: Wez Furlong  <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "jonj_com_dotnet.h"
#include "jonj_com_dotnet_internal.h"
#include "Zend/zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(com_dotnet)
static JONJ_GINIT_FUNCTION(com_dotnet);

TsHashTable jonj_com_typelibraries;

zend_class_entry
	*jonj_com_variant_class_entry,
   	*jonj_com_exception_class_entry,
	*jonj_com_saproxy_class_entry;

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_set, 0, 0, 2)
	ZEND_ARG_INFO(0, variant)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_add, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_cat, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_sub, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_mul, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_and, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_div, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_eqv, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_idiv, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_imp, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_mod, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_or, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_pow, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_xor, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_abs, 0, 0, 1)
	ZEND_ARG_INFO(0, left)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_fix, 0, 0, 1)
	ZEND_ARG_INFO(0, left)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_int, 0, 0, 1)
	ZEND_ARG_INFO(0, left)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_neg, 0, 0, 1)
	ZEND_ARG_INFO(0, left)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_not, 0, 0, 1)
	ZEND_ARG_INFO(0, left)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_round, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, decimals)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_cmp, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
	ZEND_ARG_INFO(0, lcid)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_date_to_timestamp, 0, 0, 1)
	ZEND_ARG_INFO(0, variant)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_date_from_timestamp, 0, 0, 1)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_get_type, 0, 0, 1)
	ZEND_ARG_INFO(0, variant)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_set_type, 0, 0, 2)
	ZEND_ARG_INFO(0, variant)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_cast, 0, 0, 2)
	ZEND_ARG_INFO(0, variant)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_com_get_active_object, 0, 0, 1)
	ZEND_ARG_INFO(0, progid)
	ZEND_ARG_INFO(0, code_page)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_com_create_guid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_com_event_sink, 0, 0, 2)
	ZEND_ARG_INFO(0, comobject)
	ZEND_ARG_INFO(0, sinkobject)
	ZEND_ARG_INFO(0, sinkinterface)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_com_print_typeinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, comobject)
	ZEND_ARG_INFO(0, dispinterface)
	ZEND_ARG_INFO(0, wantsink)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_com_message_pump, 0, 0, 0)
	ZEND_ARG_INFO(0, timeoutms)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_com_load_typelib, 0, 0, 1)
	ZEND_ARG_INFO(0, typelib_name)
	ZEND_ARG_INFO(0, case_insensitive)
ZEND_END_ARG_INFO()
/* }}} */

const zend_function_entry com_dotnet_functions[] = {
	JONJ_FE(variant_set, arginfo_variant_set)
	JONJ_FE(variant_add, arginfo_variant_add)
	JONJ_FE(variant_cat, arginfo_variant_cat)
	JONJ_FE(variant_sub, arginfo_variant_sub)
	JONJ_FE(variant_mul, arginfo_variant_mul)
	JONJ_FE(variant_and, arginfo_variant_and)
	JONJ_FE(variant_div, arginfo_variant_div)
	JONJ_FE(variant_eqv, arginfo_variant_eqv)
	JONJ_FE(variant_idiv, arginfo_variant_idiv)
	JONJ_FE(variant_imp, arginfo_variant_imp)
	JONJ_FE(variant_mod, arginfo_variant_mod)
	JONJ_FE(variant_or, arginfo_variant_or)
	JONJ_FE(variant_pow, arginfo_variant_pow)
	JONJ_FE(variant_xor, arginfo_variant_xor)
	JONJ_FE(variant_abs, arginfo_variant_abs)
	JONJ_FE(variant_fix, arginfo_variant_fix)
	JONJ_FE(variant_int, arginfo_variant_int)
	JONJ_FE(variant_neg, arginfo_variant_neg)
	JONJ_FE(variant_not, arginfo_variant_not)
	JONJ_FE(variant_round, arginfo_variant_round)
	JONJ_FE(variant_cmp, arginfo_variant_cmp)
	JONJ_FE(variant_date_to_timestamp, arginfo_variant_date_to_timestamp)
	JONJ_FE(variant_date_from_timestamp, arginfo_variant_date_from_timestamp)
	JONJ_FE(variant_get_type, arginfo_variant_get_type)
	JONJ_FE(variant_set_type, arginfo_variant_set_type)
	JONJ_FE(variant_cast, arginfo_variant_cast)
	/* com_com.c */
	JONJ_FE(com_create_guid, arginfo_com_create_guid)
	JONJ_FE(com_event_sink, arginfo_com_event_sink)
	JONJ_FE(com_print_typeinfo, arginfo_com_print_typeinfo)
	JONJ_FE(com_message_pump, arginfo_com_message_pump)
	JONJ_FE(com_load_typelib, arginfo_com_load_typelib)
	JONJ_FE(com_get_active_object, arginfo_com_get_active_object)
	JONJ_FE_END
};

/* {{{ com_dotnet_module_entry
 */
zend_module_entry com_dotnet_module_entry = {
	STANDARD_MODULE_HEADER,
	"com_dotnet",
	com_dotnet_functions,
	JONJ_MINIT(com_dotnet),
	JONJ_MSHUTDOWN(com_dotnet),
	JONJ_RINIT(com_dotnet),
	JONJ_RSHUTDOWN(com_dotnet),
	JONJ_MINFO(com_dotnet),
	"0.1",
	JONJ_MODULE_GLOBALS(com_dotnet),
	JONJ_GINIT(com_dotnet),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_COM_DOTNET
ZEND_GET_MODULE(com_dotnet)
#endif

/* {{{ JONJ_INI
 */

/* com.typelib_file is the path to a file containing a
 * list of typelibraries to register *persistently*.
 * lines starting with ; are comments
 * append #cis to end of typelib name to cause its constants
 * to be loaded case insensitively */
static JONJ_INI_MH(OnTypeLibFileUpdate)
{
	FILE *typelib_file;
	char *typelib_name_buffer;
	char *strtok_buf = NULL;
	int cached;

	if (!new_value || !new_value[0] || (typelib_file = VCWD_FOPEN(new_value, "r"))==NULL) {
		return FAILURE;
	}

	typelib_name_buffer = (char *) emalloc(sizeof(char)*1024);

	while (fgets(typelib_name_buffer, 1024, typelib_file)) {
		ITypeLib *pTL;
		char *typelib_name;
		char *modifier, *ptr;
		int mode = CONST_CS | CONST_PERSISTENT;	/* CONST_PERSISTENT is ok here */

		if (typelib_name_buffer[0]==';') {
			continue;
		}
		typelib_name = jonj_strtok_r(typelib_name_buffer, "\r\n", &strtok_buf); /* get rid of newlines */
		if (typelib_name == NULL) {
			continue;
		}
		typelib_name = jonj_strtok_r(typelib_name, "#", &strtok_buf);
		modifier = jonj_strtok_r(NULL, "#", &strtok_buf);
		if (modifier != NULL) {
			if (!strcmp(modifier, "cis") || !strcmp(modifier, "case_insensitive")) {
				mode &= ~CONST_CS;
			}
		}

		/* Remove leading/training white spaces on search_string */
		while (isspace(*typelib_name)) {/* Ends on '\0' in worst case */
			typelib_name ++;
		}
		ptr = typelib_name + strlen(typelib_name) - 1;
		while ((ptr != typelib_name) && isspace(*ptr)) {
			*ptr = '\0';
			ptr--;
		}

		if ((pTL = jonj_com_load_typelib_via_cache(typelib_name, COMG(code_page), &cached TSRMLS_CC)) != NULL) {
			if (!cached) {
				jonj_com_import_typelib(pTL, mode, COMG(code_page) TSRMLS_CC);
			}
			ITypeLib_Release(pTL);
		}
	}

	efree(typelib_name_buffer);
	fclose(typelib_file);

	return SUCCESS;
}

JONJ_INI_BEGIN()
    STD_JONJ_INI_ENTRY("com.allow_dcom",				"0", JONJ_INI_SYSTEM, OnUpdateBool, allow_dcom, zend_com_dotnet_globals, com_dotnet_globals)
    STD_JONJ_INI_ENTRY("com.autoregister_verbose",	"0", JONJ_INI_ALL, OnUpdateBool, autoreg_verbose, zend_com_dotnet_globals, com_dotnet_globals)
    STD_JONJ_INI_ENTRY("com.autoregister_typelib",	"0", JONJ_INI_ALL, OnUpdateBool, autoreg_on, zend_com_dotnet_globals, com_dotnet_globals)
    STD_JONJ_INI_ENTRY("com.autoregister_casesensitive",	"1", JONJ_INI_ALL, OnUpdateBool, autoreg_case_sensitive, zend_com_dotnet_globals, com_dotnet_globals)
	STD_JONJ_INI_ENTRY("com.code_page", "", JONJ_INI_ALL, OnUpdateLong, code_page, zend_com_dotnet_globals, com_dotnet_globals)
	JONJ_INI_ENTRY("com.typelib_file", "", JONJ_INI_SYSTEM, OnTypeLibFileUpdate)
JONJ_INI_END()
/* }}} */

/* {{{ JONJ_GINIT_FUNCTION
 */
static JONJ_GINIT_FUNCTION(com_dotnet)
{
	memset(com_dotnet_globals, 0, sizeof(*com_dotnet_globals));
	com_dotnet_globals->code_page = CP_ACP;
}
/* }}} */

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(com_dotnet)
{
	zend_class_entry ce, *tmp;

	jonj_com_wrapper_minit(INIT_FUNC_ARGS_PASSTHRU);
	jonj_com_persist_minit(INIT_FUNC_ARGS_PASSTHRU);

	INIT_CLASS_ENTRY(ce, "com_exception", NULL);
	jonj_com_exception_class_entry = zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
	jonj_com_exception_class_entry->ce_flags |= ZEND_ACC_FINAL;
/*	jonj_com_exception_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */

	INIT_CLASS_ENTRY(ce, "com_safearray_proxy", NULL);
	jonj_com_saproxy_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	jonj_com_saproxy_class_entry->ce_flags |= ZEND_ACC_FINAL;
/*	jonj_com_saproxy_class_entry->constructor->common.fn_flags |= ZEND_ACC_PROTECTED; */
	jonj_com_saproxy_class_entry->get_iterator = jonj_com_saproxy_iter_get;
	
	INIT_CLASS_ENTRY(ce, "variant", NULL);
	ce.create_object = jonj_com_object_new;
	jonj_com_variant_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	jonj_com_variant_class_entry->get_iterator = jonj_com_iter_get;

	INIT_CLASS_ENTRY(ce, "com", NULL);
	ce.create_object = jonj_com_object_new;
	tmp = zend_register_internal_class_ex(&ce, jonj_com_variant_class_entry, "variant" TSRMLS_CC);
	tmp->get_iterator = jonj_com_iter_get;

	zend_ts_hash_init(&jonj_com_typelibraries, 0, NULL, jonj_com_typelibrary_dtor, 1);

#if HAVE_MSCOREE_H
	INIT_CLASS_ENTRY(ce, "dotnet", NULL);
	ce.create_object = jonj_com_object_new;
	tmp = zend_register_internal_class_ex(&ce, jonj_com_variant_class_entry, "variant" TSRMLS_CC);
	tmp->get_iterator = jonj_com_iter_get;
#endif

	REGISTER_INI_ENTRIES();

#define COM_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_CS|CONST_PERSISTENT)
	
	COM_CONST(CLSCTX_INPROC_SERVER);
	COM_CONST(CLSCTX_INPROC_HANDLER);
	COM_CONST(CLSCTX_LOCAL_SERVER);
	COM_CONST(CLSCTX_REMOTE_SERVER);
	COM_CONST(CLSCTX_SERVER);
	COM_CONST(CLSCTX_ALL);

#if 0
	COM_CONST(DISPATCH_METHOD);
	COM_CONST(DISPATCH_PROPERTYGET);
	COM_CONST(DISPATCH_PROPERTYPUT);
#endif

	COM_CONST(VT_NULL);
	COM_CONST(VT_EMPTY);
	COM_CONST(VT_UI1);
	COM_CONST(VT_I1);
	COM_CONST(VT_UI2);
	COM_CONST(VT_I2);
	COM_CONST(VT_UI4);
	COM_CONST(VT_I4);
	COM_CONST(VT_R4);
	COM_CONST(VT_R8);
	COM_CONST(VT_BOOL);
	COM_CONST(VT_ERROR);
	COM_CONST(VT_CY);
	COM_CONST(VT_DATE);
	COM_CONST(VT_BSTR);
	COM_CONST(VT_DECIMAL);
	COM_CONST(VT_UNKNOWN);
	COM_CONST(VT_DISPATCH);
	COM_CONST(VT_VARIANT);
	COM_CONST(VT_INT);
	COM_CONST(VT_UINT);
	COM_CONST(VT_ARRAY);
	COM_CONST(VT_BYREF);

	COM_CONST(CP_ACP);
	COM_CONST(CP_MACCP);
	COM_CONST(CP_OEMCP);
	COM_CONST(CP_UTF7);
	COM_CONST(CP_UTF8);
	COM_CONST(CP_SYMBOL);
	COM_CONST(CP_THREAD_ACP);

	COM_CONST(VARCMP_LT);
	COM_CONST(VARCMP_EQ);
	COM_CONST(VARCMP_GT);
	COM_CONST(VARCMP_NULL);

	COM_CONST(NORM_IGNORECASE);
	COM_CONST(NORM_IGNORENONSPACE);
	COM_CONST(NORM_IGNORESYMBOLS);
	COM_CONST(NORM_IGNOREWIDTH);
	COM_CONST(NORM_IGNOREKANATYPE);
#ifdef NORM_IGNOREKASHIDA
	COM_CONST(NORM_IGNOREKASHIDA);
#endif
	COM_CONST(DISP_E_DIVBYZERO);
	COM_CONST(DISP_E_OVERFLOW);
	COM_CONST(DISP_E_BADINDEX);
	COM_CONST(MK_E_UNAVAILABLE);

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(com_dotnet)
{
	UNREGISTER_INI_ENTRIES();
#if HAVE_MSCOREE_H
	if (COMG(dotnet_runtime_stuff)) {
		jonj_com_dotnet_mshutdown(TSRMLS_C);
	}
#endif

	zend_ts_hash_destroy(&jonj_com_typelibraries);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_RINIT_FUNCTION
 */
JONJ_RINIT_FUNCTION(com_dotnet)
{
	COMG(rshutdown_started) = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_RSHUTDOWN_FUNCTION
 */
JONJ_RSHUTDOWN_FUNCTION(com_dotnet)
{
#if HAVE_MSCOREE_H
	if (COMG(dotnet_runtime_stuff)) {
		jonj_com_dotnet_rshutdown(TSRMLS_C);
	}
#endif
	COMG(rshutdown_started) = 1;
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(com_dotnet)
{
	jonj_info_print_table_start();

	jonj_info_print_table_header(2, "COM support", "enabled");
	jonj_info_print_table_header(2, "DCOM support", COMG(allow_dcom) ? "enabled" : "disabled");

#if HAVE_MSCOREE_H
	jonj_info_print_table_header(2, ".Net support", "enabled");
#else
	jonj_info_print_table_header(2, ".Net support", "not present in this build");
#endif

	jonj_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
