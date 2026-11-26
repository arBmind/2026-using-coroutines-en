<template>
  <footer
    v-if="![1, 8, 37].includes($slidev.nav.currentPage)"
    class="absolute inset-x-0 bottom-0 z-100 grid h-footer grid-cols-[1fr_auto_1fr] items-center bg-[#07090d] px-[1.45rem] text-[0.73rem] tracking-[0.025em] text-slate-50"
  >
    <span>Andreas Reischuck</span>
    <span class="font-[650] text-cyan-300">C++20 Coroutines in Action</span>
    <span
      class="justify-self-end font-mono text-[0.88rem] font-bold leading-none text-orange-300"
      >{{ $slidev.nav.currentPage }}</span
    >
  </footer>
</template>
