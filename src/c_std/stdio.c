/**
 * ECLib - E-comOS C Library
// Re-implemented C99 stdio using SLFD as the low-level interface.
/* Include project stdio explicitly to ensure the project's FILE type is used */
#include "../../include/stdio.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "slfd/io.h"
#include "slfd/syscall.h"

/* Simple FILE table to manage open streams (C stdlib-compatible facade).
 * Implementation notes:
 * - FILE.fd holds the SLFD fid_t (cast to int)
 * - Standard streams (stdin/stdout/stderr) map to fds 0/1/2
 */
static FILE file_table[FOPEN_MAX];
static int file_used[FOPEN_MAX];

static FILE _stdin = { .fd = 0, .buf = NULL, .buf_size = BUFSIZ, .error = 0, .eof = 0 };
static FILE _stdout = { .fd = 1, .buf = NULL, .buf_size = BUFSIZ, .error = 0, .eof = 0 };
static FILE _stderr = { .fd = 2, .buf = NULL, .buf_size = BUFSIZ, .error = 0, .eof = 0 };

FILE* stdin = &_stdin;
FILE* stdout = &_stdout;
FILE* stderr = &_stderr;

/* Helper: allocate FILE slot (skip 0/1/2 reserved) */
static FILE* file_alloc(void) {
    for (int i = 3; i < FOPEN_MAX; i++) {
        if (!file_used[i]) {
            file_used[i] = 1;
            file_table[i].fd = -1;
            file_table[i].buf = NULL;
            file_table[i].buf_size = BUFSIZ;
            file_table[i].error = 0;
            file_table[i].eof = 0;
            return &file_table[i];
        }
    }
    return NULL;
}

static void file_free(FILE* f) {
    if (!f) return;
    int idx = f - file_table;
    if (idx >= 0 && idx < FOPEN_MAX) file_used[idx] = 0;
}

/* fopen/fclose */
FILE* fopen(const char* filename, const char* mode) {
    if (!filename || !mode) return NULL;
    FILE* f = file_alloc();
    if (!f) return NULL;
    fid_t fid = open("f", mode, filename);
    if (fid < 0) {
        file_free(f);
        return NULL;
    }
    f->fd = (int)fid;
    return f;
}

int fclose(FILE* stream) {
    if (!stream) return EOF;
    int r = close("f", (fid_t)stream->fd);
    file_free(stream);
    return (r == 0) ? 0 : EOF;
}

/* fread/fwrite */
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!ptr || !stream) return 0;
    size_t total = size * nmemb;
    ssize_t r = read("f", (fid_t)stream->fd, ptr, total);
    if (r < 0) {
        stream->error = 1;
        return 0;
    }
    return (size_t)r / size;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!ptr || !stream) return 0;
    size_t total = size * nmemb;
    /* Prefer direct syscall for stdout/stderr to avoid file service overhead */
    if (stream == stdout || stream == stderr) {
        ssize_t r = slfd_sys_write(stream->fd, ptr, total);
        if (r < 0) { stream->error = 1; return 0; }
        return (size_t)r / size;
    }
    ssize_t r = write("f", (fid_t)stream->fd, ptr, total);
    if (r < 0) { stream->error = 1; return 0; }
    return (size_t)r / size;
}

/* putchar/puts/fputc/putc */
int fputc(int c, FILE* stream) {
    if (!stream) return EOF;
    char ch = (char)c;
    ssize_t r;
    if (stream == stdout || stream == stderr) r = slfd_sys_write(stream->fd, &ch, 1);
    else r = write("f", (fid_t)stream->fd, &ch, 1);
    if (r < 0) { stream->error = 1; return EOF; }
    return (int)ch;
}

int putc(int c, FILE* stream) { return fputc(c, stream); }

int putchar(int c) { return fputc(c, stdout); }

int puts(const char* s) {
    if (!s) return EOF;
    size_t len = strlen(s);
    ssize_t r = slfd_sys_write(stdout->fd, s, len);
    if (r < 0) { stdout->error = 1; return EOF; }
    /* write newline */
    char nl = '\n';
    slfd_sys_write(stdout->fd, &nl, 1);
    return 0;
}

