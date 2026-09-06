#pragma once

#include <stddef.h>
#include <stdint.h>

#define WINAPI
#define CALLBACK
#define TRUE 1
#define INFINITE UINT32_MAX

typedef void VOID;
typedef void *LPVOID;
typedef void *HINSTANCE;
typedef char *LPSTR;
typedef uint32_t DWORD;
typedef uintptr_t ULONG_PTR;

typedef struct ApcThread *HANDLE;
typedef DWORD(WINAPI *LPTHREAD_START_ROUTINE)(LPVOID);
typedef VOID(CALLBACK *PAPCFUNC)(ULONG_PTR);

#ifdef __cplusplus
extern "C" {
#endif

HANDLE CreateThread(
    LPVOID security,
    size_t stackSize,
    LPTHREAD_START_ROUTINE startRoutine,
    LPVOID parameter,
    DWORD creationFlags,
    DWORD *threadId);
DWORD QueueUserAPC(PAPCFUNC callback, HANDLE thread, ULONG_PTR data);
DWORD SleepEx(DWORD milliseconds, int alertable);

#ifdef __cplusplus
}
#endif
