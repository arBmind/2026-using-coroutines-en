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

<div class="grid">
  <div class="text">
    <div class="eyebrow">Meeting C++ 2026</div>
    <h1>C++20 Coroutines<br />in Action</h1>
    <div class="subtitle">Grab the low-hanging benefits—without building a framework</div>
    <div class="speaker">Andreas Reischuck</div>
  </div>
  <div class="portrait">
    <img class="logo" :src="'/images/hicknhackLogo_new_text.png'" alt="HicknHack Software GmbH" />
    <img class="person" :src="'/images/andreas.png'" alt="Andreas Reischuck" />
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
class: section-slide
---

<div class="big-claim">
C++20 Coroutines
</div>

<div class="mt-8 callout">
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

<div class="await-suspend-combined">
  <section class="handle-choice-column">
    <div class="handle-choice return-noop" v-click>
      <div class="handle-choice-code"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> std::noop_coroutine();<br>
      }</div>
      <div class="handle-next"><span>Next:</span><strong>last resumer</strong></div>
    </div>
    <div class="handle-choice return-current" v-click>
      <div class="handle-choice-code"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> continuation;<br>
      }</div>
      <div class="handle-next"><span>Next:</span><strong>co_await calling coroutine</strong></div>
    </div>
    <div class="handle-choice return-other" v-click>
      <div class="handle-choice-code"><span>Handle</span> await_suspend(<span>Handle</span> continuation) {<br>
      &nbsp;&nbsp;<b>return</b> otherCoroutine;<br>
      }</div>
      <div class="handle-next"><span>Next:</span><strong>other coroutine</strong></div>
    </div>
  </section>

  <section class="native-stack compact-native-stack">
    <div class="stack-axis"><span>NATIVE STACK · INSIDE await_suspend</span></div>
    <div class="native-frame awaiter-frame">
      <strong>awaiter</strong>
      <code>await_suspend(continuation)</code>
    </div>
    <div class="stack-called-by">↑ called by</div>
    <div class="native-frame coroutine-call-frame">
      <strong>coroutine</strong>
      <code>co_await Awaiter{};</code>
    </div>
    <div class="stack-called-by">↑ resumed by</div>
    <div class="native-frame resumer-frame">
      <strong>last resumer</strong>
      <code>coroutine.resume()</code>
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

<div class="portrait-intro mt-6">
  <img :src="'/images/andreas.png'" alt="Andreas Reischuck" />
  <div>
    <div class="big-claim !mt-0 !text-[2.55rem]">I like code that says what it is <em>trying to do.</em></div>
    <div class="identity-list">
      <div class="identity-item"><span class="identity-icon">⌁</span><span>C++ developer and software architect</span></div>
      <div class="identity-item"><span class="identity-icon">◫</span><span>Builder of tools people use every day</span></div>
      <div class="identity-item"><span class="identity-icon">✦</span><span>Host, trainer, and C++ community enthusiast</span></div>
    </div>
  </div>
</div>

<!--
Personal bridge: these examples are not invented kata. They come from tools I care about and maintain.
-->

---
layout: default
class: section-slide
---

<div class="section-number">EXAMPLE 01</div>

# Awaiting a REST API

<div class="section-promise">Turn “what callback comes next?” into a sequence that matches the user action.</div>

---

# Hourglass: a small app with real async work

<div class="two-col mt-8">
  <div class="app-card">
    <div class="app-icon">◷</div>
    <div><h2>Hourglass</h2><p>Qt Quick companion app for tracking and booking time through a Redmine REST API.</p></div>
  </div>
  <div>
    <div class="flow-node"><strong>User presses “Start”</strong><small>the UI must remain responsive</small></div>
    <div class="flow-arrow text-center">↓</div>
    <div class="flow-node"><strong>POST tracker → maybe GET issue</strong><small>each response can fail</small></div>
  </div>
</div>

<div class="takeaway">The domain story is sequential. The network API is not.</div>

<!--
Introduce the concrete action: begin tracking time, update the model, and if necessary load the referenced issue. Network latency must not block the Qt event loop.
-->

---

# Before: the shape belongs to callbacks

<div class="two-col wide-left mt-6">
<div>

