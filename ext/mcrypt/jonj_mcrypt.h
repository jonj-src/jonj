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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef JONJ_MCRYPT_H
#define JONJ_MCRYPT_H

#if HAVE_LIBMCRYPT

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry mcrypt_module_entry;
#define mcrypt_module_ptr &mcrypt_module_entry

/* Functions for both old and new API */
JONJ_FUNCTION(mcrypt_ecb);
JONJ_FUNCTION(mcrypt_cbc);
JONJ_FUNCTION(mcrypt_cfb);
JONJ_FUNCTION(mcrypt_ofb);
JONJ_FUNCTION(mcrypt_get_cipher_name);
JONJ_FUNCTION(mcrypt_get_block_size);
JONJ_FUNCTION(mcrypt_get_key_size);
JONJ_FUNCTION(mcrypt_create_iv);

/* Support functions for old API */
JONJ_FUNCTION(mcrypt_list_algorithms);
JONJ_FUNCTION(mcrypt_list_modes);
JONJ_FUNCTION(mcrypt_get_iv_size);
JONJ_FUNCTION(mcrypt_encrypt);
JONJ_FUNCTION(mcrypt_decrypt);

/* Functions for new API */
JONJ_FUNCTION(mcrypt_module_open);
JONJ_FUNCTION(mcrypt_generic_init);
JONJ_FUNCTION(mcrypt_generic);
JONJ_FUNCTION(mdecrypt_generic);
JONJ_FUNCTION(mcrypt_generic_deinit);

JONJ_FUNCTION(mcrypt_enc_self_test);
JONJ_FUNCTION(mcrypt_enc_is_block_algorithm_mode);
JONJ_FUNCTION(mcrypt_enc_is_block_algorithm);
JONJ_FUNCTION(mcrypt_enc_is_block_mode);
JONJ_FUNCTION(mcrypt_enc_get_block_size);
JONJ_FUNCTION(mcrypt_enc_get_key_size);
JONJ_FUNCTION(mcrypt_enc_get_supported_key_sizes);
JONJ_FUNCTION(mcrypt_enc_get_iv_size);
JONJ_FUNCTION(mcrypt_enc_get_algorithms_name);
JONJ_FUNCTION(mcrypt_enc_get_modes_name);
JONJ_FUNCTION(mcrypt_module_self_test);
JONJ_FUNCTION(mcrypt_module_is_block_algorithm_mode);
JONJ_FUNCTION(mcrypt_module_is_block_algorithm);
JONJ_FUNCTION(mcrypt_module_is_block_mode);
JONJ_FUNCTION(mcrypt_module_get_algo_block_size);
JONJ_FUNCTION(mcrypt_module_get_algo_key_size);
JONJ_FUNCTION(mcrypt_module_get_supported_key_sizes);
JONJ_FUNCTION(mcrypt_module_close);

ZEND_BEGIN_MODULE_GLOBALS(mcrypt)
	int le_h;
	char *modes_dir;
	char *algorithms_dir;
	int fd[2]; // RANDOM = 0, URANDOM = 1
ZEND_END_MODULE_GLOBALS(mcrypt)

#ifdef ZTS
# define MCG(v)    TSRMG(mcrypt_globals_id, zend_mcrypt_globals *, v)
#else
# define MCG(v)    (mcrypt_globals.v)
#endif

#else
#define mcrypt_module_ptr NULL
#endif

#define jonjext_mcrypt_ptr mcrypt_module_ptr

#endif
