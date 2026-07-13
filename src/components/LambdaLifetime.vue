<template>
  <div class="lambda-life">
    <div class="life-lane owner-lane">
      <div class="lane-label">owner / scope</div>
      <div class="life-event event-create" v-click="1">① lambda object created</div>
      <div class="life-event event-destroy" v-click="3">③ lambda object destroyed</div>
    </div>

    <div class="life-lane coroutine-lane">
      <div class="lane-label">coroutine</div>
      <div class="life-event event-call" v-click="2">② call returns a coroutine</div>
      <div class="life-event event-start" v-click="4">④ body starts later</div>
    </div>

    <div class="timeline-line"></div>
    <div class="capture-link" v-click="2">closure is <strong>this</strong></div>
    <div class="dangling-warning" v-click="4">💥 captured state is already gone</div>
  </div>
</template>