<<< ./snippets/compiled-patterns/main.cpp#callback-flow cpp {1-3|4-8|9-17|all}{lines:true}

</div>
<div>
  <div class="panel accent-panel"><h3>Control flow</h3><p>Encoded in nesting.</p></div>
  <div class="panel accent-panel mt-4"><h3>State</h3><p>Repeated across exit paths.</p></div>
  <div class="panel accent-panel mt-4"><h3>Lifetime</h3><p>Hidden inside captures.</p></div>
</div>
</div>

<div class="code-note">Runnable callback example from <code>compiled-patterns</code>; the project build executes this flow.</div>

<!--
Do not attack callbacks: they are a valid tool. The problem is the mismatch between the business flow and the code’s shape. Point at every place Idle must be restored and every this capture.
-->

---

# The state machine is already here

<div class="flow-row">
  <div class="flow-node" v-click><strong>WRITING</strong><small>POST in flight</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node" v-click><strong>APPLY</strong><small>update tracker</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node" v-click><strong>RESOLVE?</strong><small>maybe GET issue</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node" v-click><strong>IDLE</strong><small>or stale on error</small></div>
</div>

<div class="big-claim !text-[2.3rem]" v-click>The compiler can carry the “where was I?” state for us.</div>

<!--
This is the low-hanging-fruit test: if you already draw a state machine to explain callback progression, a coroutine may give you that state machine in a readable function.
-->

---

# The idea: make a reply awaitable

<div class="two-col wide-right mt-7">
  <div>
    <div class="flow-node"><strong>QNetworkReply</strong><small>emits finished()</small></div>
    <div class="flow-arrow text-center">↓</div>
    <div class="flow-node"><strong>awaitable adapter</strong><small>resumes coroutine handle</small></div>
  </div>
  <div>

<<< ./snippets/slide-projects/hourglass-app2/src/Hourglass/Model/NetworkTask.h#qt-reply-await-bridge cpp {1|4-7|all}{lines:true}

  </div>
</div>

<div class="takeaway">The Qt signal remains. It is contained at one boundary instead of shaping every caller.</div>

<!--
This is the minimum interesting plumbing. await_ready handles an already-finished reply. await_suspend connects completion to resume. await_resume turns the result into the type the domain code wants.
-->

---

# After: the shape belongs to the use case

<<< ./snippets/slide-projects/hourglass-app2/src/Hourglass/Model/RedmineAccount.cpp#start-time-tracker-coroutine cpp {1-3|4-11|13-20|22}{lines:true}

<div class="code-note">Imported directly from the compiling Hourglass project: <code>RedmineAccount.cpp::startTimeTrackerImpl</code>.</div>

<!--
[click] Establish state.
[click] Await the POST and handle failure beside it.
[click] Optionally await the issue lookup.
[click] Finish in Idle. We can review this in domain order.
-->

---

# What code was actually necessary?

<div class="three-col mt-10">
  <div class="panel blue-panel" v-click><div class="metric">1</div><h2>Task type</h2><p>Owns the coroutine handle and completion policy.</p></div>
  <div class="panel blue-panel" v-click><div class="metric">1</div><h2>Awaitable</h2><p>Bridges <code>finished()</code> to <code>resume()</code>.</p></div>
  <div class="panel accent-panel" v-click><div class="metric">N</div><h2>Clear flows</h2><p>Every REST operation gets ordinary control flow.</p></div>
</div>

<div class="quote mt-12" v-click>Pay the integration cost once, near the async API.</div>

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

<div class="takeaway">The win is not fewer characters. It is fewer places the reader must keep in their head.</div>

<!--
Transition: now take exactly the same model—store a continuation, resume it later—and apply it somewhere with no network request at all.
-->

---
class: section-slide
---

<div class="section-number">EXAMPLE 02</div>

# Crossing Windows threads

<div class="section-promise">Use a coroutine’s continuation as the unit of work passed through an APC.</div>

---

# Desktop Duplicator: the tool behind the talks

<div class="two-col wide-right mt-6">
  <div class="app-card !grid-cols-1 text-center">
    <div class="app-icon mx-auto">▣</div>
    <div><h2>Desktop Duplicator</h2><p>Captures part of one display and presents it elsewhere—ideal for live coding.</p></div>
  </div>
  <div>
    <img class="schema-img" src="./snippets/desktop-duplication-cpp/docs/usage-schema.png" alt="Desktop Duplicator usage schema" />
  </div>
