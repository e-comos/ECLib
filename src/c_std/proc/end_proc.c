#include <stdlib.h>
#include <slfd.h>

void exit(int status) {
    proc_exit(status);
}

void abort(void) {
    pid_t self = proc_getpid();
    proc_signal_send(self, PROCSIG_ABRT);
    
    /* Standard convention: 128 + SIGABRT (6) = 134 */
    proc_exit(134);
}
