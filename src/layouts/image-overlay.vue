<template>
  <section class="relative h-full w-full overflow-hidden text-white">
    <img
      v-if="$frontmatter.image"
      :src="$frontmatter.image"
      class="absolute inset-0 h-full w-full object-cover"
      alt=""
    />
    <div class="absolute inset-0 bg-gradient-to-r from-black/78 via-black/32 to-transparent" />
    <main class="relative z-10 flex h-full max-w-4xl flex-col justify-end px-16 py-14">
      <slot />
    </main>
  </section>
</template>
