#include "windows.h"

#include <pthread.h>
#include <stdlib.h>

struct ApcEntry {
    PAPCFUNC callback;
    ULONG_PTR data;
    struct ApcEntry *next;
};

struct ApcThread {
    pthread_t native;
    pthread_mutex_t mutex;
    pthread_cond_t ready;
    LPTHREAD_START_ROUTINE startRoutine;
    LPVOID parameter;
    struct ApcEntry *head;
    struct ApcEntry *tail;
};

static _Thread_local struct ApcThread *currentThread;

static void *runThread(void *parameter) {
    currentThread = parameter;
    currentThread->startRoutine(currentThread->parameter);
    return NULL;
}

HANDLE CreateThread(
    LPVOID security,
    size_t stackSize,
    LPTHREAD_START_ROUTINE startRoutine,
    LPVOID parameter,
    DWORD creationFlags,
    DWORD *threadId) {
    (void)security;
    (void)stackSize;
    (void)creationFlags;
    (void)threadId;

    struct ApcThread *thread = calloc(1, sizeof(*thread));
    if (thread == NULL)
        return NULL;
    thread->startRoutine = startRoutine;
    thread->parameter = parameter;
    pthread_mutex_init(&thread->mutex, NULL);
    pthread_cond_init(&thread->ready, NULL);
    if (pthread_create(&thread->native, NULL, runThread, thread) != 0)
        return NULL;
    return thread;
}

DWORD QueueUserAPC(PAPCFUNC callback, HANDLE thread, ULONG_PTR data) {
    struct ApcEntry *entry = malloc(sizeof(*entry));
    if (entry == NULL)
        return 0;
    entry->callback = callback;
    entry->data = data;
    entry->next = NULL;

    pthread_mutex_lock(&thread->mutex);
    if (thread->tail == NULL)
        thread->head = entry;
    else
        thread->tail->next = entry;
    thread->tail = entry;
    pthread_cond_signal(&thread->ready);
    pthread_mutex_unlock(&thread->mutex);
    return 1;
}

DWORD SleepEx(DWORD milliseconds, int alertable) {
    if (milliseconds != INFINITE || !alertable || currentThread == NULL)
        abort();

    pthread_mutex_lock(&currentThread->mutex);
    while (currentThread->head == NULL)
        pthread_cond_wait(&currentThread->ready, &currentThread->mutex);
    struct ApcEntry *entry = currentThread->head;
    currentThread->head = entry->next;
    if (currentThread->head == NULL)
        currentThread->tail = NULL;
    pthread_mutex_unlock(&currentThread->mutex);

    entry->callback(entry->data);
    free(entry);
    return 0x000000C0; // WAIT_IO_COMPLETION
}

#ifndef APC_NO_C_MAIN
extern int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

int main(void) {
    return WinMain(NULL, NULL, NULL, 0);
}
#endif
