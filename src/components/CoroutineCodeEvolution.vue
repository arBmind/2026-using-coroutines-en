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
const withTask = synchronous
  .replace("-> string {", "-> task<string> {")
  .replace('auto message = greeting("C++");', 'auto task = greeting("C++");');
const withAwait = withTask.replace(
  "auto profile = loadProfile(id);",
  "auto profile = co_await loadProfile(id);",
);
const withReturn = withAwait.replace(
  'return format("Hello {}", profile.userName);',
  'co_return format("Hello {}", profile.userName);',
);
const withResume = withReturn.replace(
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
    keyword: "baseline",
    explanation:
      "The caller receives a string only after loadProfile has finished.",
    lines: [] as number[],
  },
  {
    code: withTask,
    keyword: "task<string>",
    explanation:
      "Return a resumable operation immediately; the caller keeps that task.",
    lines: [1, 7],
  },
  {
    code: withAwait,
    keyword: "co_await",
    explanation:
      "Suspend greeting when the profile is not ready, without blocking the thread.",
    lines: [2],
  },
  {
    code: withReturn,
    keyword: "co_return",
    explanation:
      "Store the greeting in the coroutine promise instead of returning through this stack call.",
    lines: [3],
  },
  {
    code: withResume,
    keyword: "resume",
    explanation:
      "Supply the completed profile and continue the suspended coroutine frame.",
    lines: [8],
  },
  {
    code: withResult,
    keyword: "result",
    explanation:
      "Once the task is complete, read the value stored by co_return.",
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
  <div class="code-evolution">
    <div class="evolution-code-shell" :class="{ 'is-animating': animating }">
      <div class="evolution-highlights" aria-hidden="true">
        <span
          v-for="line in current.lines"
          :key="line"
          :style="{ '--line': line }"
        />
      </div>
      <ShikiMagicMove
        v-if="highlighter"
        class="evolution-code"
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
      <pre v-else class="evolution-loading">Loading source…</pre>
    </div>

    <div class="evolution-explanation">
      <span class="evolution-step">{{ step }}/{{ steps.length - 1 }}</span>
      <code>{{ current.keyword }}</code>
      <span>{{ current.explanation }}</span>
    </div>
  </div>
</template>

<style scoped>
.code-evolution {
  margin-top: 1.25rem;
}
.evolution-code-shell {
  position: relative;
  overflow: hidden;
  border: 1px solid #cbd5e1;
  border-radius: 12px;
  background: #f7f7f8;
  box-shadow: 0 10px 25px rgba(15, 23, 42, 0.09);
}
.evolution-highlights {
  position: absolute;
  z-index: 0;
  inset: 12px 0 auto;
  pointer-events: none;
  transition: opacity 150ms ease;
}
.is-animating .evolution-highlights {
  opacity: 0;
}
.evolution-highlights span {
  position: absolute;
  top: calc((var(--line) - 1) * 1.72rem);
  right: 0;
  left: 0;
  height: 1.72rem;
  border-left: 5px solid #ea580c;
  background: rgba(251, 146, 60, 0.2);
  transition:
    top 450ms ease,
    opacity 250ms ease;
}
.evolution-code,
.evolution-loading {
  position: relative;
  z-index: 1;
}
.evolution-explanation {
  display: grid;
  grid-template-columns: auto auto 1fr;
  gap: 0.75rem;
  align-items: center;
  min-height: 64px;
  margin-top: 0.85rem;
  border-radius: 12px;
  background: #ecfeff;
  padding: 0.8rem 1rem;
  color: #155e75;
  font-size: 0.9rem;
  font-weight: 650;
}
.evolution-explanation code {
  border-radius: 6px;
  background: #0f172a;
  color: #67e8f9;
  padding: 0.25rem 0.45rem;
}
.evolution-step {
  color: #ea580c;
  font:
    800 0.75rem/1 ui-monospace,
    monospace;
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
