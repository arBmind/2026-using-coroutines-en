# Slide-owned project sources

These files provide the source examples for the deck. The directory layout
preserves their locations in the upstream projects. The Hourglass paths contain
the coroutine excerpts used in the deck and its drafts.

Run `./build.sh` to verify that:

- the copied `void_coroutine.h` executes a coroutine; and
- the copied `Thread.h` compiles against Win32 test doubles and its `resumeOn`
  awaitable queues and resumes a suspended coroutine.

If an upstream checkout exists in `../desktop-duplication-cpp/` or
`../hourglass-app2/`, the script also checks its worktree and compares the
corresponding source excerpts.
