/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025 E-comOS Kernel Mode Team & Saladin5101
 */
#include <eclib/shutdown.h>
#include <eclib/ipc_message.h>
#include <eclib/capability.h>
#include <eclib/service.h>
#include <syscall.h>

#define SYS_SHUTDOWN 2001
#define SYS_REBOOT   2002

static struct {
    shutdown_callback_t callback;
    void *user_data;
} g_callbacks[32];

static int g_callback_count = 0;

int shutdown_system(enum shutdown_reason reason, uint32_t flags,
                    const char *message) {
    if (!check_capability(CAPABILITY_SHUTDOWN)) return -1;
    if (!(flags & SHUTDOWN_FLAG_NOWAIT))
        ipc_broadcast_msg(IPC_MSG_SHUTDOWN_REQUEST, flags, 0, 0);
    return (int)syscall(SYS_SHUTDOWN, (long)reason, (long)flags, (long)message);
}

int reboot_system(enum shutdown_reason reason, uint32_t flags,
                  const char *message) {
    if (!check_capability(CAPABILITY_REBOOT)) return -1;
    if (!(flags & SHUTDOWN_FLAG_NOWAIT))
        ipc_broadcast_msg(IPC_MSG_SHUTDOWN_REQUEST, flags, 0, 0);
    return (int)syscall(SYS_REBOOT, (long)reason, (long)flags, (long)message);
}

int register_shutdown_callback(shutdown_callback_t callback, void *user_data) {
    if (!callback || g_callback_count >= 32) return -1;
    for (int i = 0; i < g_callback_count; i++)
        if (g_callbacks[i].callback == callback) return -2;
    g_callbacks[g_callback_count].callback  = callback;
    g_callbacks[g_callback_count].user_data = user_data;
    g_callback_count++;
    return 0;
}

int unregister_shutdown_callback(shutdown_callback_t callback) {
    for (int i = 0; i < g_callback_count; i++) {
        if (g_callbacks[i].callback == callback) {
            for (int j = i; j < g_callback_count - 1; j++)
                g_callbacks[j] = g_callbacks[j + 1];
            g_callback_count--;
            return 0;
        }
    }
    return -1;
}

int get_shutdown_status(void)      { return (int)syscall(SYS_SHUTDOWN, -1L, 0L, 0L); }
int cancel_shutdown(void)          { return (int)syscall(SYS_SHUTDOWN, -2L, 0L, 0L); }

int appendix_s_begin_save(void) {
    ipc_do_not_kill_sub();
    return ipc_send_msg(IPC_MSG_APPENDIX_S_WAIT, 0, eclib_getpid(), 0, 0);
}

int appendix_s_save_complete(void) {
    return ipc_send_msg(IPC_MSG_APPENDIX_S_OKTHANKS, 0, eclib_getpid(), 0, 0);
}

int appendix_s_get_remaining_time(void) {
    return (int)syscall(SYS_SHUTDOWN, -3L, 0L, 0L);
}
