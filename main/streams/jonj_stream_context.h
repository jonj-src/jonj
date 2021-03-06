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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Stream context and status notification related definitions */

/* callback for status notifications */
typedef void (*jonj_stream_notification_func)(jonj_stream_context *context,
		int notifycode, int severity,
		char *xmsg, int xcode,
		size_t bytes_sofar, size_t bytes_max,
		void * ptr TSRMLS_DC);

#define JONJ_STREAM_NOTIFIER_PROGRESS	1

/* Attempt to fetch context from the zval passed,
   If no context was passed, use the default context
   The default context has not yet been created, do it now. */
#define jonj_stream_context_from_zval(zcontext, nocontext) ( \
		(zcontext) ? zend_fetch_resource(&(zcontext) TSRMLS_CC, -1, "Stream-Context", NULL, 1, jonj_le_stream_context(TSRMLS_C)) : \
		(nocontext) ? NULL : \
		FG(default_context) ? FG(default_context) : \
		(FG(default_context) = jonj_stream_context_alloc(TSRMLS_C)) )

#define jonj_stream_context_to_zval(context, zval) { ZVAL_RESOURCE(zval, (context)->rsrc_id); zend_list_addref((context)->rsrc_id); }

typedef struct _jonj_stream_notifier jonj_stream_notifier;

struct _jonj_stream_notifier {
	jonj_stream_notification_func func;
	void (*dtor)(jonj_stream_notifier *notifier);
	void *ptr;
	int mask;
	size_t progress, progress_max; /* position for progress notification */
};

struct _jonj_stream_context {
	jonj_stream_notifier *notifier;
	zval *options;	/* hash keyed by wrapper family or specific wrapper */
	int rsrc_id;	/* used for auto-cleanup */
};

BEGIN_EXTERN_C()
JONJAPI void jonj_stream_context_free(jonj_stream_context *context);
JONJAPI jonj_stream_context *jonj_stream_context_alloc(TSRMLS_D);
JONJAPI int jonj_stream_context_get_option(jonj_stream_context *context,
		const char *wrappername, const char *optionname, zval ***optionvalue);
JONJAPI int jonj_stream_context_set_option(jonj_stream_context *context,
		const char *wrappername, const char *optionname, zval *optionvalue);

JONJAPI jonj_stream_notifier *jonj_stream_notification_alloc(void);
JONJAPI void jonj_stream_notification_free(jonj_stream_notifier *notifier);
END_EXTERN_C()

/* not all notification codes are implemented */
#define JONJ_STREAM_NOTIFY_RESOLVE		1
#define JONJ_STREAM_NOTIFY_CONNECT		2
#define JONJ_STREAM_NOTIFY_AUTH_REQUIRED		3
#define JONJ_STREAM_NOTIFY_MIME_TYPE_IS	4
#define JONJ_STREAM_NOTIFY_FILE_SIZE_IS	5
#define JONJ_STREAM_NOTIFY_REDIRECTED	6
#define JONJ_STREAM_NOTIFY_PROGRESS		7
#define JONJ_STREAM_NOTIFY_COMPLETED		8
#define JONJ_STREAM_NOTIFY_FAILURE		9
#define JONJ_STREAM_NOTIFY_AUTH_RESULT	10

#define JONJ_STREAM_NOTIFY_SEVERITY_INFO	0
#define JONJ_STREAM_NOTIFY_SEVERITY_WARN	1
#define JONJ_STREAM_NOTIFY_SEVERITY_ERR	2

BEGIN_EXTERN_C()
JONJAPI void jonj_stream_notification_notify(jonj_stream_context *context, int notifycode, int severity,
		char *xmsg, int xcode, size_t bytes_sofar, size_t bytes_max, void * ptr TSRMLS_DC);
JONJAPI jonj_stream_context *jonj_stream_context_set(jonj_stream *stream, jonj_stream_context *context);
END_EXTERN_C()

#define jonj_stream_notify_info(context, code, xmsg, xcode)	do { if ((context) && (context)->notifier) { \
	jonj_stream_notification_notify((context), (code), JONJ_STREAM_NOTIFY_SEVERITY_INFO, \
				(xmsg), (xcode), 0, 0, NULL TSRMLS_CC); } } while (0)
			
#define jonj_stream_notify_progress(context, bsofar, bmax) do { if ((context) && (context)->notifier) { \
	jonj_stream_notification_notify((context), JONJ_STREAM_NOTIFY_PROGRESS, JONJ_STREAM_NOTIFY_SEVERITY_INFO, \
			NULL, 0, (bsofar), (bmax), NULL TSRMLS_CC); } } while(0)

#define jonj_stream_notify_progress_init(context, sofar, bmax) do { if ((context) && (context)->notifier) { \
	(context)->notifier->progress = (sofar); \
	(context)->notifier->progress_max = (bmax); \
	(context)->notifier->mask |= JONJ_STREAM_NOTIFIER_PROGRESS; \
	jonj_stream_notify_progress((context), (sofar), (bmax)); } } while (0)

#define jonj_stream_notify_progress_increment(context, dsofar, dmax) do { if ((context) && (context)->notifier && (context)->notifier->mask & JONJ_STREAM_NOTIFIER_PROGRESS) { \
	(context)->notifier->progress += (dsofar); \
	(context)->notifier->progress_max += (dmax); \
	jonj_stream_notify_progress((context), (context)->notifier->progress, (context)->notifier->progress_max); } } while (0)

#define jonj_stream_notify_file_size(context, file_size, xmsg, xcode) do { if ((context) && (context)->notifier) { \
	jonj_stream_notification_notify((context), JONJ_STREAM_NOTIFY_FILE_SIZE_IS, JONJ_STREAM_NOTIFY_SEVERITY_INFO, \
			(xmsg), (xcode), 0, (file_size), NULL TSRMLS_CC); } } while(0)
	
#define jonj_stream_notify_error(context, code, xmsg, xcode) do { if ((context) && (context)->notifier) {\
	jonj_stream_notification_notify((context), (code), JONJ_STREAM_NOTIFY_SEVERITY_ERR, \
			(xmsg), (xcode), 0, 0, NULL TSRMLS_CC); } } while(0)
	

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
