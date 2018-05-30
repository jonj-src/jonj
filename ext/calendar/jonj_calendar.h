#ifndef JONJ_CALENDAR_H
#define JONJ_CALENDAR_H

extern zend_module_entry calendar_module_entry;
#define	calendar_module_ptr &calendar_module_entry

/* Functions */

JONJ_MINIT_FUNCTION(calendar);
JONJ_MINFO_FUNCTION(calendar);

JONJ_FUNCTION(jdtogregorian);
JONJ_FUNCTION(gregoriantojd);
JONJ_FUNCTION(jdtojulian);
JONJ_FUNCTION(juliantojd);
JONJ_FUNCTION(jdtojewish);
JONJ_FUNCTION(jewishtojd);
JONJ_FUNCTION(jdtofrench);
JONJ_FUNCTION(frenchtojd);
JONJ_FUNCTION(jddayofweek);
JONJ_FUNCTION(jdmonthname);
JONJ_FUNCTION(easter_days);
JONJ_FUNCTION(easter_date);
JONJ_FUNCTION(unixtojd);
JONJ_FUNCTION(jdtounix);
JONJ_FUNCTION(cal_from_jd);
JONJ_FUNCTION(cal_to_jd);
JONJ_FUNCTION(cal_days_in_month);
JONJ_FUNCTION(cal_info);

#define jonjext_calendar_ptr calendar_module_ptr

/*
 * Specifying the easter calculation method
 * 
 * DEFAULT is Anglican, ie. use Julian calendar before 1753
 * and Gregorian after that. With ROMAN, the cutoff year is 1582.
 * ALWAYS_GREGORIAN and ALWAYS_JULIAN force the calendar
 * regardless of date.
 *
 */
 
#define CAL_EASTER_DEFAULT			0
#define CAL_EASTER_ROMAN			1
#define CAL_EASTER_ALWAYS_GREGORIAN	2
#define CAL_EASTER_ALWAYS_JULIAN	3

#endif
