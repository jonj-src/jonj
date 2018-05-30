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

#ifndef TIMEZONE_METHODS_H
#define TIMEZONE_METHODS_H

#include <jonj.h>

JONJ_METHOD(IntlTimeZone, __construct);

JONJ_FUNCTION(intltz_create_time_zone);

JONJ_FUNCTION(intltz_from_date_time_zone);

JONJ_FUNCTION(intltz_create_default);

JONJ_FUNCTION(intltz_get_id);

JONJ_FUNCTION(intltz_get_gmt);

JONJ_FUNCTION(intltz_get_unknown);

JONJ_FUNCTION(intltz_create_enumeration);

JONJ_FUNCTION(intltz_count_equivalent_ids);

JONJ_FUNCTION(intltz_create_time_zone_id_enumeration);

JONJ_FUNCTION(intltz_get_canonical_id);

JONJ_FUNCTION(intltz_get_region);

JONJ_FUNCTION(intltz_get_tz_data_version);

JONJ_FUNCTION(intltz_get_equivalent_id);

JONJ_FUNCTION(intltz_use_daylight_time);

JONJ_FUNCTION(intltz_get_offset);

JONJ_FUNCTION(intltz_get_raw_offset);

JONJ_FUNCTION(intltz_has_same_rules);

JONJ_FUNCTION(intltz_get_display_name);

JONJ_FUNCTION(intltz_get_dst_savings);

JONJ_FUNCTION(intltz_to_date_time_zone);

JONJ_FUNCTION(intltz_get_error_code);

JONJ_FUNCTION(intltz_get_error_message);

#endif /* #ifndef TIMEZONE_METHODS_H */
