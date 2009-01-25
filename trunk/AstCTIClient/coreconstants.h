#ifndef CORECONSTANTS_H
#define CORECONSTANTS_H


#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 0
#define APP_VERSION_RELEASE 1

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define APP_VERSION STRINGIFY(APP_VERSION_MAJOR) \
    "." STRINGIFY(APP_VERSION_MINOR) \
    "." STRINGIFY(APP_VERSION_RELEASE)

const char * const APP_VERSION_LONG      = APP_VERSION;
const char * const APP_AUTHOR            = "centralino-voip";
const char * const APP_YEAR              = "2009";

#ifdef APP_REVISION
const char * const APP_REVISION_STR      = STRINGIFY(APP_REVISION);
#else
const char * const APP_REVISION_STR      = "";
#endif

#undef APP_VERSION
#undef STRINGIFY
#undef STRINGIFY_INTERNAL

#define LINKS_TIMEOUT  10

#endif // CORECONSTANTS_H
