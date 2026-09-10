---
theme: default
title: C++20 Coroutines in Action
author: Andreas Reischuck
keywords: C++20, coroutines, Meeting C++, Qt, Windows APC
presenter: true
browserExporter: dev
exportFilename: coroutines-slides
lineNumbers: true
colorSchema: light
aspectRatio: 16/9
canvasWidth: 1280
transition: fade-out
magicMoveDuration: 650
layout: cover
class: cover
fonts:
  sans: Inter
  mono: JetBrains Mono
htmlAttrs:
  lang: en
defaults:
  layout: default
drawings:
  enabled: true
  persist: false
---

<div class="grid h-full grid-cols-[1.1fr_0.9fr]">
  <div class="flex flex-col justify-center pt-[4.5rem] pr-12 pb-36 pl-[4.4rem]">
    <div class="hidden text-[0.78rem] font-extrabold tracking-[0.14em] text-brand-orange uppercase">Meeting C++ 2026</div>
    <div class="text-[0.78rem] font-extrabold tracking-[0.14em] text-brand-orange uppercase">C++ Usergroup Dresden 2026</div>
    <h1 class="max-w-[700px] text-hero! leading-[0.94]!">C++20 Coroutines<br />in Action</h1>
    <div class="mt-[1.2rem] text-[1.35rem] font-[650] text-brand-blue">Grab the low-hanging benefits without building a framework</div>
    <div class="mt-4 text-[1.1rem] font-bold">Andreas Reischuck</div>
  </div>
  <div class="relative">
    <img class="absolute top-[-3%] right-[5%] z-3 max-h-[17%] w-[36%] rounded-lg bg-white/88 p-[0.3rem] object-contain" :src="'/images/hicknhackLogo_new_text.png'" alt="HicknHack Software GmbH" />
    <img class="absolute right-[13%] bottom-[3%] z-2 h-[80%]" :src="'/images/andreas.png'" alt="Andreas Reischuck" />
  </div>
</div>

<!--
Welcome. Ask for a show of hands: who has read about coroutines, and who has shipped one? The gap between those groups is the reason for this talk.
-->

---
layout: cover
class: cover ps-24!
---

# Let's Start!

<!--
Set expectations: practical use, not a language-lawyer tour. We will see the minimum plumbing, because knowing where it lives matters, but it is not the story.
-->

---
class: "flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-section-title [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="mt-[2.2rem] max-w-[970px] text-[3.1rem] font-[760] tracking-[-0.04em] leading-[1.07]">
C++20 Coroutines
</div>

<div class="mt-8">
Without any plumbing
</div>

---

# Simple Coroutine

<CoroutineCodeEvolution />

<!--
* This just a normal C++ function that is invoked.
1. [click] co_await makes it a coroutine.
1. [click] now co_return is also required.
1. [click] usually you need a special return type.
1. [click] someone has to resume it.
1. [click] the result is wrapped in the task.
-->

---

# Step By Step

<CoroutineFrame />

<!--
* Let's debug our function
1. [click] caller invokes greeting
1. [click] coroutine creates a frame and copies arguments.
1. [click] co_await suspends, frame retains state.
1. [click] resume supplies the Profile.
1. [click] profile returned by co_await.
1. [click] co_return stores the result.
1. [click] caller continues while coroutine is finished.
1. [click] frame destruction follows task lifetime.
-->

---

# The <code>Awaiter</code> interface

<div style="--prism-font-size:1.4em">

<<< ./snippets/coroutine-introduction/awaiter.cpp#awaiter cpp {3|7,10,13|16}{lines:true}

</div>

---

# <code>Awaiter</code>: Choose who runs next

