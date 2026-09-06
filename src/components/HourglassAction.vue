<script setup lang="ts">
import { useSlideContext } from "@slidev/client";
import { computed, onMounted, onUnmounted, ref, watchEffect } from "vue";

import activityIcon from "../assets/hourglass/images/activity.png";
import commentIcon from "../assets/hourglass/images/comment.png";
import issueIcon from "../assets/hourglass/images/issue.png";
import projectIcon from "../assets/hourglass/images/project.png";
import startIcon from "../assets/hourglass/images/start_tracker.png";

const stages = [
  {
    label: "READY",
    title: "The user presses Start",
    detail: "One intent enters the Qt event loop.",
  },
  {
    label: "POST",
    title: "Create the tracker",
    detail: "The request leaves; the UI thread does not wait.",
  },
  {
    label: "RESUME",
    title: "Apply the reply",
    detail: "finished() resumes exactly where the function paused.",
  },
  {
    label: "GET",
    title: "Resolve the issue, if needed",
    detail: "A second suspension keeps the optional branch local.",
  },
  {
    label: "IDLE",
    title: "The action is complete",
    detail: "The same function finishes the state transition.",
  },
];

const stage = ref(0);
const { $clicksContext: clicks } = useSlideContext();
const clickId = `hourglass-action-${Math.random()}`;

onMounted(() => {
  const clickInfo = clicks.calculateSince("+1", stages.length - 1);
  clicks.register(clickId, clickInfo);
  watchEffect(() => {
    stage.value = Math.max(
      0,
      Math.min(stages.length - 1, (clickInfo?.currentOffset.value ?? -1) + 1),
    );
  });
});

onUnmounted(() => clicks.unregister(clickId));

const current = computed(() => stages[stage.value]);
const isBusy = computed(() => stage.value > 0 && stage.value < 4);
const requestLabel = computed(() =>
  stage.value >= 3 ? "GET /issues/4711" : "POST /time_trackers/start",
);
</script>

<template>
  <div class="mt-3 grid grid-cols-[1.05fr_0.9fr_0.7fr] items-stretch gap-4">
    <section
      class="app-window overflow-hidden rounded-[18px] border border-slate-300 bg-white shadow-[0_18px_38px_rgb(15_23_42_/_0.14)]"
    >
      <header
        class="flex h-9 items-center justify-between bg-[#263238] px-3 text-[0.65rem] font-bold text-white"
      >
        <span class="flex items-center gap-2"
          ><span class="i-carbon-time text-cyan-300" /> HOURGLASS</span
        >
        <span class="flex items-center gap-1.5 text-slate-300"
          ><i class="size-1.5 rounded-full bg-emerald-400" /> Redmine ·
          live</span
        >
      </header>
      <div
        class="border-b border-slate-200 bg-slate-50 px-3 py-2 text-[0.62rem] font-extrabold tracking-[0.08em] text-slate-500"
      >
        TIME TRACKER
      </div>
      <div class="grid gap-2.5 p-3">
        <div class="grid grid-cols-2 gap-2">
          <label class="field"
            ><span><img :src="projectIcon" /> PROJECT</span
            ><b>Meeting C++ 2026</b></label
          >
          <label class="field"
            ><span><img :src="issueIcon" /> ISSUE</span
            ><b>#4711 Better Slides</b></label
          >
          <label class="field"
            ><span><img :src="activityIcon" /> ACTIVITY</span
            ><b>Development</b></label
          >
          <label class="field"
            ><span><img :src="commentIcon" /> COMMENT</span><b>Hello</b></label
          >
        </div>
        <div
          class="mt-1 flex items-center justify-between rounded-xl bg-slate-100 p-2.5"
        >
          <div>
            <div
              class="font-mono text-[1.55rem] font-extrabold leading-none text-slate-800"
            >
              00:00:<span :class="isBusy ? 'ui-tick' : ''">{{
                isBusy ? "01" : "00"
              }}</span>
            </div>
          </div>
          <button
            class="grid size-16 place-items-center rounded-2xl border-0 bg-[#00a99d] shadow-[0_8px_18px_rgb(13_148_136_/_0.25)]"
            :class="{ 'start-pressed': stage > 0 }"
          >
            <img
              :src="startIcon"
              class="size-10 object-contain brightness-0 invert"
            />
          </button>
        </div>
      </div>
    </section>

    <section
      class="relative flex flex-col rounded-[18px] border-2 border-brand-cyan bg-cyan-50/70 p-3.5 shadow-[0_12px_28px_rgb(8_145_178_/_0.1)]"
    >
      <div class="mb-3 flex items-center justify-between">
        <span class="text-base font-extrabold tracking-[0.1em] text-brand-blue"
          >COROUTINE STEPS</span
        >
        <span
          class="rounded-full px-2 py-1 font-mono text-[0.75em] font-extrabold"
          :class="
            isBusy
              ? 'bg-amber-100 text-amber-800'
              : 'bg-emerald-100 text-emerald-800'
          "
          >{{ isBusy ? "SUSPENDED / RUNNING" : current.label }}</span
        >
      </div>

      <div
        class="grid flex-1 content-center gap-2 font-mono text-[1.2rem] font-semibold text-slate-800"
      >
        <div
          class="code-step"
          :class="{ active: stage === 0, done: stage > 0 }"
        >
          <span>01</span><code>setActivity(Writing);</code>
        </div>
        <div
          class="code-step"
          :class="{ active: stage === 1, done: stage > 1 }"
        >
          <span>02</span><code><b>co_await</b> post(...);</code>
        </div>
        <div
          class="code-step"
          :class="{ active: stage === 2, done: stage > 2 }"
        >
          <span>03</span><code>applyTracker(reply);</code>
        </div>
        <div
          class="code-step"
          :class="{ active: stage === 3, done: stage > 3 }"
        >
          <span>04</span><code><b>co_await</b> get(issue);</code>
        </div>
        <div class="code-step" :class="{ active: stage === 4 }">
          <span>05</span><code>setActivity(Idle);</code>
        </div>
      </div>

      <div
        v-if="stage === 1 || stage === 3"
        class="absolute -right-[13px] top-1/2 z-5 grid size-7 -translate-y-1/2 place-items-center rounded-full bg-brand-orange text-white shadow-lg"
      >
        <span class="i-carbon-pause-filled text-sm" />
      </div>
    </section>

    <section
      class="relative flex flex-col overflow-hidden rounded-[18px] bg-[#101827] p-4 text-white shadow-[0_18px_38px_rgb(15_23_42_/_0.16)]"
    >
      <div
        class="absolute -right-8 -top-8 size-28 rounded-full border-[18px] border-cyan-300/8"
      />
      <div
        class="flex items-center justify-between text-[1em] font-extrabold tracking-[0.1em] text-slate-400"
      >
        <span>REDMINE REST API</span
        ><span class="i-carbon-cloud text-cyan-300" />
      </div>
      <div class="grid flex-1 place-items-center">
        <div class="w-full">
          <Transition name="request" mode="out-in">
            <div
              :key="requestLabel"
              class="rounded-xl border border-cyan-300/25 bg-white/7 px-3 py-3 font-mono text-[1em] font-bold text-cyan-200"
            >
              {{ requestLabel }}
            </div>
          </Transition>
          <div class="my-3 grid grid-cols-[1fr_auto_1fr] items-center gap-2">
            <div class="h-px bg-cyan-300/20" />
            <div
              class="grid size-14 place-items-center rounded-full border-2 border-cyan-300/45 bg-cyan-300/10"
              :class="{ 'server-pulse': stage === 1 || stage === 3 }"
            >
              <span class="i-carbon-data-base text-2xl text-cyan-300" />
            </div>
            <div class="h-px bg-cyan-300/20" />
          </div>
          <div
            class="rounded-xl border px-3 py-3 font-mono text-[1em] font-bold transition-all duration-300"
            :class="
              stage === 2 || stage === 4
                ? 'border-emerald-300/40 bg-emerald-300/10 text-emerald-200'
                : 'border-slate-600 bg-white/4 text-slate-500'
            "
          >
            200 OK · finished()
          </div>
        </div>
      </div>
    </section>
  </div>

  <div
    class="mt-8 grid min-h-[58px] grid-cols-[auto_1fr_auto] items-center gap-4 rounded-xl border border-slate-200 bg-white px-4 py-2 shadow-sm"
  >
    <div
      class="grid size-9 place-items-center rounded-full bg-brand-orange font-mono text-base font-extrabold text-white"
    >
      {{ stage + 1 }}
    </div>
    <div>
      <strong class="block text-[1.5rem] text-brand-blue">{{
        current.title
      }}</strong>
      <span class="text-[1.4em] text-muted">{{ current.detail }}</span>
    </div>
    <div class="flex gap-1.5" aria-hidden="true">
      <i
        v-for="(_, index) in stages"
        :key="index"
        class="h-1.5 rounded-full transition-all duration-300"
        :class="
          index === stage
            ? 'w-7 bg-brand-orange'
            : index < stage
              ? 'w-2 bg-brand-cyan'
              : 'w-2 bg-slate-300'
        "
      />
    </div>
  </div>
