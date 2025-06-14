/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _LICZBY_H_RPCGEN
#define _LICZBY_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define MAXNUMBERS 65535

struct request {
	struct {
		u_int numbers_len;
		int *numbers_val;
	} numbers;
};
typedef struct request request;

struct response {
	bool_t error;
	union {
		int suma;
		char *errmsg;
	} response_u;
};
typedef struct response response;

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_request (XDR *, request*);
extern  bool_t xdr_response (XDR *, response*);

#else /* K&R C */
extern bool_t xdr_request ();
extern bool_t xdr_response ();

#endif /* K&R C */
struct rpcgen_table {
	char	*(*proc)();
	xdrproc_t	xdr_arg;
	unsigned	len_arg;
	xdrproc_t	xdr_res;
	unsigned	len_res;
};

#ifdef __cplusplus
}
#endif

#endif /* !_LICZBY_H_RPCGEN */
