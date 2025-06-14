/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _EXAMPLE_H_RPCGEN
#define _EXAMPLE_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define MAXUSERNAME 32
#define MAXFILELEN 65535
#define MAXNAMELEN 255

enum filekind {
	TEXT = 0,
	DATA = 1,
	EXEC = 2,
};
typedef enum filekind filekind;

struct filetype {
	filekind kind;
	union {
		char *creator;
		char *interpretor;
	} filetype_u;
};
typedef struct filetype filetype;

struct file {
	char *filename;
	filetype type;
	char *owner;
	struct {
		u_int data_len;
		char *data_val;
	} data;
};
typedef struct file file;

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_filekind (XDR *, filekind*);
extern  bool_t xdr_filetype (XDR *, filetype*);
extern  bool_t xdr_file (XDR *, file*);

#else /* K&R C */
extern bool_t xdr_filekind ();
extern bool_t xdr_filetype ();
extern bool_t xdr_file ();

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

#endif /* !_EXAMPLE_H_RPCGEN */
