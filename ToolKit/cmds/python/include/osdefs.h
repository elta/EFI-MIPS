/* Portions Copyright (c) 2000 by Intel Corporation. */

#ifndef Py_OSDEFS_H
#define Py_OSDEFS_H
#ifdef __cplusplus
extern "C" {
#endif

/* Operating system dependencies */

/* Mod by chrish: QNX has WATCOM, but isn't DOS */
#if !defined(__QNX__)
#if defined(MS_WINDOWS) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__DJGPP__) || defined(PYOS_OS2)
#if defined(PYOS_OS2) && defined(PYCC_GCC)
#define MAXPATHLEN 260
#define SEP '/'
#define ALTSEP '\\'
#else
#define SEP '\\'
#define ALTSEP '/'
#define MAXPATHLEN 256
#endif
#define DELIM ';'
#endif
#endif

#if defined(EFI32) || defined(EFI64) || defined(EFIX64)
#define SEP '/'
#define DELIM ';'
#endif

#ifdef RISCOS
#define SEP '.'
#define MAXPATHLEN 256
#define DELIM ','
#endif


/* Filename separator */
#ifndef SEP
#define SEP '/'
#endif

/* Max pathname length */
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

/* Search path entry delimiter */
#ifndef DELIM
#define DELIM ':'
#endif

#ifdef __cplusplus
}
#endif
#endif /* !Py_OSDEFS_H */