<div class="mt-[1.05rem] grid items-start gap-[1.15rem]">
  <section class="grid gap-[0.65rem] text-[1.4em]">
    <div class="grid min-h-[104px] grid-cols-[1fr_350px] items-center gap-3 rounded-xl border border-line border-l-[7px] border-l-slate-500 bg-soft px-[0.8rem] py-[0.65rem]" v-click>
      <div class="font-mono font-bold leading-[1.35] whitespace-nowrap [&_span]:text-violet-700 [&_b]:font-bold [&_b]:text-red-600"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> std::noop_coroutine();<br>
      }</div>
      <div class="grid gap-1 rounded-lg bg-white p-[0.55rem] text-center [&_span]:text-[1rem] [&_span]:font-bold [&_span]:text-muted [&_span]:uppercase [&_strong]:leading-[1.15] [&_strong]:text-brand-blue"><span>Next:</span><strong>last resumer</strong></div>
    </div>
    <div class="grid min-h-[104px] grid-cols-[1fr_350px] items-center gap-3 rounded-xl border border-line border-l-[7px] border-l-brand-cyan bg-soft px-[0.8rem] py-[0.65rem]" v-click>
      <div class="font-mono font-bold leading-[1.35] whitespace-nowrap [&_span]:text-violet-700 [&_b]:font-bold [&_b]:text-red-600"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> continuation;<br>
      }</div>
      <div class="grid gap-1 rounded-lg bg-white p-[0.55rem] text-center [&_span]:text-[1rem] [&_span]:font-bold [&_span]:text-muted [&_span]:uppercase [&_strong]:leading-[1.15] [&_strong]:text-brand-blue"><span>Next:</span><strong>co_await calling coroutine</strong></div>
    </div>
    <div class="grid min-h-[104px] grid-cols-[1fr_350px] items-center gap-3 rounded-xl border border-line border-l-[7px] border-l-brand-green bg-soft px-[0.8rem] py-[0.65rem]" v-click>
      <div class="font-mono font-bold leading-[1.35] whitespace-nowrap [&_span]:text-violet-700 [&_b]:font-bold [&_b]:text-red-600"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> otherCoroutine;<br>
      }</div>
      <div class="grid gap-1 rounded-lg bg-white p-[0.55rem] text-center [&_span]:text-[1rem] [&_span]:font-bold [&_span]:text-muted [&_span]:uppercase [&_strong]:leading-[1.15] [&_strong]:text-brand-blue"><span>Next:</span><strong>other coroutine</strong></div>
    </div>
  </section>
</div>

<!--
* Awaiter has full control what happens
1. [click] Returning noop_coroutine resumes a handle that deliberately does nothing.
1. [click] Returning the continuation argument immediately resumes this coroutine.
1. [click] Transfers control to that coroutine.
-->

---

# Andreas Reischuck

<div class="mt-6 grid grid-cols-[0.56fr_1.44fr] items-center gap-[2.4rem]">
  <img class="m-auto max-h-[430px] drop-shadow-[0_12px_18px_rgb(15_23_42_/_0.18)]" :src="'/images/andreas.png'" alt="Andreas Reischuck" />
  <div>
    <div class="max-w-[970px] text-[2.55rem] font-[760] tracking-[-0.04em] leading-[1.07] [&_em]:text-brand-blue [&_em]:not-italic">I like code that says what it is <em>trying to do.</em></div>
    <div class="mt-[1.2rem] grid gap-3 text-[1.4em]">
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.5rem] text-brand-orange">⌁</span><span>C++ developer and software architect</span></div>
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.5rem] text-brand-orange">◫</span><span>Building tools people use every day</span></div>
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.5rem] text-brand-orange">✦</span><span>Host, trainer, and C++ community enthusiast</span></div>
    </div>
  </div>
</div>

<!--
Personal bridge: these examples are not invented kata. They come from tools I care about and maintain.
-->

---
layout: default
class: "relative overflow-hidden flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="absolute right-20 bottom-6 font-mono text-[10rem] font-black leading-none text-white/3">01</div>

<div class="font-mono text-base font-extrabold leading-none tracking-[0.17em] text-cyan-300">1st Use Case</div>

# Hourglass App

<div class="mt-3 text-[1.35rem] text-slate-300">Using <code class="text-cyan-500">QNetworkAccessManager</code> with Coroutines</div>

