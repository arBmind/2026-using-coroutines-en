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
class: cover ps-24!
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

# Simple Coroutine

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

# Step Through Coroutine

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
    <div class="grid min-h-[54px] grid-cols-[0.6fr_1.4fr] items-center gap-[0.45rem] rounded-[9px] border-l-6 border-brand-cyan bg-white px-[0.7rem] py-[0.55rem] shadow-[0_5px_14px_rgb(15_23_42_/_0.08)] [&_strong]:text-[0.82rem] [&_strong]:text-brand-blue [&_code]:text-base [&_code]:font-bold [&_code]:text-slate-800">
      <strong>coroutine</strong><code>co_await Awaiter{};</code>
    </div>
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
class: "relative overflow-hidden flex flex-col justify-center bg-[#0b111b] text-white [&_h1]:text-white [&_h1::after]:hidden"
---

<div class="absolute right-20 bottom-6 font-mono text-[10rem] font-black leading-none text-white/3">01</div>

<div class="font-mono text-sm font-extrabold leading-none tracking-[0.17em] text-cyan-300">1st Use Case</div>

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
    <span class="rounded-full border border-cyan-300/35 bg-cyan-300/8 px-3 py-2 font-mono text-[0.64rem] font-extrabold tracking-[0.08em] text-cyan-200">HTTP · REST</span>
    <span class="w-px h-5 bg-gradient-to-b from-cyan-300 to-transparent"></span>
  </div>
  <div class="grid place-items-center rounded-2xl border border-white/12 bg-white/5 px-6 py-5"><img class="h-[92px] w-auto" :src="'/images/qt-logo.png'" alt="Qt" /></div>
</div>

<!--
First use case: the Hourglass desktop and Android app. It is a Qt client for the REST API exposed by our Hourglass plugin for Redmine.
-->

---
transition: slide-left
---

<div class="mt-6 grid grid-cols-[1fr_auto_1fr] items-stretch gap-5">
  <section class="rounded-2xl border border-slate-300 bg-white p-5 shadow-slide-card">
    <div class="flex h-20 items-center"><img class="max-h-16 max-w-[250px]" :src="'/images/redmine-logo.png'" alt="Redmine" /></div>
    <h2 class="mt-4! text-[1.35rem]!">Project management</h2>
  </section>
  <div class="grid place-items-center text-4xl font-light text-brand-orange">+</div>
  <section class="rounded-2xl border-2 border-brand-orange bg-orange-50 p-5 shadow-[0_12px_28px_rgb(234_88_12_/_0.1)]">
    <div class="flex h-20 items-center gap-4"><img class="size-16" src="./assets/hourglass/appicon.ico" alt="Hourglass" /><strong class="text-2xl text-orange-900">Hourglass</strong></div>
    <h2 class="mt-4! text-[1.35rem]! text-orange-900!">Time tracking plugin</h2>
  </section>
</div>

<div class="mt-2 text-left text-[0.48rem] text-slate-400">Redmine logo © Martin Herr, CC BY-SA 2.5.</div>

<!--
Like most software companies, HicknHack is time constrained, so we want to understand how long each job takes. Redmine is our ticket system. We maintain the Hourglass plugin, and this app is its desktop and Android companion.
-->

---
transition: slide-left
---

<div class="mt-7 grid grid-cols-[0.48fr_1.52fr] items-center gap-8">
  <div class="grid place-items-center rounded-3xl bg-[#0b111b] p-8 shadow-[0_18px_38px_rgb(15_23_42_/_0.16)]"><img class="w-[210px]" :src="'/images/qt-logo.png'" alt="Qt" /></div>
  <div>
    <div class="mb-5 text-[1.55rem] font-[760] tracking-[-0.035em] text-brand-blue">A cross-platform C++ application framework</div>
    <div class="grid grid-cols-3 gap-3 text-center text-[0.78rem] font-bold">
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
  <div><code class="text-[1.2rem] font-bold text-brand-blue">QNetworkAccessManager</code><span class="ml-3 text-[0.9rem] text-muted">is Qt’s asynchronous HTTP client abstraction.</span></div>
</div>

<div class="mt-2 text-right text-[0.48rem] text-slate-400">Qt is a registered trademark of The Qt Company Ltd. and its subsidiaries.</div>

<!--
Qt is the GUI framework, but it also abstracts a large part of the operating system. For this use case, the important abstraction is networking—and specifically QNetworkAccessManager.
-->

---
transition: slide-left
---

# The concept

<HourglassAction />

<!--
[click] Start the tracker: the UI remains alive while the coroutine waits for POST.
[click] finished() resumes the frame and applies the tracker response.
[click] The optional issue lookup becomes a second local suspension point.
[click] The same function returns the model to Idle. Point out that no native thread was occupied while the frame was suspended.
-->

