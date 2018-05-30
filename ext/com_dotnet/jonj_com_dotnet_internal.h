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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_COM_DOTNET_INTERNAL_H
#define JONJ_COM_DOTNET_INTERNAL_H

#define _WIN32_DCOM
#define COBJMACROS
#include <ocidl.h>
#include <oleauto.h>
#include <unknwn.h>
#include <dispex.h>
#include "win32/winutil.h"

typedef struct _jonj_com_dotnet_object {
	zend_object zo;

	VARIANT v;
	int modified;

	ITypeInfo *typeinfo;
	long code_page;

	zend_class_entry *ce;

   	/* associated event sink */
	IDispatch *sink_dispatch;
	GUID sink_id;
	DWORD sink_cookie;

	/* cache for method signatures */
	HashTable *method_cache;
	/* cache for name -> DISPID */
	HashTable *id_of_name_cache;
} jonj_com_dotnet_object;

static inline int jonj_com_is_valid_object(zval *zv TSRMLS_DC)
{
	zend_class_entry *ce = Z_OBJCE_P(zv);
	return strcmp("com", ce->name) == 0 ||
		strcmp("dotnet", ce->name) == 0 ||
		strcmp("variant", ce->name) == 0;
}

#define CDNO_FETCH(zv)			(jonj_com_dotnet_object*)zend_object_store_get_object(zv TSRMLS_CC)
#define CDNO_FETCH_VERIFY(obj, zv)	do { \
	if (!jonj_com_is_valid_object(zv TSRMLS_CC)) { \
		jonj_com_throw_exception(E_UNEXPECTED, "expected a variant object" TSRMLS_CC); \
		return; \
	} \
	obj = (jonj_com_dotnet_object*)zend_object_store_get_object(zv TSRMLS_CC); \
} while(0)

/* com_extension.c */
TsHashTable jonj_com_typelibraries;
zend_class_entry *jonj_com_variant_class_entry, *jonj_com_exception_class_entry, *jonj_com_saproxy_class_entry;

/* com_handlers.c */
zend_object_value jonj_com_object_new(zend_class_entry *ce TSRMLS_DC);
void jonj_com_object_clone(void *object, void **clone_ptr TSRMLS_DC);
void jonj_com_object_free_storage(void *object TSRMLS_DC);
zend_object_handlers jonj_com_object_handlers;
void jonj_com_object_enable_event_sink(jonj_com_dotnet_object *obj, int enable TSRMLS_DC);

/* com_saproxy.c */
zend_object_iterator *jonj_com_saproxy_iter_get(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);
int jonj_com_saproxy_create(zval *com_object, zval *proxy_out, zval *index TSRMLS_DC);

