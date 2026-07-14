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
    <div class="text-[0.78rem] font-extrabold tracking-[0.14em] text-brand-orange uppercase">Meeting C++ 2026</div>
    <h1 class="max-w-[700px] text-hero! leading-[0.94]!">C++20 Coroutines<br />in Action</h1>
    <div class="mt-[1.2rem] text-[1.35rem] font-[650] text-brand-blue">Grab the low-hanging benefits—without building a framework</div>
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
class: cover
---

# Background

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

# Start with ordinary synchronous code

<div class="mt-6">

<<< ./snippets/coroutine-introduction/synchronous.cpp#synchronous-flow cpp {lines:true}

</div>

---
transition: slide-left
---

# Make only the wait asynchronous

<CoroutineCodeEvolution />

<!--
[click] First make vertical room for an asynchronous completion.
[click] Change the return surface from string to task<string>.
[click] co_await marks the suspension point.
[click] co_return stores the result in the task promise.
[click] resume supplies the event result and continues the frame.
[click] result reads the value after completion. The hidden support.hpp contains the promise type, handle ownership, and awaitable; the final source is the compiled coroutine.cpp example.
-->

---

# Where did the function go?

<CoroutineFrame />

<!--
[click] caller invokes greeting while only caller exists on the stack.
[click] entering the coroutine creates a frame and copies id into it.
[click] co_await suspends: greeting leaves the stack, while the frame retains state and the continuation.
[click] resume supplies the Profile and activates that continuation.
[click] co_return stores the formatted value and reaches final suspend.
[click] caller reads task.result(); frame destruction follows task lifetime. Allocation is visualized on the heap for the common case, though the standard permits allocation elision.
-->

---

# <code>Awaiter</code>: Choose who runs next

<div class="mt-[1.05rem] grid grid-cols-[1.18fr_0.82fr] items-start gap-[1.15rem]">
  <section class="grid gap-[0.65rem]">
    <div class="grid min-h-[104px] grid-cols-[1fr_135px] items-center gap-3 rounded-xl border border-line border-l-[7px] border-l-slate-500 bg-soft px-[0.8rem] py-[0.65rem]" v-click>
      <div class="font-mono text-base font-bold leading-[1.35] whitespace-nowrap [&_span]:text-violet-700 [&_b]:font-bold [&_b]:text-red-600"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> std::noop_coroutine();<br>
      }</div>
      <div class="grid gap-1 rounded-lg bg-white p-[0.55rem] text-center [&_span]:text-[0.62rem] [&_span]:font-bold [&_span]:text-muted [&_span]:uppercase [&_strong]:text-[0.76rem] [&_strong]:leading-[1.15] [&_strong]:text-brand-blue"><span>Next:</span><strong>last resumer</strong></div>
    </div>
    <div class="grid min-h-[104px] grid-cols-[1fr_135px] items-center gap-3 rounded-xl border border-line border-l-[7px] border-l-brand-cyan bg-soft px-[0.8rem] py-[0.65rem]" v-click>
      <div class="font-mono text-base font-bold leading-[1.35] whitespace-nowrap [&_span]:text-violet-700 [&_b]:font-bold [&_b]:text-red-600"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> continuation;<br>
      }</div>
      <div class="grid gap-1 rounded-lg bg-white p-[0.55rem] text-center [&_span]:text-[0.62rem] [&_span]:font-bold [&_span]:text-muted [&_span]:uppercase [&_strong]:text-[0.76rem] [&_strong]:leading-[1.15] [&_strong]:text-brand-blue"><span>Next:</span><strong>co_await calling coroutine</strong></div>
    </div>
    <div class="grid min-h-[104px] grid-cols-[1fr_135px] items-center gap-3 rounded-xl border border-line border-l-[7px] border-l-brand-green bg-soft px-[0.8rem] py-[0.65rem]" v-click>
      <div class="font-mono text-base font-bold leading-[1.35] whitespace-nowrap [&_span]:text-violet-700 [&_b]:font-bold [&_b]:text-red-600"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> otherCoroutine;<br>
      }</div>
      <div class="grid gap-1 rounded-lg bg-white p-[0.55rem] text-center [&_span]:text-[0.62rem] [&_span]:font-bold [&_span]:text-muted [&_span]:uppercase [&_strong]:text-[0.76rem] [&_strong]:leading-[1.15] [&_strong]:text-brand-blue"><span>Next:</span><strong>other coroutine</strong></div>
    </div>
  </section>

  <section class="relative grid min-h-[342px] gap-[0.35rem] rounded-[14px] border-2 border-slate-400 bg-slate-50 px-4 pt-[1.6rem] pb-[0.8rem]">
    <div class="absolute inset-x-[0.65rem] top-[0.35rem] flex justify-between text-[0.58rem] font-extrabold tracking-[0.08em] text-slate-500"><span>NATIVE STACK · INSIDE await_suspend</span></div>
    <div class="grid min-h-[54px] grid-cols-[0.6fr_1.4fr] items-center gap-[0.45rem] rounded-[9px] border-l-6 border-brand-orange bg-orange-50 px-[0.7rem] py-[0.55rem] shadow-[0_5px_14px_rgb(15_23_42_/_0.08)] [&_strong]:text-[0.82rem] [&_strong]:text-brand-blue [&_code]:text-base [&_code]:font-bold [&_code]:text-slate-800">
      <strong>awaiter</strong><code>await_suspend(continuation)</code>
    </div>
    <div class="text-center text-[0.58rem] font-extrabold text-brand-orange">↑ called by</div>
    <div class="grid min-h-[54px] grid-cols-[0.6fr_1.4fr] items-center gap-[0.45rem] rounded-[9px] border-l-6 border-brand-cyan bg-white px-[0.7rem] py-[0.55rem] shadow-[0_5px_14px_rgb(15_23_42_/_0.08)] [&_strong]:text-[0.82rem] [&_strong]:text-brand-blue [&_code]:text-base [&_code]:font-bold [&_code]:text-slate-800">
      <strong>coroutine</strong><code>co_await Awaiter{};</code>
    </div>
    <div class="text-center text-[0.58rem] font-extrabold text-brand-orange">↑ resumed by</div>
    <div class="grid min-h-[54px] grid-cols-[0.6fr_1.4fr] items-center gap-[0.45rem] rounded-[9px] border-l-6 border-brand-green bg-white px-[0.7rem] py-[0.55rem] shadow-[0_5px_14px_rgb(15_23_42_/_0.08)] [&_strong]:text-[0.82rem] [&_strong]:text-brand-blue [&_code]:text-base [&_code]:font-bold [&_code]:text-slate-800">
      <strong>last resumer</strong><code>coroutine.resume()</code>
    </div>
  </section>
