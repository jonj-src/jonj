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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef HTML_H
#define HTML_H

#define ENT_HTML_QUOTE_NONE			0
#define ENT_HTML_QUOTE_SINGLE		1
#define ENT_HTML_QUOTE_DOUBLE		2
#define ENT_HTML_IGNORE_ERRORS		4
#define ENT_HTML_SUBSTITUTE_ERRORS	8
#define ENT_HTML_DOC_TYPE_MASK		(16|32)
#define ENT_HTML_DOC_HTML401		0
#define ENT_HTML_DOC_XML1			16
#define ENT_HTML_DOC_XHTML			32
#define ENT_HTML_DOC_HTML5			(16|32)
/* reserve bit 6 */
#define ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS	128


#define ENT_COMPAT		ENT_HTML_QUOTE_DOUBLE
#define ENT_QUOTES		(ENT_HTML_QUOTE_DOUBLE | ENT_HTML_QUOTE_SINGLE)
#define ENT_NOQUOTES	ENT_HTML_QUOTE_NONE
#define ENT_IGNORE		ENT_HTML_IGNORE_ERRORS
#define ENT_SUBSTITUTE	ENT_HTML_SUBSTITUTE_ERRORS
#define ENT_HTML401		0
#define ENT_XML1		16
#define ENT_XHTML		32
#define ENT_HTML5		(16|32)
#define ENT_DISALLOWED	128

void register_html_constants(INIT_FUNC_ARGS);

JONJ_FUNCTION(htmlspecialchars);
JONJ_FUNCTION(htmlentities);
JONJ_FUNCTION(htmlspecialchars_decode);
JONJ_FUNCTION(html_entity_decode);
JONJ_FUNCTION(get_html_translation_table);

JONJAPI char *jonj_escape_html_entities(unsigned char *old, size_t oldlen, size_t *newlen, int all, int flags, char *hint_charset TSRMLS_DC);
JONJAPI char *jonj_escape_html_entities_ex(unsigned char *old, size_t oldlen, size_t *newlen, int all, int flags, char *hint_charset, zend_bool double_encode TSRMLS_DC);
JONJAPI char *jonj_unescape_html_entities(unsigned char *old, size_t oldlen, size_t *newlen, int all, int flags, char *hint_charset TSRMLS_DC);
JONJAPI unsigned int jonj_next_utf8_char(const unsigned char *str, size_t str_len, size_t *cursor, int *status);

#endif /* HTML_H */