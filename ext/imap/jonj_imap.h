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
   | Authors: Rex Logan           <veebert@dimensional.com>               |
   |          Mark Musone         <musone@afterfive.com>                  |
   |          Brian Wang          <brian@vividnet.com>                    |
   |          Kaj-Michael Lang    <milang@tal.org>                        |
   |          Antoni Pamies Olive <toni@readysoft.net>                    |
   |          Rasmus Lerdorf      <rasmus@jonj.tk>                        |
   |          Chuck Hagenbuch     <chuck@horde.org>                       |
   |          Andrew Skalski      <askalski@chekinc.com>                  |
   |          Hartmut Holzgraefe  <hholzgra@jonj.tk>                      |
   |          Jani Taskinen       <sniper@iki.fi>                         |
   |          Daniel R. Kalowsky  <kalowsky@jonj.tk>                      |
   | JONJ 4.0 updates:  Zeev Suraski <zeev@zend.com>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_IMAP_H
#define JONJ_IMAP_H

#if HAVE_IMAP

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
 /* these are used for quota support */
# include "c-client.h"	/* includes mail.h and rfc822.h */
# include "imap4r1.h"	/* location of c-client quota functions */
#else
# include "mail.h"
# include "rfc822.h" 
#endif

extern zend_module_entry imap_module_entry;
#define imap_module_ptr &imap_module_entry


/* Data types */

#ifdef IMAP41
#define LSIZE text.size
#define LTEXT text.data
#define DTYPE int
#define CONTENT_PART nested.part
#define CONTENT_MSG_BODY nested.msg->body
#define IMAPVER "Imap 4R1"
#else
#define LSIZE size
#define LTEXT text
#define DTYPE char
#define CONTENT_PART contents.part
#define CONTENT_MSG_BODY contents.msg.body
#define IMAPVER "Imap 4"
#endif


/* Determines how mm_list() and mm_lsub() are to return their results. */
typedef enum {
	FLIST_ARRAY,
	FLIST_OBJECT
} folderlist_style_t;

typedef struct jonj_imap_le_struct {
	MAILSTREAM *imap_stream;
	long flags;
} pils;

typedef struct jonj_imap_mailbox_struct {
	SIZEDTEXT text;
	DTYPE delimiter;
	long attributes;
	struct jonj_imap_mailbox_struct *next;
} FOBJECTLIST;

typedef struct jonj_imap_error_struct {
	SIZEDTEXT text;
	long errflg;
	struct jonj_imap_error_struct *next;
} ERRORLIST;

typedef struct _jonj_imap_message_struct {
	unsigned long msgid;
	struct _jonj_imap_message_struct *next;
} MESSAGELIST;
 

/* Functions */

JONJ_MINIT_FUNCTION(imap);
JONJ_RINIT_FUNCTION(imap);
JONJ_RSHUTDOWN_FUNCTION(imap);
JONJ_MINFO_FUNCTION(imap);

