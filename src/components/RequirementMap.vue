<template>
  <table>
    <thead>
      <tr>
        <th>Need</th>
        <th>How this setup handles it</th>
      </tr>
    </thead>
    <tbody>
      <tr v-for="item in items" :key="item.need">
        <td>{{ item.need }}</td>
        <td>{{ item.answer }}</td>
      </tr>
    </tbody>
  </table>
</template>

<script setup lang="ts">
const items = [
  { need: 'HTML slides', answer: 'Slidev build outputs a static web app in dist/.' },
  { need: 'Presenter view', answer: '/presenter shows current, next, notes, timer, and navigation.' },
  { need: 'PDF generator', answer: 'slidev export uses Chromium through Playwright.' },
  { need: 'Full-screen images', answer: 'Custom image-overlay layout plus static assets in public/.' },
  { need: 'Modern C++ code', answer: 'Shiki with cpp, line numbers, word and line highlighting.' },
  { need: 'Incremental reveals', answer: 'v-click, dynamic code line highlighting, and Magic Move.' },
  { need: 'Real code snippets', answer: 'Use <<< ./snippets/file.cpp#region cpp.' },
]
</script>