/* fgetc/getc/getchar (basic implementations using slfd_sys_read for stdin) */
int fgetc(FILE* stream) {
    if (!stream) return EOF;
    char ch;
    ssize_t r;
    if (stream == stdin) r = slfd_sys_read(stream->fd, &ch, 1);
    else r = read("f", (fid_t)stream->fd, &ch, 1);
    if (r <= 0) { stream->eof = 1; stream->error = (r<0); return EOF; }
    return (int)(unsigned char)ch;
}

int getc(FILE* stream) { return fgetc(stream); }

int getchar(void) { return fgetc(stdin); }

/* fseek/ftell (use io_seek shim) */
int fseek(FILE* stream, long int offset, int whence) {
    if (!stream) return EOF;
    off_t r = io_seek("f", (fid_t)stream->fd, (off_t)offset, whence);
    if (r < 0) { stream->error = 1; return EOF; }
    return 0;
}

long int ftell(FILE* stream) {
    if (!stream) return -1;
    /* Seek with 0, SEEK_CUR to get position */
    off_t pos = io_seek("f", (fid_t)stream->fd, 0, SEEK_CUR);
    if (pos < 0) { stream->error = 1; return -1; }
    return (long)pos;
}

void rewind(FILE* stream) { if (stream) fseek(stream, 0, SEEK_SET); }

int fgetpos(FILE* stream, fpos_t* pos) { if (!stream||!pos) return EOF; long v = ftell(stream); if (v<0) return EOF; *pos = (fpos_t)v; return 0; }
int fsetpos(FILE* stream, const fpos_t* pos) { if (!stream||!pos) return EOF; return fseek(stream, (long)*pos, SEEK_SET); }

/* fflush stub (no buffering implemented) */
int fflush(FILE *stream) { (void)stream; return 0; }

/* error handling */
void clearerr(FILE* stream) { if (stream) { stream->error = 0; stream->eof = 0; } }
int feof(FILE* stream) { return stream ? stream->eof : 0; }
int ferror(FILE* stream) { return stream ? stream->error : 0; }

void perror(const char* s) { (void)s; /* Could map to strerror and write to stderr */ }

/* Other stubs left as simple placeholders (formatted I/O left unimplemented for brevity) */
int printf(const char* format, ...) { (void)format; return EOF; }
int fprintf(FILE* stream, const char* format, ...) { (void)stream; (void)format; return EOF; }
int sprintf(char* s, const char* format, ...) { (void)s; (void)format; return EOF; }
int snprintf(char* s, size_t n, const char* format, ...) { (void)s; (void)n; (void)format; return EOF; }
int scanf(const char* format, ...) { (void)format; return EOF; }
int fscanf(FILE* stream, const char* format, ...) { (void)stream; (void)format; return EOF; }
int sscanf(const char* s, const char* format, ...) { (void)s; (void)format; return EOF; }

char* fgets(char* s, int n, FILE* stream) { (void)s; (void)n; (void)stream; return NULL; }
int fputc(int c, FILE* stream) { return fputc(c, stream); }
int putc_unlocked(int c, FILE* stream) { return fputc(c, stream); }

void rewind(FILE* stream) { (void)stream; }
int fgetpos(FILE* stream, fpos_t* pos) { (void)stream; (void)pos; return EOF; }
int fsetpos(FILE* stream, const fpos_t* pos) { (void)stream; (void)pos; return EOF; }

// 7.23.8 Error Handling Stubs
void clearerr(FILE* stream) { (void)stream; }
int feof(FILE* stream) { (void)stream; return 0; }
int ferror(FILE* stream) { (void)stream; return 0; }
void perror(const char* s) { (void)s; }

// 7.23.9 File Operation Stubs
FILE* fopen(const char* filename, const char* mode) { (void)filename; (void)mode; return NULL; }
int fclose(FILE* stream) { (void)stream; return EOF; }
int remove(const char* filename) { (void)filename; return EOF; }
int rename(const char* old, const char* new) { (void)old; (void)new; return EOF; }
FILE* tmpfile(void) { return NULL; }
char* tmpnam(char* s) { (void)s; return NULL; }