</div>

<!--
Start on the right and read the native stack bottom-up: a caller or scheduler resumed the coroutine; the coroutine reached co_await; and the awaiter is now executing await_suspend.
[click] Returning noop_coroutine resumes a handle that deliberately does nothing. The native stack unwinds and the last resumer continues. A void-returning await_suspend has the same visible outcome.
[click] Returning the continuation argument immediately resumes this coroutine, which proceeds through await_resume and continues after co_await.
[click] Returning another valid suspended handle transfers control to that coroutine. The current coroutine remains suspended.
The coroutine machinery evaluates resume on the returned handle. Do not return a null handle or one already at final suspend; noop_coroutine is the safe “nobody” value.
-->

---

# Hello, I’m Andreas

<div class="mt-6 grid grid-cols-[0.56fr_1.44fr] items-center gap-[2.4rem]">
  <img class="m-auto max-h-[430px] drop-shadow-[0_12px_18px_rgb(15_23_42_/_0.18)]" :src="'/images/andreas.png'" alt="Andreas Reischuck" />
  <div>
    <div class="max-w-[970px] text-[2.55rem] font-[760] tracking-[-0.04em] leading-[1.07] [&_em]:text-brand-blue [&_em]:not-italic">I like code that says what it is <em>trying to do.</em></div>
    <div class="mt-[1.2rem] grid gap-3">
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.35rem] text-brand-orange">⌁</span><span>C++ developer and software architect</span></div>
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.35rem] text-brand-orange">◫</span><span>Builder of tools people use every day</span></div>
      <div class="grid grid-cols-[2.2rem_1fr] items-center border-b border-line py-[0.55rem]"><span class="text-[1.35rem] text-brand-orange">✦</span><span>Host, trainer, and C++ community enthusiast</span></div>
    </div>
  </div>
</div>

