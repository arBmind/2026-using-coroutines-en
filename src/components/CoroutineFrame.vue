<script setup lang="ts">
import { useSlideContext } from "@slidev/client";
import cpp from "@shikijs/langs/cpp";
import githubLight from "@shikijs/themes/github-light";
import { createHighlighterCore, type ThemedToken } from "shiki/core";
import { createJavaScriptRegexEngine } from "shiki/engine/javascript";
import { computed, onMounted, onUnmounted, ref, watchEffect } from "vue";

import coroutineSource from "../snippets/coroutine-introduction/coroutine.cpp?raw";

function extractRegion(source: string, name: string) {
  const start = `// #region ${name}`;
  const end = `// #endregion ${name}`;
  return source
    .slice(source.indexOf(start) + start.length, source.indexOf(end))
    .trim()
    .replace(/\s*\/\/ \[!code highlight\]/g, "");
}

const sourceLines = extractRegion(coroutineSource, "coroutine-flow").split(
  "\n",
);
const sourceTokens = ref<ThemedToken[][]>([]);

type FrameMember = {
  name: string;
  value: string;
};

type StackSlot = {
  name: string;
  value: string;
};

function stackFrame(task: string, active?: string): StackSlot[] {
  return [
    { name: "caller", value: "caller()" },
    { name: "task", value: `task = ${task}` },
    ...(active ? [{ name: "active", value: active }] : []),
  ];
}

function coroutineFrame(
  state: string,
  profile: string,
  result: string,
  continuation: string,
): FrameMember[] {
  return [
    { name: "state", value: state },
    { name: "id", value: '"C++"' },
    { name: "profile", value: profile },
    { name: "result", value: result },
    { name: "continuation", value: continuation },
  ];
}

const stages = [
  {
    line: 7,
    title: "Call greeting",
    explanation:
      "The task variable already has a stack slot; no coroutine frame exists yet.",
    stack: stackFrame("—", ""),
    heap: [] as FrameMember[],
  },
  {
    line: 1,
    title: "Create the coroutine",
    explanation:
      "The task receives a handle. The frame allocates every coroutine member up front.",
    stack: stackFrame("—", "greeting()"),
    heap: coroutineFrame("RUNNING", "—", "—", "line 2"),
  },
  {
    line: 2,
    title: "Suspend at co_await",
    explanation:
      "greeting leaves the stack, but every frame member stays alive behind task's handle.",
    stack: stackFrame("handle", "greeting()"),
    heap: coroutineFrame("SUSPENDED", "—", "—", "line 3"),
  },
  {
    line: 8,
    title: "Resume with a Profile",
    explanation:
      "The caller supplies the event result and resumes the stored continuation through task.",
    stack: stackFrame("handle"),
    heap: coroutineFrame("RESUMING", "Meeting C++ 2026", "—", "line 3"),
  },
  {
    line: 3,
    title: "Complete with co_return",
    explanation:
      "greeting runs again and stores the formatted string in the promise portion of its frame.",
    stack: stackFrame("handle", "greeting()"),
    heap: coroutineFrame(
      "COMPLETING",
      "Meeting C++ 2026",
      "Hello Meeting C++…",
      "final suspend",
    ),
  },
  {
    line: 9,
    title: "Read the result",
    explanation:
      "caller reads the stored value; task later destroys the completed frame with all of its members.",
    stack: stackFrame("handle"),
    heap: coroutineFrame(
      "DONE",
      "Meeting C++ 2026",
      "Hello Meeting C++…",
      "final suspend",
    ),
  },
];

const stage = ref(0);
const { $clicksContext: clicks } = useSlideContext();
const clickId = `coroutine-frame-${Math.random()}`;