JONJ_FUNCTION(imap_open);
JONJ_FUNCTION(imap_popen);
JONJ_FUNCTION(imap_reopen);
JONJ_FUNCTION(imap_num_msg);
JONJ_FUNCTION(imap_num_recent);
JONJ_FUNCTION(imap_headers);
JONJ_FUNCTION(imap_headerinfo);
JONJ_FUNCTION(imap_rfc822_parse_headers);
JONJ_FUNCTION(imap_body);
JONJ_FUNCTION(imap_fetchstructure);
JONJ_FUNCTION(imap_fetchbody);
JONJ_FUNCTION(imap_fetchmime);
JONJ_FUNCTION(imap_savebody);
JONJ_FUNCTION(imap_gc);
JONJ_FUNCTION(imap_expunge);
JONJ_FUNCTION(imap_delete);
JONJ_FUNCTION(imap_undelete);
JONJ_FUNCTION(imap_check);
JONJ_FUNCTION(imap_close);
JONJ_FUNCTION(imap_mail_copy);
JONJ_FUNCTION(imap_mail_move);
JONJ_FUNCTION(imap_createmailbox);
JONJ_FUNCTION(imap_renamemailbox);
JONJ_FUNCTION(imap_deletemailbox);
JONJ_FUNCTION(imap_listmailbox);
JONJ_FUNCTION(imap_scanmailbox);
JONJ_FUNCTION(imap_subscribe);
JONJ_FUNCTION(imap_unsubscribe);
JONJ_FUNCTION(imap_append);
JONJ_FUNCTION(imap_ping);
JONJ_FUNCTION(imap_base64);
JONJ_FUNCTION(imap_qprint);
JONJ_FUNCTION(imap_8bit);
JONJ_FUNCTION(imap_binary);
JONJ_FUNCTION(imap_mailboxmsginfo);
JONJ_FUNCTION(imap_rfc822_write_address);
JONJ_FUNCTION(imap_rfc822_parse_adrlist);
JONJ_FUNCTION(imap_setflag_full);
JONJ_FUNCTION(imap_clearflag_full);
JONJ_FUNCTION(imap_sort);
JONJ_FUNCTION(imap_fetchheader);
JONJ_FUNCTION(imap_fetchtext);
JONJ_FUNCTION(imap_uid);
JONJ_FUNCTION(imap_msgno);
JONJ_FUNCTION(imap_list);
JONJ_FUNCTION(imap_list_full);
JONJ_FUNCTION(imap_listscan);
JONJ_FUNCTION(imap_lsub);
JONJ_FUNCTION(imap_lsub_full);
JONJ_FUNCTION(imap_create);
JONJ_FUNCTION(imap_rename);
JONJ_FUNCTION(imap_status);
JONJ_FUNCTION(imap_bodystruct);
JONJ_FUNCTION(imap_fetch_overview);
JONJ_FUNCTION(imap_mail_compose);
JONJ_FUNCTION(imap_alerts);
JONJ_FUNCTION(imap_errors);
JONJ_FUNCTION(imap_last_error);
JONJ_FUNCTION(imap_mail);
JONJ_FUNCTION(imap_search);
JONJ_FUNCTION(imap_utf8);
JONJ_FUNCTION(imap_utf7_decode);
JONJ_FUNCTION(imap_utf7_encode);
#ifdef HAVE_IMAP_MUTF7
JONJ_FUNCTION(imap_utf8_to_mutf7);
JONJ_FUNCTION(imap_mutf7_to_utf8);
#endif
JONJ_FUNCTION(imap_mime_header_decode);
JONJ_FUNCTION(imap_thread);
JONJ_FUNCTION(imap_timeout);

#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
JONJ_FUNCTION(imap_get_quota);
JONJ_FUNCTION(imap_get_quotaroot);
JONJ_FUNCTION(imap_set_quota);
JONJ_FUNCTION(imap_setacl);
JONJ_FUNCTION(imap_getacl);
#endif


ZEND_BEGIN_MODULE_GLOBALS(imap)
	char *imap_user;
	char *imap_password;

	STRINGLIST *imap_alertstack;
	ERRORLIST *imap_errorstack;

	STRINGLIST *imap_folders;
	STRINGLIST *imap_folders_tail;
	STRINGLIST *imap_sfolders;
	STRINGLIST *imap_sfolders_tail;
	MESSAGELIST *imap_messages;
	MESSAGELIST *imap_messages_tail;
	FOBJECTLIST *imap_folder_objects;
	FOBJECTLIST *imap_folder_objects_tail;
	FOBJECTLIST *imap_sfolder_objects;
	FOBJECTLIST *imap_sfolder_objects_tail;

	folderlist_style_t folderlist_style;
	long status_flags;
	unsigned long status_messages;
	unsigned long status_recent;
	unsigned long status_unseen;
	unsigned long status_uidnext;
	unsigned long status_uidvalidity;
#if defined(HAVE_IMAP2000) || defined(HAVE_IMAP2001)
	zval **quota_return;
	zval *imap_acl_list;
#endif
	/* jonj_stream for jonj_mail_gets() */
	jonj_stream *gets_stream;
ZEND_END_MODULE_GLOBALS(imap)

#ifdef ZTS
# define IMAPG(v) TSRMG(imap_globals_id, zend_imap_globals *, v)
#else
# define IMAPG(v) (imap_globals.v)
#endif

#else

#define imap_module_ptr NULL

#endif

#define jonjext_imap_ptr imap_module_ptr

#endif /* JONJ_IMAP_H */