<!--
Personal bridge: these examples are not invented kata. They come from tools I care about and maintain.
-->

---
layout: default
class: "flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-section-title [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="font-mono text-base font-extrabold leading-none tracking-[0.15em] text-cyan-300">EXAMPLE 01</div>

# Awaiting a REST API

<div class="mt-[1.3rem] max-w-[820px] text-[1.45rem] text-slate-300">Turn “what callback comes next?” into a sequence that matches the user action.</div>

---

# Hourglass: a small app with real async work

<div class="mt-8 grid grid-cols-2 items-center gap-[2.1rem]">
  <div class="grid grid-cols-[105px_1fr] items-center gap-[1.2rem] rounded-2xl border border-line bg-soft p-5 [&_h2]:m-0 [&_h2]:text-[1.6rem] [&_p]:mt-[0.3rem] [&_p]:mb-0 [&_p]:text-muted">
    <div class="grid size-[100px] place-items-center rounded-3xl bg-ink text-[3.2rem] text-white">◷</div>
    <div><h2>Hourglass</h2><p>Qt Quick companion app for tracking and booking time through a Redmine REST API.</p></div>
  </div>
  <div>
    <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted"><strong>User presses “Start”</strong><small>the UI must remain responsive</small></div>
    <div class="self-center text-center text-[1.8rem] font-extrabold text-brand-orange">↓</div>
    <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted"><strong>POST tracker → maybe GET issue</strong><small>each response can fail</small></div>
  </div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800">The domain story is sequential. The network API is not.</div>

<!--
Introduce the concrete action: begin tracking time, update the model, and if necessary load the referenced issue. Network latency must not block the Qt event loop.
-->

---

# Before: the shape belongs to callbacks

<div class="mt-6 grid grid-cols-[1.28fr_0.72fr] items-center gap-[2.1rem]">
<div>

<<< ./snippets/compiled-patterns/main.cpp#callback-flow cpp {1-3|4-8|9-17|all}{lines:true}

</div>
<div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>Control flow</h3><p>Encoded in nesting.</p></div>
  <div class="mt-4 rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>State</h3><p>Repeated across exit paths.</p></div>
  <div class="mt-4 rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>Lifetime</h3><p>Hidden inside captures.</p></div>
</div>
</div>

<div class="mt-[0.55rem] text-[0.78rem] text-muted">Runnable callback example from <code>compiled-patterns</code>; the project build executes this flow.</div>

<!--
Do not attack callbacks: they are a valid tool. The problem is the mismatch between the business flow and the code’s shape. Point at every place Idle must be restored and every this capture.
-->

---

# The state machine is already here

<div class="mt-[1.4rem] flex items-stretch gap-[0.7rem]">
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>WRITING</strong><small>POST in flight</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>APPLY</strong><small>update tracker</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>RESOLVE?</strong><small>maybe GET issue</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>IDLE</strong><small>or stale on error</small></div>
</div>

<div class="mt-[2.2rem] max-w-[970px] text-[2.3rem] font-[760] tracking-[-0.04em] leading-[1.07]" v-click>The compiler can carry the “where was I?” state for us.</div>

<!--
This is the low-hanging-fruit test: if you already draw a state machine to explain callback progression, a coroutine may give you that state machine in a readable function.
-->

---

# The idea: make a reply awaitable

<div class="mt-7 grid grid-cols-[0.76fr_1.24fr] items-center gap-[2.1rem]">
  <div>
    <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted"><strong>QNetworkReply</strong><small>emits finished()</small></div>
    <div class="self-center text-center text-[1.8rem] font-extrabold text-brand-orange">↓</div>
    <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted"><strong>awaitable adapter</strong><small>resumes coroutine handle</small></div>
  </div>
  <div>

<<< ./snippets/slide-projects/hourglass-app2/src/Hourglass/Model/NetworkTask.h#qt-reply-await-bridge cpp {1|4-7|all}{lines:true}

  </div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800">The Qt signal remains. It is contained at one boundary instead of shaping every caller.</div>

<!--
This is the minimum interesting plumbing. await_ready handles an already-finished reply. await_suspend connects completion to resume. await_resume turns the result into the type the domain code wants.
-->

---

# After: the shape belongs to the use case

