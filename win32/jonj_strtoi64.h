#if _MSC_VERS <= 1300
#include "jonj.h"
#include "jonj_stdint.h"

JONJAPI int64_t _strtoi64(const char *nptr, char **endptr, int base);
#define _strtoui64 _strtoi64
#endif