<div class="absolute right-20 top-20 bottom-60 w-[40%] flex flex-col items-expand gap-1">
  <div class="flex items-center justify-center gap-5 rounded-2xl border border-white/12 bg-white/5 px-6 py-5">
    <img class="h-16 w-auto rounded-md bg-white px-3 py-2" :src="'/images/redmine-logo.png'" alt="Redmine" />
    <span class="text-3xl font-light text-slate-500">+</span>
    <div class="grid justify-items-center gap-2"><img class="size-16" src="./assets/hourglass/appicon.ico" alt="Hourglass" /><strong class="text-sm">Hourglass</strong></div>
  </div>
  <div class="flex flex-col items-center gap-1 text-center">
    <span class="w-px h-5 bg-gradient-to-t from-cyan-300 to-transparent"></span>
    <span class="rounded-full border border-cyan-300/35 bg-cyan-300/8 px-3 py-2 font-mono text-[1rem] font-extrabold tracking-[0.08em] text-cyan-200">HTTP · REST</span>
    <span class="w-px h-5 bg-gradient-to-b from-cyan-300 to-transparent"></span>
  </div>
  <div class="grid place-items-center rounded-2xl border border-white/12 bg-white/5 px-6 py-5"><img class="h-[92px] w-auto" :src="'/images/qt-logo.png'" alt="Qt" /></div>
</div>

<!--
First use case: the Hourglass desktop and Android app. It is a Qt client for the REST API exposed by our Hourglass plugin for Redmine.

Billing our time is the most transparent evaluation. We use Redmine is to organize the company and projects. We maintain the Hourglass plugin to track what we have done. This app is its desktop and Android companion.
-->

---
transition: slide-left
---

# The concept

<HourglassAction />

<!--
* User clicks start, Gui is waiting
1. [click] post to Redmine Hourglass
1. [click] apply results
1. [click] fetch issue details from Redmine
1. [click] unlock the GUI and replace Start with the active tracker’s Stop button
-->

---
transition: slide-left
---

<div class="mt-7 grid grid-cols-[0.48fr_1.52fr] items-center gap-8">
  <div class="grid place-items-center rounded-3xl bg-[#0b111b] p-8 shadow-[0_18px_38px_rgb(15_23_42_/_0.16)]"><img class="w-[210px]" :src="'/images/qt-logo.png'" alt="Qt" /></div>
  <div>
    <div class="mb-5 text-[2rem] font-[760] tracking-[-0.035em] text-brand-blue">A cross-platform C++ application framework</div>
    <div class="grid grid-cols-3 gap-3 text-center text-[1.8rem] font-bold">
      <div class="rounded-xl border border-line bg-soft p-4">GUI + QML</div>
      <div class="rounded-xl border border-line bg-soft p-4">Files + settings</div>
      <div class="rounded-xl border border-line bg-soft p-4">Threads + timers</div>
      <div class="rounded-xl border border-line bg-soft p-4">Multimedia</div>
      <div class="rounded-xl border border-line bg-soft p-4">Devices</div>
      <div class="rounded-xl border-2 border-brand-cyan bg-cyan-50 p-4 text-cyan-900 shadow-[0_8px_20px_rgb(8_145_178_/_0.1)]">Networking</div>
    </div>
  </div>
</div>

<div class="mt-7 grid grid-cols-[auto_1fr] items-center gap-4 rounded-2xl border-l-[7px] border-brand-orange bg-orange-50 px-5 py-4">
  <div><code class="text-[1.5rem]! font-900! text-brand-blue">QNetworkAccessManager</code><span class="ml-3 text-[1.5rem] text-muted">is Qt’s asynchronous HTTP client abstraction.</span></div>
</div>

<div class="mt-2 text-right text-[0.48rem] text-slate-400">Qt is a registered trademark of The Qt Company Ltd. and its subsidiaries.</div>

<!--
Qt is the GUI framework, but it also abstracts a large part of the operating system. For this use case, the important abstraction is networking—and specifically QNetworkAccessManager.
-->

---
transition: slide-left
---

<div class="mt-7 grid grid-cols-[1fr_auto] items-center gap-7">
  <div class="flex flex-col gap-5" style="--prism-font-size:1.3em">

<div class="mb-[-0.75rem] font-mono text-[1rem] font-extrabold tracking-[0.14em] text-brand-blue uppercase">Qt header</div>

<<< ./snippets/qnetwork-access-manager.cpp#qnam-api cpp {}{lines:true}

<div v-click="1" class="transition-opacity duration-500">

<div class="mb-[0.75rem] font-mono text-[1rem] font-extrabold tracking-[0.14em] text-brand-blue uppercase">Usage</div>

