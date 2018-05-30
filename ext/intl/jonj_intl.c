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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Kirti Velankar <kirtig@yahoo-inc.com>   			  |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "jonj_intl.h"
#include "intl_error.h"
#include "collator/collator_class.h"
#include "collator/collator.h"
#include "collator/collator_attr.h"
#include "collator/collator_compare.h"
#include "collator/collator_sort.h"
#include "collator/collator_convert.h"
#include "collator/collator_locale.h"
#include "collator/collator_create.h"
#include "collator/collator_error.h"

#include "converter/converter.h"

#include "formatter/formatter.h"
#include "formatter/formatter_class.h"
#include "formatter/formatter_attr.h"
#include "formatter/formatter_format.h"
#include "formatter/formatter_main.h"
#include "formatter/formatter_parse.h"

#include "grapheme/grapheme.h"

#include "msgformat/msgformat.h"
#include "msgformat/msgformat_class.h"
#include "msgformat/msgformat_attr.h"
#include "msgformat/msgformat_format.h"
#include "msgformat/msgformat_parse.h"

#include "normalizer/normalizer.h"
#include "normalizer/normalizer_class.h"
#include "normalizer/normalizer_normalize.h"

#include "locale/locale.h"
#include "locale/locale_class.h"
#include "locale/locale_methods.h"

#include "dateformat/dateformat.h"
#include "dateformat/dateformat_class.h"
#include "dateformat/dateformat_attr.h"
#include "dateformat/dateformat_attrcpp.h"
#include "dateformat/dateformat_format.h"
#include "dateformat/dateformat_format_object.h"
#include "dateformat/dateformat_parse.h"
#include "dateformat/dateformat_data.h"

#include "resourcebundle/resourcebundle_class.h"

#include "transliterator/transliterator.h"
#include "transliterator/transliterator_class.h"
#include "transliterator/transliterator_methods.h"

#include "timezone/timezone_class.h"
#include "timezone/timezone_methods.h"

#include "calendar/calendar_class.h"
#include "calendar/calendar_methods.h"
#include "calendar/gregoriancalendar_methods.h"

#include "breakiterator/breakiterator_class.h"
#include "breakiterator/breakiterator_iterators.h"

#include "idn/idn.h"

#if U_ICU_VERSION_MAJOR_NUM * 1000 + U_ICU_VERSION_MINOR_NUM >= 4002
# include "spoofchecker/spoofchecker_class.h"
# include "spoofchecker/spoofchecker.h"
# include "spoofchecker/spoofchecker_create.h"
# include "spoofchecker/spoofchecker_main.h"
#endif

#include "msgformat/msgformat.h"
#include "common/common_error.h"
#include "common/common_enum.h"

#include <unicode/uloc.h>
#include <unicode/uclean.h>
#include <ext/standard/info.h>

#include "jonj_ini.h"
#define INTL_MODULE_VERSION JONJ_INTL_VERSION

/*
 * locale_get_default has a conflict since ICU also has
 * a function with the same  name
 * in fact ICU appends the version no. to it also
 * Hence the following undef for ICU version
 * Same true for the locale_set_default function
*/
#undef locale_get_default
#undef locale_set_default

ZEND_DECLARE_MODULE_GLOBALS( intl )

const char *intl_locale_get_default( TSRMLS_D )
{
	if( INTL_G(default_locale) == NULL ) {
		return uloc_getDefault();
	}
	return INTL_G(default_locale);
}

/* {{{ Arguments info */
ZEND_BEGIN_ARG_INFO_EX(collator_static_0_args, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_static_1_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_static_2_args, 0, 0, 2)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_0_args, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_1_arg, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_2_args, 0, 0, 3)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(collator_sort_args, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, object, Collator, 0)
	ZEND_ARG_ARRAY_INFO(1, arr, 0)
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(numfmt_parse_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, formatter)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(numfmt_parse_currency_arginfo, 0, 0, 3)
	ZEND_ARG_INFO(0, formatter)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, currency)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_0_args, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_1_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_2_args, 0, 0, 2)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_3_args, 0, 0, 3)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(locale_4_args, 0, 0, 4)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
	ZEND_ARG_INFO(0, arg4)
ZEND_END_ARG_INFO()

#define intl_0_args collator_static_0_args
#define intl_1_arg collator_static_1_arg

