#ifndef ELOG_DEMANGLE_H_
#define ELOG_DEMANGLE_H_

#ifdef _MSC_VER
# include "demangle_msvc.h"
#else
# include "demangle_gxx.h"
#endif

#endif  // ELOG_DEMANGLE_H_
