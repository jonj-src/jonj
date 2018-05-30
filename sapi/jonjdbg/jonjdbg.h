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
   | Authors: Felipe Pena <felipe@jonj.tk>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@jonj.tk>                                |
   +----------------------------------------------------------------------+
*/

#ifndef JONJDBG_H
#define JONJDBG_H

#ifdef JONJ_WIN32
# define JONJDBG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define JONJDBG_API __attribute__ ((visibility("default")))
#else
# define JONJDBG_API
#endif

#include "jonj.h"
#include "jonj_globals.h"
#include "jonj_variables.h"
#include "jonj_getopt.h"
#include "zend_builtin_functions.h"
#include "zend_extensions.h"
#include "zend_modules.h"
#include "zend_globals.h"
#include "zend_ini_scanner.h"
#include "zend_stream.h"
#ifndef _WIN32
# include "zend_signal.h"
#endif
#include "SAPI.h"
#include <fcntl.h>
#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
# include <windows.h>
# include "config.w32.h"
# undef  strcasecmp
# undef  strncasecmp
# define strcasecmp _stricmp 
# define strncasecmp _strnicmp 
#else
# include "jonj_config.h"
#endif
#ifndef O_BINARY
#	define O_BINARY 0
#endif
#include "jonj_main.h"

#ifdef ZTS
# include "TSRM.h"
#endif

#ifdef LIBREADLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif
#ifdef HAVE_LIBEDIT
#   include <editline/readline.h>
#endif

#include "jonjdbg_lexer.h"
#include "jonjdbg_cmd.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_btree.h"
#include "jonjdbg_watch.h"

int jonjdbg_do_parse(jonjdbg_param_t *stack, char *input TSRMLS_DC);

#ifdef ZTS
# define JONJDBG_G(v) TSRMG(jonjdbg_globals_id, zend_jonjdbg_globals *, v)
#else
# define JONJDBG_G(v) (jonjdbg_globals.v)
#endif

#define JONJDBG_NEXT   2
#define JONJDBG_UNTIL  3
#define JONJDBG_FINISH 4
#define JONJDBG_LEAVE  5

/*
 BEGIN: DO NOT CHANGE DO NOT CHANGE DO NOT CHANGE
*/

/* {{{ tables */
#define JONJDBG_BREAK_FILE            0
#define JONJDBG_BREAK_SYM             1
#define JONJDBG_BREAK_OPLINE          2
#define JONJDBG_BREAK_METHOD          3
#define JONJDBG_BREAK_COND            4
#define JONJDBG_BREAK_OPCODE          5
#define JONJDBG_BREAK_FUNCTION_OPLINE 6
#define JONJDBG_BREAK_METHOD_OPLINE   7
#define JONJDBG_BREAK_FILE_OPLINE     8
#define JONJDBG_BREAK_MAP             9
#define JONJDBG_BREAK_TABLES          10 /* }}} */

/* {{{ flags */
#define JONJDBG_HAS_FILE_BP            (1<<1)
#define JONJDBG_HAS_SYM_BP             (1<<2)
#define JONJDBG_HAS_OPLINE_BP          (1<<3)
#define JONJDBG_HAS_METHOD_BP          (1<<4)
#define JONJDBG_HAS_COND_BP            (1<<5)
#define JONJDBG_HAS_OPCODE_BP          (1<<6)
#define JONJDBG_HAS_FUNCTION_OPLINE_BP (1<<7)
#define JONJDBG_HAS_METHOD_OPLINE_BP   (1<<8)
#define JONJDBG_HAS_FILE_OPLINE_BP     (1<<9) /* }}} */

/*
 END: DO NOT CHANGE DO NOT CHANGE DO NOT CHANGE
*/

#define JONJDBG_IN_COND_BP             (1<<10)
#define JONJDBG_IN_EVAL                (1<<11)

#define JONJDBG_IS_STEPPING            (1<<12)
#define JONJDBG_STEP_OPCODE            (1<<13)
#define JONJDBG_IS_QUIET               (1<<14)
#define JONJDBG_IS_QUITTING            (1<<15)
#define JONJDBG_IS_COLOURED            (1<<16)
#define JONJDBG_IS_CLEANING            (1<<17)

#define JONJDBG_IN_UNTIL               (1<<18)
#define JONJDBG_IN_FINISH              (1<<19)
#define JONJDBG_IN_LEAVE               (1<<20)

#define JONJDBG_IS_REGISTERED          (1<<21)
#define JONJDBG_IS_STEPONEVAL          (1<<22)
#define JONJDBG_IS_INITIALIZING        (1<<23)
#define JONJDBG_IS_SIGNALED            (1<<24)
#define JONJDBG_IS_INTERACTIVE         (1<<25)
#define JONJDBG_IS_BP_ENABLED          (1<<26)
#define JONJDBG_IS_REMOTE              (1<<27)
#define JONJDBG_IS_DISCONNECTED        (1<<28)

