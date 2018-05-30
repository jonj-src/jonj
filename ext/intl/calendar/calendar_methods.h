/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@netcabo.pt>                       |
   +----------------------------------------------------------------------+
 */

#ifndef CALENDAR_METHODS_H
#define CALENDAR_METHODS_H

#include <jonj.h>

JONJ_METHOD(IntlCalendar, __construct);

JONJ_FUNCTION(intlcal_create_instance);

JONJ_FUNCTION(intlcal_get_keyword_values_for_locale);

JONJ_FUNCTION(intlcal_get_now);

JONJ_FUNCTION(intlcal_get_available_locales);

JONJ_FUNCTION(intlcal_get);

JONJ_FUNCTION(intlcal_get_time);

JONJ_FUNCTION(intlcal_set_time);

JONJ_FUNCTION(intlcal_add);

JONJ_FUNCTION(intlcal_set_time_zone);

JONJ_FUNCTION(intlcal_after);

JONJ_FUNCTION(intlcal_before);

JONJ_FUNCTION(intlcal_set);

JONJ_FUNCTION(intlcal_roll);

JONJ_FUNCTION(intlcal_clear);

JONJ_FUNCTION(intlcal_field_difference);

JONJ_FUNCTION(intlcal_get_actual_maximum);

JONJ_FUNCTION(intlcal_get_actual_minimum);

JONJ_FUNCTION(intlcal_get_day_of_week_type);

JONJ_FUNCTION(intlcal_get_first_day_of_week);

JONJ_FUNCTION(intlcal_get_greatest_minimum);

JONJ_FUNCTION(intlcal_get_least_maximum);

JONJ_FUNCTION(intlcal_get_locale);

JONJ_FUNCTION(intlcal_get_maximum);

JONJ_FUNCTION(intlcal_get_minimal_days_in_first_week);

JONJ_FUNCTION(intlcal_get_minimum);

JONJ_FUNCTION(intlcal_get_time_zone);

JONJ_FUNCTION(intlcal_get_type);

JONJ_FUNCTION(intlcal_get_weekend_transition);

JONJ_FUNCTION(intlcal_in_daylight_time);

JONJ_FUNCTION(intlcal_is_equivalent_to);

JONJ_FUNCTION(intlcal_is_lenient);

JONJ_FUNCTION(intlcal_is_set);

JONJ_FUNCTION(intlcal_is_weekend);

JONJ_FUNCTION(intlcal_set_first_day_of_week);

JONJ_FUNCTION(intlcal_set_lenient);

JONJ_FUNCTION(intlcal_set_minimal_days_in_first_week);

JONJ_FUNCTION(intlcal_equals);

JONJ_FUNCTION(intlcal_get_repeated_wall_time_option);

JONJ_FUNCTION(intlcal_get_skipped_wall_time_option);

JONJ_FUNCTION(intlcal_set_repeated_wall_time_option);

JONJ_FUNCTION(intlcal_set_skipped_wall_time_option);

JONJ_FUNCTION(intlcal_from_date_time);

JONJ_FUNCTION(intlcal_to_date_time);

JONJ_FUNCTION(intlcal_get_error_code);

JONJ_FUNCTION(intlcal_get_error_message);

#endif /* #ifndef CALENDAR_METHODS_H */