<<< ./snippets/qnetwork-access-manager.cpp#qnam-start cpp {0|1|2|3-5}{lines:true}

</div>

  </div>
  <div class="grid gap-5">
    <div class="grid grid-cols-[52px_1fr] items-center gap-3 rounded-xl border border-line bg-soft p-3" v-click="2"><span class="grid size-11 place-items-center rounded-full bg-brand-blue font-mono font-black text-white">1</span><div><strong class="text-lg text-brand-blue">Call <code>get(request)</code></strong><span class="mt-1 block text-muted">Queue work</span></div></div>
    <div class="grid grid-cols-[52px_1fr] items-center gap-3 rounded-xl border border-line bg-soft p-3" v-click="2"><span class="grid size-11 place-items-center rounded-full bg-brand-orange font-mono font-black text-white">2</span><div><strong class="text-lg text-brand-blue">Receive <code>QNetworkReply</code></strong><span class="mt-1 block text-muted">Future for the work.</span></div></div>
    <div class="grid grid-cols-[52px_1fr] items-center gap-3 rounded-xl border-2 border-brand-cyan bg-cyan-50 p-3" v-click="3"><span class="grid size-11 place-items-center rounded-full bg-brand-cyan font-mono font-black text-white">3</span><div><strong class="text-lg text-cyan-900">Signal <code>finished()</code></strong><span class="mt-1 block text-cyan-900/65">Called when done.</span></div></div>
  </div>
</div>

<!--
The QNetworkAccessManager has asynchronous get operation.
1. [click] Build the request.
1. [click] Call get and receive a QNetworkReply pointer, although the response is not ready yet.
1. [click] Connect the finished signal. Completion arrives later.
-->

---
transition: slide-left
---

# Callback Implementation

<div class="mt-4 grid grid-cols-[auto_auto] items-start gap-5">
  <div style="--prism-font-size:1.3em ">

<<< ./snippets/qnetwork-access-manager.cpp#qnam-response cpp {0|2-5|6-10|11-15|16|17}{lines:true}

  </div>
  <div class="grid gap-3 pt-1">
    <div class="rounded-xl border border-line bg-soft px-4 py-3" v-click="1"><b class="text-brand-blue">Error Handling</b></div>
    <div class="rounded-xl border border-line bg-soft px-4 py-3" v-click="2"><b class="text-brand-blue">HTTP Status</b></div>
    <div class="rounded-xl border border-line bg-soft px-4 py-3" v-click="3"><b class="text-brand-blue">Parse JSON</b></div>
    <div class="rounded-xl border border-line bg-soft px-4 py-3" v-click="4"><b class="text-brand-blue">Handle Data</b></div>
    <div class="rounded-xl border-2 border-brand-orange bg-orange-50 px-4 py-3" v-click="5"><b class="text-orange-900">2nd request?</b><small class="mt-1 block text-orange-900/65">keep <code>reply</code> alive</small></div>
  </div>
</div>

<!--
1. [click] Handle all network errors and abort.
1. [click] Evaluate HTTP response code.
1. [click] Parse response as JSON.
1. [click] Process the result.
1. [click] Continue with next request.
-->

---
transition: slide-left
class: code-slide
---

# NetworkTask for CoRoutine

<AwaiterMachinery />

<!--
The NetworkTask is the main.
1. [click] The promise type is required from the book. await_transform is the magic.
1. [click] Return the Awaiter implementation storing the given QNetworkReply.
1. [click] await_ready matches isFinished().
1. [click] await_suspend connects to finished signal and resumes coroutine.
1. [click] await_resume handles all default handling of the reply.
1. [click] Finish with the whole adapter in view: the task hook, the three Awaiter methods, and the returned Awaiter all fit together.
-->

---
transition: slide-left
---

# Usage

<div style="--prism-font-size:1.4em">

<<< ./snippets/qnetwork-access-manager.cpp#refresh-coroutine cpp {0|1-2|4-6|8-10|12-14|16}{lines:true}

</div>

<!--
1. [click] Establish the updating state.
1. [click] Await and apply the current user; its failure path stays beside it.
1. [click] Do the same for activities.
1. [click] Do the same for the active tracker, including the expected not-found case.
1. [click] Mark the account live. Three network waits now read as one refresh policy.
-->

