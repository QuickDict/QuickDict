#include "utils.h"
#include <QString>

FILE *fopen_unicode(const char *pathname, const char *mode)
{
#if defined(_MSC_VER) || defined(__MINGW64__)
    return _wfopen(QString(pathname).toStdWString().c_str(), QString(mode).toStdWString().c_str());
#else
    return fopen(pathname, mode);
#endif
}
