/*
 *
 * This header configures the level of verbosity of the TSManager module
 * Verbose level can range from TS_LEVEL_MUTE (No messages) to TS_LEVEL_DEBUG (All messages)
 * To set the level of verbosity, modify macro TS_PRINT_MESSAGES_LEVEL a few lines below
 *
 */

#ifndef TS_TSCONFIG_H
#define TS_TSCONFIG_H

namespace TS_CPP{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT " AT " __FILE__ ":" TOSTRING(__LINE__)

#define TS_LEVEL_DEBUG 3
#define TS_LEVEL_INFO 2
#define TS_LEVEL_ERROR 1
#define TS_LEVEL_MUTE 0

#ifndef TS_DISPLACEMENT_ATTR_NAME
/// This macro set the name of the builtin attribute name
#define TS_DISPLACEMENT_ATTR_NAME "Despl"
#endif

#ifndef TS_SIZE_ATTR_NAME
    /// This macro set the name of the builtin attribute name
#define TS_SIZE_ATTR_NAME "Size"
#endif

#ifndef TS_PRINT_MESSAGES_LEVEL
/// This macro set the level of verbosity of the TSManager to one of this values: TS_LEVEL_MUTE, TS_LEVEL_ERROR, TS_LEVEL_INFO, TS_LEVEL_DEBUG
#define TS_PRINT_MESSAGES_LEVEL TS_LEVEL_ERROR // CHANGE THIS LINE TO SET THE LEVEL OF PRINT MESSAGES
#endif

#ifndef TS_NULLSTRINGREPLACEMENT
/// If a nullptr is placed inside an attribute value of type string of an entry, this string will be used instead for better human readability
#define TS_NULLSTRINGREPLACEMENT "'-'"
#endif

#if TS_PRINT_MESSAGES_LEVEL == TS_LEVEL_DEBUG
#define tsdebug(...) {fprintf(stderr,"TS_DEBUG: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr,"\n"); fflush(stderr);}
#else
#define tsdebug(...)
#endif

#if TS_PRINT_MESSAGES_LEVEL >= TS_LEVEL_INFO
#define tsinfo(...) {fprintf(stderr,"TS_INFO: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr,"\n"); fflush(stderr);}
#else
#define tsinfo(...)
#endif

#if TS_PRINT_MESSAGES_LEVEL >= TS_LEVEL_ERROR
#define tserror(...) {fprintf(stderr,"TS_ERROR:" AT "; "); fprintf(stderr, __VA_ARGS__); fprintf(stderr,"\n"); fflush(stderr);}
#else
#define tserror(...)
#endif

}

#endif //TS_TSCONFIG_H
