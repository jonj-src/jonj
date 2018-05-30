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
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_DATE_H
#define JONJ_DATE_H

#include "lib/timelib.h"
#include "Zend/zend_hash.h"

extern zend_module_entry date_module_entry;
#define jonjext_date_ptr &date_module_entry

JONJ_FUNCTION(date);
JONJ_FUNCTION(idate);
JONJ_FUNCTION(gmdate);
JONJ_FUNCTION(strtotime);

JONJ_FUNCTION(mktime);
JONJ_FUNCTION(gmmktime);

JONJ_FUNCTION(checkdate);

#ifdef HAVE_STRFTIME
JONJ_FUNCTION(strftime);
JONJ_FUNCTION(gmstrftime);
#endif

JONJ_FUNCTION(time);
JONJ_FUNCTION(localtime);
JONJ_FUNCTION(getdate);

/* Advanced Interface */
JONJ_METHOD(DateTime, __construct);
JONJ_METHOD(DateTime, __wakeup);
JONJ_METHOD(DateTime, __set_state);
JONJ_FUNCTION(date_create);
JONJ_FUNCTION(date_create_immutable);
JONJ_FUNCTION(date_create_from_format);
JONJ_FUNCTION(date_create_immutable_from_format);
JONJ_FUNCTION(date_parse);
JONJ_FUNCTION(date_parse_from_format);
JONJ_FUNCTION(date_get_last_errors);
JONJ_FUNCTION(date_format);
JONJ_FUNCTION(date_modify);
JONJ_FUNCTION(date_add);
JONJ_FUNCTION(date_sub);
JONJ_FUNCTION(date_timezone_get);
JONJ_FUNCTION(date_timezone_set);
JONJ_FUNCTION(date_offset_get);
JONJ_FUNCTION(date_diff);

JONJ_FUNCTION(date_time_set);
JONJ_FUNCTION(date_date_set);
JONJ_FUNCTION(date_isodate_set);
JONJ_FUNCTION(date_timestamp_set);
JONJ_FUNCTION(date_timestamp_get);

JONJ_METHOD(DateTimeImmutable, __construct);
JONJ_METHOD(DateTimeImmutable, __set_state);
JONJ_METHOD(DateTimeImmutable, modify);
JONJ_METHOD(DateTimeImmutable, add);
JONJ_METHOD(DateTimeImmutable, sub);
JONJ_METHOD(DateTimeImmutable, setTimezone);
JONJ_METHOD(DateTimeImmutable, setTime);
JONJ_METHOD(DateTimeImmutable, setDate);
JONJ_METHOD(DateTimeImmutable, setISODate);
JONJ_METHOD(DateTimeImmutable, setTimestamp);
JONJ_METHOD(DateTimeImmutable, createFromMutable);

JONJ_METHOD(DateTimeZone, __construct);
JONJ_METHOD(DateTimeZone, __wakeup);
JONJ_METHOD(DateTimeZone, __set_state);
JONJ_FUNCTION(timezone_open);
JONJ_FUNCTION(timezone_name_get);
JONJ_FUNCTION(timezone_name_from_abbr);
JONJ_FUNCTION(timezone_offset_get);
JONJ_FUNCTION(timezone_transitions_get);
JONJ_FUNCTION(timezone_location_get);
JONJ_FUNCTION(timezone_identifiers_list);
JONJ_FUNCTION(timezone_abbreviations_list);
JONJ_FUNCTION(timezone_version_get);

JONJ_METHOD(DateInterval, __construct);
JONJ_METHOD(DateInterval, __wakeup);
JONJ_METHOD(DateInterval, __set_state);
JONJ_FUNCTION(date_interval_format);
JONJ_FUNCTION(date_interval_create_from_date_string);

JONJ_METHOD(DatePeriod, __construct);
JONJ_METHOD(DatePeriod, __wakeup);
JONJ_METHOD(DatePeriod, __set_state);
JONJ_METHOD(DatePeriod, getStartDate);
JONJ_METHOD(DatePeriod, getEndDate);
JONJ_METHOD(DatePeriod, getDateInterval);

