#ifndef JONJ_SENDRECVMSG_H
#define JONJ_SENDRECVMSG_H 1

#include <jonj.h>
#include "conversions.h"

/* for sockets.c */
JONJ_FUNCTION(socket_sendmsg);
JONJ_FUNCTION(socket_recvmsg);
JONJ_FUNCTION(socket_cmsg_space);

void jonj_socket_sendrecvmsg_init(INIT_FUNC_ARGS);
void jonj_socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS);

int jonj_do_setsockopt_ipv6_rfc3542(jonj_socket *jonj_sock, int level, int optname, zval **arg4 TSRMLS_DC);
int jonj_do_getsockopt_ipv6_rfc3542(jonj_socket *jonj_sock, int level, int optname, zval *result TSRMLS_DC);

/* for conversions.c */
typedef struct {
	int	cmsg_level;	/* originating protocol */
	int	cmsg_type;	/* protocol-specific type */
} anc_reg_key;

typedef size_t (calculate_req_space)(const zval *value, ser_context *ctx);

typedef struct {
	socklen_t size; /* size of native structure */
	socklen_t var_el_size; /* size of repeatable component */
	calculate_req_space *calc_space;
	from_zval_write_field *from_array;
	to_zval_read_field *to_array;
} ancillary_reg_entry;

ancillary_reg_entry *get_ancillary_reg_entry(int cmsg_level, int msg_type);

#endif
