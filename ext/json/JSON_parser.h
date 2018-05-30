/* JSON_parser.h */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "jonj.h"
#include "ext/standard/jonj_smart_str.h"
#include "jonj_json.h"

#define JSON_PARSER_DEFAULT_DEPTH 512

typedef struct JSON_parser_struct {
    int state;
    int depth;
    int top;
	int error_code;
    int* stack;
    zval **the_zstack;
    zval *the_static_zstack[JSON_PARSER_DEFAULT_DEPTH];
} * JSON_parser;

enum error_codes {
	JONJ_JSON_ERROR_NONE = 0,
    JONJ_JSON_ERROR_DEPTH, 
    JONJ_JSON_ERROR_STATE_MISMATCH,  
    JONJ_JSON_ERROR_CTRL_CHAR,   
    JONJ_JSON_ERROR_SYNTAX,
    JONJ_JSON_ERROR_UTF8,
    JONJ_JSON_ERROR_RECURSION,
    JONJ_JSON_ERROR_INF_OR_NAN,
    JONJ_JSON_ERROR_UNSUPPORTED_TYPE
};

extern JSON_parser new_JSON_parser(int depth);
extern int parse_JSON_ex(JSON_parser jp, zval *z, unsigned short utf16_json[], int length, int options TSRMLS_DC);
extern int free_JSON_parser(JSON_parser jp);

static inline int parse_JSON(JSON_parser jp, zval *z, unsigned short utf16_json[], int length, int assoc TSRMLS_DC)
{
	return parse_JSON_ex(jp, z, utf16_json, length, assoc ? JONJ_JSON_OBJECT_AS_ARRAY : 0 TSRMLS_CC);
}

#endif