---
transition: slide-left
---

<div class="mt-7 grid grid-cols-[0.82fr_0.48fr] items-center gap-7">
  <div class="flex flex-col gap-5">

<<< ./snippets/qnetwork-access-manager.cpp#qnam-api cpp {3}{lines:true}

<<< ./snippets/qnetwork-access-manager.cpp#qnam-start cpp {0|2|3-5}{lines:true}

  </div>
  <div class="grid gap-5">
    <div class="grid grid-cols-[52px_1fr] items-center gap-3 rounded-xl border border-line bg-soft p-3" v-click="1"><span class="grid size-11 place-items-center rounded-full bg-brand-blue font-mono font-black text-white">1</span><div><strong class="text-lg text-brand-blue">Call <code>get(request)</code></strong><span class="mt-1 block text-muted">Queue work</span></div></div>
    <div class="grid grid-cols-[52px_1fr] items-center gap-3 rounded-xl border border-line bg-soft p-3" v-click="1"><span class="grid size-11 place-items-center rounded-full bg-brand-orange font-mono font-black text-white">2</span><div><strong class="text-lg text-brand-blue">Receive <code>QNetworkReply</code></strong><span class="mt-1 block text-muted">Future for the work.</span></div></div>
    <div class="grid grid-cols-[52px_1fr] items-center gap-3 rounded-xl border-2 border-brand-cyan bg-cyan-50 p-3" v-click="3"><span class="grid size-11 place-items-center rounded-full bg-brand-cyan font-mono font-black text-white">3</span><div><strong class="text-lg text-cyan-900">Signal <code>finished()</code></strong><span class="mt-1 block text-cyan-900/65">Called when done.</span></div></div>
  </div>
</div>

<!--
[click] Call get with a request.
[click] The return value is a QNetworkReply pointer, but the response is not ready yet.
[click] Completion arrives later as a Qt signal. The UI thread remains available to process input and paint.
-->

---
transition: slide-left
---

# Callback

<div class="mt-4 grid grid-cols-[1.24fr_0.76fr] items-start gap-5">
  <div>

<<< ./snippets/qnetwork-access-manager.cpp#qnam-response cpp {0|2-5|6-10|11-16|17|18}{lines:true}

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
[click] Transport errors belong to the reply.
[click] HTTP failure is a separate protocol layer.
[click] A successful HTTP body can still fail JSON parsing.
[click] Only then process the value and decide what runs next—while also managing reply lifetime. One request has accumulated five reader concerns.
-->

---
transition: slide-left
---

# CoRoutine Machinery

<<< ./snippets/hourglass-reply-await-bridge.cpp cpp {2|3-5,23|6-7,20-21|9|12-13|16-18}{lines:true}

<!--
The Qt API still emits finished().
[click] One narrow adapter connects that signal to the saved coroutine handle.
[click] Domain code sees an awaitable operation. The callback did not disappear—it stopped shaping every caller.
[click] await_ready preserves the fast path when the reply has already completed.
[click] await_suspend is the entire signal-to-resume bridge. This excerpt only reformats the real NetworkTask.h lines to fit the projector; the production await_resume converts network, HTTP, and JSON outcomes into NetworkResponse.
-->

---
transition: slide-left
---

# Now the refresh policy reads top to bottom

<<< ./snippets/qnetwork-access-manager.cpp#refresh-coroutine cpp {0|1-2|4-6|8-10|12-14|16}{lines:true}

<div class="mt-3 grid grid-cols-5 gap-2 text-[0.68rem] font-bold">
  <div class="rounded-lg border-l-4 border-brand-blue bg-slate-100 px-3 py-2"><span class="text-muted">01</span> Updating</div>
  <div class="rounded-lg border-l-4 border-brand-orange bg-orange-50 px-3 py-2"><span class="text-muted">02</span> Current user</div>
  <div class="rounded-lg border-l-4 border-brand-orange bg-orange-50 px-3 py-2"><span class="text-muted">03</span> Activities</div>
  <div class="rounded-lg border-l-4 border-brand-orange bg-orange-50 px-3 py-2"><span class="text-muted">04</span> Tracker</div>
  <div class="rounded-lg border-l-4 border-brand-green bg-emerald-50 px-3 py-2"><span class="text-muted">05</span> Live</div>
</div>

<div class="mt-2 flex items-center gap-2 text-[0.7rem] text-muted"><span class="i-carbon-document"></span> The coroutine version from the sketch, with names shortened for the projector.</div>

<!--
[click] Establish the updating state.
[click] Await and apply the current user; its failure path stays beside it.
[click] Do the same for activities.
[click] Do the same for the active tracker, including the expected not-found case.
[click] Mark the account live. Three network waits now read as one refresh policy.
-->

---
transition: fade-out
---

# Small boundary. Repeated leverage.

