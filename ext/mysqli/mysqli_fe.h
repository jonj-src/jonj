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
  | Author: Georg Richter <georg@jonj.tk>                                |
            Andrey Hristov <andrey@jonj.tk>                              |
  +----------------------------------------------------------------------+

  $Id: jonj_mysqli_structs.h 302179 2010-08-13 09:57:04Z andrey $
*/

#ifndef MYSQLI_FE_H
#define MYSQLI_FE_H

JONJ_FUNCTION(mysqli);
JONJ_FUNCTION(mysqli_affected_rows);
JONJ_FUNCTION(mysqli_autocommit);
JONJ_FUNCTION(mysqli_begin_transaction);
JONJ_FUNCTION(mysqli_change_user);
JONJ_FUNCTION(mysqli_character_set_name);
JONJ_FUNCTION(mysqli_set_charset);
JONJ_FUNCTION(mysqli_close);
JONJ_FUNCTION(mysqli_commit);
JONJ_FUNCTION(mysqli_connect);
JONJ_FUNCTION(mysqli_connect_errno);
JONJ_FUNCTION(mysqli_connect_error);
JONJ_FUNCTION(mysqli_data_seek);
JONJ_FUNCTION(mysqli_debug);
JONJ_FUNCTION(mysqli_dump_debug_info);
JONJ_FUNCTION(mysqli_errno);
JONJ_FUNCTION(mysqli_error);
JONJ_FUNCTION(mysqli_error_list);
JONJ_FUNCTION(mysqli_fetch_all);
JONJ_FUNCTION(mysqli_fetch_array);
JONJ_FUNCTION(mysqli_fetch_assoc);
JONJ_FUNCTION(mysqli_fetch_object);
JONJ_FUNCTION(mysqli_fetch_field);
JONJ_FUNCTION(mysqli_fetch_fields);
JONJ_FUNCTION(mysqli_fetch_field_direct);
JONJ_FUNCTION(mysqli_fetch_lengths);
JONJ_FUNCTION(mysqli_fetch_row);
JONJ_FUNCTION(mysqli_field_count);
JONJ_FUNCTION(mysqli_field_seek);
JONJ_FUNCTION(mysqli_field_tell);
JONJ_FUNCTION(mysqli_free_result);
JONJ_FUNCTION(mysqli_get_cache_stats);
JONJ_FUNCTION(mysqli_get_client_stats);
JONJ_FUNCTION(mysqli_get_connection_stats);
JONJ_FUNCTION(mysqli_get_charset);
JONJ_FUNCTION(mysqli_get_client_info);
JONJ_FUNCTION(mysqli_get_client_version);
JONJ_FUNCTION(mysqli_get_host_info);
JONJ_FUNCTION(mysqli_get_links_stats);
JONJ_FUNCTION(mysqli_get_proto_info);
JONJ_FUNCTION(mysqli_get_server_info);
JONJ_FUNCTION(mysqli_get_server_version);
JONJ_FUNCTION(mysqli_get_warnings);
JONJ_FUNCTION(mysqli_info);
JONJ_FUNCTION(mysqli_insert_id);
JONJ_FUNCTION(mysqli_init);
JONJ_FUNCTION(mysqli_kill);
JONJ_FUNCTION(mysqli_link_construct);
JONJ_FUNCTION(mysqli_set_local_infile_default);
JONJ_FUNCTION(mysqli_set_local_infile_handler);
JONJ_FUNCTION(mysqli_more_results);
JONJ_FUNCTION(mysqli_multi_query);
JONJ_FUNCTION(mysqli_next_result);
JONJ_FUNCTION(mysqli_num_fields);
JONJ_FUNCTION(mysqli_num_rows);
JONJ_FUNCTION(mysqli_options);
JONJ_FUNCTION(mysqli_ping);
JONJ_FUNCTION(mysqli_poll);
JONJ_FUNCTION(mysqli_prepare);
JONJ_FUNCTION(mysqli_query);
JONJ_FUNCTION(mysqli_stmt_result_metadata);
JONJ_FUNCTION(mysqli_report);
JONJ_FUNCTION(mysqli_read_query_result);
JONJ_FUNCTION(mysqli_real_connect);
JONJ_FUNCTION(mysqli_real_query);
JONJ_FUNCTION(mysqli_real_escape_string);
JONJ_FUNCTION(mysqli_reap_async_query);
JONJ_FUNCTION(mysqli_rollback);
JONJ_FUNCTION(mysqli_row_seek);
JONJ_FUNCTION(mysqli_select_db);
JONJ_FUNCTION(mysqli_stmt_attr_get);
JONJ_FUNCTION(mysqli_stmt_attr_set);
JONJ_FUNCTION(mysqli_stmt_bind_param);
JONJ_FUNCTION(mysqli_stmt_bind_result);
JONJ_FUNCTION(mysqli_stmt_execute);
JONJ_FUNCTION(mysqli_stmt_field_count);
JONJ_FUNCTION(mysqli_stmt_init);
JONJ_FUNCTION(mysqli_stmt_prepare);
JONJ_FUNCTION(mysqli_stmt_fetch);
JONJ_FUNCTION(mysqli_stmt_param_count);
JONJ_FUNCTION(mysqli_stmt_send_long_data);
JONJ_FUNCTION(mysqli_embedded_server_end);
JONJ_FUNCTION(mysqli_embedded_server_start);
JONJ_FUNCTION(mysqli_sqlstate);
JONJ_FUNCTION(mysqli_ssl_set);
JONJ_FUNCTION(mysqli_stat);
JONJ_FUNCTION(mysqli_refresh);
JONJ_FUNCTION(mysqli_savepoint);
JONJ_FUNCTION(mysqli_release_savepoint);
JONJ_FUNCTION(mysqli_stmt_affected_rows);
JONJ_FUNCTION(mysqli_stmt_close);
JONJ_FUNCTION(mysqli_stmt_data_seek);
JONJ_FUNCTION(mysqli_stmt_errno);
JONJ_FUNCTION(mysqli_stmt_error);
JONJ_FUNCTION(mysqli_stmt_error_list);
JONJ_FUNCTION(mysqli_stmt_free_result);
JONJ_FUNCTION(mysqli_stmt_get_result);
JONJ_FUNCTION(mysqli_stmt_get_warnings);
JONJ_FUNCTION(mysqli_stmt_reset);
JONJ_FUNCTION(mysqli_stmt_insert_id);
JONJ_FUNCTION(mysqli_stmt_more_results);
JONJ_FUNCTION(mysqli_stmt_next_result);
JONJ_FUNCTION(mysqli_stmt_num_rows);
JONJ_FUNCTION(mysqli_stmt_sqlstate);
JONJ_FUNCTION(mysqli_stmt_store_result);
JONJ_FUNCTION(mysqli_store_result);
JONJ_FUNCTION(mysqli_thread_id);
JONJ_FUNCTION(mysqli_thread_safe);
JONJ_FUNCTION(mysqli_use_result);
JONJ_FUNCTION(mysqli_warning_count);

JONJ_FUNCTION(mysqli_stmt_construct);
JONJ_FUNCTION(mysqli_result_construct);
JONJ_FUNCTION(mysqli_driver_construct);
JONJ_METHOD(mysqli_warning,__construct);

#endif /* MYSQLI_FE_H */

