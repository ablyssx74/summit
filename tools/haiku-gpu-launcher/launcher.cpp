// "Summit GPU": starts /boot/system/apps/Summit with the thread-local-storage workaround (libsummit_tlsfix.so, see tlsfix.cpp) preloaded, so
// that Summit's web process does not crash when it uses the GPU. Arguments (a URL, --profile DIR, ...) are passed on to Summit.
//
// Summit is started as a team of its own (load_image) and this program then ends. A launcher that replaced itself with Summit (exec) left
// Summit stuck at start-up when it was started from Deskbar or Tracker: the system had registered the launcher's team as an application.
#include <OS.h>
#include <image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

int main(int argc, char **argv)
{
    static const char kFix[] = "/boot/system/lib/libsummit_tlsfix.so";
    static const char kSummit[] = "/boot/system/apps/Summit";

    // LD_PRELOAD = the workaround library (in front of whatever is set already)
    const char *old = getenv("LD_PRELOAD");
    if (old == NULL || old[0] == '\0') {
        setenv("LD_PRELOAD", kFix, 1);
    } else if (strstr(old, kFix) == NULL) {
        char *both = (char *)malloc(strlen(kFix) + strlen(old) + 2);
        if (both != NULL) {
            sprintf(both, "%s:%s", kFix, old);
            setenv("LD_PRELOAD", both, 1);
            free(both);
        }
    }

    const char **args = (const char **)malloc((argc + 1) * sizeof(char *));
    if (args == NULL)
        return 1;
    args[0] = kSummit;
    for (int i = 1; i < argc; i++)
        args[i] = argv[i];
    args[argc] = NULL;

    thread_id thread = load_image(argc, args, (const char **)environ);
    if (thread < 0) {
        fprintf(stderr, "Summit GPU: cannot start %s: %s\n", kSummit, strerror(thread));
        return 1;
    }
    resume_thread(thread);
    return 0;
}