ZEND_BEGIN_ARG_INFO_EX(normalizer_args, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, form)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_1_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_search_args, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_substr_args, 0, 0, 2)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_strstr_args, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, before_needle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(grapheme_extract_args, 0, 0, 2)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, arg3)
	ZEND_ARG_INFO(0, arg4)
	ZEND_ARG_INFO(1, arg5)  /* 1 = pass by reference */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(datefmt_parse_args, 0, 0, 2)
	ZEND_ARG_INFO(0, formatter)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_create, 0, 0, 2)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, style)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_get_error_code, 0, 0, 1)
	ZEND_ARG_INFO(0, nf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_format, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_format_currency, 0, 0, 3)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, currency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_get_attribute, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, attr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_set_attribute, 0, 0, 3)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, attr)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_set_symbol, 0, 0, 3)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, attr)
	ZEND_ARG_INFO(0, symbol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_set_pattern, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_get_locale, 0, 0, 1)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_create, 0, 0, 2)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_get_error_code, 0, 0, 1)
	ZEND_ARG_INFO(0, nf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_get_error_message, 0, 0, 1)
	ZEND_ARG_INFO(0, coll)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_format, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_format_message, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, nf)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_numfmt_parse_message, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_set_pattern, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_msgfmt_get_locale, 0, 0, 1)
	ZEND_ARG_INFO(0, mf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_set_pattern, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_set_timezone, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_set_calendar, 0, 0, 2)
	ZEND_ARG_INFO(0, mf)
	ZEND_ARG_INFO(0, calendar)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_format, 0, 0, 0)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_format_object, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, locale)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_datefmt_create, 0, 0, 3)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, date_type)
	ZEND_ARG_INFO(0, time_type)
	ZEND_ARG_INFO(0, timezone_str)
	ZEND_ARG_INFO(0, calendar)
	ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_idn_to_ascii, 0, 0, 1)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, variant)
	ZEND_ARG_INFO(1, idn_info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_idn_to_utf8, 0, 0, 1)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, variant)
	ZEND_ARG_INFO(1, idn_info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_create_proc, 0, 0, 2 )
	ZEND_ARG_INFO( 0, locale )
	ZEND_ARG_INFO( 0, bundlename )
	ZEND_ARG_INFO( 0, fallback )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_get_proc, 0, 0, 2 )
    ZEND_ARG_INFO( 0, bundle )
	ZEND_ARG_INFO( 0, index )
	ZEND_ARG_INFO( 0, fallback )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_count_proc, 0, 0, 1 )
  ZEND_ARG_INFO( 0, bundle )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_locales_proc, 0, 0, 1 )
	ZEND_ARG_INFO( 0, bundlename )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_get_error_code_proc, 0, 0, 1 )
  ZEND_ARG_INFO( 0, bundle )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_resourcebundle_get_error_message_proc, 0, 0, 1 )
  ZEND_ARG_INFO( 0, bundle )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_transliterator_void, 0, 0, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_transliterator_create, 0, 0, 1 )
	ZEND_ARG_INFO( 0, id )
	ZEND_ARG_INFO( 0, direction )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_transliterator_create_from_rules, 0, 0, 1 )
	ZEND_ARG_INFO( 0, rules )
	ZEND_ARG_INFO( 0, direction )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_transliterator_create_inverse, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, orig_trans, Transliterator, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_transliterator_transliterate, 0, 0, 2 )
	ZEND_ARG_INFO( 0, trans )
	ZEND_ARG_INFO( 0, subject )
	ZEND_ARG_INFO( 0, start )
	ZEND_ARG_INFO( 0, end )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_transliterator_error, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, trans, Transliterator, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_idarg_static, 0, 0, 1 )
	ZEND_ARG_INFO( 0, zoneId )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_from_date_time_zone, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, dateTimeZone, DateTimeZone, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_create_enumeration, 0, 0, 0 )
	ZEND_ARG_INFO( 0, countryOrRawOffset )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_count_equivalent_ids, 0, 0, 1 )
	ZEND_ARG_INFO( 0, zoneId )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_create_time_zone_id_enumeration, 0, 0, 1 )
	ZEND_ARG_INFO( 0, zoneType )
	ZEND_ARG_INFO( 0, region )
	ZEND_ARG_INFO( 0, rawOffset )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_get_canonical_id, 0, 0, 1 )
	ZEND_ARG_INFO( 0, zoneId )
	ZEND_ARG_INFO( 1, isSystemID )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_get_equivalent_id, 0, 0, 2 )
	ZEND_ARG_INFO( 0, zoneId )
	ZEND_ARG_INFO( 0, index )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_get_offset, 0, 0, 5 )
	ZEND_ARG_OBJ_INFO( 0, timeZone, IntlTimeZone, 0 )
	ZEND_ARG_INFO( 0, date )
	ZEND_ARG_INFO( 0, local )
	ZEND_ARG_INFO( 1, rawOffset )
	ZEND_ARG_INFO( 1, dstOffset )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_has_same_rules, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, timeZone, IntlTimeZone, 0 )
	ZEND_ARG_OBJ_INFO( 0, otherTimeZone, IntlTimeZone, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_get_display_name, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, timeZone, IntlTimeZone, 0 )
	ZEND_ARG_INFO( 0, isDaylight )
	ZEND_ARG_INFO( 0, style )
	ZEND_ARG_INFO( 0, locale )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_only_tz, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, timeZone, IntlTimeZone, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( arginfo_tz_void, 0, 0, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_create_instance, 0, 0, 0 )
	ZEND_ARG_INFO( 0, timeZone )
	ZEND_ARG_INFO( 0, locale )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_only_cal, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_void, 0, 0, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_field, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, field )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_dow, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, dayOfWeek )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_other_cal, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_OBJ_INFO( 0, otherCalendar, IntlCalendar, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_date, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, date )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_date_optional, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, date )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_get_keyword_values_for_locale, 0, 0, 3)
	ZEND_ARG_INFO( 0, key )
	ZEND_ARG_INFO( 0, locale )
	ZEND_ARG_INFO( 0, commonlyUsed )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_add, 0, 0, 3 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, field )
	ZEND_ARG_INFO( 0, amount )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_set_time_zone, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, timeZone )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_set, 0, 0, 3 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, fieldOrYear )
	ZEND_ARG_INFO( 0, valueOrMonth )
	ZEND_ARG_INFO( 0, dayOfMonth )
	ZEND_ARG_INFO( 0, hour )
	ZEND_ARG_INFO( 0, minute )
	ZEND_ARG_INFO( 0, second )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_roll, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, field )
	ZEND_ARG_INFO( 0, amountOrUpOrDown )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_clear, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, field )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_field_difference, 0, 0, 3 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, when )
	ZEND_ARG_INFO( 0, field )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_get_locale, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, localeType )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_set_lenient, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, isLenient )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_set_minimal_days_in_first_week, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, numberOfDays )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ainfo_cal_from_date_time, 0, 0, 1)
	ZEND_ARG_INFO(0, dateTime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_cal_wall_time_option, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlCalendar, 0 )
	ZEND_ARG_INFO( 0, wallTimeOption )
