/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2008-2016 The JONJ Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Pierre A. Joye <pierre@jonj.tk>                             |
   +----------------------------------------------------------------------+
 */

#include "jonj.h"

#include <windows.h>
#include <Winbase.h >
#include <Windns.h>

#include "jonj_dns.h"

#define JONJ_DNS_NUM_TYPES	12	/* Number of DNS Types Supported by JONJ currently */

#define JONJ_DNS_A      0x00000001
#define JONJ_DNS_NS     0x00000002
#define JONJ_DNS_CNAME  0x00000010
#define JONJ_DNS_SOA    0x00000020
#define JONJ_DNS_PTR    0x00000800
#define JONJ_DNS_HINFO  0x00001000
#define JONJ_DNS_MX     0x00004000
#define JONJ_DNS_TXT    0x00008000
#define JONJ_DNS_A6     0x01000000
#define JONJ_DNS_SRV    0x02000000
#define JONJ_DNS_NAPTR  0x04000000
#define JONJ_DNS_AAAA   0x08000000
#define JONJ_DNS_ANY    0x10000000
#define JONJ_DNS_ALL    (JONJ_DNS_A|JONJ_DNS_NS|JONJ_DNS_CNAME|JONJ_DNS_SOA|JONJ_DNS_PTR|JONJ_DNS_HINFO|JONJ_DNS_MX|JONJ_DNS_TXT|JONJ_DNS_A6|JONJ_DNS_SRV|JONJ_DNS_NAPTR|JONJ_DNS_AAAA)

JONJ_FUNCTION(dns_get_mx) /* {{{ */
{
	char *hostname;
	int hostname_len;
	zval *mx_list, *weight_list = NULL;

	DNS_STATUS      status;                 /* Return value of DnsQuery_A() function */
	PDNS_RECORD     pResult, pRec;          /* Pointer to DNS_RECORD structure */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &hostname, &hostname_len, &mx_list, &weight_list) == FAILURE) {
		return;
	}

	status = DnsQuery_A(hostname, DNS_TYPE_MX, DNS_QUERY_STANDARD, NULL, &pResult, NULL);

	if (status) {
		RETURN_FALSE;
	}

	zval_dtor(mx_list);
	array_init(mx_list);

	if (weight_list) {
		zval_dtor(weight_list);
		array_init(weight_list);
	}

	for (pRec = pResult; pRec; pRec = pRec->pNext) {
		DNS_SRV_DATA *srv = &pRec->Data.Srv;

		if (pRec->wType != DNS_TYPE_MX) {
			continue;
		}

		add_next_index_string(mx_list, pRec->Data.MX.pNameExchange, 1);
		if (weight_list) {
			add_next_index_long(weight_list, srv->wPriority);
		}
	}

	/* Free memory allocated for DNS records. */
	DnsRecordListFree(pResult, DnsFreeRecordListDeep);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool dns_check_record(string host [, string type])
   Check DNS records corresponding to a given Internet host name or IP address */
JONJ_FUNCTION(dns_check_record)
{
	char *hostname, *rectype = NULL;
	int hostname_len, rectype_len = 0;
	int type = DNS_TYPE_MX;

	DNS_STATUS      status;                 /* Return value of DnsQuery_A() function */
	PDNS_RECORD     pResult;          /* Pointer to DNS_RECORD structure */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &hostname, &hostname_len, &rectype, &rectype_len) == FAILURE) {
		return;
	}

	if (hostname_len == 0) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Host cannot be empty");
		RETURN_FALSE;
	}

	if (rectype) {
		     if (!strcasecmp("A",     rectype)) type = DNS_TYPE_A;
		else if (!strcasecmp("NS",    rectype)) type = DNS_TYPE_NS;
		else if (!strcasecmp("MX",    rectype)) type = DNS_TYPE_MX;
		else if (!strcasecmp("PTR",   rectype)) type = DNS_TYPE_PTR;
		else if (!strcasecmp("ANY",   rectype)) type = DNS_TYPE_ANY;
		else if (!strcasecmp("SOA",   rectype)) type = DNS_TYPE_SOA;
		else if (!strcasecmp("TXT",   rectype)) type = DNS_TYPE_TEXT;
		else if (!strcasecmp("CNAME", rectype)) type = DNS_TYPE_CNAME;
		else if (!strcasecmp("AAAA",  rectype)) type = DNS_TYPE_AAAA;
		else if (!strcasecmp("SRV",   rectype)) type = DNS_TYPE_SRV;
		else if (!strcasecmp("NAPTR", rectype)) type = DNS_TYPE_NAPTR;
		else if (!strcasecmp("A6",    rectype)) type = DNS_TYPE_A6;
		else {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%s' not supported", rectype);
			RETURN_FALSE;
		}
	}

	status = DnsQuery_A(hostname, type, DNS_QUERY_STANDARD, NULL, &pResult, NULL);

	if (status) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ jonj_parserr */