---
transition: fade-out
---

# Hourglass App Summary

<ul class="mt-32! grid gap-5 pl-8">
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Use existing Qt Networking</strong></li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Qt Signal/Slot as Co-Routine Runtime</strong></li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Minimal Infrastructure needed</strong></li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Readable Network Orchestration</strong></li>
</ul>

<!--
1. [click] Keep the proven Qt networking layer.
1. [click] Signals and slots provide the callback that resumes the coroutine.
1. [click] The adapter stays deliberately small.
1. [click] The result is network orchestration that reads from top to bottom.
-->

---
class: "flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-section-title [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="font-mono text-base font-extrabold leading-none tracking-[0.15em] text-cyan-300">EXAMPLE 02</div>

# Desktop Duplicator

<div class="mt-[1.3rem] max-w-[820px] text-[1.45rem] text-slate-300">Use a coroutine’s continuation across Win32 threads.</div>

---

# Desktop Duplicator

<DesktopDuplicatorIntro />

<!--
Start with a clean desktop and an idle projector. The duplicator begins with no application or region selected.
1. [click] Open Qt Creator. The IDE includes the project tree, real-looking coroutine capture code, and its application output.
1. [click] Make the IDE smaller and open presenter notes beside it. The notes remain useful locally but must stay private.
1. [click] Show the transparent capture rectangle. It moves directly into place over the complete editor tab, and the beamer follows while notes and console stay private.
1. [click] Move to the remote-work variant: one ultrawide monitor holds two source files side by side and a Teams call on the right.
1. [click] Select the CaptureThread source pane with the same transparent rectangle. Teams shows the live region immediately.
1. [click] Move and expand the rectangle over the complete pane. The Teams share follows it while the second source file, console, and rest of the desktop stay local.
-->

---

# The Core Loop

<div class="mt-[1.4rem] flex items-stretch gap-[0.7rem] text-[1.8rem]">
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-blue-600 p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>CAPTURE</strong><small>acquire changes</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-orange p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>RENDER</strong><small>update texture + draw</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-green p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>MAIN</strong><small>update UI status</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>↩</div>
</div>

<!--
Clarify that thread hopping is a requirement, not cleverness: capture APIs, renderer resources, and UI each have affinity. APC means Asynchronous Procedure Call: ask a target thread to execute a callback when it enters an alertable wait.
-->

---

# Win32: <code>QueueUserAPC</code>

<div class="apc-signature mt-7">

```cpp
DWORD QueueUserAPC(
  [in] PAPCFUNC  pfnAPC,
  [in] HANDLE    hThread,
  [in] ULONG_PTR dwData
);
```

</div>

<div v-click class="mt-10 border-l-[7px] border-brand-orange bg-orange-50 px-7 py-6 text-[1.65rem] font-[650] leading-[1.3] text-brand-blue">
“Adds a user-mode asynchronous procedure call (APC) object to the APC queue of the specified thread.”
</div>

<div v-click class="mt-6 font-mono text-[1.05rem] font-bold text-muted"><a href="https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-queueuserapc">Microsoft Learn</a> · callback + target thread + one pointer-sized value</div>

<!--
Start with the Win32 declaration: a callback, a target thread, and one pointer-sized value.
1. [click] Use Microsoft’s definition verbatim. Queueing is a request; it does not interrupt arbitrary user-mode code.
1. [click] The target thread invokes the callback when it enters an alertable wait.
-->

---

# UserAPC Example

<div class="apc-minimal mt-4" style="--prism-font-size:1.2rem">

<<< ./snippets/apc-introduction/main.c#minimal-apc c {14|6|15|9-11|10|16}{lines:true}

</div>

<!--
WinMain creates the worker thread.
1. [click] The worker loops in an alertable wait, which gives Windows a place to dispatch its APC queue.
1. [click] WinMain queues the APC with 42 as its pointer-sized value.
1. [click] Windows invokes the APC callback on the worker thread.
1. [click] The callback prints the observed value and flushes stdout so redirected output is visible.
1. [click] WinMain deliberately never exits; the example test kills it after observing the output.
-->

---
class: code-slide
---

<div style="--prism-font-size:1.0rem"><!-- exact code is compiled by apc-introduction/build.sh -->

