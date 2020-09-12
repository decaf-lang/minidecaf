#ifndef _ERROR_H_
#define _ERROR_H_

#include "lexer.h"

void init_error(char*, char*);
void error(const char *fmt, ...);
void error_at(char *loc, const char *fmt, ...);
void error_tok(TKPtr tok, const char *fmt, ...);
void warn_tok(TKPtr tok, const char *fmt, ...);

#endif // _ERROR_H_