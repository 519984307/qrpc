/**
  @file
  @author Stefan Frings
*/

#ifndef HTTPGLOBAL_H
#define HTTPGLOBAL_H

#include <QtGlobal>

// This is specific to Windows dll's
#if defined(Q_OS_WIN)
    #if defined(QTWEBAPPLIB_EXPORT)
        #define DECLSPEC Q_DECL_EXPORT
    #elif defined(QTWEBAPPLIB_IMPORT)
        #define DECLSPEC Q_DECL_IMPORT
#endif
#endif
#if !defined(DECLSPEC)
    #define DECLSPEC
#endif

#define XDebug1(v1)

#define XDebug2(v1,v2)

#define XDebug3(v1,v2,v3)

#define XDebug4(v1,v2,v3,v4)


//    qDebug(v,x)

/** Get the library version number */
DECLSPEC const char* getQtWebAppLibVersion();


#endif // HTTPGLOBAL_H

