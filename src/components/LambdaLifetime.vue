<template>
  <div class="relative mt-[0.8rem] h-[390px] pt-5">
    <div class="relative ml-[130px] h-[145px] border-t-2 border-slate-400">
      <div
        class="absolute top-[-13px] left-[-130px] w-[110px] text-right text-xs font-bold text-muted"
      >
        owner / scope
      </div>
      <div
        class="absolute top-[18px] left-[4%] w-[190px] rounded-[10px] bg-brand-blue p-[0.7rem] text-center text-xs font-bold text-white"
        v-click="1"
      >
        ① lambda object created
      </div>
      <div
        class="absolute top-[18px] left-[43%] w-[190px] rounded-[10px] bg-rose-700 p-[0.7rem] text-center text-xs font-bold text-white"
        v-click="3"
      >
        ③ lambda object destroyed
      </div>
    </div>

    <div class="relative ml-[130px] h-[145px] border-t-2 border-slate-400">
      <div
        class="absolute top-[-13px] left-[-130px] w-[110px] text-right text-xs font-bold text-muted"
      >
        coroutine
      </div>
      <div
        class="absolute top-[18px] left-[23%] w-[190px] rounded-[10px] bg-brand-cyan p-[0.7rem] text-center text-xs font-bold text-white"
        v-click="2"
      >
        ② call returns a coroutine
      </div>
      <div
        class="absolute top-[18px] left-[69%] w-[190px] rounded-[10px] bg-brand-green p-[0.7rem] text-center text-xs font-bold text-white"
        v-click="4"
      >
        ④ body starts later
      </div>
    </div>

    <div
      class="absolute top-[92px] left-[37%] h-28 border-l-3 border-dashed border-brand-orange pl-2 text-xs text-brand-orange"
      v-click="2"
    >
      closure is <strong>this</strong>
    </div>
    <div
      class="absolute right-[2%] bottom-[18px] rounded-xl border-3 border-rose-700 bg-rose-50 px-4 py-3 text-base font-[850] text-rose-800"
      v-click="4"
    >
      💥 captured state is already gone
    </div>
  </div>
</template>
