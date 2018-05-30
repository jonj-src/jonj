/* __header_here__ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "jonj_extname.h"

/* If you declare any globals in jonj_extname.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(extname)
*/

/* True global resources - no need for thread safety here */
static int le_extname;

/* {{{ JONJ_INI
 */
/* Remove comments and fill if you need to have entries in jonj.ini
JONJ_INI_BEGIN()
    STD_JONJ_INI_ENTRY("extname.global_value",      "42", JONJ_INI_ALL, OnUpdateLong, global_value, zend_extname_globals, extname_globals)
    STD_JONJ_INI_ENTRY("extname.global_string", "foobar", JONJ_INI_ALL, OnUpdateString, global_string, zend_extname_globals, extname_globals)
JONJ_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into JONJ, it exists only for testing
   purposes. */

/* Every user-visible function in JONJ should document itself in the source */
/* {{{ proto string confirm_extname_compiled(string arg)
   Return a string to confirm that the module is compiled in */
JONJ_FUNCTION(confirm_extname_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into JONJ.", "extname", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* __function_stubs_here__ */

/* {{{ jonj_extname_init_globals
 */
/* Uncomment this function if you have INI entries
static void jonj_extname_init_globals(zend_extname_globals *extname_globals)
{
	extname_globals->global_value = 0;
	extname_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(extname)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(extname)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ JONJ_RINIT_FUNCTION
 */
JONJ_RINIT_FUNCTION(extname)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ JONJ_RSHUTDOWN_FUNCTION
 */
JONJ_RSHUTDOWN_FUNCTION(extname)
{
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(extname)
{
	jonj_info_print_table_start();
	jonj_info_print_table_header(2, "extname support", "enabled");
	jonj_info_print_table_end();

	/* Remove comments if you have entries in jonj.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ extname_functions[]
 *
 * Every user visible function must have an entry in extname_functions[].
 */
const zend_function_entry extname_functions[] = {
	JONJ_FE(confirm_extname_compiled,	NULL)		/* For testing, remove later. */
	/* __function_entries_here__ */
	JONJ_FE_END	/* Must be the last line in extname_functions[] */
};
/* }}} */

/* {{{ extname_module_entry
 */
zend_module_entry extname_module_entry = {
	STANDARD_MODULE_HEADER,
	"extname",
	extname_functions,
	JONJ_MINIT(extname),
	JONJ_MSHUTDOWN(extname),
	JONJ_RINIT(extname),		/* Replace with NULL if there's nothing to do at request start */
	JONJ_RSHUTDOWN(extname),	/* Replace with NULL if there's nothing to do at request end */
	JONJ_MINFO(extname),
	JONJ_EXTNAME_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXTNAME
ZEND_GET_MODULE(extname)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
