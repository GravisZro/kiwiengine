#ifndef ERROR_REPORTING_H
#define ERROR_REPORTING_H

#include <QString>

# ifndef complaint
#  define complaint(x) QString("%1: %2").arg(__func__, x)
# endif


#endif // ERROR_REPORTING_H
