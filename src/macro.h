#ifndef MACRO_H
#define MACRO_H

#define error_minusone(exp,msg,cmd) if((exp) == -1){printf("error: %s\n",msg); cmd;}

#endif