<<< ./snippets/slide-projects/hourglass-app2/src/Hourglass/Model/RedmineAccount.cpp#start-time-tracker-coroutine cpp {1-3|4-11|13-20|22}{lines:true}

<div class="mt-[0.55rem] text-[0.78rem] text-muted">Imported directly from the compiling Hourglass project: <code>RedmineAccount.cpp::startTimeTrackerImpl</code>.</div>

<!--
[click] Establish state.
[click] Await the POST and handle failure beside it.
[click] Optionally await the issue lookup.
[click] Finish in Idle. We can review this in domain order.
-->

---

# What code was actually necessary?

<div class="mt-10 grid grid-cols-3 gap-[1.2rem]">
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><div class="text-[2.7rem] font-extrabold text-brand-blue">1</div><h2>Task type</h2><p>Owns the coroutine handle and completion policy.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><div class="text-[2.7rem] font-extrabold text-brand-blue">1</div><h2>Awaitable</h2><p>Bridges <code>finished()</code> to <code>resume()</code>.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><div class="text-[2.7rem] font-extrabold text-brand-blue">N</div><h2>Clear flows</h2><p>Every REST operation gets ordinary control flow.</p></div>
</div>

<div class="mt-12 border-l-[7px] border-brand-orange pl-[1.2rem] text-[1.7rem] font-[650] leading-[1.25]" v-click>Pay the integration cost once, near the async API.</div>

<!--
Be honest: coroutines are not free of machinery. The leverage comes from centralizing it. NetworkTask is the policy, the awaitable is the bridge, and the account methods are the beneficiaries.
-->

---

# Example 01: what changed for the reader?

| Question during review     | Callback version               | Coroutine version             |
| -------------------------- | ------------------------------ | ----------------------------- |
| What happens next?         | Follow the registered callback | Read the next statement       |
| Where is failure handled?  | Search each branch             | Beside the awaited operation  |
| What state survives?       | Inspect captures               | Inspect locals and parameters |
| Where can execution pause? | Know the API                   | Find `co_await`               |

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800">The win is not fewer characters. It is fewer places the reader must keep in their head.</div>

<!--
Transition: now take exactly the same model—store a continuation, resume it later—and apply it somewhere with no network request at all.
-->

---
class: "flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-section-title [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="font-mono text-base font-extrabold leading-none tracking-[0.15em] text-cyan-300">EXAMPLE 02</div>

# Crossing Windows threads

<div class="mt-[1.3rem] max-w-[820px] text-[1.45rem] text-slate-300">Use a coroutine’s continuation as the unit of work passed through an APC.</div>

---

# Desktop Duplicator: the tool behind the talks

<div class="mt-6 grid grid-cols-[0.76fr_1.24fr] items-center gap-[2.1rem]">
  <div class="grid grid-cols-1 items-center gap-[1.2rem] rounded-2xl border border-line bg-soft p-5 text-center [&_h2]:m-0 [&_h2]:text-[1.6rem] [&_p]:mt-[0.3rem] [&_p]:mb-0 [&_p]:text-muted">
    <div class="mx-auto grid size-[100px] place-items-center rounded-3xl bg-ink text-[3.2rem] text-white">▣</div>
    <div><h2>Desktop Duplicator</h2><p>Captures part of one display and presents it elsewhere—ideal for live coding.</p></div>
  </div>
  <div>
    <img class="w-full rounded-[10px] border border-line bg-white p-3" src="./snippets/desktop-duplication-cpp/docs/usage-schema.png" alt="Desktop Duplicator usage schema" />
  </div>
</div>

<!--
Emotional connection: if you have watched live coding and wondered how the presenter keeps private notes while projecting only the editor, this tool exists for that.
-->

---

# One loop, three thread affinities

<div class="mt-[1.4rem] flex items-stretch gap-[0.7rem]">
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-blue-600 p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>CAPTURE</strong><small>acquire desktop frame</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-orange p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>RENDER</strong><small>update texture + draw</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-green p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>MAIN</strong><small>update UI status</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>↩</div>
</div>

<div class="mt-[2.2rem] max-w-[970px] text-[2.3rem] font-[760] tracking-[-0.04em] leading-[1.07]" v-click>The loop is one story. The work must execute in three places.</div>

<!--
Clarify that thread hopping is a requirement, not cleverness: capture APIs, renderer resources, and UI each have affinity. APC means Asynchronous Procedure Call: ask a target thread to execute a callback when it enters an alertable wait.
-->

