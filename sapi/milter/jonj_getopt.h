/* Borrowed from Apache NT Port */
#include "jonj.h"

extern char *ap_jonj_optarg;
extern int ap_jonj_optind;

int ap_jonj_getopt(int argc, char* const *argv, const char *optstr);
