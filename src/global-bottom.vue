<template>
  <footer v-if="$slidev.nav.currentPage > 1" class="deck-footer">
    <span class="footer-author">Andreas Reischuck</span>
    <span class="footer-title">C++20 Coroutines in Action</span>
    <span class="footer-page">{{ $slidev.nav.currentPage }}</span>
  </footer>
</template>
