// #region minimal-apc
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

DWORD WINAPI ThreadLoop(LPVOID ignored) {
    while (true) SleepEx(INFINITE, TRUE); // alertable
}

VOID CALLBACK PrintApc(ULONG_PTR arg) {
    printf("Hello from APC. Got: %d\n", (int)arg); fflush(stdout);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR commandLine, int show) {
    HANDLE thread = CreateThread(NULL, 0, ThreadLoop, NULL, 0, NULL);
    QueueUserAPC(PrintApc, thread, 42);
    while (true);
}
// #endregion minimal-apc