</div>

<!--
Emotional connection: if you have watched live coding and wondered how the presenter keeps private notes while projecting only the editor, this tool exists for that.
-->

---

# One loop, three thread affinities

<div class="flow-row">
  <div class="flow-node thread-capture" v-click><strong>CAPTURE</strong><small>acquire desktop frame</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node thread-render" v-click><strong>RENDER</strong><small>update texture + draw</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node thread-main" v-click><strong>MAIN</strong><small>update UI status</small></div>
  <div class="flow-arrow" v-click>↩</div>
</div>

<div class="big-claim !text-[2.3rem]" v-click>The loop is one story. The work must execute in three places.</div>

<!--
Clarify that thread hopping is a requirement, not cleverness: capture APIs, renderer resources, and UI each have affinity. APC means Asynchronous Procedure Call: ask a target thread to execute a callback when it enters an alertable wait.
-->

---

# APC: a C callback plus one integer

<<< ./snippets/compiled-patterns/main.cpp#apc-contract cpp {1|2-5|all}{lines:true}

<div class="code-note">A portable, compiling test double for the Windows APC callback-and-integer contract.</div>

<div class="three-col mt-7">
  <div class="panel"><h3>API gives us</h3><p>A function pointer and an integer-sized payload.</p></div>
  <div class="panel"><h3>Our work needs</h3><p>Typed state with a clear owner.</p></div>
  <div class="panel accent-panel"><h3>The gap</h3><p>Who allocates, invokes, and deletes?</p></div>
</div>

<!--
The Windows API is intentionally low-level. The target thread must enter an alertable wait such as SleepEx(..., TRUE). Focus on the ownership gap.
-->

---

# First improvement: make ownership explicit

<div class="two-col mt-7">
  <div>

<<< ./snippets/compiled-patterns/main.cpp#unique-apc-transfer cpp {1-2|3-5|all}{lines:true}

  </div>
  <div>
    <div class="panel blue-panel"><h3>Before queueing</h3><p><code>unique_ptr</code> owns the work.</p></div>
    <div class="panel accent-panel mt-4"><h3>After success</h3><p>The callback must reconstruct ownership.</p></div>
    <div class="panel mt-4"><h3>After failure</h3><p>RAII cleans up normally.</p></div>
  </div>
</div>

<div class="takeaway">Safer memory, but ownership protocol still distracts from the capture loop.</div>

<!--
This is a worthwhile step. We should not pretend the only good migration jumps straight to coroutines. unique_ptr establishes explicit transfer and fixes failure cleanup.
-->

---

# Second improvement: hide the callback adapter

<div class="two-col wide-left mt-7">
  <div>

<<< ./snippets/compiled-patterns/main.cpp#lambda-apc-call cpp {1-5}{lines:true}

  </div>
  <div>
    <div class="panel blue-panel"><h3>Better local syntax</h3><p>Typed capture replaces casts and raw deletion.</p></div>
    <div class="panel accent-panel mt-4"><h3>Still fragmented</h3><p>Each hop ends one scope and begins another.</p></div>
  </div>
</div>

<div class="quote mt-10">A lambda packages work. It does not reconnect the story.</div>

<!--
The project has a UniqueCallbackAdapter which does this carefully, including small-object storage. It is useful infrastructure. But a chain of posted lambdas still makes the whole loop hard to see.
-->

---

# The coroutine frame is the work package

<div class="two-col wide-right mt-7">
  <div>
    <div class="flow-node thread-capture"><strong>coroutine handle</strong><small>address fits in ULONG_PTR</small></div>
    <div class="flow-arrow text-center">↓ QueueUserAPC</div>
    <div class="flow-node thread-render"><strong>resume()</strong><small>continue on target thread</small></div>
  </div>
  <div>

<<< ./snippets/desktop-duplication-cpp/src/win32/Thread.h#resume-on-awaitable cpp {1-4|5-12|13-19|all}{lines:true}

  </div>
</div>