static void jonj_parserr(PDNS_RECORD pRec, int type_to_fetch, int store, int raw, zval **subarray)
{
	int type;
	u_long ttl;

	type = pRec->wType;
	ttl = pRec->dwTtl;

	if (type_to_fetch != DNS_TYPE_ANY && type != type_to_fetch) {
		return;
	}

	if (!store) {
		return;
	}

	ALLOC_INIT_ZVAL(*subarray);
	array_init(*subarray);

	add_assoc_string(*subarray, "host", pRec->pName, 1);
	add_assoc_string(*subarray, "class", "IN", 1);
	add_assoc_long(*subarray, "ttl", ttl);

	if (raw) {
		add_assoc_long(*subarray, "type", type);
		add_assoc_stringl(*subarray, "data", (char*) &pRec->Data, (uint) pRec->wDataLength, 1);
		return;
	}

	switch (type) {
		case DNS_TYPE_A: {
			IN_ADDR ipaddr;
			ipaddr.S_un.S_addr = (pRec->Data.A.IpAddress);
			add_assoc_string(*subarray, "type", "A", 1);
			add_assoc_string(*subarray, "ip", inet_ntoa(ipaddr), 1);
			break;
		}

		case DNS_TYPE_MX:
			add_assoc_string(*subarray, "type", "MX", 1);
			add_assoc_long(*subarray, "pri", pRec->Data.Srv.wPriority);
			/* no break; */

		case DNS_TYPE_CNAME:
			if (type == DNS_TYPE_CNAME) {
				add_assoc_string(*subarray, "type", "CNAME", 1);
			}
			/* no break; */

		case DNS_TYPE_NS:
			if (type == DNS_TYPE_NS) {
				add_assoc_string(*subarray, "type", "NS", 1);
			}
			/* no break; */

		case DNS_TYPE_PTR:
			if (type == DNS_TYPE_PTR) {
				add_assoc_string(*subarray, "type", "PTR", 1);
			}
			add_assoc_string(*subarray, "target", pRec->Data.MX.pNameExchange, 1);
			break;

		/* Not available on windows, the query is possible but there is no DNS_HINFO_DATA structure */
		case DNS_TYPE_HINFO:
		case DNS_TYPE_TEXT:
			{
				DWORD i = 0;
				DNS_TXT_DATA *data_txt = &pRec->Data.TXT;
				DWORD count = data_txt->dwStringCount;
				char *txt, *txt_dst;
				long txt_len = 0;
				zval *entries;

				add_assoc_string(*subarray, "type", "TXT", 1);
				
				ALLOC_INIT_ZVAL(entries);
				array_init(entries);
				
				for (i = 0; i < count; i++) {
					txt_len += strlen(data_txt->pStringArray[i]) + 1;
				}

				txt = ecalloc(txt_len * 2, 1);
				txt_dst = txt;
				for (i = 0; i < count; i++) {
					int len = strlen(data_txt->pStringArray[i]);
					memcpy(txt_dst, data_txt->pStringArray[i], len);
					add_next_index_stringl(entries, data_txt->pStringArray[i], len, 1);
					txt_dst += len;
				}

				add_assoc_string(*subarray, "txt", txt, 0);
				add_assoc_zval(*subarray, "entries", entries);
			}
			break;

		case DNS_TYPE_SOA:
			{
				DNS_SOA_DATA *data_soa = &pRec->Data.Soa;

				add_assoc_string(*subarray, "type", "SOA", 1);

				add_assoc_string(*subarray, "mname", data_soa->pNamePrimaryServer, 1);
				add_assoc_string(*subarray, "rname", data_soa->pNameAdministrator, 1);
				add_assoc_long(*subarray, "serial", data_soa->dwSerialNo);
				add_assoc_long(*subarray, "refresh", data_soa->dwRefresh);
				add_assoc_long(*subarray, "retry", data_soa->dwRetry);
				add_assoc_long(*subarray, "expire", data_soa->dwExpire);
				add_assoc_long(*subarray, "minimum-ttl", data_soa->dwDefaultTtl);
			}
			break;

		case DNS_TYPE_AAAA:
			{
				DNS_AAAA_DATA *data_aaaa = &pRec->Data.AAAA;
				char buf[sizeof("AAAA:AAAA:AAAA:AAAA:AAAA:AAAA:AAAA:AAAA")];
				char *tp = buf;
				int i;
				unsigned short out[8];
				int have_v6_break = 0, in_v6_break = 0;

				for (i = 0; i < 4; ++i) {
					DWORD chunk = data_aaaa->Ip6Address.IP6Dword[i];
					out[i * 2]     = htons(LOWORD(chunk));
					out[i * 2 + 1] = htons(HIWORD(chunk));
				}

				for(i=0; i < 8; i++) {
					if (out[i] != 0) {
						if (tp > (u_char *)buf) {
							in_v6_break = 0;
							tp[0] = ':';
							tp++;
						}
						tp += sprintf((char*)tp,"%x", out[i]);
					} else {
						if (!have_v6_break) {
							have_v6_break = 1;
							in_v6_break = 1;
							tp[0] = ':';
							tp++;
						} else if (!in_v6_break) {
							tp[0] = ':';
							tp++;
							tp[0] = '0';
							tp++;
						}
					}
				}

				if (have_v6_break && in_v6_break) {
					tp[0] = ':';
					tp++;
				}
				tp[0] = '\0';

				add_assoc_string(*subarray, "type", "AAAA", 1);
				add_assoc_string(*subarray, "ipv6", buf, 1);
			}
			break;

#if 0
		/* Won't be implemented. A6 is deprecated. (Pierre) */
		case DNS_TYPE_A6:
			break;
#endif

		case DNS_TYPE_SRV:
			{
				DNS_SRV_DATA *data_srv = &pRec->Data.Srv;

				add_assoc_string(*subarray, "type", "SRV", 1);
				add_assoc_long(*subarray, "pri", data_srv->wPriority);
				add_assoc_long(*subarray, "weight", data_srv->wWeight);
				add_assoc_long(*subarray, "port", data_srv->wPort);
				add_assoc_string(*subarray, "target", data_srv->pNameTarget, 1);
			}
			break;

#if _MSC_VER >= 1500
		case DNS_TYPE_NAPTR:
			{
				DNS_NAPTR_DATA * data_naptr = &pRec->Data.Naptr;

				add_assoc_string(*subarray, "type", "NAPTR", 1);
				add_assoc_long(*subarray, "order", data_naptr->wOrder);
				add_assoc_long(*subarray, "pref", data_naptr->wPreference);
				add_assoc_string(*subarray, "flags", data_naptr->pFlags, 1);
				add_assoc_string(*subarray, "services", data_naptr->pService, 1);
				add_assoc_string(*subarray, "regex", data_naptr->pRegularExpression, 1);
				add_assoc_string(*subarray, "replacement", data_naptr->pReplacement, 1);
			}
			break;
#endif

		default:
			/* unknown type */
			zval_ptr_dtor(subarray);
			*subarray = NULL;
			return;
	}

}
/* }}} */

