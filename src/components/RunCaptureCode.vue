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

import source from "../snippets/slide-projects/desktop-duplication-cpp/src/DuplicationController.cpp?raw";

function extractRegion(text: string, name: string) {
  const start = `// #region ${name}`;
  const end = `// #endregion ${name}`;
  return text
    .slice(text.indexOf(start) + start.length, text.indexOf(end))
    .trim()
    .split("\n")
    .filter((line) => !/^\s*\/\/ #(?:end)?region /.test(line))
    .join("\n");
}

function range(first: number, last: number) {
  return Array.from({ length: last - first + 1 }, (_, index) => first + index);
}

const sourceLines = extractRegion(source, "run-capture-coroutine").split("\n");
const sourceTokens = ref<ThemedToken[][]>([]);

type Stage = {
  visible: number[];
  focus: number[];
  overview?: boolean;
};

const stages: Stage[] = [
  {
    visible: [...range(1, 5), ...range(32, 37)],
    focus: range(3, 3),
  },
  {
    visible: [...range(1, 5), ...range(32, 37)],
    focus: range(4, 4),
  },
  {
    visible: [...range(1, 11), ...range(32, 37)],
    focus: range(6, 11),
  },
  {
    visible: [1, 2, 5, ...range(1, 18), ...range(32, 37)],
    focus: range(12, 18),
  },
  {
    visible: [1, 2, 5, ...range(1, 37)],
    focus: range(19, 29),
  },
  {
    visible: [1, 2, 5, ...range(1, 37)],
    focus: range(30, 31),
  },
  {
    visible: range(1, 37),
    focus: [],
    overview: true,
  },
];

const stage = ref(0);
const codeViewport = ref<HTMLElement>();
const { $clicksContext: clicks } = useSlideContext();
const clickId = `run-capture-code-${Math.random()}`;
let scrollFrame: number | undefined;

const visibleLines = computed(() => new Set(stages[stage.value].visible));
const focusLines = computed(() => new Set(stages[stage.value].focus));
const overview = computed(() => stages[stage.value].overview ?? false);

type DisplayRow =
  | { kind: "line"; key: string; lineNumber: number }
  | { kind: "gap"; key: string; afterLine: number; indent: number };

const displayRows: DisplayRow[] = sourceLines.flatMap((line, index) => {
  const lineNumber = index + 1;
  const nextLine = sourceLines[index + 1] ?? "";
  return [
    { kind: "line", key: `line-${lineNumber}`, lineNumber } as DisplayRow,
    ...(lineNumber < sourceLines.length
      ? [
          {
            kind: "gap",
            key: `gap-${lineNumber}`,
            afterLine: lineNumber,
            indent: nextLine.match(/^\s*/)?.[0].length ?? 0,
          } as DisplayRow,
        ]
      : []),
  ];
});

function rowIsVisible(row: DisplayRow) {
  if (row.kind === "line") return visibleLines.value.has(row.lineNumber);
  if (!visibleLines.value.has(row.afterLine)) return false;
  if (visibleLines.value.has(row.afterLine + 1)) return false;
  return stages[stage.value].visible.some((line) => line > row.afterLine);
}

function rowIsFocused(row: DisplayRow) {
  return (
    row.kind === "line" &&
    rowIsVisible(row) &&
    focusLines.value.has(row.lineNumber)
  );
}

function rowClasses(row: DisplayRow) {
  return [
    row.kind === "line" ? "code-line" : "code-gap",
    {
      "is-visible": rowIsVisible(row),
      "is-focus": rowIsFocused(row),
    },
  ];
}

function scrollTarget(viewport: HTMLElement) {
  if (overview.value) return 0;
  const focused = viewport.querySelectorAll<HTMLElement>(".code-line.is-focus");
  if (!focused.length) return viewport.scrollTop;
  const first = focused[0];
  const last = focused[focused.length - 1];
  if (focused.length > 3) {
    return Math.max(
      0,
      Math.min(
        first.offsetTop - 16,
        viewport.scrollHeight - viewport.clientHeight,
      ),
    );
  }
  const center = (first.offsetTop + last.offsetTop + last.offsetHeight) / 2;
  return Math.max(
    0,
    Math.min(
      center - viewport.clientHeight / 2,
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

watch(stage, (_, previous) => centerFocusedCode(previous === undefined), {
  immediate: true,
});

onUnmounted(() => {
  if (scrollFrame !== undefined) cancelAnimationFrame(scrollFrame);
  clicks.unregister(clickId);
});
</script>

<template>
  <div
    ref="codeViewport"
    class="code-viewport relative mt-3 h-[33rem] overflow-hidden rounded-xl border border-slate-300 bg-[#f7f7f8] py-2 shadow-slide-card"
    :class="{ 'is-overview': overview }"
  >
    <div v-for="row in displayRows" :key="row.key" :class="rowClasses(row)">
      <template v-if="row.kind === 'gap'">
        <span
          class="col-start-2 font-mono text-[1rem] leading-none text-slate-400"
          :style="{ paddingLeft: `${row.indent}ch` }"
          >⋮</span
        >
      </template>
      <template v-else>
        <span class="text-center font-mono text-[1rem] text-slate-400">{{
          row.lineNumber
        }}</span>
        <code class="overflow-hidden font-mono whitespace-pre text-slate-800">
          <template v-if="sourceTokens[row.lineNumber - 1]">
            <span
              v-for="(token, tokenIndex) in sourceTokens[row.lineNumber - 1]"
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
</template>

<style scoped>
.code-line,
.code-gap {
  display: grid;
  grid-template-columns: 3.25rem 1fr;
  align-items: center;
  height: 0;
  min-height: 0;
  overflow: hidden;
  border-left: 5px solid transparent;
  opacity: 0;
  transform: translateY(0.3rem);
  transition:
    height 520ms cubic-bezier(0.22, 1, 0.36, 1),
    opacity 240ms ease,
    transform 520ms cubic-bezier(0.22, 1, 0.36, 1),
    background-color 240ms ease,
    border-color 240ms ease;
}

.code-line.is-visible {
  height: 2rem;
  opacity: 1;
  transform: translateY(0);
}

.code-gap.is-visible {
  height: 1rem;
  opacity: 1;
  transform: translateY(0);
}

.code-line.is-focus {
  border-left-color: #ea580c;
  background: rgb(251 146 60 / 0.22);
}

.code-line code {
  overflow: visible;
  border-radius: 0;
  background: transparent;
  padding: 0;
  font-size: 1.3rem !important;
  line-height: 2rem;
}

.code-viewport.is-overview .code-line.is-visible {
  height: 0.84rem;
}

.code-viewport.is-overview .code-line > span {
  font-size: 0.68rem !important;
}

.code-viewport.is-overview .code-line code {
  font-size: 0.78rem !important;
  line-height: 0.84rem;
}
</style>