ZEND_END_ARG_INFO()

/* Gregorian Calendar */
ZEND_BEGIN_ARG_INFO_EX( ainfo_gregcal_create_instance, 0, 0, 0 )
	ZEND_ARG_INFO(0, timeZoneOrYear)
	ZEND_ARG_INFO(0, localeOrMonth)
	ZEND_ARG_INFO(0, dayOfMonth)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, minute)
	ZEND_ARG_INFO(0, second)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_gregcal_is_leap_year, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlGregorianCalendar, 0 )
	ZEND_ARG_INFO( 0, year )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_gregcal_only_gregcal, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlGregorianCalendar, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_gregcal_set_gregorian_change, 0, 0, 2 )
	ZEND_ARG_OBJ_INFO( 0, calendar, IntlGregorianCalendar, 0 )
	ZEND_ARG_INFO( 0, date )
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ intl_functions
 *
 * Every user visible function must have an entry in intl_functions[].
 */
zend_function_entry intl_functions[] = {

	/* collator functions */
	JONJ_FE( collator_create, collator_static_1_arg )
	JONJ_FE( collator_compare, collator_2_args )
	JONJ_FE( collator_get_attribute, collator_1_arg )
	JONJ_FE( collator_set_attribute, collator_2_args )
	JONJ_FE( collator_get_strength, collator_0_args )
	JONJ_FE( collator_set_strength, collator_1_arg )
	JONJ_FE( collator_sort, collator_sort_args )
	JONJ_FE( collator_sort_with_sort_keys, collator_sort_args )
	JONJ_FE( collator_asort, collator_sort_args )
	JONJ_FE( collator_get_locale, collator_1_arg )
	JONJ_FE( collator_get_error_code, collator_0_args )
	JONJ_FE( collator_get_error_message, collator_0_args )
	JONJ_FE( collator_get_sort_key, collator_2_args )

	/* formatter functions */
	JONJ_FE( numfmt_create, arginfo_numfmt_create )
	JONJ_FE( numfmt_format, arginfo_numfmt_format )
	JONJ_FE( numfmt_parse, numfmt_parse_arginfo )
	JONJ_FE( numfmt_format_currency, arginfo_numfmt_format_currency )
	JONJ_FE( numfmt_parse_currency, numfmt_parse_currency_arginfo )
	JONJ_FE( numfmt_set_attribute, arginfo_numfmt_set_attribute )
	JONJ_FE( numfmt_get_attribute, arginfo_numfmt_get_attribute )
	JONJ_FE( numfmt_set_text_attribute, arginfo_numfmt_set_attribute )
	JONJ_FE( numfmt_get_text_attribute, arginfo_numfmt_get_attribute )
	JONJ_FE( numfmt_set_symbol, arginfo_numfmt_set_symbol )
	JONJ_FE( numfmt_get_symbol, arginfo_numfmt_get_attribute )
	JONJ_FE( numfmt_set_pattern, arginfo_numfmt_set_pattern )
	JONJ_FE( numfmt_get_pattern, arginfo_numfmt_get_error_code )
	JONJ_FE( numfmt_get_locale, arginfo_numfmt_get_locale )
	JONJ_FE( numfmt_get_error_code, arginfo_numfmt_get_error_code )
	JONJ_FE( numfmt_get_error_message, arginfo_numfmt_get_error_code )

	/* normalizer functions */
	JONJ_FE( normalizer_normalize, normalizer_args )
	JONJ_FE( normalizer_is_normalized, normalizer_args )

	/* Locale functions */
	JONJ_NAMED_FE( locale_get_default, zif_locale_get_default, locale_0_args )
	JONJ_NAMED_FE( locale_set_default, zif_locale_set_default, locale_1_arg )
	JONJ_FE( locale_get_primary_language, locale_1_arg )
	JONJ_FE( locale_get_script, locale_1_arg )
	JONJ_FE( locale_get_region, locale_1_arg )
	JONJ_FE( locale_get_keywords, locale_1_arg )
	JONJ_FE( locale_get_display_script, locale_2_args )
	JONJ_FE( locale_get_display_region, locale_2_args )
	JONJ_FE( locale_get_display_name, locale_2_args )
	JONJ_FE( locale_get_display_language, locale_2_args)
	JONJ_FE( locale_get_display_variant, locale_2_args )
	JONJ_FE( locale_compose, locale_1_arg )
	JONJ_FE( locale_parse, locale_1_arg )
	JONJ_FE( locale_get_all_variants, locale_1_arg )
	JONJ_FE( locale_filter_matches, locale_3_args )
	JONJ_FE( locale_canonicalize, locale_1_arg )
	JONJ_FE( locale_lookup, locale_4_args )
	JONJ_FE( locale_accept_from_http, locale_1_arg )

	/* MessageFormatter functions */
	JONJ_FE( msgfmt_create, arginfo_msgfmt_create )
	JONJ_FE( msgfmt_format, arginfo_msgfmt_format )
	JONJ_FE( msgfmt_format_message, arginfo_msgfmt_format_message )
	JONJ_FE( msgfmt_parse, arginfo_msgfmt_parse )
	JONJ_FE( msgfmt_parse_message, arginfo_numfmt_parse_message )
	JONJ_FE( msgfmt_set_pattern, arginfo_msgfmt_set_pattern )
	JONJ_FE( msgfmt_get_pattern, arginfo_msgfmt_get_locale )
	JONJ_FE( msgfmt_get_locale, arginfo_msgfmt_get_locale )
	JONJ_FE( msgfmt_get_error_code, arginfo_msgfmt_get_error_code )
	JONJ_FE( msgfmt_get_error_message, arginfo_msgfmt_get_error_message )

	/* IntlDateFormatter functions */
	JONJ_FE( datefmt_create, arginfo_datefmt_create )
	JONJ_FE( datefmt_get_datetype, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_get_timetype, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_get_calendar, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_get_calendar_object, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_set_calendar, arginfo_datefmt_set_calendar )
	JONJ_FE( datefmt_get_locale, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_get_timezone_id, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_set_timezone_id, arginfo_datefmt_set_timezone )
	JONJ_FE( datefmt_get_timezone, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_set_timezone, arginfo_datefmt_set_timezone )
	JONJ_FE( datefmt_get_pattern, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_set_pattern, arginfo_datefmt_set_pattern )
	JONJ_FE( datefmt_is_lenient, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_set_lenient, arginfo_msgfmt_get_locale )
	JONJ_FE( datefmt_format, arginfo_datefmt_format )
	JONJ_FE( datefmt_format_object, arginfo_datefmt_format_object )
	JONJ_FE( datefmt_parse, datefmt_parse_args )
	JONJ_FE( datefmt_localtime , datefmt_parse_args )
	JONJ_FE( datefmt_get_error_code, arginfo_msgfmt_get_error_code )
	JONJ_FE( datefmt_get_error_message, arginfo_msgfmt_get_error_message )

	/* grapheme functions */
	JONJ_FE( grapheme_strlen, grapheme_1_arg )
	JONJ_FE( grapheme_strpos, grapheme_search_args )
	JONJ_FE( grapheme_stripos, grapheme_search_args )
	JONJ_FE( grapheme_strrpos, grapheme_search_args )
	JONJ_FE( grapheme_strripos, grapheme_search_args )
	JONJ_FE( grapheme_substr, grapheme_substr_args )
	JONJ_FE( grapheme_strstr, grapheme_strstr_args )
	JONJ_FE( grapheme_stristr, grapheme_strstr_args )
	JONJ_FE( grapheme_extract, grapheme_extract_args )

	/* IDN functions */
	JONJ_FE( idn_to_ascii, arginfo_idn_to_ascii)
	JONJ_FE( idn_to_utf8, arginfo_idn_to_ascii)

	/* ResourceBundle functions */
	JONJ_FE( resourcebundle_create, arginfo_resourcebundle_create_proc )
	JONJ_FE( resourcebundle_get, arginfo_resourcebundle_get_proc )
	JONJ_FE( resourcebundle_count, arginfo_resourcebundle_count_proc )
	JONJ_FE( resourcebundle_locales, arginfo_resourcebundle_locales_proc )
	JONJ_FE( resourcebundle_get_error_code, arginfo_resourcebundle_get_error_code_proc )
	JONJ_FE( resourcebundle_get_error_message, arginfo_resourcebundle_get_error_message_proc )
	
	/* Transliterator functions */
	JONJ_FE( transliterator_create, arginfo_transliterator_create )
	JONJ_FE( transliterator_create_from_rules, arginfo_transliterator_create_from_rules )
	JONJ_FE( transliterator_list_ids, arginfo_transliterator_void )
	JONJ_FE( transliterator_create_inverse, arginfo_transliterator_create_inverse)
	JONJ_FE( transliterator_transliterate, arginfo_transliterator_transliterate )
	JONJ_FE( transliterator_get_error_code, arginfo_transliterator_error )
	JONJ_FE( transliterator_get_error_message, arginfo_transliterator_error )

	/* TimeZone functions */
	JONJ_FE( intltz_create_time_zone, arginfo_tz_idarg_static )
	JONJ_FE( intltz_from_date_time_zone, arginfo_tz_from_date_time_zone )
	JONJ_FE( intltz_create_default, arginfo_tz_void )
	JONJ_FE( intltz_get_id, arginfo_tz_only_tz )
	JONJ_FE( intltz_get_gmt, arginfo_tz_void )
#if U_ICU_VERSION_MAJOR_NUM >= 49
	JONJ_FE( intltz_get_unknown, arginfo_tz_void )
#endif
	JONJ_FE( intltz_create_enumeration, arginfo_tz_create_enumeration )
	JONJ_FE( intltz_count_equivalent_ids, arginfo_tz_idarg_static )
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
	JONJ_FE( intltz_create_time_zone_id_enumeration, arginfo_tz_create_time_zone_id_enumeration )
#endif
	JONJ_FE( intltz_get_canonical_id, arginfo_tz_get_canonical_id )
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
	JONJ_FE( intltz_get_region, arginfo_tz_idarg_static )
#endif
	JONJ_FE( intltz_get_tz_data_version, arginfo_tz_void )
	JONJ_FE( intltz_get_equivalent_id, arginfo_tz_get_equivalent_id )
	JONJ_FE( intltz_use_daylight_time, arginfo_tz_only_tz )
	JONJ_FE( intltz_get_offset, arginfo_tz_get_offset )
	JONJ_FE( intltz_get_raw_offset, arginfo_tz_only_tz )
	JONJ_FE( intltz_has_same_rules, arginfo_tz_has_same_rules )
	JONJ_FE( intltz_get_display_name, arginfo_tz_get_display_name )
	JONJ_FE( intltz_get_dst_savings, arginfo_tz_only_tz )
	JONJ_FE( intltz_to_date_time_zone, arginfo_tz_only_tz )
	JONJ_FE( intltz_get_error_code, arginfo_tz_only_tz )
	JONJ_FE( intltz_get_error_message, arginfo_tz_only_tz )

	JONJ_FE( intlcal_create_instance, ainfo_cal_create_instance )
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 42
	JONJ_FE( intlcal_get_keyword_values_for_locale, ainfo_cal_get_keyword_values_for_locale )
#endif
	JONJ_FE( intlcal_get_now, ainfo_cal_void )
	JONJ_FE( intlcal_get_available_locales, ainfo_cal_void )
	JONJ_FE( intlcal_get, ainfo_cal_field )
	JONJ_FE( intlcal_get_time, ainfo_cal_only_cal )
	JONJ_FE( intlcal_set_time, ainfo_cal_date )
	JONJ_FE( intlcal_add, ainfo_cal_add )
	JONJ_FE( intlcal_set_time_zone, ainfo_cal_set_time_zone )
	JONJ_FE( intlcal_after, ainfo_cal_other_cal )
	JONJ_FE( intlcal_before, ainfo_cal_other_cal )
	JONJ_FE( intlcal_set, ainfo_cal_set )
	JONJ_FE( intlcal_roll, ainfo_cal_roll )
	JONJ_FE( intlcal_clear, ainfo_cal_clear )
	JONJ_FE( intlcal_field_difference, ainfo_cal_field_difference )
	JONJ_FE( intlcal_get_actual_maximum, ainfo_cal_field )
	JONJ_FE( intlcal_get_actual_minimum, ainfo_cal_field )
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	JONJ_FE( intlcal_get_day_of_week_type, ainfo_cal_dow )
#endif
	JONJ_FE( intlcal_get_first_day_of_week, ainfo_cal_only_cal )
	JONJ_FE( intlcal_get_greatest_minimum, ainfo_cal_field )
	JONJ_FE( intlcal_get_least_maximum, ainfo_cal_field )
	JONJ_FE( intlcal_get_locale, ainfo_cal_get_locale )
	JONJ_FE( intlcal_get_maximum, ainfo_cal_field )
	JONJ_FE( intlcal_get_minimal_days_in_first_week, ainfo_cal_only_cal )
	JONJ_FE( intlcal_get_minimum, ainfo_cal_field )
	JONJ_FE( intlcal_get_time_zone, ainfo_cal_only_cal )
	JONJ_FE( intlcal_get_type, ainfo_cal_only_cal )
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	JONJ_FE( intlcal_get_weekend_transition, ainfo_cal_dow )
#endif
	JONJ_FE( intlcal_in_daylight_time, ainfo_cal_only_cal )
	JONJ_FE( intlcal_is_equivalent_to, ainfo_cal_other_cal )
	JONJ_FE( intlcal_is_lenient, ainfo_cal_only_cal )
	JONJ_FE( intlcal_is_set, ainfo_cal_field )
#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 44
	JONJ_FE( intlcal_is_weekend, ainfo_cal_date_optional )
#endif
	JONJ_FE( intlcal_set_first_day_of_week, ainfo_cal_dow )
	JONJ_FE( intlcal_set_lenient, ainfo_cal_set_lenient )
	JONJ_FE( intlcal_set_minimal_days_in_first_week, ainfo_cal_set_minimal_days_in_first_week )
	JONJ_FE( intlcal_equals, ainfo_cal_other_cal )
	JONJ_FE( intlcal_from_date_time, ainfo_cal_from_date_time )
	JONJ_FE( intlcal_to_date_time, ainfo_cal_only_cal )
#if U_ICU_VERSION_MAJOR_NUM >= 49
	JONJ_FE( intlcal_get_repeated_wall_time_option, ainfo_cal_only_cal )
	JONJ_FE( intlcal_get_skipped_wall_time_option, ainfo_cal_only_cal )
	JONJ_FE( intlcal_set_repeated_wall_time_option, ainfo_cal_wall_time_option )
	JONJ_FE( intlcal_set_skipped_wall_time_option, ainfo_cal_wall_time_option )
#endif
	JONJ_FE( intlcal_get_error_code, ainfo_cal_only_cal )
	JONJ_FE( intlcal_get_error_message, ainfo_cal_only_cal )

	JONJ_FE( intlgregcal_create_instance, ainfo_gregcal_create_instance )
	JONJ_FE( intlgregcal_set_gregorian_change, ainfo_gregcal_set_gregorian_change )
	JONJ_FE( intlgregcal_get_gregorian_change, ainfo_gregcal_only_gregcal )
	JONJ_FE( intlgregcal_is_leap_year, ainfo_gregcal_is_leap_year )

	/* common functions */
	JONJ_FE( intl_get_error_code, intl_0_args )
	JONJ_FE( intl_get_error_message, intl_0_args )
	JONJ_FE( intl_is_failure, intl_1_arg )
	JONJ_FE( intl_error_name, intl_1_arg )

	JONJ_FE_END
};
/* }}} */

