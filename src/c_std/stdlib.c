#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int atoi(const char *nptr) {
    return (int)strtol(nptr, NULL, 10);
}

long int atol(const char *nptr) {
    return strtol(nptr, NULL, 10);
}

long long int atoll(const char *nptr) {
    return strtoll(nptr, NULL, 10);
}

long int strtol(const char *restrict nptr, char **restrict endptr, int base) {
    const char *s = nptr;
    unsigned long int acc = 0;
    int c;
    unsigned long int cutoff;
    int neg = 0, any, cutlim;

    /* Skip white space and find sign */
    while (*s == ' ' || (*s >= '\t' && *s <= '\r'))
        s++;
    
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    /* If base is 0, pick base heuristic */
    if (base == 0) {
        if (*s == '0') {
            if (s[1] == 'x' || s[1] == 'X') {
                base = 16;
                s += 2;
            } else if (s[1] == 'b' || s[1] == 'B') {
                base = 2;
                s += 2;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (*s == '0' && (s[1] == 'x' || s[1] == 'X'))
            s += 2;
    } else if (base == 2) {
        if (*s == '0' && (s[1] == 'b' || s[1] == 'B'))
            s += 2;
    }

    cutoff = neg ? -(unsigned long int)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long int)base;
    cutoff /= (unsigned long int)base;

    for (acc = 0, any = 0;; c = *s++, c = (unsigned char)c) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;

        if (c >= base)
            break;

        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= (unsigned long int)base;
            acc += c;
        }
    }

    if (any < 0) {
        acc = neg ? (unsigned long int)LONG_MIN : (unsigned long int)LONG_MAX;
        errno = ERANGE;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != NULL)
        *endptr = (char *)(any ? s - 1 : nptr);

    return (long int)acc;
}

long long int strtoll(const char *restrict nptr, char **restrict endptr, int base) {
    // Mirroring strtol logic for long long range
    return (long long int)strtol(nptr, endptr, base);
}

double atof(const char *nptr) {
    return strtod(nptr, NULL);
}

double strtod(const char *restrict nptr, char **restrict endptr) {
    // Basic stub or floating-point parser implementation
    if (endptr) *endptr = (char *)nptr;
    return 0.0;
}
