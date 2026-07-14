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
  <div class="mt-4 grid grid-cols-[1.06fr_0.94fr] gap-4">
    <div
      class="self-start overflow-hidden rounded-xl border border-slate-300 bg-[#f7f7f8] py-[0.65rem] text-[1.1rem] shadow-slide-card"
    >
      <div
        v-for="(line, index) in sourceLines"
        :key="index"
        class="grid min-h-[1.65rem] grid-cols-[2rem_1fr] items-center border-l-[5px] border-transparent pr-2 transition-[background,border-color] duration-250"
        :class="{
          'border-y border-y-orange-300 border-l-brand-orange bg-orange-400/22':
            current.line === index + 1,
        }"
      >
        <span class="text-center font-mono text-[1.1rem] text-slate-400">{{
          index + 1
        }}</span>
        <code
          class="overflow-hidden font-mono text-[1.1rem] whitespace-pre text-slate-800"
        >
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

    <div class="grid min-h-[320px] grid-cols-[0.8fr_1.2fr] gap-[0.7rem]">
      <section
        class="rounded-xl border-2 border-slate-300 bg-slate-50 p-[0.7rem]"
      >
        <header class="mb-[0.65rem] flex flex-col">
          <strong class="text-base tracking-[0.1em] text-brand-blue"
            >STACK</strong
          >
        </header>
        <TransitionGroup name="memory-card" tag="div" class="grid gap-2">
          <div
            v-for="entry in current.stack"
            :key="entry.name"
            class="rounded-lg border-l-4 border-blue-600 bg-white px-[0.6rem] py-[0.55rem] font-mono text-base font-[650] leading-[1.3] text-blue-900"
          >
            <span
              :class="{
                'animate-[value-change_500ms_ease-out] rounded-[3px] bg-amber-100 font-extrabold text-orange-800 shadow-[0_0_0_2px_#fdba74]':
                  valueChanged(entry),
              }"
            >
              {{ entry.value }}
            </span>
          </div>
        </TransitionGroup>
      </section>

      <section
        class="rounded-xl border-2 border-brand-cyan bg-cyan-50 p-[0.7rem]"
      >
        <header class="mb-[0.65rem] flex flex-col">
          <strong class="text-base tracking-[0.1em] text-brand-blue"
            >HEAP</strong
          >
        </header>
        <div
          v-if="current.heap.length"
          class="relative rounded-[10px] border-2 border-dashed border-brand-cyan px-2 pt-[1.7rem] pb-2"
          :class="{
            'animate-[frame-created_500ms_ease-out] border-brand-orange shadow-[0_0_0_4px_rgb(251_146_60_/_0.25)]':
              stage === 1,
          }"
        >
          <div
            class="absolute top-0 right-0 rounded-tr-[7px] rounded-bl-[7px] bg-brand-blue px-[0.45rem] py-1 text-[0.55rem] font-extrabold tracking-[0.06em] text-white"
          >
            COROUTINE FRAME
          </div>
          <TransitionGroup name="memory-card" tag="div" class="grid gap-2">
            <div
              v-for="entry in current.heap"
              :key="entry.name"
              class="rounded-lg border-l-4 border-brand-cyan bg-white px-[0.6rem] py-[0.55rem] font-mono text-base font-[650] leading-[1.3] text-cyan-800"
            >
              {{ entry.name }} =
              <span
                :class="{
                  'animate-[value-change_500ms_ease-out] rounded-[3px] bg-amber-100 font-extrabold text-orange-800 shadow-[0_0_0_2px_#fdba74]':
                    valueChanged(entry),
                }"
              >
                {{ entry.value }}
              </span>
            </div>
          </TransitionGroup>
        </div>
        <div
          v-else
          class="grid min-h-[210px] place-items-center rounded-[10px] border-2 border-dashed border-slate-400 text-xs text-slate-500"
        >
          no allocation
        </div>
      </section>
    </div>

    <div
      class="col-span-full grid min-h-[58px] grid-cols-[auto_auto_1fr] items-center gap-3 rounded-xl bg-slate-900 px-[0.9rem] py-[0.7rem] text-white"
    >
      <span
        class="font-mono text-base font-extrabold leading-none text-orange-300"
        >{{ stage + 1 }}/{{ stages.length }}</span
      >
      <strong class="text-base text-cyan-300">{{ current.title }}</strong>
      <p class="m-0 text-base text-slate-300">
        {{ current.explanation }}
      </p>
    </div>
  </div>
</template>

<style scoped>
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
@keyframes frame-created {
  from {
    box-shadow: 0 0 0 10px rgba(251, 146, 60, 0.5);
  }
  to {
    box-shadow: 0 0 0 4px rgba(251, 146, 60, 0.25);
  }
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
