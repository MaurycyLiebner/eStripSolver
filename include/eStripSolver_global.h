#ifndef ESTRIPSOLVER_GLOBAL_H
#define ESTRIPSOLVER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ESTRIPSOLVER_LIBRARY)
#  define ESTRIPSOLVER_EXPORT Q_DECL_EXPORT
#else
#  define ESTRIPSOLVER_EXPORT Q_DECL_IMPORT
#endif

#endif // ESTRIPSOLVER_GLOBAL_H
