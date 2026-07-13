/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025 E-comOS Kernel Mode Team & Saladin5101
 */
#include <eclib/service.h>
#include <eclib/ipc_message.h>
#include <string.h>
#include <syscall.h>

#define SYS_GETPID 1010

static struct {
    struct service_info services[256];
    int count;
    int next_id;
} g_registry = {0};

int service_register(const char *name) {
    if (!name || strlen(name) >= 64) return -1;
    if (g_registry.count >= 256) return -2;
    for (int i = 0; i < g_registry.count; i++)
        if (strcmp(g_registry.services[i].name, name) == 0) return -3;
    struct service_info *svc = &g_registry.services[g_registry.count];
    svc->id  = g_registry.next_id++;
    strncpy(svc->name, name, 63);
    svc->state = SERVICE_RUNNING;
    svc->pid   = (int)syscall(SYS_GETPID);
    g_registry.count++;
    ipc_broadcast_msg(0x53455256, 0, 0, 0);
    return svc->id;
}

int service_unregister(int service_id) {
    for (int i = 0; i < g_registry.count; i++) {
        if (g_registry.services[i].id == service_id) {
            for (int j = i; j < g_registry.count - 1; j++)
                g_registry.services[j] = g_registry.services[j + 1];
            g_registry.count--;
            ipc_broadcast_msg(0x53455256, 0, 0, 0);
            return 0;
        }
    }
    return -1;
}

int service_set_state(int service_id, enum service_state state) {
    for (int i = 0; i < g_registry.count; i++) {
        if (g_registry.services[i].id == service_id) {
            g_registry.services[i].state = state;
            return 0;
        }
    }
    return -1;
}

int service_heartbeat(int service_id) {
    for (int i = 0; i < g_registry.count; i++) {
        if (g_registry.services[i].id == service_id)
            return 0;
    }
    return -1;
}

int service_find_by_name(const char *name, struct service_info *info) {
    if (!name || !info) return -1;
    for (int i = 0; i < g_registry.count; i++) {
        if (strcmp(g_registry.services[i].name, name) == 0) {
            *info = g_registry.services[i];
            return 0;
        }
    }
    return -1;
}

int service_list(struct service_info *list, int max_count) {
    if (!list || max_count <= 0) return -1;
    int n = g_registry.count < max_count ? g_registry.count : max_count;
    for (int i = 0; i < n; i++) list[i] = g_registry.services[i];
    return n;
}