</template>

<style scoped>
.field {
  @apply rounded-lg border border-slate-200 bg-white px-2.5 py-2 shadow-sm;
}
.field span {
  @apply flex items-center gap-1.5 text-[0.62rem] font-extrabold tracking-[0.08em] text-slate-400;
}
.field img {
  @apply size-3 object-contain opacity-60;
}
.field b {
  @apply mt-1 block truncate text-[1.0rem] text-slate-700;
}
.code-step {
  @apply grid min-h-9 grid-cols-[1.5rem_minmax(0,1fr)_auto] items-center rounded-lg border border-transparent px-2 opacity-48 transition-all duration-300;
}
.code-step span {
  @apply text-[0.75rem] text-slate-600;
}
.code-step code {
  @apply whitespace-nowrap font-mono text-[inherit] font-[inherit] text-[inherit];
}
.code-step b {
  @apply text-brand-orange;
}
.code-step.active {
  @apply translate-x-1 border-orange-300 bg-white opacity-100 shadow-[0_5px_14px_rgb(15_23_42_/_0.09)];
}
.code-step.done {
  @apply opacity-70;
}
.code-step.done::after {
  content: "✓";
  @apply justify-self-end text-brand-green;
}
.ui-tick {
  animation: ui-tick 900ms steps(1) infinite;
}
.start-pressed {
  animation: press 420ms ease-out;
}
.server-pulse {
  animation: server-pulse 1.2s ease-in-out infinite;
}
.request-enter-active,
.request-leave-active {
  transition: all 220ms ease;
}
.request-enter-from {
  transform: translateX(-14px);
  opacity: 0;
}
.request-leave-to {
  transform: translateX(14px);
  opacity: 0;
}
@keyframes ui-tick {
  50% {
    opacity: 0.35;
  }
}
@keyframes press {
  45% {
    transform: scale(0.88);
  }
}
@keyframes server-pulse {
  50% {
    box-shadow: 0 0 0 12px rgb(103 232 249 / 0.08);
    border-color: rgb(103 232 249 / 0.9);
  }
}
</style>