---

# APC: a C callback plus one integer

<<< ./snippets/compiled-patterns/main.cpp#apc-contract cpp {1|2-5|all}{lines:true}

<div class="mt-[0.55rem] text-[0.78rem] text-muted">A portable, compiling test double for the Windows APC callback-and-integer contract.</div>

<div class="mt-7 grid grid-cols-3 gap-[1.2rem]">
  <div class="rounded-[14px] border border-line bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>API gives us</h3><p>A function pointer and an integer-sized payload.</p></div>
  <div class="rounded-[14px] border border-line bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>Our work needs</h3><p>Typed state with a clear owner.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>The gap</h3><p>Who allocates, invokes, and deletes?</p></div>
</div>

<!--
The Windows API is intentionally low-level. The target thread must enter an alertable wait such as SleepEx(..., TRUE). Focus on the ownership gap.
-->

---

# First improvement: make ownership explicit

<div class="mt-7 grid grid-cols-2 items-center gap-[2.1rem]">
  <div>

<<< ./snippets/compiled-patterns/main.cpp#unique-apc-transfer cpp {1-2|3-5|all}{lines:true}

  </div>
  <div>
    <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>Before queueing</h3><p><code>unique_ptr</code> owns the work.</p></div>
    <div class="mt-4 rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>After success</h3><p>The callback must reconstruct ownership.</p></div>
    <div class="mt-4 rounded-[14px] border border-line bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>After failure</h3><p>RAII cleans up normally.</p></div>
  </div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800">Safer memory, but ownership protocol still distracts from the capture loop.</div>

<!--
This is a worthwhile step. We should not pretend the only good migration jumps straight to coroutines. unique_ptr establishes explicit transfer and fixes failure cleanup.
-->

---

# Second improvement: hide the callback adapter

<div class="mt-7 grid grid-cols-[1.28fr_0.72fr] items-center gap-[2.1rem]">
  <div>

<<< ./snippets/compiled-patterns/main.cpp#lambda-apc-call cpp {1-5}{lines:true}

  </div>
  <div>
    <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>Better local syntax</h3><p>Typed capture replaces casts and raw deletion.</p></div>
    <div class="mt-4 rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>Still fragmented</h3><p>Each hop ends one scope and begins another.</p></div>
  </div>
</div>

<div class="mt-10 border-l-[7px] border-brand-orange pl-[1.2rem] text-[1.7rem] font-[650] leading-[1.25]">A lambda packages work. It does not reconnect the story.</div>

<!--
The project has a UniqueCallbackAdapter which does this carefully, including small-object storage. It is useful infrastructure. But a chain of posted lambdas still makes the whole loop hard to see.
-->

---

# The coroutine frame is the work package

<div class="mt-7 grid grid-cols-[0.76fr_1.24fr] items-center gap-[2.1rem]">
  <div>
    <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-blue-600 p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted"><strong>coroutine handle</strong><small>address fits in ULONG_PTR</small></div>
    <div class="self-center text-center text-[1.8rem] font-extrabold text-brand-orange">↓ QueueUserAPC</div>
    <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-orange p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted"><strong>resume()</strong><small>continue on target thread</small></div>
  </div>
  <div>

<<< ./snippets/desktop-duplication-cpp/src/win32/Thread.h#resume-on-awaitable cpp {1-4|5-12|13-19|all}{lines:true}

  </div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800"><code>co_await resumeOn(thread)</code> means exactly: continue this function over there.</div>

<!--
The payload is no longer a bespoke Work allocation. It is the coroutine handle—the continuation plus access to its frame. Production code must handle QueueUserAPC failure; the real implementation destroys the handle on failure.
-->

---

# The loop now expresses thread intent

<<< ./snippets/desktop-duplication-cpp/src/DuplicationController.cpp#run-capture-acquire-render cpp {1-3|4-10|11-17|all}{lines:true}

<div class="mt-[0.55rem] text-[0.78rem] text-muted">Imported directly from <code>DuplicationController.cpp::runCapture</code> in the Windows project.</div>

<!--
[click] Start on capture.
[click] Acquire there.
[click] Resume and render there.
[click] Detour to main only when status needs it.
[click] Return to capture before releasing the frame. The active thread is visible at each await.
-->

