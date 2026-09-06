<script setup lang="ts">
import { useSlideContext } from "@slidev/client";
import cpp from "@shikijs/langs/cpp";
import githubLight from "@shikijs/themes/github-light";
import { createHighlighterCore, type ThemedToken } from "shiki/core";
import { createJavaScriptRegexEngine } from "shiki/engine/javascript";
import {
  computed,
  nextTick,
  onMounted,
  onUnmounted,
  ref,
  watch,
  watchEffect,
} from "vue";

import source from "../snippets/hourglass-reply-await-bridge.cpp?raw";

const sourceLines = source.trim().split("\n");
const sourceTokens = ref<ThemedToken[][]>([]);

const stages = [
  {
    title: "store the coroutine",
    explanation: "The task owns the handle that can continue it.",
    added: [1, 2, 24],
    focus: [2],
  },
  {
    title: "intercept the reply",
    explanation: "The promise turns QNetworkReply into an awaitable.",
    added: [3, 4, 5, 22, 23],
    focus: [3, 4, 5, 22, 23],
  },
  {
    title: "build one Awaiter",
    explanation: "It keeps the reply and returns it to the coroutine.",
    added: [6, 7, 20, 21],
    focus: [6, 7, 20, 21],
  },
  {
    title: "skip needless suspension",
    explanation: "A finished reply can continue immediately.",
    added: [8, 9, 10],
    focus: [8, 9, 10],
  },
  {
    title: "resume on finished()",
    explanation: "The Qt callback continues the saved coroutine.",
    added: [11, 12, 13, 14],
    focus: [11, 12, 13, 14],
  },
  {
    title: "return the result",
    explanation: "Resumption converts the reply into the domain value.",
    added: [15, 16, 17, 18, 19],
    focus: [15, 16, 17, 18, 19],
  },
  {
    title: "complete Awaiter",
    explanation: "The full adapter fits in one final overview.",
    added: [],
    focus: [],
  },
];

const stage = ref(0);
const codeViewport = ref<HTMLElement>();
const { $clicksContext: clicks } = useSlideContext();
const clickId = `awaiter-machinery-${Math.random()}`;
let scrollFrame: number | undefined;

const visibleLineNumbers = computed(() => {
  const visible = new Set<number>();
  for (let index = 0; index <= stage.value; index += 1) {
    for (const line of stages[index].added) visible.add(line);
  }
  return visible;
});

const current = computed(() => stages[stage.value]);
const focusLines = computed(() => new Set(current.value.focus));
const completeView = computed(() => stage.value === stages.length - 1);
const activeGapAfter = computed(
  () => [2, 5, 7, 10, 14, undefined, undefined][stage.value],
);

type DisplayRow =
  | { kind: "line"; key: string; lineNumber: number }
  | {
      kind: "gap";
      key: string;
      indent: number;
      afterLine: number;
    };

const gapAfterLines = new Set([2, 5, 7, 10, 14]);
const displayRows: DisplayRow[] = sourceLines.flatMap((line, index) => {
  const lineNumber = index + 1;
  const rows: DisplayRow[] = [];
  rows.push({ kind: "line", key: `line-${lineNumber}`, lineNumber });
  if (gapAfterLines.has(lineNumber)) {
    const firstOmittedLine = lineNumber + 1;
    rows.push({
      kind: "gap",
      key: `gap-after-${lineNumber}`,
      indent: sourceLines[firstOmittedLine - 1].match(/^\s*/)?.[0].length ?? 0,
      afterLine: lineNumber,
    });
  }
  return rows;
});

function rowIsVisible(row: DisplayRow) {
  if (row.kind === "line") return visibleLineNumbers.value.has(row.lineNumber);
  return activeGapAfter.value === row.afterLine;
}

function rowIsFocused(row: DisplayRow | undefined) {
  if (!row) return false;
  if (!rowIsVisible(row)) return false;
  if (row.kind === "line") return focusLines.value.has(row.lineNumber);

  const nextVisibleLine = [...visibleLineNumbers.value]
    .filter((line) => line > row.afterLine)
    .sort((left, right) => left - right)[0];
  return (
    focusLines.value.has(row.afterLine) &&
    nextVisibleLine !== undefined &&
    focusLines.value.has(nextVisibleLine)
  );
}