/* {{{ INI Settings */
JONJ_INI_BEGIN()
    STD_JONJ_INI_ENTRY(LOCALE_INI_NAME, NULL, JONJ_INI_ALL, OnUpdateStringUnempty, default_locale, zend_intl_globals, intl_globals)
    STD_JONJ_INI_ENTRY("intl.error_level", "0", JONJ_INI_ALL, OnUpdateLong, error_level, zend_intl_globals, intl_globals)
	STD_JONJ_INI_ENTRY("intl.use_exceptions", "0", JONJ_INI_ALL, OnUpdateBool, use_exceptions, zend_intl_globals, intl_globals)
JONJ_INI_END()
/* }}} */

static JONJ_GINIT_FUNCTION(intl);

/* {{{ intl_module_entry */
zend_module_entry intl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"intl",
	intl_functions,
	JONJ_MINIT( intl ),
	JONJ_MSHUTDOWN( intl ),
	JONJ_RINIT( intl ),
	JONJ_RSHUTDOWN( intl ),
	JONJ_MINFO( intl ),
	INTL_MODULE_VERSION,
	JONJ_MODULE_GLOBALS(intl),   /* globals descriptor */
	JONJ_GINIT(intl),            /* globals ctor */
	NULL,                       /* globals dtor */
	NULL,                       /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_INTL
