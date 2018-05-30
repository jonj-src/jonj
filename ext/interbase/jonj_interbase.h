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
   | Authors: Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Andrew Avdeev <andy@simgts.mv.ru>                           |
   |          Ard Biesheuvel <a.k.biesheuvel@its.tudelft.nl>              |
   +----------------------------------------------------------------------+
 */

#ifndef JONJ_INTERBASE_H
#define JONJ_INTERBASE_H

extern zend_module_entry ibase_module_entry;
#define jonjext_interbase_ptr &ibase_module_entry

JONJ_MINIT_FUNCTION(ibase);
JONJ_RINIT_FUNCTION(ibase);
JONJ_MSHUTDOWN_FUNCTION(ibase);
JONJ_RSHUTDOWN_FUNCTION(ibase);
JONJ_MINFO_FUNCTION(ibase);

JONJ_FUNCTION(ibase_connect);
JONJ_FUNCTION(ibase_pconnect);
JONJ_FUNCTION(ibase_close);
JONJ_FUNCTION(ibase_drop_db);
JONJ_FUNCTION(ibase_query);
JONJ_FUNCTION(ibase_fetch_row);
JONJ_FUNCTION(ibase_fetch_assoc);
JONJ_FUNCTION(ibase_fetch_object);
JONJ_FUNCTION(ibase_free_result);
JONJ_FUNCTION(ibase_name_result);
JONJ_FUNCTION(ibase_prepare);
JONJ_FUNCTION(ibase_execute);
JONJ_FUNCTION(ibase_free_query);

JONJ_FUNCTION(ibase_timefmt);

JONJ_FUNCTION(ibase_gen_id);
JONJ_FUNCTION(ibase_num_fields);
JONJ_FUNCTION(ibase_num_params);
#if abies_0
JONJ_FUNCTION(ibase_num_rows);
#endif
JONJ_FUNCTION(ibase_affected_rows);
JONJ_FUNCTION(ibase_field_info);
JONJ_FUNCTION(ibase_param_info);

JONJ_FUNCTION(ibase_trans);
JONJ_FUNCTION(ibase_commit);
JONJ_FUNCTION(ibase_rollback);
JONJ_FUNCTION(ibase_commit_ret);
JONJ_FUNCTION(ibase_rollback_ret);

JONJ_FUNCTION(ibase_blob_create);
JONJ_FUNCTION(ibase_blob_add);
JONJ_FUNCTION(ibase_blob_cancel);
JONJ_FUNCTION(ibase_blob_open);
JONJ_FUNCTION(ibase_blob_get);
JONJ_FUNCTION(ibase_blob_close);
JONJ_FUNCTION(ibase_blob_echo);
JONJ_FUNCTION(ibase_blob_info);
JONJ_FUNCTION(ibase_blob_import);

JONJ_FUNCTION(ibase_add_user);
JONJ_FUNCTION(ibase_modify_user);
JONJ_FUNCTION(ibase_delete_user);

JONJ_FUNCTION(ibase_service_attach);
JONJ_FUNCTION(ibase_service_detach);
JONJ_FUNCTION(ibase_backup);
JONJ_FUNCTION(ibase_restore);
JONJ_FUNCTION(ibase_maintain_db);
JONJ_FUNCTION(ibase_db_info);
JONJ_FUNCTION(ibase_server_info);

JONJ_FUNCTION(ibase_errmsg);
JONJ_FUNCTION(ibase_errcode);

JONJ_FUNCTION(ibase_wait_event);
JONJ_FUNCTION(ibase_set_event_handler);
JONJ_FUNCTION(ibase_free_event_handler);

#else

#define jonjext_interbase_ptr NULL

#endif /* JONJ_INTERBASE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