<<< ./snippets/apc-introduction/ownership.cpp#owned-apc cpp {21|22|10-12|23|13|14-16|24}{lines:true}

</div>

<!--
The adapter heap-owns every callback, keeping the ownership handoff explicit.
1. [click] The PAPCFUNC-compatible thunk reconstructs the unique_ptr, invokes the functor, and destroys it on return.
1. [click] The constructor creates the unique_ptr, so a failed or skipped queue operation cleans up normally.
1. [click] c_parameter exposes the pointer as ULONG_PTR, and release transfers ownership away from the temporary adapter.
1. [click] queueUserApc releases only after QueueUserAPC succeeds; otherwise the adapter destructor still owns and deletes the functor.
-->

---

# Using APC for DesktopDuplicator

<div style="--prism-font-size:1.0rem">

<<< ./snippets/apc-introduction/before-coroutines.cpp#class-apcs cpp {0|2-4|9-11|15|16|17-20}{lines:true}

</div>

<!--
Git history gives us a clean before-and-after: this is the parent of commit d07fe0a, which replaced the thread dance with one C++20 coroutine.
1. [click] DuplicationController owned explicit main, render, and capture thread abstractions. One comment collapses the unrelated machinery.
1. [click] A captured frame became a lambda queued to the render thread through the APC wrapper.
1. [click] Rendering either kicked capture directly or queued another lambda to the main thread, which updated status and kicked capture from there.
-->

---

# The coroutine glue

<div style="--prism-font-size:1.1rem">

<<< ./snippets/slide-projects/desktop-duplication-cpp/src/win32/Thread.h#resume-on-awaitable cpp {7-10|11|12}{lines:true}

</div>

<!--
The APC callback reconstructs the coroutine handle and resumes it on the target thread.
1. [click] await_suspend queues that callback with the handle address as its payload.
1. [click] If QueueUserAPC fails, nobody can resume the suspended coroutine, so destroy its frame.
-->

---

# Simplest coroutine trait for <code>void</code>

<div style="--prism-font-size:1.5rem">

<<< ./snippets/slide-projects/desktop-duplication-cpp/src/meta/void_coroutine.h cpp {0|4-5|6|7|8-9|10}{lines:true}

</div>

<!--
Returning void still needs a promise type, supplied here through coroutine_traits.
1. [click] promise_type defines the compiler-facing protocol and produces no wrapper object.
1. [click] suspend_never starts immediately and destroys the frame automatically at completion.
1. [click] Normal completion returns no value; this minimal example also consumes unhandled exceptions.
-->

---

# Usage

<RunCaptureCode />

<!--
Enter the coroutine on the capture thread and keep the outer loop visible as the fixed structure.
1. [click] Initialize capture before entering the loop.
1. [click] Unfold frame acquisition on the capture thread.
1. [click] Fold acquisition and follow the frame onto the render thread.
1. [click] Scroll into the optional main-thread status detour.
1. [click] Resume the capture thread before releasing its frame.
1. [click] Zoom out and reveal the complete coroutine without folding or scrolling.
-->

---

# Desktop Duplicator: Summary

<ul class="mt-32! grid gap-5 pl-8">
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Use existing Win32 APC APIs</strong></li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Simple coroutine glue code</strong></li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>All coordination in one coroutine</strong></li>
</ul>

---
class: "flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-section-title [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="font-mono text-base font-extrabold leading-none tracking-[0.15em] text-cyan-300">LEARNINGS + PITFALLS</div>

# Lifetime with Coroutines

<div class="mt-[1.3rem] max-w-[820px] text-[1.45rem] text-slate-300">Coroutines may outlive the data!</div>

---

# The tempting coroutine lambda

<div style="--prism-font-size:1.5rem">

<<< ./snippets/coroutine-lifetime/main.cpp#lambda-risky cpp {2-5|6|3}{lines:true}

</div>

<!--
This is the key pitfall. Captures are members of the closure object; invoking a coroutine lambda is a coroutine member call. Depending on initial_suspend, the body may not run before the temporary/local closure dies. Even if it starts immediately, a later resume can still outlive referenced objects.
-->

---

# What happens

<LambdaLifetime />

