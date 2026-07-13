/*
 * ECLib - E-comOS C Library
 * SLFD Specification v10.2 - <slfd/ula.h>
 */
#ifndef SYS_ULA_H
#define SYS_ULA_H

#define LV_HIGHEST  0
#define LV_GENERAL  1
#define LV_LOW      2
#define LV_GETTING  3

#define ULA_CREATOR_SHIFT   24
#define ULA_CALLER_SHIFT    16
#define ULA_FLAG_EXEC       (1<<8)
#define ULA_FLAG_HIDDEN     (1<<9)
#define ULA_FLAG_ARCHIVE    (1<<10)
#define ULA_TYPE_FILE       (0<<4)
#define ULA_TYPE_DIR        (1<<4)
#define ULA_TYPE_LINK       (2<<4)
#define ULA_TYPE_DEV        (3<<4)

#define ULA_MAKE_ATTR(creator, caller, flags, type) \
    (((creator) << ULA_CREATOR_SHIFT) | ((caller) << ULA_CALLER_SHIFT) | (flags) | (type))

#endif /* SYS_ULA_H */
