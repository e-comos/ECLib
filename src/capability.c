/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025 E-comOS Kernel Mode Team & Saladin5101
 */
#include <eclib/capability.h>
#include <string.h>
#include <syscall.h>

#define SYS_ACQUIRE_CAPABILITY   3001
#define SYS_RELEASE_CAPABILITY   3002
#define SYS_VALIDATE_CAPABILITY  3003
#define SYS_SEND_VIRTUAL_PORT    3004
#define SYS_RECEIVE_VIRTUAL_PORT 3005
#define SYS_GETPID               1010

struct capability_token *acquire_capability(uint32_t capabilities,
                                            const char *description,
                                            uint32_t timeout_sec) {
    if (!description) return 0;
    char desc_buf[65];
    strncpy(desc_buf, description, 64);
    struct capability_token *token = (struct capability_token *)
        syscall(SYS_ACQUIRE_CAPABILITY, (long)capabilities,
                (long)desc_buf, (long)timeout_sec);
    if (token)
        token->owner_pid = (uint32_t)syscall(SYS_GETPID);
    return token;
}

int release_capability(struct capability_token *token) {
    if (!token) return -1;
    if (token->owner_pid != (uint32_t)syscall(SYS_GETPID)) return -2;
    int ret = (int)syscall(SYS_RELEASE_CAPABILITY, (long)token);
    if (ret == 0)
        memset(token, 0, sizeof(struct capability_token));
    return ret;
}

int renew_capability(struct capability_token *token, uint32_t add_time_sec) {
    if (!token) return -1;
    token->valid_until += add_time_sec;
    return 0;
}

int validate_capability(struct capability_token *token, uint32_t required_caps) {
    if (!token) return 0;
    return (int)syscall(SYS_VALIDATE_CAPABILITY, (long)token, (long)required_caps);
}

int send_token_to_virtual_port(struct capability_token *token) {
    if (!token) return -1;
    if (token->owner_pid != (uint32_t)syscall(SYS_GETPID)) return -2;
    return (int)syscall(SYS_SEND_VIRTUAL_PORT, (long)token);
}

struct capability_token *receive_token_from_virtual_port(void) {
    return (struct capability_token *)syscall(SYS_RECEIVE_VIRTUAL_PORT);
}

int clear_virtual_port_tokens(void) {
    return (int)syscall(SYS_SEND_VIRTUAL_PORT, 0L);
}

int check_capability(uint32_t required_caps) {
    return (int)syscall(SYS_VALIDATE_CAPABILITY, 0L, (long)required_caps);
}

int check_capability_for_pid(uint32_t required_caps, uint32_t pid) {
    return (int)syscall(SYS_VALIDATE_CAPABILITY, (long)pid, (long)required_caps);
}