/* {{{ proto array|false dns_get_record(string hostname [, int type[, array authns, array addtl]])
   Get any Resource Record corresponding to a given Internet host name */
JONJ_FUNCTION(dns_get_record)
{
	char *hostname;
	int hostname_len;
	long type_param = JONJ_DNS_ANY;
	zval *authns = NULL, *addtl = NULL;
	int type, type_to_fetch, first_query = 1, store_results = 1;
	zend_bool raw = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz!z!b",
			&hostname, &hostname_len, &type_param, &authns, &addtl, &raw) == FAILURE) {
		return;
	}

	if (authns) {
		zval_dtor(authns);
		array_init(authns);
	}
	if (addtl) {
		zval_dtor(addtl);
		array_init(addtl);
	}

	if (!raw) {
		if ((type_param & ~JONJ_DNS_ALL) && (type_param != JONJ_DNS_ANY)) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Type '%ld' not supported", type_param);
			RETURN_FALSE;
		}
	} else {
		if ((type_param < 1) || (type_param > 0xFFFF)) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Numeric DNS record type must be between 1 and 65535, '%ld' given", type_param);
			RETURN_FALSE;
		}
	}

	/* Initialize the return array */
	array_init(return_value);

	if (raw) {
		type = -1;
	} else if (type_param == JONJ_DNS_ANY) {
		type = JONJ_DNS_NUM_TYPES + 1;
	} else {
		type = 0;
	}

	for ( ;
		type < (addtl ? (JONJ_DNS_NUM_TYPES + 2) : JONJ_DNS_NUM_TYPES) || first_query;
		type++
	) {
		DNS_STATUS      status;                 /* Return value of DnsQuery_A() function */
		PDNS_RECORD     pResult, pRec;          /* Pointer to DNS_RECORD structure */

		first_query = 0;
		switch (type) {
			case -1: /* raw */
				type_to_fetch = type_param;
				/* skip over the rest and go directly to additional records */
				type = JONJ_DNS_NUM_TYPES - 1;
				break;
			case 0:
				type_to_fetch = type_param&JONJ_DNS_A     ? DNS_TYPE_A     : 0;
				break;
			case 1:
				type_to_fetch = type_param&JONJ_DNS_NS    ? DNS_TYPE_NS    : 0;
				break;
			case 2:
				type_to_fetch = type_param&JONJ_DNS_CNAME ? DNS_TYPE_CNAME : 0;
				break;
			case 3:
				type_to_fetch = type_param&JONJ_DNS_SOA   ? DNS_TYPE_SOA   : 0;
				break;
			case 4:
				type_to_fetch = type_param&JONJ_DNS_PTR   ? DNS_TYPE_PTR   : 0;
				break;
			case 5:
				type_to_fetch = type_param&JONJ_DNS_HINFO ? DNS_TYPE_HINFO : 0;
				break;
			case 6:
				type_to_fetch = type_param&JONJ_DNS_MX    ? DNS_TYPE_MX    : 0;
				break;
			case 7:
				type_to_fetch = type_param&JONJ_DNS_TXT   ? DNS_TYPE_TEXT   : 0;
				break;
			case 8:
				type_to_fetch = type_param&JONJ_DNS_AAAA	 ? DNS_TYPE_AAAA  : 0;
				break;
			case 9:
				type_to_fetch = type_param&JONJ_DNS_SRV   ? DNS_TYPE_SRV   : 0;
				break;
			case 10:
				type_to_fetch = type_param&JONJ_DNS_NAPTR ? DNS_TYPE_NAPTR : 0;
				break;
			case 11:
				type_to_fetch = type_param&JONJ_DNS_A6	 ? DNS_TYPE_A6 : 0;
				break;
			case JONJ_DNS_NUM_TYPES:
				store_results = 0;
				continue;
			default:
			case (JONJ_DNS_NUM_TYPES + 1):
				type_to_fetch = DNS_TYPE_ANY;
				break;
		}

		if (type_to_fetch) {
			status = DnsQuery_A(hostname, type_to_fetch, DNS_QUERY_STANDARD, NULL, &pResult, NULL);

			if (status) {
				if (status == DNS_INFO_NO_RECORDS || status == DNS_ERROR_RCODE_NAME_ERROR) {
					continue;
				} else {
					jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "DNS Query failed");
					zval_dtor(return_value);
					RETURN_FALSE;
				}
			}

			for (pRec = pResult; pRec; pRec = pRec->pNext) {
				DNS_SRV_DATA *srv = &pRec->Data.Srv;
				zval *retval = NULL;

				if (pRec->Flags.S.Section == DnsSectionAnswer) {
					jonj_parserr(pRec, type_to_fetch, store_results, raw, &retval);
					if (retval != NULL && store_results) {
						add_next_index_zval(return_value, retval);
					}
				}

				if (authns && pRec->Flags.S.Section == DnsSectionAuthority) {

					jonj_parserr(pRec, type_to_fetch, 1, raw, &retval);
					if (retval != NULL) {
						add_next_index_zval(authns, retval);
					}
				}

/* Stupid typo in PSDK 6.1, WinDNS.h(1258)... */
#ifndef DnsSectionAdditional
# ifdef DnsSectionAddtional
#  define DnsSectionAdditional DnsSectionAddtional
# else
# define DnsSectionAdditional 3
# endif
#endif
				if (addtl && pRec->Flags.S.Section == DnsSectionAdditional) {
					jonj_parserr(pRec, type_to_fetch, 1, raw, &retval);
					if (retval != NULL) {
						add_next_index_zval(addtl, retval);
					}
				}
			}
			/* Free memory allocated for DNS records. */
			DnsRecordListFree(pResult, DnsFreeRecordListDeep);
		}
	}
}
/* }}} */
