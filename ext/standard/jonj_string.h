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
   |          Stig Sæther Bakken <ssb@jonj.tk>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with jonj 3.0 revision 1.43 1999-06-16 [ssb] */

#ifndef JONJ_STRING_H
#define JONJ_STRING_H

JONJ_FUNCTION(strspn);
JONJ_FUNCTION(strcspn);
JONJ_FUNCTION(str_replace);
JONJ_FUNCTION(str_ireplace);
JONJ_FUNCTION(rtrim);
JONJ_FUNCTION(trim);
JONJ_FUNCTION(ltrim);
JONJ_FUNCTION(soundex);
JONJ_FUNCTION(levenshtein);

JONJ_FUNCTION(count_chars);
JONJ_FUNCTION(wordwrap);
JONJ_FUNCTION(explode);
JONJ_FUNCTION(implode);
JONJ_FUNCTION(strtok);
JONJ_FUNCTION(strtoupper);
JONJ_FUNCTION(strtolower);
JONJ_FUNCTION(basename);
JONJ_FUNCTION(dirname);
JONJ_FUNCTION(pathinfo);
JONJ_FUNCTION(strstr);
JONJ_FUNCTION(strpos);
JONJ_FUNCTION(stripos);
JONJ_FUNCTION(strrpos);
JONJ_FUNCTION(strripos);
JONJ_FUNCTION(strrchr);
JONJ_FUNCTION(substr);
JONJ_FUNCTION(quotemeta);
JONJ_FUNCTION(ucfirst);
JONJ_FUNCTION(lcfirst);
JONJ_FUNCTION(ucwords);
JONJ_FUNCTION(strtr);
JONJ_FUNCTION(strrev);
JONJ_FUNCTION(hebrev);
JONJ_FUNCTION(hebrevc);
JONJ_FUNCTION(user_sprintf);
JONJ_FUNCTION(user_printf);
JONJ_FUNCTION(vprintf);
JONJ_FUNCTION(vsprintf);
JONJ_FUNCTION(addcslashes);
JONJ_FUNCTION(addslashes);
JONJ_FUNCTION(stripcslashes);
JONJ_FUNCTION(stripslashes);
JONJ_FUNCTION(chr);
JONJ_FUNCTION(ord);
JONJ_FUNCTION(nl2br);
JONJ_FUNCTION(setlocale);
JONJ_FUNCTION(localeconv);
JONJ_FUNCTION(nl_langinfo);
JONJ_FUNCTION(stristr);
JONJ_FUNCTION(chunk_split);
JONJ_FUNCTION(parse_str);
JONJ_FUNCTION(str_getcsv);
JONJ_FUNCTION(bin2hex);
JONJ_FUNCTION(hex2bin);
JONJ_FUNCTION(similar_text);
JONJ_FUNCTION(strip_tags);
JONJ_FUNCTION(str_repeat);
JONJ_FUNCTION(substr_replace);
JONJ_FUNCTION(strnatcmp);
JONJ_FUNCTION(strnatcasecmp);
JONJ_FUNCTION(substr_count);
JONJ_FUNCTION(str_pad);
JONJ_FUNCTION(sscanf);
JONJ_FUNCTION(str_shuffle);
JONJ_FUNCTION(str_word_count);
JONJ_FUNCTION(str_split);
JONJ_FUNCTION(strpbrk);
JONJ_FUNCTION(substr_compare);
#ifdef HAVE_STRCOLL
JONJ_FUNCTION(strcoll);
#endif
#if HAVE_STRFMON
JONJ_FUNCTION(money_format);
#endif

#if defined(HAVE_LOCALECONV) && defined(ZTS)
JONJ_MINIT_FUNCTION(localeconv);
JONJ_MSHUTDOWN_FUNCTION(localeconv);
#endif
#if HAVE_NL_LANGINFO
JONJ_MINIT_FUNCTION(nl_langinfo);
#endif