const rowStates = computed(() => {
  const states = new Map<
    string,
    { visible: boolean; focused: boolean; start: boolean; end: boolean }
  >();
  const visibleRows = displayRows.filter(rowIsVisible);
  visibleRows.forEach((row, index) => {
    const focused = rowIsFocused(row);
    states.set(row.key, {
      visible: true,
      focused,
      start: focused && !rowIsFocused(visibleRows[index - 1]),
      end: focused && !rowIsFocused(visibleRows[index + 1]),
    });
  });
  for (const row of displayRows) {
    if (!states.has(row.key)) {
      states.set(row.key, {
        visible: false,
        focused: false,
        start: false,
        end: false,
      });
    }
  }
  return states;
});

function rowClasses(row: DisplayRow) {
  const state = rowStates.value.get(row.key)!;
  return [
    row.kind === "gap"
      ? "code-gap grid grid-cols-[3rem_1fr] items-center border-l-[5px] border-transparent pr-5"
      : "code-line grid grid-cols-[3rem_1fr] items-center border-l-[5px] border-transparent pr-5",
    {
      "is-visible": state.visible,
      "is-focus": state.focused,
      "is-focus-start": state.start,
      "is-focus-end": state.end,
    },
  ];
}

function scrollTarget(viewport: HTMLElement) {
  if (completeView.value) return 0;
  const focused = viewport.querySelectorAll<HTMLElement>(".code-line.is-focus");
  if (!focused.length) return viewport.scrollTop;
  const first = focused[0];
  const last = focused[focused.length - 1];
  const focusCenter =
    (first.offsetTop + last.offsetTop + last.offsetHeight) / 2;
  return Math.max(
    0,
    Math.min(
      focusCenter - viewport.clientHeight / 2,
      viewport.scrollHeight - viewport.clientHeight,
    ),
  );
}

async function centerFocusedCode(immediate: boolean) {
  if (scrollFrame !== undefined) cancelAnimationFrame(scrollFrame);
  await nextTick();
  const viewport = codeViewport.value;
  if (!viewport) return;

  if (immediate) {
    viewport.scrollTop = scrollTarget(viewport);
    return;
  }

  const started = performance.now();
  const duration = 620;
  const update = (now: number) => {
    const target = scrollTarget(viewport);
    viewport.scrollTop += (target - viewport.scrollTop) * 0.24;
    if (now - started < duration) {
      scrollFrame = requestAnimationFrame(update);
    } else {
      viewport.scrollTop = target;
      scrollFrame = undefined;
    }
  };
  scrollFrame = requestAnimationFrame(update);
}

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

watch(stage, (next, previous) => centerFocusedCode(previous === undefined), {
  immediate: true,
});

onUnmounted(() => {
  if (scrollFrame !== undefined) cancelAnimationFrame(scrollFrame);
  clicks.unregister(clickId);
});
</script>