onMounted(() => {
  const clickInfo = clicks.calculateSince("+1", stages.length - 1);
  clicks.register(clickId, clickInfo);
  watchEffect(() => {
    stage.value = Math.max(
      0,
      Math.min(stages.length - 1, (clickInfo?.currentOffset.value ?? -1) + 1),
    );
  });

  createHighlighterCore({
    themes: [githubLight],
    langs: [cpp],
    engine: createJavaScriptRegexEngine(),
  }).then((highlighter) => {
    sourceTokens.value = highlighter.codeToTokensBase(sourceLines.join("\n"), {
      lang: "cpp",
      theme: "github-light",
    });
    highlighter.dispose();
  });
});

onUnmounted(() => clicks.unregister(clickId));

const current = computed(() => stages[stage.value]);
const previous = computed(() =>
  stage.value > 0 ? stages[stage.value - 1] : undefined,
);

function valueChanged<T extends { name: string; value: string }>(entry: T) {
  if (!previous.value) return false;

  // Frame allocation is the point of step two. Emphasize the frame itself,
  // rather than making every just-created member compete for attention.
  if (
    stage.value === 1 &&
    current.value.heap.some(({ name }) => name === entry.name)
  )
    return false;

  const previousEntry = [...previous.value.stack, ...previous.value.heap].find(
    (candidate) => candidate.name === entry.name,
  );
  return previousEntry?.value !== entry.value;
}
</script>

<template>
  <div class="execution-visual">
    <div class="execution-code">
      <div
        v-for="(line, index) in sourceLines"
        :key="index"
        class="execution-line"
        :class="{ active: current.line === index + 1 }"
      >
        <span>{{ index + 1 }}</span>
        <code>
          <template v-if="sourceTokens[index]">
            <span
              v-for="(token, tokenIndex) in sourceTokens[index]"
              :key="tokenIndex"
              :style="{ color: token.color }"
              >{{ token.content }}</span
            >
          </template>
          <template v-else>{{ line || " " }}</template>
        </code>
      </div>
    </div>

    <div class="memory-visual">
      <section class="memory-area stack-area">
        <header><strong>STACK</strong></header>
        <TransitionGroup name="memory-card" tag="div" class="memory-cards">
          <div
            v-for="entry in current.stack"
            :key="entry.name"
            class="memory-card stack-card"
          >
            <span :class="{ 'changed-value': valueChanged(entry) }">
              {{ entry.value }}
            </span>
          </div>
        </TransitionGroup>
      </section>

      <section class="memory-area heap-area">
        <header><strong>HEAP</strong></header>
        <div
          v-if="current.heap.length"
          class="heap-frame"
          :class="{ 'frame-created': stage === 1 }"
        >
          <div class="frame-label">COROUTINE FRAME</div>
          <TransitionGroup name="memory-card" tag="div" class="memory-cards">
            <div
              v-for="entry in current.heap"
              :key="entry.name"
              class="memory-card heap-card"
            >
              {{ entry.name }} =
              <span :class="{ 'changed-value': valueChanged(entry) }">
                {{ entry.value }}
              </span>
            </div>
          </TransitionGroup>
        </div>
        <div v-else class="heap-empty">no allocation</div>
      </section>
    </div>

    <div class="execution-explanation">
      <span>{{ stage + 1 }}/{{ stages.length }}</span>
      <strong>{{ current.title }}</strong>
      <p>{{ current.explanation }}</p>
    </div>
  </div>
</template>