<!--
1. [click] We create a closure containing state.
1. [click] Calling its coroutine operator creates/returns a coroutine; the closure is effectively its this object.
1. [click] launch returns and destroys the local closure.
1. [click] The coroutine body begins or resumes and reaches through a dangling this to state. The frame survived, but the capture did not move into it just because the body looks local.
-->

---

# Prefer global coroutines

<div style="--prism-font-size:1.5rem">

<<< ./snippets/coroutine-lifetime/main.cpp#named-safe cpp {1-4|6|2}{lines:true}

</div>

<!--
This is the simplest habit. Value parameters are copied/moved into the frame according to coroutine rules. For objects that cannot be copied, establish explicit shared ownership or cancellation/lifetime protocols. A raw this capture does not establish lifetime.
-->

---

# Recap

<ul class="mt-32! grid gap-5 pl-8">
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Hourglass App</strong> - Use Qt Networking & Signals</li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Desktop Duplicator</strong> - Use Win32 UserAPCs</li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Hints</strong> - Be aware of the lifetimes</li>
</ul>

---

# Where are your low-hanging coroutines?

<ul class="mt-32! grid gap-5 pl-8">
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Sagas</strong> over multiple steps</li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>State Machine</strong> managed in one place</li>
  <li v-click class="text-[2rem]! leading-[1.2]!"><strong>Callback Chains</strong></li>
</ul>

<!--
Give the audience a practical action for Monday. Find a single boundary, build or adopt a narrow awaitable, and compare readability plus lifetime behavior. Do not convert synchronous code merely because the syntax is new.
-->

---

# Want to build useful things with us?

<div class="mt-8 grid grid-cols-2 items-center gap-[2.1rem]">
  <div class="text-center">
    <img class="mx-auto max-h-[190px] max-w-[460px] object-contain" :src="'/images/hicknhackLogo_new_text.png'" alt="HicknHack Software GmbH" />
  </div>
  <div>
    <div class="max-w-[970px] text-[2.65rem] font-[760] tracking-[-0.04em] leading-[1.07] [&_em]:text-brand-blue [&_em]:not-italic">Bring curiosity.<br /><em>Keep the ownership clear.</em></div>
    <p class="mt-6 text-xl text-muted">If real C++ systems, thoughtful architecture, and sharing what you learn sound fun—come work with HicknHack Software GmbH.</p>
  </div>
</div>

<!--
Invitation, not a hard sell. Mention current hiring/contact details verbally or add a QR code once a stable jobs URL is chosen.
-->

---

# Your experiment could be the next talk

<div class="mt-7 grid grid-cols-2 items-center gap-[2.1rem]">
  <div class="text-center">
    <img class="m-auto max-h-[235px]" :src="'/images/cppug.png'" alt="C++ user group" />
  </div>
  <div>
    <div class="border-l-[7px] border-brand-orange pl-[1.2rem] text-[1.7rem] font-[650] leading-[1.25]">You do not need the final answer to give a valuable user-group talk.</div>
    <div class="mt-8 grid gap-3">
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.35rem] text-brand-orange">①</span><span>Show the problem you actually met</span></div>
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.35rem] text-brand-orange">②</span><span>Show the attempt that taught you something</span></div>
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.35rem] text-brand-orange">③</span><span>Invite the room into the next step</span></div>
    </div>
  </div>
</div>

<!--
Invite people to the local/user group and to submit even a short talk. The useful story includes the failed intermediate attempts—just like unique_ptr and lambdas in example two.
-->

---
class: "flex flex-col items-center justify-center bg-[#07111d] text-center text-white [&_h1]:text-hero [&_h1]:text-white [&_h1::after]:hidden [&_p]:text-xl [&_p]:text-slate-300"
---

<div class="text-[2rem] mb-10 font-extrabold tracking-[0.14em] text-brand-orange uppercase">THANK YOU · QUESTIONS?</div>

# <span class="text-cyan-300">play more</span><br />with coroutines

<p class="text-[1.5rem]! mt-10!">Start small. Read the flow. Respect the lifetime.</p>

<div class="mt-15 flex items-center gap-8 text-[1.2rem]">
  <span>Andreas Reischuck</span>
  <span class="text-cyan-300">HicknHack Software GmbH</span>
</div>

<!--
End on the requested phrase. Leave this slide up for questions.
-->
