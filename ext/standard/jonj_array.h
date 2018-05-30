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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@jonj.tk>                             |
   |          Andrei Zmievski <andrei@jonj.tk>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_ARRAY_H
#define JONJ_ARRAY_H

JONJ_MINIT_FUNCTION(array);
JONJ_MSHUTDOWN_FUNCTION(array);

JONJ_FUNCTION(ksort);
JONJ_FUNCTION(krsort);
JONJ_FUNCTION(natsort);
JONJ_FUNCTION(natcasesort);
JONJ_FUNCTION(asort);
JONJ_FUNCTION(arsort);
JONJ_FUNCTION(sort);
JONJ_FUNCTION(rsort);
JONJ_FUNCTION(usort);
JONJ_FUNCTION(uasort);
JONJ_FUNCTION(uksort);
JONJ_FUNCTION(array_walk);
JONJ_FUNCTION(array_walk_recursive);
JONJ_FUNCTION(count);
JONJ_FUNCTION(end);
JONJ_FUNCTION(prev);
JONJ_FUNCTION(next);
JONJ_FUNCTION(reset);
JONJ_FUNCTION(current);
JONJ_FUNCTION(key);
JONJ_FUNCTION(min);
JONJ_FUNCTION(max);
JONJ_FUNCTION(in_array);
JONJ_FUNCTION(array_search);
JONJ_FUNCTION(extract);
JONJ_FUNCTION(compact);
JONJ_FUNCTION(array_fill);
JONJ_FUNCTION(array_fill_keys);
JONJ_FUNCTION(range);
JONJ_FUNCTION(shuffle);
JONJ_FUNCTION(array_multisort);
JONJ_FUNCTION(array_push);
JONJ_FUNCTION(array_pop);
JONJ_FUNCTION(array_shift);
JONJ_FUNCTION(array_unshift);
JONJ_FUNCTION(array_splice);
JONJ_FUNCTION(array_slice);
JONJ_FUNCTION(array_merge);
JONJ_FUNCTION(array_merge_recursive);
JONJ_FUNCTION(array_replace);
JONJ_FUNCTION(array_replace_recursive);
JONJ_FUNCTION(array_keys);
JONJ_FUNCTION(array_values);
JONJ_FUNCTION(array_count_values);
JONJ_FUNCTION(array_column);
JONJ_FUNCTION(array_reverse);
JONJ_FUNCTION(array_reduce);
JONJ_FUNCTION(array_pad);
JONJ_FUNCTION(array_flip);
JONJ_FUNCTION(array_change_key_case);
JONJ_FUNCTION(array_rand);
JONJ_FUNCTION(array_unique);
JONJ_FUNCTION(array_intersect);
JONJ_FUNCTION(array_intersect_key);
JONJ_FUNCTION(array_intersect_ukey);
JONJ_FUNCTION(array_uintersect);
JONJ_FUNCTION(array_intersect_assoc);
JONJ_FUNCTION(array_uintersect_assoc);
JONJ_FUNCTION(array_intersect_uassoc);
JONJ_FUNCTION(array_uintersect_uassoc);
JONJ_FUNCTION(array_diff);
JONJ_FUNCTION(array_diff_key);
JONJ_FUNCTION(array_diff_ukey);
JONJ_FUNCTION(array_udiff);
JONJ_FUNCTION(array_diff_assoc);
JONJ_FUNCTION(array_udiff_assoc);
JONJ_FUNCTION(array_diff_uassoc);
JONJ_FUNCTION(array_udiff_uassoc);
JONJ_FUNCTION(array_sum);
JONJ_FUNCTION(array_product);
JONJ_FUNCTION(array_filter);
JONJ_FUNCTION(array_map);
JONJ_FUNCTION(array_key_exists);
JONJ_FUNCTION(array_chunk);
JONJ_FUNCTION(array_combine);

JONJAPI void jonj_splice(HashTable *ht, zend_uint offset, zend_uint length, zval ***list, zend_uint list_count, HashTable *removed TSRMLS_DC);
JONJAPI int jonj_array_merge(HashTable *dest, HashTable *src, int recursive TSRMLS_DC);
JONJAPI int jonj_array_replace_recursive(HashTable *dest, HashTable *src TSRMLS_DC);
JONJAPI int jonj_multisort_compare(const void *a, const void *b TSRMLS_DC);
JONJAPI int jonj_count_recursive(zval *array, long mode TSRMLS_DC);

#define JONJ_SORT_REGULAR            0
#define JONJ_SORT_NUMERIC            1
#define JONJ_SORT_STRING             2
#define JONJ_SORT_DESC               3
#define JONJ_SORT_ASC                4
#define JONJ_SORT_LOCALE_STRING      5
#define JONJ_SORT_NATURAL            6
#define JONJ_SORT_FLAG_CASE          8

#define COUNT_NORMAL      0
#define COUNT_RECURSIVE   1

#define ARRAY_FILTER_USE_BOTH	1
#define ARRAY_FILTER_USE_KEY	2

ZEND_BEGIN_MODULE_GLOBALS(array) 
	int *multisort_flags[2];
	int (*compare_func)(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_END_MODULE_GLOBALS(array) 

#ifdef ZTS
#define ARRAYG(v) TSRMG(array_globals_id, zend_array_globals *, v)
#else
#define ARRAYG(v) (array_globals.v)
#endif

#endif /* JONJ_ARRAY_H */
