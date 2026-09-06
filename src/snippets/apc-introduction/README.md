# APC introduction example

`main.c` is the Win32 source shown in the deck. It intentionally never exits.

Run the executable check with:

```sh
./build.sh
```

The repository is tested on Linux, so `build.sh` compiles the unchanged source
against the test-only Win32 compatibility layer in `test/`. The layer implements
the thread-local APC queue and the alertable `SleepEx` wait used by this example.
The script waits for `Hello from APC. Got: 42`, prints the observed output, and
then terminates the process.

`ownership.cpp` contains the complete heap-owning `UniqueCallbackAdapter` shown
on the RAII slide. The test compiles that exact code, queues a capturing lambda,
observes its output, and terminates the deliberately infinite test process.
`before-coroutines.cpp` is a projector-sized excerpt of `DuplicationController`
immediately before commit `d07fe0a` (“replace thread dance with a single C++20
coroutine”).
