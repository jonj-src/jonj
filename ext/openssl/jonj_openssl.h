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
   | Authors: Stig Venaas <venaas@jonj.tk>                                |
   |          Wez Furlong <wez@thebrainroom.com                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_OPENSSL_H
#define JONJ_OPENSSL_H
/* HAVE_OPENSSL would include SSL MySQL stuff */
#ifdef HAVE_OPENSSL_EXT
extern zend_module_entry openssl_module_entry;
#define jonjext_openssl_ptr &openssl_module_entry

#define OPENSSL_RAW_DATA 1
#define OPENSSL_ZERO_PADDING 2

#define OPENSSL_ERROR_X509_PRIVATE_KEY_VALUES_MISMATCH 0x0B080074

/* Used for client-initiated handshake renegotiation DoS protection*/
#define OPENSSL_DEFAULT_RENEG_LIMIT 2
#define OPENSSL_DEFAULT_RENEG_WINDOW 300
#define OPENSSL_DEFAULT_STREAM_VERIFY_DEPTH 9
#define OPENSSL_DEFAULT_STREAM_CIPHERS "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:" \
	"ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:" \
	"DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:" \
	"ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:" \
	"ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:" \
	"DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:" \
	"AES256-GCM-SHA384:AES128:AES256:HIGH:!SSLv2:!aNULL:!eNULL:!EXPORT:!DES:!MD5:!RC4:!ADH"

jonj_stream_transport_factory_func jonj_openssl_ssl_socket_factory;

JONJ_MINIT_FUNCTION(openssl);
JONJ_MSHUTDOWN_FUNCTION(openssl);
JONJ_MINFO_FUNCTION(openssl);

JONJ_FUNCTION(openssl_pkey_get_private);
JONJ_FUNCTION(openssl_pkey_get_public);
JONJ_FUNCTION(openssl_pkey_free);
JONJ_FUNCTION(openssl_pkey_new);
JONJ_FUNCTION(openssl_pkey_export);
JONJ_FUNCTION(openssl_pkey_export_to_file);
JONJ_FUNCTION(openssl_pkey_get_details);

JONJ_FUNCTION(openssl_sign);
JONJ_FUNCTION(openssl_verify);
JONJ_FUNCTION(openssl_seal);
JONJ_FUNCTION(openssl_open);
JONJ_FUNCTION(openssl_private_encrypt);
JONJ_FUNCTION(openssl_private_decrypt);
JONJ_FUNCTION(openssl_public_encrypt);
JONJ_FUNCTION(openssl_public_decrypt);

JONJ_FUNCTION(openssl_pbkdf2);

JONJ_FUNCTION(openssl_pkcs7_verify);
JONJ_FUNCTION(openssl_pkcs7_decrypt);
JONJ_FUNCTION(openssl_pkcs7_sign);
JONJ_FUNCTION(openssl_pkcs7_encrypt);

JONJ_FUNCTION(openssl_error_string);

JONJ_FUNCTION(openssl_x509_read);
JONJ_FUNCTION(openssl_x509_free);
JONJ_FUNCTION(openssl_x509_parse);
JONJ_FUNCTION(openssl_x509_checkpurpose);
JONJ_FUNCTION(openssl_x509_export);
JONJ_FUNCTION(openssl_x509_fingerprint);
JONJ_FUNCTION(openssl_x509_export_to_file);
JONJ_FUNCTION(openssl_x509_check_private_key);

JONJ_FUNCTION(openssl_pkcs12_export);
JONJ_FUNCTION(openssl_pkcs12_export_to_file);
JONJ_FUNCTION(openssl_pkcs12_read);

JONJ_FUNCTION(openssl_csr_new);
JONJ_FUNCTION(openssl_csr_export);
JONJ_FUNCTION(openssl_csr_export_to_file);
JONJ_FUNCTION(openssl_csr_sign);
JONJ_FUNCTION(openssl_csr_get_subject);
JONJ_FUNCTION(openssl_csr_get_public_key);

JONJ_FUNCTION(openssl_spki_new);
JONJ_FUNCTION(openssl_spki_verify);
JONJ_FUNCTION(openssl_spki_export);
JONJ_FUNCTION(openssl_spki_export_challenge);

JONJ_FUNCTION(openssl_get_cert_locations);
#else

#define jonjext_openssl_ptr NULL

#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
