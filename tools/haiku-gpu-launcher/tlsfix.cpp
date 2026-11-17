// libsummit_tlsfix.so: a workaround for Summit's web process crashing at start-up on Haiku with a GPU driver.
//
// What was seen: worker threads that Mesa starts (NVK's queue thread, Zink's zfq/zcfq/gdrv threads) crashed with a jump to address 0
// inside pthread_once(): Mesa's util_call_once_data_slow() stores a function in a thread_local variable and, a moment later, the
// once-callback reads it back in the same thread, and got zeros. The main thread never failed.
//
// The workaround: every thread the process starts first touches thread-local data of its own twice, before it runs its routine. With
// that, none of those crashes happen (WebGL 1 and 2 run on the GPU through Zink and NVK). Why the second touch matters is not proven;
// Haiku's runtime_loader (src/system/runtime_loader/elf_tls.cpp) keeps a generation counter per thread that starts at 0, which is the
// prime suspect.
//
// Use it with LD_PRELOAD (the summit-gl launcher does). It changes nothing else.
#include <pthread.h>
#include <dlfcn.h>
#include <stdlib.h>

static thread_local volatile int tlsfix_touch;

struct Start {
    void *(*fn)(void *);
    void *arg;
};

static void *trampoline(void *p)
{
    Start s = *(Start *)p;
    free(p);
    tlsfix_touch = 1;                     // the first access makes the thread's vector of thread-local blocks
    tlsfix_touch = tlsfix_touch + 1;      // the second brings the thread's generation counter up to date
    return s.fn(s.arg);
}

extern "C" int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*fn)(void *), void *arg)
{
    typedef int (*create_fn)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
    static create_fn real;

    if (real == NULL)
        real = (create_fn)dlsym(RTLD_NEXT, "pthread_create");
    Start *s = (Start *)malloc(sizeof(Start));
    if (s == NULL)
        return real(thread, attr, fn, arg);
    s->fn = fn;
    s->arg = arg;
    return real(thread, attr, trampoline, s);
}