<div class="takeaway"><code>co_await resumeOn(thread)</code> means exactly: continue this function over there.</div>

<!--
The payload is no longer a bespoke Work allocation. It is the coroutine handle—the continuation plus access to its frame. Production code must handle QueueUserAPC failure; the real implementation destroys the handle on failure.
-->

---

# The loop now expresses thread intent

<<< ./snippets/desktop-duplication-cpp/src/DuplicationController.cpp#run-capture-acquire-render cpp {1-3|4-10|11-17|all}{lines:true}

<div class="code-note">Imported directly from <code>DuplicationController.cpp::runCapture</code> in the Windows project.</div>

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

<div class="takeaway"><code>releaseFrame()</code> visibly happens on the capture thread—even after the optional main-thread detour.</div>

<!--
[click] Inspect status on the render thread.
[click] Detour to main only for the UI update.
[click] Resume capture before releasing its frame. This is the ownership rule expressed as control flow.
-->

---

# Read `co_await` as a lane change

<div class="flow-row">
  <div class="flow-node thread-capture" v-click><strong>CAPTURE</strong><small>initialize + acquire</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node thread-render" v-click><strong>RENDER</strong><small>update + draw</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node thread-main" v-click><strong>MAIN?</strong><small>status detour</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node thread-capture" v-click><strong>CAPTURE</strong><small>release + repeat</small></div>
</div>

<div class="quote mt-12" v-click>Ownership follows the frame. Execution follows the await.</div>

<!--
This is the architectural payoff. We no longer need the reader to reconstruct a graph of callbacks. The coroutine is the graph, flattened into a sequence.
-->

---

# Example 02: the migration ladder

<div class="flow-row">
  <div class="flow-node" v-click><strong>raw APC payload</strong><small>manual allocation</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node" v-click><strong>unique_ptr transfer</strong><small>safe ownership</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node" v-click><strong>lambda adapter</strong><small>typed packaging</small></div>
  <div class="flow-arrow" v-click>→</div>
  <div class="flow-node" v-click><strong>coroutine handle</strong><small>continuous intent</small></div>
</div>

<div class="takeaway" v-click>Each rung was useful. Coroutines became compelling only after the ownership boundary was understood.</div>

<!--
This is a migration story, not a rewrite story. Keep improvements incremental and observable. Coroutines did two jobs: first simplify memory ownership, then reveal architecture.
-->

---
class: section-slide
---

<div class="section-number">LEARNINGS + PITFALLS</div>

# The frame survives.<br />Does your object?

<div class="section-promise">Coroutine syntax can look safely local while execution actually begins—or resumes—after local owners are gone.</div>

---

# The tempting coroutine lambda

<div class="two-col wide-left mt-7">
  <div>

<<< ./snippets/coroutine-lifetime/main.cpp#lambda-risky cpp {1-2|3-6|7}{lines:true}

  </div>
  <div>
    <div class="panel accent-panel"><h3>The capture belongs to…</h3><p>the lambda closure object.</p></div>
    <div class="panel blue-panel mt-4"><h3>The coroutine owns…</h3><p>a frame that may outlive that closure.</p></div>
  </div>
</div>

<div class="takeaway danger">A coroutine member body can start after the closure object has already been destroyed.</div>

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

<div class="two-col mt-6">
<div>
  <div class="code-label danger">RISKY · closure owns the state</div>

<<< ./snippets/coroutine-lifetime/main.cpp#lambda-risky cpp {2-6}{lines:true}

</div>
<div>
  <div class="code-label good">CLEARER · frame owns the value</div>

<<< ./snippets/coroutine-lifetime/main.cpp#named-safe cpp {1-4|6}{lines:true}

</div>
</div>

<div class="takeaway">Pass required state by value into a named coroutine. Make external lifetime dependencies explicit and guarded.</div>

<!--
This is the simplest habit. Value parameters are copied/moved into the frame according to coroutine rules. For objects that cannot be copied, establish explicit shared ownership or cancellation/lifetime protocols. A raw this capture does not establish lifetime.
-->

---

# Four checks before shipping

