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
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@zend.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifndef JONJ_SOAP_H
#define JONJ_SOAP_H

#include "jonj.h"
#include "jonj_globals.h"
#include "ext/standard/info.h"
#include "ext/standard/jonj_standard.h"
#if HAVE_JONJ_SESSION && !defined(COMPILE_DL_SESSION)
#include "ext/session/jonj_session.h"
#endif
#include "ext/standard/jonj_smart_str.h"
#include "jonj_ini.h"
#include "SAPI.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>

#ifndef JONJ_HAVE_STREAMS
# error You lose - must be compiled against JONJ 4.3.0 or later
#endif

#ifndef JONJ_WIN32
# define TRUE 1
# define FALSE 0
# define stricmp strcasecmp
#endif

extern int le_url;

typedef struct _encodeType encodeType, *encodeTypePtr;
typedef struct _encode encode, *encodePtr;

typedef struct _sdl sdl, *sdlPtr;
typedef struct _sdlRestrictionInt sdlRestrictionInt, *sdlRestrictionIntPtr;
typedef struct _sdlRestrictionChar sdlRestrictionChar, *sdlRestrictionCharPtr;
typedef struct _sdlRestrictions sdlRestrictions, *sdlRestrictionsPtr;
typedef struct _sdlType sdlType, *sdlTypePtr;
typedef struct _sdlParam sdlParam, *sdlParamPtr;
typedef struct _sdlFunction sdlFunction, *sdlFunctionPtr;
typedef struct _sdlAttribute sdlAttribute, *sdlAttributePtr;
typedef struct _sdlBinding sdlBinding, *sdlBindingPtr;
typedef struct _sdlSoapBinding sdlSoapBinding, *sdlSoapBindingPtr;
typedef struct _sdlSoapBindingFunction sdlSoapBindingFunction, *sdlSoapBindingFunctionPtr;
typedef struct _sdlSoapBindingFunctionBody sdlSoapBindingFunctionBody, *sdlSoapBindingFunctionBodyPtr;

typedef struct _soapMapping soapMapping, *soapMappingPtr;
typedef struct _soapService soapService, *soapServicePtr;

#include "jonj_xml.h"
#include "jonj_encoding.h"
#include "jonj_sdl.h"
#include "jonj_schema.h"
#include "jonj_http.h"
#include "jonj_packet_soap.h"

struct _soapMapping {
	zval *to_xml;
	zval *to_zval;
};

struct _soapHeader;

struct _soapService {
	sdlPtr sdl;

	struct _soap_functions {
		HashTable *ft;
		int functions_all;
	} soap_functions;

	struct _soap_class {
		zend_class_entry *ce;
		zval **argv;
		int argc;
		int persistance;
	} soap_class;

	zval *soap_object;

	HashTable *typemap;
	int        version;
	int        type;
	char      *actor;
	char      *uri;
	xmlCharEncodingHandlerPtr encoding;
	HashTable *class_map;
	int        features;
	struct _soapHeader **soap_headers_ptr;
	int send_errors;
};

#define SOAP_CLASS 1
#define SOAP_FUNCTIONS 2
#define SOAP_OBJECT 3
#define SOAP_FUNCTIONS_ALL 999

#define SOAP_MAP_FUNCTION 1
#define SOAP_MAP_CLASS 2

#define SOAP_PERSISTENCE_SESSION 1
#define SOAP_PERSISTENCE_REQUEST 2

#define SOAP_1_1 1
#define SOAP_1_2 2

#define SOAP_ACTOR_NEXT             1
#define SOAP_ACTOR_NONE             2
#define SOAP_ACTOR_UNLIMATERECEIVER 3

#define SOAP_1_1_ACTOR_NEXT             "http://schemas.xmlsoap.org/soap/actor/next"

#define SOAP_1_2_ACTOR_NEXT             "http://www.w3.org/2003/05/soap-envelope/role/next"
#define SOAP_1_2_ACTOR_NONE             "http://www.w3.org/2003/05/soap-envelope/role/none"
#define SOAP_1_2_ACTOR_UNLIMATERECEIVER "http://www.w3.org/2003/05/soap-envelope/role/ultimateReceiver"

#define SOAP_COMPRESSION_ACCEPT  0x20
#define SOAP_COMPRESSION_GZIP    0x00
#define SOAP_COMPRESSION_DEFLATE 0x10

#define SOAP_AUTHENTICATION_BASIC   0
#define SOAP_AUTHENTICATION_DIGEST  1

#define SOAP_SINGLE_ELEMENT_ARRAYS  (1<<0)
#define SOAP_WAIT_ONE_WAY_CALLS     (1<<1)
#define SOAP_USE_XSI_ARRAY_TYPE     (1<<2)

#define WSDL_CACHE_NONE     0x0
#define WSDL_CACHE_DISK     0x1
#define WSDL_CACHE_MEMORY   0x2
#define WSDL_CACHE_BOTH     0x3

/* New SOAP SSL Method Constants */
#define SOAP_SSL_METHOD_TLS     0
#define SOAP_SSL_METHOD_SSLv2   1
#define SOAP_SSL_METHOD_SSLv3   2
#define SOAP_SSL_METHOD_SSLv23  3


ZEND_BEGIN_MODULE_GLOBALS(soap)
	HashTable  defEncNs;     /* mapping of default namespaces to prefixes */
	HashTable  defEnc;
	HashTable  defEncIndex;
	HashTable *typemap;
	int        cur_uniq_ns;
	int        soap_version;
	sdlPtr     sdl;
	zend_bool  use_soap_error_handler;
	char*      error_code;
	zval*      error_object;
	char       cache;
	char       cache_mode;
	char       cache_enabled;
	char*      cache_dir;
	long       cache_ttl;
	long       cache_limit;
	HashTable *mem_cache;
	xmlCharEncodingHandlerPtr encoding;
	HashTable *class_map;
	int        features;
	HashTable  wsdl_cache;
	int        cur_uniq_ref;
	HashTable *ref_map;
ZEND_END_MODULE_GLOBALS(soap)

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry soap_module_entry;
#define soap_module_ptr &soap_module_entry
#define jonjext_soap_ptr soap_module_ptr

ZEND_EXTERN_MODULE_GLOBALS(soap)

#ifdef ZTS
# define SOAP_GLOBAL(v) TSRMG(soap_globals_id, zend_soap_globals *, v)
#else
# define SOAP_GLOBAL(v) (soap_globals.v)
#endif

extern zend_class_entry* soap_var_class_entry;

zval* add_soap_fault(zval *obj, char *fault_code, char *fault_string, char *fault_actor, zval *fault_detail TSRMLS_DC);

#define soap_error0(severity, format) \
	jonj_error(severity, "SOAP-ERROR: " format)

#define soap_error1(severity, format, param1) \
	jonj_error(severity, "SOAP-ERROR: " format, param1)

#define soap_error2(severity, format, param1, param2) \
	jonj_error(severity, "SOAP-ERROR: " format, param1, param2)

#define soap_error3(severity, format, param1, param2, param3) \
	jonj_error(severity, "SOAP-ERROR: " format, param1, param2, param3)

#endif
