/*
 * ECLib - E-comOS C Library
 * C99 stdio implementation using SLFD as the low-level interface.
 */
#include "../../include/stdio.h"
#include "../../include/stdint.h"
#include "../../include/stddef.h"
#include <slfd.h>

static FILE file_table[FOPEN_MAX];
static int  file_used[FOPEN_MAX];

static FILE _stdin  = {0, 0, BUFSIZ, 0, 0};
static FILE _stdout = {1, 0, BUFSIZ, 0, 0};
static FILE _stderr = {2, 0, BUFSIZ, 0, 0};

FILE *stdin  = &_stdin;
FILE *stdout = &_stdout;
FILE *stderr = &_stderr;

static FILE *file_alloc(void) {
    for (int i = 3; i < FOPEN_MAX; i++) {
        if (!file_used[i]) {
            file_used[i] = 1;
            file_table[i] = (FILE){-1, 0, BUFSIZ, 0, 0};
            return &file_table[i];
        }
    }
    return 0;
}

static void file_free(FILE *f) {
    if (!f) return;
    int idx = (int)(f - file_table);
    if (idx >= 0 && idx < FOPEN_MAX) file_used[idx] = 0;
}

FILE *fopen(const char *filename, const char *mode) {
    if (!filename || !mode) return 0;
    FILE *f = file_alloc();
    if (!f) return 0;
    fid_t fid = open("f", mode, filename);
    if (fid < 0) { file_free(f); return 0; }
    f->fd = (int)fid;
    return f;
}

int fclose(FILE *stream) {
    if (!stream) return EOF;
    close("f", (fid_t)stream->fd);
    file_free(stream);
    return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!ptr || !stream || size == 0) return 0;
    ssize_t r = read("f", (fid_t)stream->fd, ptr, size * nmemb);
    if (r < 0) { stream->error = 1; return 0; }
    return (size_t)r / size;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!ptr || !stream || size == 0) return 0;
    ssize_t r = write("f", (fid_t)stream->fd, ptr, size * nmemb);
    if (r < 0) { stream->error = 1; return 0; }
    return (size_t)r / size;
}

int fputc(int c, FILE *stream) {
    if (!stream) return EOF;
    char ch = (char)c;
    ssize_t r = write("f", (fid_t)stream->fd, &ch, 1);
    if (r < 0) { stream->error = 1; return EOF; }
    return (unsigned char)ch;
}

int putc(int c, FILE *stream) { return fputc(c, stream); }
int putchar(int c)             { return fputc(c, stdout); }

int puts(const char *s) {
    if (!s) return EOF;
    size_t len = 0;
    while (s[len]) len++;
    if (write("f", stdout->fd, s, len) < 0) return EOF;
    char nl = '\n';
    write("f", stdout->fd, &nl, 1);
    return 0;
}

int fgetc(FILE *stream) {
    if (!stream) return EOF;
    char ch;
    ssize_t r = read("f", (fid_t)stream->fd, &ch, 1);
    if (r <= 0) { stream->eof = (r == 0); stream->error = (r < 0); return EOF; }
    return (unsigned char)ch;
}

int getc(FILE *stream) { return fgetc(stream); }
int getchar(void)      { return fgetc(stdin); }

char *fgets(char *s, int n, FILE *stream) {
    if (!s || n <= 0 || !stream) return 0;
    int i = 0;
    for (; i < n - 1; i++) {
        int c = fgetc(stream);
        if (c == EOF) break;
        s[i] = (char)c;
        if (c == '\n') { i++; break; }
    }
    if (i == 0) return 0;
    s[i] = '\0';
    return s;
}

int fseek(FILE *stream, long offset, int whence) {
    if (!stream) return EOF;
    off_t r = io_seek("f", (fid_t)stream->fd, (off_t)offset, whence);
    if (r < 0) { stream->error = 1; return EOF; }
    return 0;
}

long ftell(FILE *stream) {
    if (!stream) return -1;
    off_t pos = io_seek("f", (fid_t)stream->fd, 0, SEEK_CUR);
    if (pos < 0) { stream->error = 1; return -1; }
    return (long)pos;
}

void rewind(FILE *stream)                        { if (stream) fseek(stream, 0, SEEK_SET); }
int  fgetpos(FILE *stream, fpos_t *pos)          { if (!stream || !pos) return EOF; long v = ftell(stream); if (v < 0) return EOF; *pos = (fpos_t)v; return 0; }
int  fsetpos(FILE *stream, const fpos_t *pos)    { if (!stream || !pos) return EOF; return fseek(stream, (long)*pos, SEEK_SET); }
int  fflush(FILE *stream)                        { (void)stream; return 0; }
void clearerr(FILE *stream)                      { if (stream) { stream->error = 0; stream->eof = 0; } }
int  feof(FILE *stream)                          { return stream ? stream->eof   : 0; }
int  ferror(FILE *stream)                        { return stream ? stream->error : 0; }
void perror(const char *s)                       { if (s) puts(s); }
int  remove(const char *filename)                { (void)filename; return -1; }
int  rename(const char *old, const char *newname){ (void)old; (void)newname; return -1; }
FILE *tmpfile(void)                              { return 0; }
char *tmpnam(char *s)                            { (void)s; return 0; }

int printf(const char *fmt, ...)                              { (void)fmt; return EOF; }
int fprintf(FILE *stream, const char *fmt, ...)               { (void)stream; (void)fmt; return EOF; }
int sprintf(char *s, const char *fmt, ...)                    { (void)s; (void)fmt; return EOF; }
int snprintf(char *s, size_t n, const char *fmt, ...)         { (void)s; (void)n; (void)fmt; return EOF; }
int scanf(const char *fmt, ...)                               { (void)fmt; return EOF; }
int fscanf(FILE *stream, const char *fmt, ...)                { (void)stream; (void)fmt; return EOF; }
int sscanf(const char *s, const char *fmt, ...)               { (void)s; (void)fmt; return EOF; }
