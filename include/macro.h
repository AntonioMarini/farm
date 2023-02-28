#ifndef MACRO_H
#define MACRO_H

#include<stdlib.h>

#define error_minusone(exp,msg,cmd) if((exp) == -1){perror(msg); cmd;}

#define error_null(exp,msg,cmd) if((exp) == NULL){perror(msg); cmd;}

#endif