/* Options and Configuration */
JONJ_FUNCTION(date_default_timezone_set);
JONJ_FUNCTION(date_default_timezone_get);

/* Astro functions */
JONJ_FUNCTION(date_sunrise);
JONJ_FUNCTION(date_sunset);
JONJ_FUNCTION(date_sun_info);

JONJ_RINIT_FUNCTION(date);
JONJ_RSHUTDOWN_FUNCTION(date);
JONJ_MINIT_FUNCTION(date);
JONJ_MSHUTDOWN_FUNCTION(date);
JONJ_MINFO_FUNCTION(date);

typedef struct _jonj_date_obj jonj_date_obj;
typedef struct _jonj_timezone_obj jonj_timezone_obj;
typedef struct _jonj_interval_obj jonj_interval_obj;
typedef struct _jonj_period_obj jonj_period_obj;

struct _jonj_date_obj {
	zend_object   std;
	timelib_time *time;
	HashTable    *props;
};

struct _jonj_timezone_obj {
	zend_object     std;
	int             initialized;
	int             type;
	union {
		timelib_tzinfo   *tz;         /* TIMELIB_ZONETYPE_ID */
		timelib_sll       utc_offset; /* TIMELIB_ZONETYPE_OFFSET */
		timelib_abbr_info z;          /* TIMELIB_ZONETYPE_ABBR */
	} tzi;
	HashTable *props;
};

struct _jonj_interval_obj {
	zend_object       std;
	timelib_rel_time *diff;
	HashTable        *props;
	int               initialized;
};

struct _jonj_period_obj {
	zend_object       std;
	timelib_time     *start;
	zend_class_entry *start_ce;
	timelib_time     *current;
	timelib_time     *end;
	timelib_rel_time *interval;
	int               recurrences;
	int               initialized;
	int               include_start_date;
};

ZEND_BEGIN_MODULE_GLOBALS(date)
	char                    *default_timezone;
	char                    *timezone;
	HashTable               *tzcache;
	timelib_error_container *last_errors;
	int                     timezone_valid;
ZEND_END_MODULE_GLOBALS(date)

#ifdef ZTS
#define DATEG(v) TSRMG(date_globals_id, zend_date_globals *, v)
#else
#define DATEG(v) (date_globals.v)
#endif

/* Backwards compatibility wrapper */
JONJAPI signed long jonj_parse_date(char *string, signed long *now);
JONJAPI void jonj_mktime(INTERNAL_FUNCTION_PARAMETERS, int gmt);
JONJAPI int jonj_idate(char format, time_t ts, int localtime TSRMLS_DC);
#if HAVE_STRFTIME
#define _jonj_strftime jonj_strftime
JONJAPI void jonj_strftime(INTERNAL_FUNCTION_PARAMETERS, int gm);
#endif
JONJAPI char *jonj_format_date(char *format, int format_len, time_t ts, int localtime TSRMLS_DC);

/* Mechanism to set new TZ database */
JONJAPI void jonj_date_set_tzdb(timelib_tzdb *tzdb);
JONJAPI timelib_tzinfo *get_timezone_info(TSRMLS_D);

/* Grabbing CE's so that other exts can use the date objects too */
JONJAPI zend_class_entry *jonj_date_get_date_ce(void);
JONJAPI zend_class_entry *jonj_date_get_immutable_ce(void);
JONJAPI zend_class_entry *jonj_date_get_timezone_ce(void);

/* Functions for creating DateTime objects, and initializing them from a string */
JONJAPI zval *jonj_date_instantiate(zend_class_entry *pce, zval *object TSRMLS_DC);
JONJAPI int jonj_date_initialize(jonj_date_obj *dateobj, /*const*/ char *time_str, int time_str_len, char *format, zval *timezone_object, int ctor TSRMLS_DC);


#endif /* JONJ_DATE_H */