<div class="mt-7 grid grid-cols-[0.78fr_auto_1.22fr] items-center gap-6">
  <div class="grid grid-cols-2 gap-3">
    <div class="rounded-2xl border-2 border-brand-cyan bg-cyan-50 p-5 text-center" v-click>
      <div class="text-[3.5rem] font-black leading-none text-brand-blue">1</div>
      <h2 class="mt-2! text-[1rem]!">task policy</h2>
      <p class="m-0! text-[0.72rem] text-muted">owns the frame</p>
    </div>
    <div class="rounded-2xl border-2 border-brand-cyan bg-cyan-50 p-5 text-center" v-click>
      <div class="text-[3.5rem] font-black leading-none text-brand-blue">1</div>
      <h2 class="mt-2! text-[1rem]!">reply awaiter</h2>
      <p class="m-0! text-[0.72rem] text-muted">bridges the signal</p>
    </div>
  </div>

  <div class="flex flex-col items-center gap-2 text-brand-orange" v-click>
    <span class="font-mono text-[0.62rem] font-extrabold tracking-[0.1em]">UNLOCKS</span>
    <span class="i-carbon-arrow-right text-4xl"></span>
  </div>

  <div class="rounded-2xl bg-[#0f172a] p-5 text-white shadow-[0_18px_35px_rgb(15_23_42_/_0.16)]" v-click>
    <div class="flex items-end justify-between border-b border-white/12 pb-3">
      <div><span class="text-[3.8rem] font-black leading-none text-cyan-300">7</span><span class="ml-2 text-lg font-bold">straight-line flows</span></div>
      <span class="i-carbon-flow text-2xl text-cyan-300"></span>
    </div>
    <div class="mt-3 grid grid-cols-2 gap-x-4 gap-y-2 font-mono text-[0.68rem] text-slate-300">
      <span>refresh account</span><span>refresh time</span><span>start tracker</span><span>update tracker</span><span>stop tracker</span><span>search issues</span><span>resolve issues</span>
    </div>
  </div>
</div>

<div class="mt-9 border-l-[7px] border-brand-orange pl-5 text-[1.65rem] font-[700] leading-[1.2]" v-click>Pay the integration cost once—<span class="text-brand-blue">where the asynchronous API enters your code.</span></div>

<!--
[click] One small task type owns the coroutine handle and completion policy.
[click] One awaiter turns QNetworkReply completion into resumption.
[click] Those two pieces support seven real REST workflows in this model today.
[click] This is the low-hanging-benefit test: narrow plumbing at a stable boundary, repeated clarity above it.
-->

---
class: "relative overflow-hidden [&_h1::after]:hidden"
---

# The win is not fewer characters.

<div class="mt-8 grid grid-cols-2 gap-5">
  <section class="rounded-2xl border border-slate-300 bg-slate-100 p-5 opacity-72" v-click>
    <div class="mb-4 flex items-center justify-between"><span class="font-mono text-xs font-extrabold tracking-[0.11em] text-muted">CALLBACK REVIEW</span><span class="i-carbon-direction-loop-right text-2xl text-slate-400"></span></div>
    <div class="grid gap-3 text-[0.9rem]">
      <div>What callback runs next?</div>
      <div>Which captures are still alive?</div>
      <div>Which branch restores the state?</div>
      <div>Where can execution pause?</div>
    </div>
  </section>
  <section class="rounded-2xl border-2 border-brand-cyan bg-cyan-50 p-5 shadow-[0_14px_30px_rgb(8_145_178_/_0.12)]" v-click>
    <div class="mb-4 flex items-center justify-between"><span class="font-mono text-xs font-extrabold tracking-[0.11em] text-brand-blue">COROUTINE REVIEW</span><span class="i-carbon-direction-straight-right text-2xl text-brand-cyan"></span></div>
    <div class="grid gap-3 text-[0.9rem] font-semibold text-cyan-950">
      <div>Read the next statement.</div>
      <div>Inspect locals and parameters.</div>
      <div>Handle failure beside the operation.</div>
      <div>Find <code>co_await</code>.</div>
    </div>
  </section>
</div>

<div class="mt-8 text-center text-[2.2rem] font-[780] tracking-[-0.04em]" v-click>It is fewer <span class="relative text-brand-blue">suspended questions.<svg class="absolute -bottom-2 left-0 w-full" viewBox="0 0 300 12" aria-hidden="true"><path d="M4 8 C78 2 210 12 296 5" fill="none" stroke="#ea580c" stroke-width="5" stroke-linecap="round" /></svg></span></div>

<!--
[click] In the callback version, review questions are graph questions.
[click] In the coroutine version, the answers are local in the function.
[click] The value is not a source-line contest; it is reducing the number of unresolved questions the reader carries. Now reuse the same continuation idea somewhere with no network request at all.
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