<div class="two-col mt-8">
  <div class="panel blue-panel" v-click><h2>① Suspension</h2><p>Mark every point where other code may run before continuation.</p></div>
  <div class="panel blue-panel" v-click><h2>② Ownership</h2><p>Know which values live in the frame and which are merely referenced.</p></div>
  <div class="panel accent-panel" v-click><h2>③ Cancellation</h2><p>Decide what happens if the owner or operation disappears.</p></div>
  <div class="panel accent-panel" v-click><h2>④ Destruction</h2><p>Know who destroys the frame on success, failure, and abandonment.</p></div>
</div>

<!--
Coroutines simplify control flow; they do not repeal lifetime rules. This checklist is a compact design review. If these answers are unclear, hide less machinery until they become clear.
-->

---

# Where are your low-hanging coroutines?

<div class="three-col mt-10">
  <div class="panel" v-click><h2>Async boundary</h2><p>Callbacks, signals, futures, completion events.</p></div>
  <div class="panel" v-click><h2>Leaking state machine</h2><p>Enums and branches whose job is “what happens next”.</p></div>
  <div class="panel" v-click><h2>Manual hand-off</h2><p>Work objects carried between threads or event loops.</p></div>
</div>

<div class="quote mt-12" v-click>Start with one adapter and one painful flow—not a coroutine framework.</div>

<!--
Give the audience a practical action for Monday. Find a single boundary, build or adopt a narrow awaitable, and compare readability plus lifetime behavior. Do not convert synchronous code merely because the syntax is new.
-->

---

# The whole talk in three lines

<div class="three-col mt-12">
  <div class="panel blue-panel"><div class="metric">1</div><h2>Await the event</h2><p>Keep asynchronous plumbing at the boundary.</p></div>
  <div class="panel blue-panel"><div class="metric">2</div><h2>Express the intent</h2><p>Let the use case read in execution order.</p></div>
  <div class="panel accent-panel"><div class="metric">3</div><h2>Audit lifetime</h2><p>The frame survives; references may not.</p></div>
</div>

<div class="big-claim !text-[2.25rem]" v-click>Coroutines are most valuable where your code is <em>already pretending to be sequential.</em></div>

<!--
Recap both examples. Same mechanism, very different APIs. The durable lesson is to make continuations visible while keeping ownership explicit.
-->

---

# Want to build useful things with us?

<div class="two-col mt-8">
  <div class="text-center">
    <img class="brand-logo mx-auto !max-h-[190px] !max-w-[460px]" :src="'/images/hicknhackLogo_new_text.png'" alt="HicknHack Software GmbH" />
  </div>
  <div>
    <div class="big-claim !mt-0 !text-[2.65rem]">Bring curiosity.<br /><em>Keep the ownership clear.</em></div>
    <p class="text-xl muted mt-6">If real C++ systems, thoughtful architecture, and sharing what you learn sound fun—come work with HicknHack Software GmbH.</p>
  </div>
</div>

<!--
Invitation, not a hard sell. Mention current hiring/contact details verbally or add a QR code once a stable jobs URL is chosen.
-->

---

# Your experiment could be the next talk

<div class="two-col mt-7">
  <div class="text-center">
    <img class="community-logo" :src="'/images/cppug.png'" alt="C++ user group" />
  </div>
  <div>
    <div class="quote">You do not need the final answer to give a valuable user-group talk.</div>
    <div class="identity-list mt-8">
      <div class="identity-item"><span class="identity-icon">①</span><span>Show the problem you actually met</span></div>
      <div class="identity-item"><span class="identity-icon">②</span><span>Show the attempt that taught you something</span></div>
      <div class="identity-item"><span class="identity-icon">③</span><span>Invite the room into the next step</span></div>
    </div>
  </div>
</div>

<!--
Invite people to the local/user group and to submit even a short talk. The useful story includes the failed intermediate attempts—just like unique_ptr and lambdas in example two.
-->

---
class: final-slide
---

<div class="eyebrow">THANK YOU · QUESTIONS?</div>

# <span class="play">play more</span><br />with coroutines

<p>Start small. Read the flow. Respect the lifetime.</p>

<div class="mt-10 flex items-center gap-8">
  <span>Andreas Reischuck</span>
  <span class="text-cyan-300">HicknHack Software GmbH</span>
</div>

<!--
End on the requested phrase. Leave this slide up for questions.
-->