/* com_olechar.c */
JONJ_COM_DOTNET_API char *jonj_com_olestring_to_string(OLECHAR *olestring,
		uint *string_len, int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API OLECHAR *jonj_com_string_to_olestring(char *string,
		uint string_len, int codepage TSRMLS_DC);


/* com_com.c */
JONJ_FUNCTION(com_create_instance);
JONJ_FUNCTION(com_event_sink);
JONJ_FUNCTION(com_create_guid);
JONJ_FUNCTION(com_print_typeinfo);
JONJ_FUNCTION(com_message_pump);
JONJ_FUNCTION(com_load_typelib);
JONJ_FUNCTION(com_get_active_object);

HRESULT jonj_com_invoke_helper(jonj_com_dotnet_object *obj, DISPID id_member,
		WORD flags, DISPPARAMS *disp_params, VARIANT *v, int silent, int allow_noarg TSRMLS_DC);
HRESULT jonj_com_get_id_of_name(jonj_com_dotnet_object *obj, char *name,
		int namelen, DISPID *dispid TSRMLS_DC);
int jonj_com_do_invoke_by_id(jonj_com_dotnet_object *obj, DISPID dispid,
		WORD flags,	VARIANT *v, int nargs, zval **args, int silent, int allow_noarg TSRMLS_DC);
int jonj_com_do_invoke(jonj_com_dotnet_object *obj, char *name, int namelen,
		WORD flags,	VARIANT *v, int nargs, zval **args, int allow_noarg TSRMLS_DC);
int jonj_com_do_invoke_byref(jonj_com_dotnet_object *obj, char *name, int namelen,
		WORD flags,	VARIANT *v, int nargs, zval ***args TSRMLS_DC);

/* com_wrapper.c */
int jonj_com_wrapper_minit(INIT_FUNC_ARGS);
JONJ_COM_DOTNET_API IDispatch *jonj_com_wrapper_export_as_sink(zval *val, GUID *sinkid, HashTable *id_to_name TSRMLS_DC);
JONJ_COM_DOTNET_API IDispatch *jonj_com_wrapper_export(zval *val TSRMLS_DC);

/* com_persist.c */
int jonj_com_persist_minit(INIT_FUNC_ARGS);

/* com_variant.c */
JONJ_FUNCTION(com_variant_create_instance);
JONJ_FUNCTION(variant_set);
JONJ_FUNCTION(variant_add);
JONJ_FUNCTION(variant_cat);
JONJ_FUNCTION(variant_sub);
JONJ_FUNCTION(variant_mul);
JONJ_FUNCTION(variant_and);
JONJ_FUNCTION(variant_div);
JONJ_FUNCTION(variant_eqv);
JONJ_FUNCTION(variant_idiv);
JONJ_FUNCTION(variant_imp);
JONJ_FUNCTION(variant_mod);
JONJ_FUNCTION(variant_or);
JONJ_FUNCTION(variant_pow);
JONJ_FUNCTION(variant_xor);
JONJ_FUNCTION(variant_abs);
JONJ_FUNCTION(variant_fix);
JONJ_FUNCTION(variant_int);
JONJ_FUNCTION(variant_neg);
JONJ_FUNCTION(variant_not);
JONJ_FUNCTION(variant_round);
JONJ_FUNCTION(variant_cmp);
JONJ_FUNCTION(variant_date_to_timestamp);
JONJ_FUNCTION(variant_date_from_timestamp);
JONJ_FUNCTION(variant_get_type);
JONJ_FUNCTION(variant_set_type);
JONJ_FUNCTION(variant_cast);

JONJ_COM_DOTNET_API void jonj_com_variant_from_zval_with_type(VARIANT *v, zval *z, VARTYPE type, int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API void jonj_com_variant_from_zval(VARIANT *v, zval *z, int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API int jonj_com_zval_from_variant(zval *z, VARIANT *v, int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API int jonj_com_copy_variant(VARIANT *dst, VARIANT *src TSRMLS_DC);

/* com_dotnet.c */
JONJ_FUNCTION(com_dotnet_create_instance);
void jonj_com_dotnet_rshutdown(TSRMLS_D);
void jonj_com_dotnet_mshutdown(TSRMLS_D);

/* com_misc.c */
void jonj_com_throw_exception(HRESULT code, char *message TSRMLS_DC);
JONJ_COM_DOTNET_API void jonj_com_wrap_dispatch(zval *z, IDispatch *disp,
		int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API void jonj_com_wrap_variant(zval *z, VARIANT *v,
		int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API int jonj_com_safearray_get_elem(VARIANT *array, VARIANT *dest, LONG dim1 TSRMLS_DC);

/* com_typeinfo.c */
JONJ_COM_DOTNET_API ITypeLib *jonj_com_load_typelib_via_cache(char *search_string,
		int codepage, int *cached TSRMLS_DC);
JONJ_COM_DOTNET_API ITypeLib *jonj_com_load_typelib(char *search_string, int codepage TSRMLS_DC);
JONJ_COM_DOTNET_API int jonj_com_import_typelib(ITypeLib *TL, int mode,
		int codepage TSRMLS_DC);
void jonj_com_typelibrary_dtor(void *pDest);
ITypeInfo *jonj_com_locate_typeinfo(char *typelibname, jonj_com_dotnet_object *obj, char *dispname, int sink TSRMLS_DC);
int jonj_com_process_typeinfo(ITypeInfo *typeinfo, HashTable *id_to_name, int printdef, GUID *guid, int codepage TSRMLS_DC);

/* com_iterator.c */
zend_object_iterator *jonj_com_iter_get(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);


#endif
