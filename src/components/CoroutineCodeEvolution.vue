<script setup lang="ts">
import { useSlideContext } from "@slidev/client";
import { ShikiMagicMove } from "@shikijs/magic-move/vue";
import cpp from "@shikijs/langs/cpp";
import githubLight from "@shikijs/themes/github-light";
import { createHighlighterCore } from "shiki/core";
import { createJavaScriptRegexEngine } from "shiki/engine/javascript";
import {
  computed,
  onMounted,
  onUnmounted,
  ref,
  shallowRef,
  watchEffect,
} from "vue";
import "@shikijs/magic-move/style.css";

import synchronousSource from "../snippets/coroutine-introduction/synchronous.cpp?raw";

function extractRegion(source: string, name: string) {
  const start = `// #region ${name}`;
  const end = `// #endregion ${name}`;
  return source
    .slice(source.indexOf(start) + start.length, source.indexOf(end))
    .trim();
}

const synchronous = extractRegion(synchronousSource, "synchronous-flow");
const withAwait = synchronous.replace(
  "auto profile = loadProfile(id);",
  "auto profile = co_await loadProfile(id);",
);
const withReturn = withAwait.replace(
  'return format("Hello {}", profile.userName);',
  'co_return format("Hello {}", profile.userName);',
);
const withTask = withReturn
  .replace("-> string {", "-> task<string> {")
  .replace('auto message = greeting("C++");', 'auto task = greeting("C++");');
const withResume = withTask.replace(
  '\n    println("{}!", message);',
  '\n    task.resume(Profile{.userName = "Meeting C++ 2026"});\n    println("{}!", message);',
);
const withResult = withResume.replace(
  'println("{}!", message);',
  'println("{}!", task.result());',
);

const steps = [
  {
    code: synchronous,
    title: "baseline",
    explanation: "Start with regular C++.",
    lines: [1, 7],
  },
  {
    code: withAwait,
    title: "co_await",
    explanation: "Await something.",
    lines: [2],
  },
  {
    code: withReturn,
    title: "co_return",
    explanation: "Replace return.",
    lines: [3],
  },
  {
    code: withTask,
    title: "task<string>",
    explanation: "Coroutine enabled return type.",
    lines: [1, 7],
  },
  {
    code: withResume,
    title: "resume",
    explanation: "Emulate a runtime.",
    lines: [8],
  },
  {
    code: withResult,
    title: "result",
    explanation: "Extract from task.",
    lines: [9],
  },
];

const step = ref(0);
const animating = ref(false);
const highlighter =
  shallowRef<Awaited<ReturnType<typeof createHighlighterCore>>>();
const { $clicksContext: clicks } = useSlideContext();
const clickId = `coroutine-code-evolution-${Math.random()}`;

onMounted(() => {
  const clickInfo = clicks.calculateSince("+1", steps.length - 1);
  clicks.register(clickId, clickInfo);
  watchEffect(() => {
    const next = Math.max(
      0,
      Math.min(steps.length - 1, (clickInfo?.currentOffset.value ?? -1) + 1),
    );
    if (next !== step.value) animating.value = true;
    step.value = next;
  });
  createHighlighterCore({
    themes: [githubLight],
    langs: [cpp],
    engine: createJavaScriptRegexEngine(),
  }).then((value) => {
    highlighter.value = value;
  });
});

onUnmounted(() => {
  clicks.unregister(clickId);
  highlighter.value?.dispose();
});

const current = computed(() => steps[step.value]);
</script>

<template>
  <div class="mt-4 flex flex-col gap-4">
    <div
      class="relative overflow-hidden rounded-xl border border-slate-300 bg-[#f7f7f8] shadow-slide-card"
      :class="{ 'is-animating': animating }"
    >
      <div
        class="evolution-highlights pointer-events-none absolute inset-x-0 top-3 z-0 transition-opacity duration-150"
        aria-hidden="true"
      >
        <span
          v-for="line in current.lines"
          :key="line"
          class="absolute inset-x-0 h-[1.72rem] border-l-[5px] border-brand-orange bg-orange-400/20 transition-[top,opacity] duration-[450ms] ease-out"
          :style="{ '--line': line }"
        />
      </div>
      <ShikiMagicMove
        v-if="highlighter"
        class="relative z-1"
        :highlighter="highlighter"
        lang="cpp"
        theme="github-light"
        :code="current.code"
        :options="{
          duration: 520,
          delayMove: 0,
          delayLeave: 0,
          delayEnter: 0.12,
          delayContainer: 0,
          stagger: 0,
          lineNumbers: true,
          animateContainer: false,
          enhanceMatching: true,
          splitTokens: true,
        }"
        @end="animating = false"
      />
      <pre v-else class="relative z-1">Loading source…</pre>
    </div>

    <div
      class="col-span-full grid min-h-[58px] grid-cols-[auto_auto_1fr] items-center gap-4 rounded-xl bg-slate-900 px-[0.9rem] py-[0.7rem]"
    >
      <span class="font-mono text-base leading-none text-orange-300"
        >{{ step }}/{{ steps.length - 1 }}</span
      >
      <strong class="text-base text-cyan-300">{{ current.title }}</strong>
      <span class="m-0 text-base text-slate-300">{{
        current.explanation
      }}</span>
    </div>
  </div>
</template>

<style scoped>
.is-animating .evolution-highlights {
  opacity: 0;
}
.evolution-highlights span {
  top: calc((var(--line) - 1) * 1.72rem);
}
:deep(.shiki-magic-move-container) {
  min-height: 300px;
  margin: 0 !important;
  border: 0 !important;
  background: transparent !important;
  box-shadow: none !important;
  padding: 12px 14px !important;
  font-size: 1.1rem !important;
  line-height: 1.72rem !important;
}
.is-animating :deep(.shiki-magic-move-leave-active) {
  visibility: hidden !important;
  opacity: 0 !important;
  transition: none !important;
}
</style>