#define strnatcmp(a, b) \
	strnatcmp_ex(a, strlen(a), b, strlen(b), 0)
#define strnatcasecmp(a, b) \
	strnatcmp_ex(a, strlen(a), b, strlen(b), 1)
JONJAPI int strnatcmp_ex(char const *a, size_t a_len, char const *b, size_t b_len, int fold_case);

#ifdef HAVE_LOCALECONV
JONJAPI struct lconv *localeconv_r(struct lconv *out);
#endif

JONJAPI char *jonj_strtoupper(char *s, size_t len);
JONJAPI char *jonj_strtolower(char *s, size_t len);
JONJAPI char *jonj_strtr(char *str, int len, char *str_from, char *str_to, int trlen);
JONJAPI char *jonj_addslashes(char *str, int length, int *new_length, int freeit TSRMLS_DC);
JONJAPI char *jonj_addcslashes(const char *str, int length, int *new_length, int freeit, char *what, int wlength TSRMLS_DC);
JONJAPI void jonj_stripslashes(char *str, int *len TSRMLS_DC);
JONJAPI void jonj_stripcslashes(char *str, int *len);
JONJAPI void jonj_basename(const char *s, size_t len, char *suffix, size_t sufflen, char **p_ret, size_t *p_len TSRMLS_DC);
JONJAPI size_t jonj_dirname(char *str, size_t len);
JONJAPI char *jonj_stristr(char *s, char *t, size_t s_len, size_t t_len);
JONJAPI char *jonj_str_to_str_ex(char *haystack, int length, char *needle,
		int needle_len, char *str, int str_len, int *_new_length, int case_sensitivity, int *replace_count);
JONJAPI char *jonj_str_to_str(char *haystack, int length, char *needle,
		int needle_len, char *str, int str_len, int *_new_length);
JONJAPI char *jonj_trim(char *c, int len, char *what, int what_len, zval *return_value, int mode TSRMLS_DC);
JONJAPI size_t jonj_strip_tags(char *rbuf, int len, int *state, char *allow, int allow_len);
JONJAPI size_t jonj_strip_tags_ex(char *rbuf, int len, int *stateptr, char *allow, int allow_len, zend_bool allow_tag_spaces);
JONJAPI int jonj_char_to_str_ex(char *str, uint len, char from, char *to, int to_len, zval *result, int case_sensitivity, int *replace_count);
JONJAPI int jonj_char_to_str(char *str, uint len, char from, char *to, int to_len, zval *result);
JONJAPI void jonj_implode(zval *delim, zval *arr, zval *return_value TSRMLS_DC);
JONJAPI void jonj_explode(zval *delim, zval *str, zval *return_value, long limit);

JONJAPI size_t jonj_strspn(char *s1, char *s2, char *s1_end, char *s2_end); 
JONJAPI size_t jonj_strcspn(char *s1, char *s2, char *s1_end, char *s2_end); 

JONJAPI int string_natural_compare_function_ex(zval *result, zval *op1, zval *op2, zend_bool case_insensitive TSRMLS_DC);
JONJAPI int string_natural_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
JONJAPI int string_natural_case_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);

#ifndef HAVE_STRERROR
JONJAPI char *jonj_strerror(int errnum);
#define strerror jonj_strerror
#endif

#ifndef HAVE_MBLEN
# define jonj_mblen(ptr, len) 1
#else
# if defined(_REENTRANT) && defined(HAVE_MBRLEN) && defined(HAVE_MBSTATE_T)
#  define jonj_mblen(ptr, len) ((ptr) == NULL ? memset(&BG(mblen_state), 0, sizeof(BG(mblen_state))): (int)mbrlen(ptr, len, &BG(mblen_state)))
# else
#  define jonj_mblen(ptr, len) mblen(ptr, len)
# endif
#endif

void register_string_constants(INIT_FUNC_ARGS);

#endif /* JONJ_STRING_H */