---

# Return before releasing the captured frame

<<< ./snippets/desktop-duplication-cpp/src/DuplicationController.cpp#run-capture-status-release cpp {1-7|8-14|15-17|all}{lines:true}

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800"><code>releaseFrame()</code> visibly happens on the capture thread—even after the optional main-thread detour.</div>

<!--
[click] Inspect status on the render thread.
[click] Detour to main only for the UI update.
[click] Resume capture before releasing its frame. This is the ownership rule expressed as control flow.
-->

---

# Read `co_await` as a lane change

<div class="mt-[1.4rem] flex items-stretch gap-[0.7rem]">
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-blue-600 p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>CAPTURE</strong><small>initialize + acquire</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-orange p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>RENDER</strong><small>update + draw</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-brand-green p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>MAIN?</strong><small>status detour</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line border-t-[7px] border-t-blue-600 p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>CAPTURE</strong><small>release + repeat</small></div>
</div>

<div class="mt-12 border-l-[7px] border-brand-orange pl-[1.2rem] text-[1.7rem] font-[650] leading-[1.25]" v-click>Ownership follows the frame. Execution follows the await.</div>

<!--
This is the architectural payoff. We no longer need the reader to reconstruct a graph of callbacks. The coroutine is the graph, flattened into a sequence.
-->

---

# Example 02: the migration ladder

<div class="mt-[1.4rem] flex items-stretch gap-[0.7rem]">
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>raw APC payload</strong><small>manual allocation</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>unique_ptr transfer</strong><small>safe ownership</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>lambda adapter</strong><small>typed packaging</small></div>
  <div class="self-center text-[1.8rem] font-extrabold text-brand-orange" v-click>→</div>
  <div class="flex min-h-[115px] flex-1 flex-col justify-center rounded-xl border-2 border-line p-[0.8rem] text-center [&_strong]:text-brand-blue [&_small]:mt-[0.35rem] [&_small]:text-muted" v-click><strong>coroutine handle</strong><small>continuous intent</small></div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800" v-click>Each rung was useful. Coroutines became compelling only after the ownership boundary was understood.</div>

<!--
This is a migration story, not a rewrite story. Keep improvements incremental and observable. Coroutines did two jobs: first simplify memory ownership, then reveal architecture.
-->

---
class: "flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-section-title [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="font-mono text-base font-extrabold leading-none tracking-[0.15em] text-cyan-300">LEARNINGS + PITFALLS</div>

# The frame survives.<br />Does your object?

<div class="mt-[1.3rem] max-w-[820px] text-[1.45rem] text-slate-300">Coroutine syntax can look safely local while execution actually begins—or resumes—after local owners are gone.</div>

---

# The tempting coroutine lambda

<div class="mt-7 grid grid-cols-[1.28fr_0.72fr] items-center gap-[2.1rem]">
  <div>

<<< ./snippets/coroutine-lifetime/main.cpp#lambda-risky cpp {1-2|3-6|7}{lines:true}

  </div>
  <div>
    <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>The capture belongs to…</h3><p>the lambda closure object.</p></div>
    <div class="mt-4 rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_h3]:mt-0 [&_h3]:mb-2 [&_h3]:text-base [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><h3>The coroutine owns…</h3><p>a frame that may outlive that closure.</p></div>
  </div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-rose-700">A coroutine member body can start after the closure object has already been destroyed.</div>

<!--
This is the key pitfall. Captures are members of the closure object; invoking a coroutine lambda is a coroutine member call. Depending on initial_suspend, the body may not run before the temporary/local closure dies. Even if it starts immediately, a later resume can still outlive referenced objects.
-->

---

# What happens behind the syntax

<LambdaLifetime />

<!--
[click] We create a closure containing state.
[click] Calling its coroutine operator creates/returns a coroutine; the closure is effectively its this object.
[click] launch returns and destroys the local closure.
[click] The coroutine body begins or resumes and reaches through a dangling this to state. The frame survived, but the capture did not move into it just because the body looks local.
-->

---

# Prefer a named coroutine with value parameters

<div class="mt-6 grid grid-cols-2 items-center gap-[2.1rem]">
<div>
  <div class="mb-[0.45rem] font-mono text-xs font-[650] leading-none text-rose-700">RISKY · closure owns the state</div>

