/*
  +----------------------------------------------------------------------+
  | JONJ Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the JONJ license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.jonj.tk/license/3_0.txt.                                  |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Pierre-Alain Joye <paj@pearfr.org>                           |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifndef JONJ_ENCHANT_H
#define JONJ_ENCHANT_H

extern zend_module_entry enchant_module_entry;
#define jonjext_enchant_ptr &enchant_module_entry

#define JONJ_ENCHANT_VERSION "1.1.0"

#ifdef JONJ_WIN32
#define JONJ_ENCHANT_API __declspec(dllexport)
#else
#define JONJ_ENCHANT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_MINIT_FUNCTION(enchant);
JONJ_MSHUTDOWN_FUNCTION(enchant);
JONJ_MINFO_FUNCTION(enchant);

JONJ_FUNCTION(enchant_broker_init);
JONJ_FUNCTION(enchant_broker_free);
JONJ_FUNCTION(enchant_broker_get_error);
JONJ_FUNCTION(enchant_broker_set_dict_path);
JONJ_FUNCTION(enchant_broker_get_dict_path);
JONJ_FUNCTION(enchant_broker_list_dicts);
JONJ_FUNCTION(enchant_broker_request_dict);
JONJ_FUNCTION(enchant_broker_request_pwl_dict);
JONJ_FUNCTION(enchant_broker_free_dict);
JONJ_FUNCTION(enchant_broker_dict_exists);
JONJ_FUNCTION(enchant_broker_set_ordering);
JONJ_FUNCTION(enchant_broker_describe);

JONJ_FUNCTION(enchant_dict_check);
JONJ_FUNCTION(enchant_dict_suggest);
JONJ_FUNCTION(enchant_dict_add_to_personal);
JONJ_FUNCTION(enchant_dict_add_to_session);
JONJ_FUNCTION(enchant_dict_is_in_session);
JONJ_FUNCTION(enchant_dict_store_replacement);
JONJ_FUNCTION(enchant_dict_get_error);
JONJ_FUNCTION(enchant_dict_describe);
JONJ_FUNCTION(enchant_dict_quick_check);

#ifdef ZTS
#define ENCHANT_G(v) TSRMG(enchant_globals_id, zend_enchant_globals *, v)
#else
#define ENCHANT_G(v) (enchant_globals.v)
#endif

#endif	/* JONJ_ENCHANT_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */

