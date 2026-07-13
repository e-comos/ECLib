/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025 E-comOS Kernel Mode Team & Saladin5101
 *
 * This file is part of ECLib.
 * ECLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 */
#include <string.h>
#include <errno.h>

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else {
        for (size_t i = n; i > 0; i--) d[i-1] = s[i-1];
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    for (size_t i = 0; i < n; i++) p[i] = (unsigned char)c;
    return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* a = s1;
    const unsigned char* b = s2;
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = s;
    for (size_t i = 0; i < n; i++) {
        if (p[i] == (unsigned char)c) return (void*)(p + i);
    }
    return 0;
}

size_t strlen(const char* s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* restrict dest, const char* restrict src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) dest[i] = src[i];
    for (; i < n; i++) dest[i] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest + strlen(dest);
    while ((*d++ = *src++));
    return dest;
}

char* strncat(char* restrict dest, const char* restrict src, size_t n) {
    char* d = dest + strlen(dest);
    size_t i;
    for (i = 0; i < n && src[i]; i++) d[i] = src[i];
    d[i] = '\0';
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || !s1[i])
            return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}

char* strchr(const char* s, int c) {
    for (; *s; s++) if (*s == (char)c) return (char*)s;
    return (c == '\0') ? (char*)s : 0;
}

char* strrchr(const char* s, int c) {
    const char* last = 0;
    for (; *s; s++) if (*s == (char)c) last = s;
    return (c == '\0') ? (char*)s : (char*)last;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    size_t nlen = strlen(needle);
    for (; *haystack; haystack++) {
        if (memcmp(haystack, needle, nlen) == 0) return (char*)haystack;
    }
    return 0;
}

char* strpbrk(const char* s1, const char* s2) {
    for (; *s1; s1++) {
        for (const char* p = s2; *p; p++) {
            if (*s1 == *p) return (char*)s1;
        }
    }
    return 0;
}

size_t strspn(const char* s1, const char* s2) {
    size_t n = 0;
    for (; s1[n]; n++) {
        int found = 0;
        for (size_t j = 0; s2[j]; j++) if (s1[n] == s2[j]) { found = 1; break; }
        if (!found) break;
    }
    return n;
}

size_t strcspn(const char* s1, const char* s2) {
    size_t n = 0;
    for (; s1[n]; n++) {
        for (size_t j = 0; s2[j]; j++) if (s1[n] == s2[j]) return n;
    }
    return n;
}

static char* strtok_state = 0;
char* strtok(char* restrict str, const char* restrict delim) {
    if (str) strtok_state = str;
    if (!strtok_state) return 0;
    strtok_state += strspn(strtok_state, delim);
    if (!*strtok_state) return 0;
    char* token = strtok_state;
    strtok_state += strcspn(strtok_state, delim);
    if (*strtok_state) *strtok_state++ = '\0';
    return token;
}

char* strerror(int errnum) {
    switch (errnum) {
        case 0:   return "No error";
        case 1:   return "Operation not permitted";
        case 2:   return "No such file or directory";
        case 3:   return "No such process";
        case 4:   return "Interrupted system call";
        case 5:   return "I/O error";
        case 8:   return "Exec format error";
        case 9:   return "Bad file descriptor";
        case 10:  return "No child process";
        case 11:  return "Resource temporarily unavailable";
        case 12:  return "Out of memory";
        case 13:  return "Permission denied";
        case 15:  return "Invalid argument";
        case 100: return "IPC error";
        default:  return "Unknown error";
    }
}