<<< ./snippets/coroutine-lifetime/main.cpp#lambda-risky cpp {2-6}{lines:true}

</div>
<div>
  <div class="mb-[0.45rem] font-mono text-xs font-[650] leading-none text-brand-green">CLEARER · frame owns the value</div>

<<< ./snippets/coroutine-lifetime/main.cpp#named-safe cpp {1-4|6}{lines:true}

</div>
</div>

<div class="mt-[1.4rem] rounded-xl bg-cyan-50 px-[1.1rem] py-[0.9rem] text-base font-bold text-cyan-800">Pass required state by value into a named coroutine. Make external lifetime dependencies explicit and guarded.</div>

<!--
This is the simplest habit. Value parameters are copied/moved into the frame according to coroutine rules. For objects that cannot be copied, establish explicit shared ownership or cancellation/lifetime protocols. A raw this capture does not establish lifetime.
-->

---

# Four checks before shipping

<div class="mt-8 grid grid-cols-2 items-center gap-[2.1rem]">
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>① Suspension</h2><p>Mark every point where other code may run before continuation.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>② Ownership</h2><p>Know which values live in the frame and which are merely referenced.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>③ Cancellation</h2><p>Decide what happens if the owner or operation disappears.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>④ Destruction</h2><p>Know who destroys the frame on success, failure, and abandonment.</p></div>
</div>

<!--
Coroutines simplify control flow; they do not repeal lifetime rules. This checklist is a compact design review. If these answers are unclear, hide less machinery until they become clear.
-->

---

# Where are your low-hanging coroutines?

<div class="mt-10 grid grid-cols-3 gap-[1.2rem]">
  <div class="rounded-[14px] border border-line bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>Async boundary</h2><p>Callbacks, signals, futures, completion events.</p></div>
  <div class="rounded-[14px] border border-line bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>Leaking state machine</h2><p>Enums and branches whose job is “what happens next”.</p></div>
  <div class="rounded-[14px] border border-line bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted" v-click><h2>Manual hand-off</h2><p>Work objects carried between threads or event loops.</p></div>
</div>

<div class="mt-12 border-l-[7px] border-brand-orange pl-[1.2rem] text-[1.7rem] font-[650] leading-[1.25]" v-click>Start with one adapter and one painful flow—not a coroutine framework.</div>

<!--
Give the audience a practical action for Monday. Find a single boundary, build or adopt a narrow awaitable, and compare readability plus lifetime behavior. Do not convert synchronous code merely because the syntax is new.
-->

---

# The whole talk in three lines

<div class="mt-12 grid grid-cols-3 gap-[1.2rem]">
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><div class="text-[2.7rem] font-extrabold text-brand-blue">1</div><h2>Await the event</h2><p>Keep asynchronous plumbing at the boundary.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-cyan bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><div class="text-[2.7rem] font-extrabold text-brand-blue">2</div><h2>Express the intent</h2><p>Let the use case read in execution order.</p></div>
  <div class="rounded-[14px] border border-line border-t-[5px] border-t-brand-orange bg-soft px-5 py-[1.1rem] [&_p]:m-0 [&_p]:text-[0.9rem] [&_p]:text-muted"><div class="text-[2.7rem] font-extrabold text-brand-blue">3</div><h2>Audit lifetime</h2><p>The frame survives; references may not.</p></div>
</div>

<div class="mt-[2.2rem] max-w-[970px] text-[2.25rem] font-[760] tracking-[-0.04em] leading-[1.07] [&_em]:text-brand-blue [&_em]:not-italic" v-click>Coroutines are most valuable where your code is <em>already pretending to be sequential.</em></div>

<!--
Recap both examples. Same mechanism, very different APIs. The durable lesson is to make continuations visible while keeping ownership explicit.
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

<div class="text-[0.78rem] font-extrabold tracking-[0.14em] text-brand-orange uppercase">THANK YOU · QUESTIONS?</div>

# <span class="text-cyan-300">play more</span><br />with coroutines

<p>Start small. Read the flow. Respect the lifetime.</p>

<div class="mt-10 flex items-center gap-8">
  <span>Andreas Reischuck</span>
  <span class="text-cyan-300">HicknHack Software GmbH</span>
</div>

<!--
End on the requested phrase. Leave this slide up for questions.
-->