ZEND_GET_MODULE( intl )
#endif

/* {{{ intl_init_globals */
static JONJ_GINIT_FUNCTION(intl)
{
	memset( intl_globals, 0, sizeof(zend_intl_globals) );
}
/* }}} */

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION( intl )
{
	/* For the default locale jonj.ini setting */
	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("INTL_MAX_LOCALE_LEN", INTL_MAX_LOCALE_LEN, CONST_CS);
	REGISTER_STRING_CONSTANT("INTL_ICU_VERSION", U_ICU_VERSION, CONST_PERSISTENT | CONST_CS);
#ifdef U_ICU_DATA_VERSION
	REGISTER_STRING_CONSTANT("INTL_ICU_DATA_VERSION", U_ICU_DATA_VERSION, CONST_PERSISTENT | CONST_CS);
#endif	

	/* Register 'Collator' JONJ class */
	collator_register_Collator_class( TSRMLS_C );

	/* Expose Collator constants to JONJ scripts */
	collator_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'NumberFormatter' JONJ class */
	formatter_register_class( TSRMLS_C );

	/* Expose NumberFormatter constants to JONJ scripts */
	formatter_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'Normalizer' JONJ class */
	normalizer_register_Normalizer_class( TSRMLS_C );

	/* Expose Normalizer constants to JONJ scripts */
	normalizer_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'Locale' JONJ class */
	locale_register_Locale_class( TSRMLS_C );

	/* Expose Locale constants to JONJ scripts */
	locale_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	msgformat_register_class(TSRMLS_C);

	grapheme_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'DateFormat' JONJ class */
	dateformat_register_IntlDateFormatter_class( TSRMLS_C );

	/* Expose DateFormat constants to JONJ scripts */
	dateformat_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'ResourceBundle' JONJ class */
	resourcebundle_register_class( TSRMLS_C);

	/* Register 'Transliterator' JONJ class */
	transliterator_register_Transliterator_class( TSRMLS_C );

	/* Register Transliterator constants */
	transliterator_register_constants( INIT_FUNC_ARGS_PASSTHRU );

	/* Register 'IntlTimeZone' JONJ class */
	timezone_register_IntlTimeZone_class( TSRMLS_C );

	/* Register 'IntlCalendar' JONJ class */
	calendar_register_IntlCalendar_class( TSRMLS_C );

	/* Expose ICU error codes to JONJ scripts. */
	intl_expose_icu_error_codes( INIT_FUNC_ARGS_PASSTHRU );

	/* Expose IDN constants to JONJ scripts. */
	idn_register_constants(INIT_FUNC_ARGS_PASSTHRU);

#if U_ICU_VERSION_MAJOR_NUM * 1000 + U_ICU_VERSION_MINOR_NUM >= 4002
	/* Register 'Spoofchecker' JONJ class */
	spoofchecker_register_Spoofchecker_class( TSRMLS_C );

	/* Expose Spoofchecker constants to JONJ scripts */
	spoofchecker_register_constants( INIT_FUNC_ARGS_PASSTHRU );
#endif

	/* Register 'IntlException' JONJ class */
	intl_register_IntlException_class( TSRMLS_C );

	/* Register 'IntlIterator' JONJ class */
	intl_register_IntlIterator_class( TSRMLS_C );

	/* Register 'BreakIterator' class */
	breakiterator_register_BreakIterator_class( TSRMLS_C );

	/* Register 'IntlPartsIterator' class */
	breakiterator_register_IntlPartsIterator_class( TSRMLS_C );

	/* Global error handling. */
	intl_error_init( NULL TSRMLS_CC );

	/* 'Converter' class for codepage conversions */
	jonj_converter_minit(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

#define EXPLICIT_CLEANUP_ENV_VAR "INTL_EXPLICIT_CLEANUP"

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION( intl )
{
	const char *cleanup;
    /* For the default locale jonj.ini setting */
    UNREGISTER_INI_ENTRIES();

	cleanup = getenv(EXPLICIT_CLEANUP_ENV_VAR);
    if (cleanup != NULL && !(cleanup[0] == '0' && cleanup[1] == '\0')) {
		u_cleanup();
    }

    return SUCCESS;
}
/* }}} */

/* {{{ JONJ_RINIT_FUNCTION
 */
JONJ_RINIT_FUNCTION( intl )
{
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_RSHUTDOWN_FUNCTION
 */
JONJ_RSHUTDOWN_FUNCTION( intl )
{
	if(INTL_G(current_collator)) {
		INTL_G(current_collator) = NULL;
	}
	if (INTL_G(grapheme_iterator)) {
		grapheme_close_global_iterator( TSRMLS_C );
		INTL_G(grapheme_iterator) = NULL;
	}

	intl_error_reset( NULL TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION( intl )
{
	jonj_info_print_table_start();
	jonj_info_print_table_header( 2, "Internationalization support", "enabled" );
	jonj_info_print_table_row( 2, "version", INTL_MODULE_VERSION );
	jonj_info_print_table_row( 2, "ICU version", U_ICU_VERSION );
#ifdef U_ICU_DATA_VERSION
	jonj_info_print_table_row( 2, "ICU Data version", U_ICU_DATA_VERSION );
#endif
	jonj_info_print_table_end();

    /* For the default locale jonj.ini setting */
    DISPLAY_INI_ENTRIES() ;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
