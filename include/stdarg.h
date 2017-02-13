
#ifndef INCLUDE_STDARG_H
#define INCLUDE_STDARG_H

typedef int* va_list;

#define va_start(ap, v) ((void) (ap = (va_list) &v + sizeof(v)))
#define va_end(ap) ((void) (ap = 0))
#define va_arg(ap) (*(ap)++)

#endif