#define JONJDBG_SHOW_REFCOUNTS         (1<<29)

#define JONJDBG_SEEK_MASK              (JONJDBG_IN_UNTIL|JONJDBG_IN_FINISH|JONJDBG_IN_LEAVE)
#define JONJDBG_BP_RESOLVE_MASK		  (JONJDBG_HAS_FUNCTION_OPLINE_BP|JONJDBG_HAS_METHOD_OPLINE_BP|JONJDBG_HAS_FILE_OPLINE_BP)
#define JONJDBG_BP_MASK                (JONJDBG_HAS_FILE_BP|JONJDBG_HAS_SYM_BP|JONJDBG_HAS_METHOD_BP|JONJDBG_HAS_OPLINE_BP|JONJDBG_HAS_COND_BP|JONJDBG_HAS_OPCODE_BP|JONJDBG_HAS_FUNCTION_OPLINE_BP|JONJDBG_HAS_METHOD_OPLINE_BP|JONJDBG_HAS_FILE_OPLINE_BP)

#ifndef _WIN32
#	define JONJDBG_DEFAULT_FLAGS (JONJDBG_IS_QUIET|JONJDBG_IS_COLOURED|JONJDBG_IS_BP_ENABLED)
#else
#	define JONJDBG_DEFAULT_FLAGS (JONJDBG_IS_QUIET|JONJDBG_IS_BP_ENABLED)
#endif /* }}} */

/* {{{ strings */
#define JONJDBG_NAME "jonjdbg"
#define JONJDBG_AUTHORS "Felipe Pena, Joe Watkins and Bob Weinand" /* Ordered by last name */
#define JONJDBG_URL "http://jonjdbg.com"
#define JONJDBG_ISSUES "http://github.com/krakjoe/jonjdbg/issues"
#define JONJDBG_VERSION "0.4.0"
#define JONJDBG_INIT_FILENAME ".jonjdbginit"
/* }}} */

/* {{{ output descriptors */
#define JONJDBG_STDIN 			0
#define JONJDBG_STDOUT			1
#define JONJDBG_STDERR			2
#define JONJDBG_IO_FDS 			3 /* }}} */


/* {{{ structs */
ZEND_BEGIN_MODULE_GLOBALS(jonjdbg)
	HashTable bp[JONJDBG_BREAK_TABLES];           /* break points */
	HashTable registered;                        /* registered */
	HashTable seek;                              /* seek oplines */
	jonjdbg_frame_t frame;                        /* frame */
	zend_uint last_line;                         /* last executed line */

	jonjdbg_lexer_data lexer;                     /* lexer data */
	jonjdbg_param_t *parser_stack;                /* param stack during lexer / parser phase */

#ifndef _WIN32
	struct sigaction old_sigsegv_signal;         /* segv signal handler */
#endif
	jonjdbg_btree watchpoint_tree;                /* tree with watchpoints */
	jonjdbg_btree watch_HashTables;               /* tree with original dtors of watchpoints */
	HashTable watchpoints;                       /* watchpoints */
	zend_llist watchlist_mem;                    /* triggered watchpoints */
	zend_bool watchpoint_hit;                    /* a watchpoint was hit */
	void (*original_free_function)(void *);      /* the original AG(mm_heap)->_free function */

	char *exec;                                  /* file to execute */
	size_t exec_len;                             /* size of exec */
	zend_op_array *ops;                 	     /* op_array */
	zval *retval;                                /* return value */
	int bp_count;                                /* breakpoint count */
	int vmret;                                   /* return from last opcode handler execution */

	FILE *oplog;                                 /* opline log */
	FILE *io[JONJDBG_IO_FDS];                     /* io */

	char *prompt[2];                             /* prompt */
	const jonjdbg_color_t *colors[JONJDBG_COLORS]; /* colors */
	char *buffer;                                /* buffer */

	zend_ulong flags;                            /* jonjdbg flags */
ZEND_END_MODULE_GLOBALS(jonjdbg) /* }}} */

/* the beginning (= the important part) of the _zend_mm_heap struct defined in Zend/zend_alloc.c
   Needed for realizing watchpoints */
struct _zend_mm_heap {
	int   use_zend_alloc;
	void *(*_malloc)(size_t);
	void  (*_free)(void *);
	void *(*_realloc)(void *, size_t);
	size_t              free_bitmap;
	size_t              large_free_bitmap;
	size_t              block_size;
	size_t              compact_size;
	zend_mm_segment    *segments_list;
	zend_mm_storage    *storage;
};

#endif /* JONJDBG_H */