<style scoped>
.execution-visual {
  display: grid;
  grid-template-columns: 1.06fr 0.94fr;
  gap: 1rem;
  margin-top: 1rem;
}
.execution-code {
  align-self: start;
  overflow: hidden;
  border: 1px solid #cbd5e1;
  border-radius: 12px;
  background: #f7f7f8;
  padding: 0.65rem 0;
  font-size: 1.1rem;
  box-shadow: 0 10px 25px rgba(15, 23, 42, 0.08);
}
.execution-line {
  display: grid;
  grid-template-columns: 2rem 1fr;
  min-height: 1.65rem;
  align-items: center;
  border-left: 5px solid transparent;
  padding-right: 0.5rem;
  transition:
    background 250ms ease,
    border-color 250ms ease;

  & > span {
    color: #94a3b8;
    font:
      1.1rem ui-monospace,
      monospace;
    text-align: center;
  }
  code {
    overflow: hidden;
    color: #1e293b;
    font:
      1.1rem ui-monospace,
      monospace;
    white-space: pre;
  }
  &.active {
    border-left-color: #ea580c;
    background: rgba(251, 146, 60, 0.22);
  }
}
.memory-visual {
  display: grid;
  grid-template-columns: 0.8fr 1.2fr;
  gap: 0.7rem;
  min-height: 320px;
}
.memory-area {
  border: 2px solid #cbd5e1;
  border-radius: 12px;
  background: #f8fafc;
  padding: 0.7rem;
  header {
    display: flex;
    flex-direction: column;
    margin-bottom: 0.65rem;
    strong {
      color: #075985;
      font-size: 1rem;
      letter-spacing: 0.1em;
    }
  }
}
.heap-area {
  border-color: #0891b2;
  background: #ecfeff;
}
.memory-cards {
  display: grid;
  gap: 0.5rem;
}
.memory-card {
  border-radius: 8px;
  padding: 0.55rem 0.6rem;
  font:
    650 1rem/1.3 ui-monospace,
    monospace;
}
.stack-card {
  border-left: 4px solid #2563eb;
  background: white;
  color: #1e3a8a;
}
.heap-card {
  border-left: 4px solid #0891b2;
  background: white;
  color: #155e75;
}
.changed-value {
  border-radius: 3px;
  background: #fef3c7;
  box-shadow: 0 0 0 2px #fdba74;
  color: #9a3412;
  font-weight: 800;
  animation: value-change 500ms ease-out;
}
@keyframes value-change {
  from {
    background: #fb923c;
    box-shadow: 0 0 0 5px rgba(251, 146, 60, 0.42);
  }
  to {
    background: #fef3c7;
    box-shadow: 0 0 0 2px #fdba74;
  }
}
.heap-frame {
  position: relative;
  border: 2px dashed #0891b2;
  border-radius: 10px;
  padding: 1.7rem 0.5rem 0.5rem;
}
.heap-frame.frame-created {
  border-color: #ea580c;
  box-shadow: 0 0 0 4px rgba(251, 146, 60, 0.25);
  animation: frame-created 500ms ease-out;
}
@keyframes frame-created {
  from {
    box-shadow: 0 0 0 10px rgba(251, 146, 60, 0.5);
  }
  to {
    box-shadow: 0 0 0 4px rgba(251, 146, 60, 0.25);
  }
}
.frame-label {
  position: absolute;
  top: 0;
  right: 0;
  border-radius: 0 7px 0 7px;
  background: #075985;
  color: white;
  padding: 0.25rem 0.45rem;
  font-size: 0.55rem;
  font-weight: 800;
  letter-spacing: 0.06em;
}
.heap-empty {
  display: grid;
  min-height: 210px;
  place-items: center;
  border: 2px dashed #94a3b8;
  border-radius: 10px;
  color: #64748b;
  font-size: 0.72rem;
}
.execution-explanation {
  grid-column: 1 / -1;
  display: grid;
  grid-template-columns: auto auto 1fr;
  gap: 0.75rem;
  align-items: center;
  min-height: 58px;
  border-radius: 12px;
  background: #0f172a;
  color: white;
  padding: 0.7rem 0.9rem;
}
.execution-explanation span {
  color: #fdba74;
  font:
    800 1rem/1 ui-monospace,
    monospace;
}
.execution-explanation strong {
  color: #67e8f9;
  font-size: 1rem;
}
.execution-explanation p {
  margin: 0;
  color: #cbd5e1;
  font-size: 1rem;
}
.memory-card-enter-active,
.memory-card-leave-active {
  transition: all 350ms ease;
}
.memory-card-enter-from,
.memory-card-leave-to {
  opacity: 0;
  transform: translateY(8px);
}
</style>