<template>
  <div class="mt-2 grid gap-2">
    <div
      ref="codeViewport"
      class="code-viewport h-[31rem] overflow-hidden rounded-xl border border-slate-300 bg-[#f7f7f8] py-[0.35rem] shadow-slide-card"
      :class="{ 'is-complete': completeView }"
    >
      <div class="code-lines">
        <div v-for="row in displayRows" :key="row.key" :class="rowClasses(row)">
          <template v-if="row.kind === 'gap'">
            <span
              class="col-start-2 font-mono text-[0.72rem] leading-none text-slate-400"
              :style="{ paddingLeft: `${row.indent}ch` }"
              aria-label="code omitted until a later step"
              >⋮</span
            >
          </template>
          <template v-else>
            <span class="text-center font-mono text-[1.05rem] text-slate-400">{{
              row.lineNumber
            }}</span>
            <code
              class="awaiter-source-code overflow-hidden font-mono whitespace-pre text-slate-800"
            >
              <template v-if="sourceTokens[row.lineNumber - 1]">
                <span
                  v-for="(token, tokenIndex) in sourceTokens[
                    row.lineNumber - 1
                  ]"
                  :key="tokenIndex"
                  :style="{ color: token.color }"
                  >{{ token.content }}</span
                >
              </template>
              <template v-else>{{
                sourceLines[row.lineNumber - 1] || " "
              }}</template>
            </code>
          </template>
        </div>
      </div>
    </div>

    <div
      class="grid min-h-[3.8rem] grid-cols-[auto_auto_1fr] items-center gap-4 rounded-xl bg-slate-900 px-4 py-3"
    >
      <span class="font-mono text-[1.5rem] leading-none text-orange-300"
        >{{ stage + 1 }}/{{ stages.length }}</span
      >
      <strong class="text-[1.5rem] text-cyan-300">{{ current.title }}</strong>
      <span class="text-[1.5rem] text-slate-300">{{
        current.explanation
      }}</span>
    </div>
  </div>
</template>

<style scoped>
.code-line,
.code-gap {
  height: 0;
  min-height: 0;
  overflow: hidden;
  opacity: 0;
  pointer-events: none;
  transform: translateY(0.35rem);
  will-change: height, opacity, transform;
  transition:
    height 560ms cubic-bezier(0.22, 1, 0.36, 1),
    opacity 260ms ease,
    transform 560ms cubic-bezier(0.22, 1, 0.36, 1),
    background-color 260ms ease,
    border-color 260ms ease,
    box-shadow 260ms ease;
}

.code-line.is-visible {
  height: 1.7rem;
  opacity: 1;
  pointer-events: auto;
  transform: translateY(0);
}

.code-gap.is-visible {
  height: 0.8rem;
  opacity: 1;
  transform: translateY(0);
}

.code-line > span,
.code-line code {
  transition:
    font-size 520ms cubic-bezier(0.22, 1, 0.36, 1),
    line-height 520ms cubic-bezier(0.22, 1, 0.36, 1);
}

.code-line > span {
  line-height: 1;
}

.code-line code {
  border-radius: 0;
  background: transparent;
  /* Slidev treats any code element outside a pre as inline code (0.9em). */
  font-size: 1.2rem !important;
  line-height: 1.7rem;
  padding: 0;
}

.code-lines > .code-line,
.code-lines > .code-gap {
  position: relative;
}

.code-lines > .is-focus {
  z-index: 10;
  border-left-color: #ea580c;
  background: rgb(251 146 60 / 0.22);
  box-shadow:
    inset -1px 0 0 rgb(251 146 60 / 0.72),
    0 0 10px rgb(251 146 60 / 0.18);
}

.code-lines > .code-line::before,
.code-lines > .code-line::after,
.code-lines > .code-gap::before,
.code-lines > .code-gap::after {
  position: absolute;
  right: 0;
  left: -5px;
  height: 1px;
  background: #fb923c;
  content: "";
  opacity: 0;
  transition: opacity 220ms ease;
}

.code-lines > .code-line::before,
.code-lines > .code-gap::before {
  top: -1px;
  box-shadow:
    0 -1px 0 rgb(251 146 60 / 0.42),
    0 -4px 10px rgb(251 146 60 / 0.2);
}

.code-lines > .code-line::after,
.code-lines > .code-gap::after {
  bottom: -1px;
  box-shadow:
    0 1px 0 rgb(251 146 60 / 0.42),
    0 4px 10px rgb(251 146 60 / 0.2);
}

.code-lines > .is-focus-start::before,
.code-lines > .is-focus-end::after {
  opacity: 1;
}

.code-viewport.is-complete .code-line.is-visible {
  height: 1.18rem;
}

.code-viewport.is-complete .code-line > span {
  font-size: 1.2rem;
}

.code-viewport.is-complete .code-line code {
  font-size: 1.1rem !important;
  line-height: 1.18rem;
}
</style>
