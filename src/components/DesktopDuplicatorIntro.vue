<script setup lang="ts">
import { useSlideContext } from "@slidev/client";
import { computed, onMounted, onUnmounted, ref, watchEffect } from "vue";

import cppUsergroupLogo from "../public/images/cppug.png";

const stages = [
  { step: "01", label: "EMPTY DESKTOP", scene: "LOCAL PROJECTOR" },
  { step: "02", label: "OPEN THE IDE", scene: "LOCAL PROJECTOR" },
  { step: "03", label: "KEEP NOTES PRIVATE", scene: "LOCAL PROJECTOR" },
  { step: "04", label: "CAPTURE THE WHOLE TAB", scene: "LOCAL PROJECTOR" },
  { step: "05", label: "ULTRAWIDE + TEAMS", scene: "REMOTE CALL" },
  { step: "06", label: "SHARE THE LIVE CAPTURE", scene: "REMOTE CALL" },
  { step: "07", label: "ALIGN TO THE WHOLE PANE", scene: "REMOTE CALL" },
];

const stage = ref(0);
const { $clicksContext: clicks } = useSlideContext();
const clickId = `desktop-duplicator-intro-${Math.random()}`;

onMounted(() => {
  const clickInfo = clicks.calculateSince("+1", stages.length - 1);
  clicks.register(clickId, clickInfo);
  watchEffect(() => {
    stage.value = Math.max(
      0,
      Math.min(stages.length - 1, (clickInfo?.currentOffset.value ?? -1) + 1),
    );
  });
});

onUnmounted(() => clicks.unregister(clickId));

const current = computed(() => stages[stage.value]);
const firstScene = computed(() => stage.value < 4);
</script>

<template>
  <div class="mt-2 h-[500px] w-full">
    <svg
      class="h-full w-full"
      viewBox="0 0 1160 500"
      role="img"
      aria-label="Animated desktop duplication workflow: a selected editor region is projected locally, then one source pane is shared into a Teams call"
    >
      <defs>
        <linearGradient id="desktop-wallpaper" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0" stop-color="#e0f2fe" />
          <stop offset="0.55" stop-color="#f8fafc" />
          <stop offset="1" stop-color="#dbeafe" />
        </linearGradient>
        <linearGradient id="ultrawide-wallpaper" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0" stop-color="#dbeafe" />
          <stop offset="0.5" stop-color="#f8fafc" />
          <stop offset="1" stop-color="#cffafe" />
        </linearGradient>
        <filter id="window-shadow" x="-20%" y="-20%" width="140%" height="150%">
          <feDropShadow
            dx="0"
            dy="8"
            stdDeviation="8"
            flood-color="#0f172a"
            flood-opacity="0.2"
          />
        </filter>
        <filter id="soft-shadow" x="-20%" y="-20%" width="140%" height="150%">
          <feDropShadow
            dx="0"
            dy="5"
            stdDeviation="5"
            flood-color="#0f172a"
            flood-opacity="0.16"
          />
        </filter>
        <clipPath id="desktop-screen-clip">
          <rect x="50" y="62" width="654" height="332" rx="7" />
        </clipPath>
        <clipPath id="wide-screen-clip">
          <rect x="64" y="65" width="1032" height="348" rx="8" />
        </clipPath>
        <clipPath id="projector-screen-clip">
          <rect x="805" y="91" width="306" height="210" rx="4" />
        </clipPath>
        <clipPath id="teams-share-clip">
          <rect x="790" y="130" width="276" height="172" rx="5" />
        </clipPath>
      </defs>

      <!-- Scene one: desktop plus projector. -->
      <g class="scene scene-one" :class="{ hidden: !firstScene }">
        <g class="scene-label">
          <rect x="34" y="5" width="282" height="34" rx="17" fill="#e0f2fe" />
          <circle cx="52" cy="22" r="10" fill="#075985" />
          <text
            x="52"
            y="26"
            text-anchor="middle"
            fill="white"
            class="step-number"
          >
            1
          </text>
          <text x="72" y="26" class="eyebrow">
            PRIVATE NOTES → CLEAN PROJECTOR
          </text>
        </g>

        <!-- Monitor hardware and intentionally empty desktop. -->
        <rect
          x="34"
          y="46"
          width="686"
          height="366"
          rx="18"
          fill="#172033"
          filter="url(#soft-shadow)"
        />
        <rect
          x="50"
          y="62"
          width="654"
          height="332"
          rx="7"
          fill="url(#desktop-wallpaper)"
        />
        <g clip-path="url(#desktop-screen-clip)">
          <path
            d="M25 345 C190 220 312 360 475 218 C573 134 681 150 752 94 L752 420 L25 420Z"
            fill="#bfdbfe"
            opacity="0.54"
          />
          <path
            d="M15 385 C168 288 292 403 430 295 C554 198 641 235 751 172 L751 420 L15 420Z"
            fill="#67e8f9"
            opacity="0.34"
          />
          <rect
            x="50"
            y="374"
            width="654"
            height="20"
            fill="#0f172a"
            opacity="0.88"
          />
          <circle cx="376" cy="384" r="5" fill="#38bdf8" />
          <rect
            x="388"
            y="379"
            width="10"
            height="10"
            rx="2"
            fill="#f8fafc"
            opacity="0.86"
          />

          <!-- Large IDE enters first, then visibly contracts into the compact version. -->
          <g
            class="ide-large light-ide"
            :class="{ visible: stage >= 1, compacting: stage >= 2 }"
            filter="url(#window-shadow)"
          >
            <rect
              x="66"
              y="78"
              width="622"
              height="280"
              rx="8"
              fill="#0f172a"
            />
            <rect x="66" y="78" width="622" height="27" rx="8" fill="#20293a" />
            <circle cx="82" cy="91.5" r="4" fill="#fb7185" />
            <circle cx="96" cy="91.5" r="4" fill="#fbbf24" />
            <circle cx="110" cy="91.5" r="4" fill="#34d399" />
            <text x="126" y="95" class="window-title">
              DesktopDuplicator — Qt Creator
            </text>
            <rect
              x="66"
              y="105"
              width="135"
              height="184"
              fill="#111827"
              stroke="#cbd5e1"
              stroke-width="1.5"
            />
            <text x="78" y="124" class="panel-label">PROJECT</text>
            <text x="79" y="146" class="tree">▾ DesktopDuplicator</text>
            <text x="91" y="164" class="tree muted">▾ src</text>
            <rect
              x="72"
              y="173"
              width="124"
              height="20"
              rx="3"
              fill="#0e7490"
              opacity="0.52"
            />
            <text x="103" y="187" class="tree bright">◆ Duplication.cpp</text>
            <text x="103" y="207" class="tree muted">◇ CaptureArea.cpp</text>
            <text x="103" y="227" class="tree muted">◇ Renderer.cpp</text>
            <text x="103" y="247" class="tree muted">◇ main.cpp</text>
            <rect
              x="201"
              y="105"
              width="487"
              height="184"
              fill="#101827"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <rect
              x="201"
              y="105"
              width="487"
              height="25"
              fill="#182235"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <text x="216" y="121" class="tab active">Duplication.cpp</text>
            <text x="225" y="151" class="line-no">31</text>
            <text x="246" y="151" class="code">
              <tspan class="kw">while</tspan>
              (running) {
            </text>
            <text x="225" y="170" class="line-no">32</text>
            <text x="260" y="170" class="code">
              <tspan class="type">auto</tspan>
              frame =
              <tspan class="kw">co_await</tspan>
              output.nextFrame();
            </text>
            <text x="225" y="189" class="line-no">33</text>
            <text x="260" y="189" class="code">
              <tspan class="kw">co_await</tspan>
              renderThread.resumeOn();
            </text>
            <text x="225" y="208" class="line-no">34</text>
            <text x="260" y="208" class="code">renderer.update(frame);</text>
            <text x="225" y="227" class="line-no">35</text>
            <text x="260" y="227" class="code">
              <tspan class="kw">co_await</tspan>
              mainThread.resumeOn();
            </text>
            <text x="225" y="246" class="line-no">36</text>
            <text x="260" y="246" class="code">status.setFrameReady();</text>
            <text x="225" y="265" class="line-no">37</text>
            <text x="246" y="265" class="code">}</text>
            <rect
              x="66"
              y="289"
              width="622"
              height="69"
              fill="#0b1220"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <text x="78" y="306" class="panel-label">APPLICATION OUTPUT</text>
            <text x="79" y="326" class="console">
              <tspan fill="#34d399">✓</tspan>
              Desktop duplication initialized
            </text>
            <text x="79" y="344" class="console">
              <tspan fill="#38bdf8">→</tspan>
              output 0 · 1920 × 1080 · 60 fps
            </text>
          </g>

          <!-- Compact IDE replaces the large IDE after its resize transition. -->
          <g
            class="compact-window light-ide"
            :class="{ visible: stage >= 2 }"
            filter="url(#window-shadow)"
          >
            <rect
              x="66"
              y="83"
              width="405"
              height="266"
              rx="7"
              fill="#0f172a"
            />
            <rect x="66" y="83" width="405" height="25" rx="7" fill="#20293a" />
            <circle cx="81" cy="95.5" r="3.5" fill="#fb7185" />
            <circle cx="94" cy="95.5" r="3.5" fill="#fbbf24" />
            <circle cx="107" cy="95.5" r="3.5" fill="#34d399" />
            <text x="121" y="99" class="window-title">
              DesktopDuplicator — Qt Creator
            </text>
            <rect
              x="66"
              y="108"
              width="90"
              height="166"
              fill="#111827"
              stroke="#cbd5e1"
              stroke-width="1.5"
            />
            <text x="75" y="126" class="panel-label">PROJECT</text>
            <text x="75" y="145" class="tree tiny">▾ src</text>
            <rect
              x="70"
              y="153"
              width="81"
              height="20"
              rx="3"
              fill="#0e7490"
              opacity="0.52"
            />
            <text x="79" y="167" class="tree tiny bright">
              ◆ Duplication.cpp
            </text>
            <text x="79" y="188" class="tree tiny muted">
              ◇ CaptureArea.cpp
            </text>
            <text x="79" y="209" class="tree tiny muted">◇ Renderer.cpp</text>
            <text x="79" y="230" class="tree tiny muted">◇ main.cpp</text>
            <rect
              x="156"
              y="108"
              width="315"
              height="166"
              fill="#101827"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <rect
              x="156"
              y="108"
              width="315"
              height="23"
              fill="#182235"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <text x="169" y="123" class="tab active">Duplication.cpp</text>
            <text x="165" y="149" class="line-no">31</text>
            <text x="184" y="149" class="code small">
              <tspan class="kw">while</tspan>
              (running) {
            </text>
            <text x="165" y="168" class="line-no">32</text>
            <text x="193" y="168" class="code small">
              <tspan class="type">auto</tspan>
              frame =
              <tspan class="kw">co_await</tspan>
              nextFrame();
            </text>
            <text x="165" y="187" class="line-no">33</text>
            <text x="193" y="187" class="code small">
              <tspan class="kw">co_await</tspan>
              renderThread.resumeOn();
            </text>
            <text x="165" y="206" class="line-no">34</text>
            <text x="193" y="206" class="code small">
              renderer.update(frame);
            </text>
            <text x="165" y="225" class="line-no">35</text>
            <text x="193" y="225" class="code small">
              <tspan class="kw">co_await</tspan>
              mainThread.resumeOn();
            </text>
            <text x="165" y="244" class="line-no">36</text>
            <text x="193" y="244" class="code small">
              status.setFrameReady();
            </text>
            <text x="184" y="263" class="code small">}</text>
            <rect
              x="66"
              y="274"
              width="405"
              height="75"
              fill="#0b1220"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <text x="76" y="291" class="panel-label">APPLICATION OUTPUT</text>
            <text x="76" y="312" class="console">
              <tspan fill="#34d399">✓</tspan>
              duplication initialized
            </text>
            <text x="76" y="332" class="console">
              <tspan fill="#38bdf8">→</tspan>
              1920 × 1080 · 60 fps
            </text>
          </g>

          <!-- Private notes appear in the space created by resizing the IDE. -->
          <g
            class="notes-window"
            :class="{ visible: stage >= 2 }"
            filter="url(#window-shadow)"
          >
            <rect
              x="484"
              y="83"
              width="202"
              height="266"
              rx="7"
              fill="#fffbeb"
              stroke="#f59e0b"
              stroke-width="1.5"
            />
            <rect
              x="484"
              y="83"
              width="202"
              height="27"
              rx="7"
              fill="#fef3c7"
            />
            <text x="497" y="101" class="notes-title">
              Presenter notes · PRIVATE
            </text>
            <text x="499" y="133" class="notes-heading">DEMO CHECKLIST</text>
            <circle cx="501" cy="157" r="3" fill="#ea580c" />
            <text x="511" y="161" class="note">Explain thread affinity</text>
            <circle cx="501" cy="184" r="3" fill="#ea580c" />
            <text x="511" y="188" class="note">Show capture rectangle</text>
            <circle cx="501" cy="211" r="3" fill="#ea580c" />
            <text x="511" y="215" class="note">Share source — not notes</text>
            <line
              x1="499"
              y1="239"
              x2="670"
              y2="239"
              stroke="#f59e0b"
              stroke-opacity="0.35"
            />
            <text x="499" y="260" class="note hand">
              “Audience sees only the code.”
            </text>
            <text x="499" y="316" class="notes-heading">
              NEXT: APC → COROUTINE
            </text>
          </g>

          <!-- Transparent capture area over only the useful IDE content. -->
          <g class="capture-area capture-one" :class="{ visible: stage >= 3 }">
            <rect
              class="capture-frame"
              x="156"
              y="108"
              width="315"
              height="166"
              rx="4"
              fill="#ef4444"
              fill-opacity="0.1"
              stroke="#ef4444"
              stroke-width="4"
            />
          </g>
        </g>
        <rect x="318" y="412" width="118" height="12" rx="6" fill="#1e293b" />
        <path d="M342 411 L330 442 H424 L412 411Z" fill="#334155" />
        <rect x="309" y="439" width="136" height="9" rx="4.5" fill="#172033" />

        <!-- Beamer and its projected screen. -->
        <rect
          x="787"
          y="72"
          width="342"
          height="247"
          rx="8"
          fill="#e2e8f0"
          stroke="#94a3b8"
          stroke-width="2"
          filter="url(#soft-shadow)"
        />
        <rect x="805" y="91" width="306" height="210" rx="4" fill="#f8fafc" />
        <text
          v-if="stage < 3"
          x="958"
          y="187"
          text-anchor="middle"
          class="empty-title"
        >
          PROJECTOR / BEAMER
        </text>
        <text
          v-if="stage < 3"
          x="958"
          y="210"
          text-anchor="middle"
          class="empty-subtitle"
        >
          waiting for capture…
        </text>
        <g clip-path="url(#projector-screen-clip)">
          <g
            class="projected-content light-ide"
            :class="{ visible: stage >= 3 }"
          >
            <rect
              x="805"
              y="91"
              width="306"
              height="210"
              fill="#101827"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <rect
              x="805"
              y="91"
              width="306"
              height="27"
              fill="#182235"
              stroke="#94a3b8"
              stroke-width="1.5"
            />
            <text x="821" y="109" class="tab active">Duplication.cpp</text>
            <text x="821" y="141" class="line-no projected">31</text>
            <text x="849" y="141" class="code projected">
              <tspan class="kw">while</tspan>
              (running) {
            </text>
            <text x="821" y="167" class="line-no projected">32</text>
            <text x="865" y="167" class="code projected">
              <tspan class="type">auto</tspan>
              frame =
              <tspan class="kw">co_await</tspan>
              nextFrame();
            </text>
            <text x="821" y="193" class="line-no projected">33</text>
            <text x="865" y="193" class="code projected">
              <tspan class="kw">co_await</tspan>
              renderThread.resumeOn();
            </text>
            <text x="821" y="219" class="line-no projected">34</text>
            <text x="865" y="219" class="code projected">
              renderer.update(frame);
            </text>
            <text x="821" y="245" class="line-no projected">35</text>
            <text x="865" y="245" class="code projected">
              <tspan class="kw">co_await</tspan>
              mainThread.resumeOn();
            </text>
            <text x="821" y="271" class="line-no projected">36</text>
            <text x="865" y="271" class="code projected">
              status.setFrameReady();
            </text>
          </g>
        </g>
        <rect
          x="879"
          y="333"
          width="158"
          height="68"
          rx="14"
          fill="#cbd5e1"
          stroke="#94a3b8"
          stroke-width="2"
          filter="url(#soft-shadow)"
        />
        <circle cx="1013" cy="367" r="17" fill="#334155" />
        <circle cx="1013" cy="367" r="10" fill="#38bdf8" opacity="0.65" />
        <rect x="895" y="347" width="76" height="8" rx="4" fill="#94a3b8" />
        <rect x="895" y="363" width="50" height="6" rx="3" fill="#94a3b8" />
        <text x="958" y="426" text-anchor="middle" class="hardware-label">
          BEAMER OUTPUT
        </text>
      </g>

      <!-- Scene two: ultrawide screen and Teams call. -->
      <g class="scene scene-two" :class="{ visible: !firstScene }">
        <rect
          x="44"
          y="45"
          width="1072"
          height="386"
          rx="20"
          fill="#172033"
          filter="url(#soft-shadow)"
        />
        <rect
          x="64"
          y="65"
          width="1032"
          height="348"
          rx="8"
          fill="url(#ultrawide-wallpaper)"
        />
        <g clip-path="url(#wide-screen-clip)">
          <!-- Wide IDE with two source panes. -->
          <g filter="url(#window-shadow)">
            <rect
              x="78"
              y="79"
              width="665"
              height="314"
              rx="7"
              fill="#0f172a"
            />
            <rect x="78" y="79" width="665" height="25" rx="7" fill="#20293a" />
            <circle cx="93" cy="91.5" r="3.5" fill="#fb7185" />
            <circle cx="106" cy="91.5" r="3.5" fill="#fbbf24" />
            <circle cx="119" cy="91.5" r="3.5" fill="#34d399" />
            <text x="134" y="95" class="window-title">
              DesktopDuplicator — Qt Creator
            </text>
            <rect
              x="78"
              y="104"
              width="105"
              height="222"
              fill="#111827"
              stroke="#475569"
              stroke-width="2"
            />
            <text x="89" y="123" class="panel-label">PROJECT</text>
            <text x="89" y="144" class="tree tiny">▾ src</text>
            <rect
              x="83"
              y="151"
              width="95"
              height="20"
              rx="3"
              fill="#0e7490"
              opacity="0.5"
            />
            <text x="91" y="165" class="tree tiny bright">
              ◆ CaptureThread.cpp
            </text>
            <text x="91" y="187" class="tree tiny muted">
              ◇ FrameUpdater.cpp
            </text>
            <text x="91" y="209" class="tree tiny muted">
              ◇ WindowRenderer.cpp
            </text>
            <text x="91" y="231" class="tree tiny muted">◇ MainThread.cpp</text>

            <!-- Left source file. -->
            <rect
              x="183"
              y="104"
              width="280"
              height="222"
              fill="#0f1b2d"
              stroke="#64748b"
              stroke-width="2"
            />
            <rect
              x="183"
              y="104"
              width="280"
              height="24"
              fill="#183252"
              stroke="#64748b"
              stroke-width="2"
            />
            <text x="196" y="120" class="tab active">CaptureThread.cpp</text>
            <text x="192" y="150" class="line-no">49</text>
            <text x="216" y="150" class="code small">
              <tspan class="type">task</tspan>
              &lt;Frame&gt; CaptureThread::next() {
            </text>
            <text x="192" y="172" class="line-no">50</text>
            <text x="230" y="172" class="code small">
              <tspan class="kw">co_await</tspan>
              resumeOn(captureThread);
            </text>
            <text x="192" y="194" class="line-no">51</text>
            <text x="230" y="194" class="code small">
              <tspan class="type">auto</tspan>
              frame = acquireFrame();
            </text>
            <text x="192" y="216" class="line-no">52</text>
            <text x="230" y="216" class="code small">
              <tspan class="kw">co_return</tspan>
              frame;
            </text>
            <text x="192" y="238" class="line-no">53</text>
            <text x="216" y="238" class="code small">}</text>
            <text x="192" y="270" class="line-no">61</text>
            <text x="216" y="270" class="code small">
              <tspan class="comment">// capture API affinity</tspan>
            </text>

            <!-- Right source file. -->
            <rect
              x="464"
              y="104"
              width="279"
              height="222"
              fill="#17162b"
              stroke="#64748b"
              stroke-width="2"
            />
            <rect
              x="464"
              y="104"
              width="279"
              height="24"
              fill="#302b4c"
              stroke="#64748b"
              stroke-width="2"
            />
            <text x="477" y="120" class="tab active">FrameUpdater.cpp</text>
            <text x="474" y="150" class="line-no">108</text>
            <text x="504" y="150" class="code small">
              <tspan class="type">void</tspan>
              FrameUpdater::update(Frame f) {
            </text>
            <text x="474" y="172" class="line-no">109</text>
            <text x="518" y="172" class="code small">
              <tspan class="type">auto</tspan>
              texture = createTexture(f);
            </text>
            <text x="474" y="194" class="line-no">110</text>
            <text x="518" y="194" class="code small">
              copyChangedRegions(texture);
            </text>
            <text x="474" y="216" class="line-no">111</text>
            <text x="518" y="216" class="code small">renderer.present();</text>
            <text x="474" y="238" class="line-no">112</text>
            <text x="504" y="238" class="code small">}</text>
            <text x="474" y="270" class="line-no">120</text>
            <text x="504" y="270" class="code small">
              <tspan class="comment">// render thread affinity</tspan>
            </text>

            <rect
              x="78"
              y="326"
              width="665"
              height="67"
              fill="#0b1220"
              stroke="#475569"
              stroke-width="2"
            />
            <text x="89" y="344" class="panel-label">APPLICATION OUTPUT</text>
            <text x="89" y="366" class="console">
              <tspan fill="#34d399">✓</tspan>
              capture thread ready
            </text>
            <text x="89" y="384" class="console">
              <tspan fill="#38bdf8">→</tspan>
              sharing region 1600 × 900
            </text>
          </g>

          <!-- Teams window. -->
          <g filter="url(#window-shadow)">
            <rect
              x="756"
              y="79"
              width="326"
              height="314"
              rx="8"
              fill="#f8fafc"
            />
            <rect
              x="756"
              y="79"
              width="326"
              height="35"
              rx="8"
              fill="#4f46a5"
            />
            <circle cx="774" cy="96.5" r="4" fill="#c4b5fd" />
            <text x="789" y="101" class="teams-title">Teams · C++ pairing</text>
            <rect
              x="770"
              y="126"
              width="298"
              height="190"
              rx="7"
              fill="#111827"
            />

            <!-- Normal call view fades out when sharing starts. -->
            <g class="call-grid" :class="{ hidden: stage >= 5 }">
              <rect
                x="782"
                y="139"
                width="132"
                height="76"
                rx="6"
                fill="#334155"
              />
              <circle cx="848" cy="166" r="17" fill="#0ea5e9" />
              <text x="848" y="172" text-anchor="middle" class="avatar">
                AR
              </text>
              <text x="793" y="207" class="person">Andreas</text>
              <rect
                x="924"
                y="139"
                width="132"
                height="76"
                rx="6"
                fill="#312e81"
              />
              <circle cx="990" cy="166" r="17" fill="#a78bfa" />
              <text x="990" y="172" text-anchor="middle" class="avatar">
                MC
              </text>
              <text x="935" y="207" class="person">Meeting C++</text>
              <rect
                x="782"
                y="225"
                width="274"
                height="76"
                rx="6"
                fill="#1e293b"
              />
              <rect
                x="790"
                y="229"
                width="58"
                height="58"
                rx="7"
                fill="white"
              />
              <image
                :href="cppUsergroupLogo"
                x="793"
                y="232"
                width="52"
                height="52"
                preserveAspectRatio="xMidYMid meet"
              />
              <text x="855" y="263" class="person group-name">
                C++ Usergroup Dresden
              </text>
            </g>

            <!-- The selected source pane becomes the main share. -->
            <g clip-path="url(#teams-share-clip)">
              <g
                class="teams-share"
                :class="{ visible: stage >= 5, settled: stage >= 6 }"
              >
                <rect
                  x="790"
                  y="130"
                  width="276"
                  height="172"
                  rx="5"
                  fill="#101827"
                />
                <rect x="790" y="130" width="276" height="23" fill="#182235" />
                <text x="802" y="146" class="tab active">
                  CaptureThread.cpp
                </text>
                <text x="801" y="174" class="line-no projected">49</text>
                <text x="831" y="174" class="code projected">
                  <tspan class="type">task</tspan>
                  &lt;Frame&gt; CaptureThread::next() {
                </text>
                <text x="801" y="200" class="line-no projected">50</text>
                <text x="847" y="200" class="code projected">
                  <tspan class="kw">co_await</tspan>
                  resumeOn(captureThread);
                </text>
                <text x="801" y="226" class="line-no projected">51</text>
                <text x="847" y="226" class="code projected">
                  <tspan class="type">auto</tspan>
                  frame = acquireFrame();
                </text>
                <text x="801" y="252" class="line-no projected">52</text>
                <text x="847" y="252" class="code projected">
                  <tspan class="kw">co_return</tspan>
                  frame;
                </text>
                <text x="801" y="278" class="line-no projected">53</text>
                <text x="831" y="278" class="code projected">}</text>
                <text x="801" y="297" class="line-no projected">61</text>
                <text x="831" y="297" class="code projected">
                  <tspan class="comment">// capture API affinity</tspan>
                </text>
              </g>
            </g>

            <rect
              x="770"
              y="327"
              width="298"
              height="49"
              rx="7"
              fill="#eef2ff"
            />
            <circle cx="823" cy="351" r="14" fill="#ffffff" stroke="#c7d2fe" />
            <path
              d="M817 351 H829 M823 345 V357"
              stroke="#4f46a5"
              stroke-width="2"
              stroke-linecap="round"
            />
            <circle cx="867" cy="351" r="14" fill="#ffffff" stroke="#c7d2fe" />
            <path
              d="M861 347 L873 355 M873 347 L861 355"
              stroke="#4f46a5"
              stroke-width="2"
              stroke-linecap="round"
            />
            <rect
              x="911"
              y="337"
              width="88"
              height="28"
              rx="14"
              :fill="stage >= 5 ? '#16a34a' : '#4f46a5'"
            />
            <text x="955" y="355" text-anchor="middle" class="share-button">
              {{ stage >= 5 ? "SHARING" : "SHARE" }}
            </text>
            <circle cx="1032" cy="351" r="14" fill="#dc2626" />
            <path
              d="M1026 351 H1038"
              stroke="white"
              stroke-width="2.5"
              stroke-linecap="round"
            />
          </g>

          <!-- Capture selection and its route into the call. -->
          <g
            class="capture-area capture-two"
            :class="{ visible: stage >= 5, settled: stage >= 6 }"
          >
            <rect
              class="capture-frame"
              x="183"
              y="104"
              width="280"
              height="222"
              rx="4"
              fill="#ef4444"
              fill-opacity="0.1"
              stroke="#ef4444"
              stroke-width="4"
            />
          </g>
        </g>
        <rect x="497" y="430" width="166" height="10" rx="5" fill="#1e293b" />
        <path d="M527 428 L514 456 H646 L633 428Z" fill="#334155" />
        <rect x="490" y="454" width="180" height="8" rx="4" fill="#172033" />
      </g>
    </svg>
  </div>
</template>

<style scoped>
.scene {
  transition:
    opacity 480ms ease,
    transform 650ms cubic-bezier(0.22, 1, 0.36, 1);
}
.scene-one.hidden {
  opacity: 0;
  transform: translateX(-70px) scale(0.97);
  pointer-events: none;
}
.scene-two {
  opacity: 0;
  transform: translateX(70px) scale(0.97);
  pointer-events: none;
}
.scene-two.visible {
  opacity: 1;
  transform: translateX(0) scale(1);
  pointer-events: auto;
}
.ide-large {
  opacity: 0;
  transform: translateY(16px) scale(0.97);
  transform-origin: 66px 78px;
  transition:
    opacity 360ms ease,
    transform 650ms cubic-bezier(0.22, 1, 0.36, 1);
}
.ide-large.visible {
  opacity: 1;
  transform: translateY(0) scale(1);
}
.ide-large.compacting {
  opacity: 0;
  transform: translate(0, 5px) scale(0.65, 0.95);
}
.compact-window,
.notes-window,
.capture-area,
.projected-content,
.teams-share {
  opacity: 0;
  transition:
    opacity 360ms ease,
    transform 520ms cubic-bezier(0.22, 1, 0.36, 1);
}
.compact-window {
  transform: translateY(10px) scale(0.985);
  transform-origin: 66px 83px;
  transition-delay: 180ms;
}
.notes-window {
  transform: translateX(26px) scale(0.98);
  transform-origin: 484px 83px;
  transition-delay: 260ms;
}
.capture-area {
  transform: scale(1.035);
  transform-origin: center;
}
.projected-content,
.teams-share {
  transform-origin: center;
}
.teams-share {
  transform: translate(-6px, -4px) scale(1.015);
}
.compact-window.visible,
.notes-window.visible,
.capture-area.visible {
  opacity: 1;
  transform: translate(0) scale(1);
}
.projected-content.visible,
.teams-share.visible {
  opacity: 1;
}
.projected-content.visible {
  animation: projector-settle 650ms cubic-bezier(0.22, 1, 0.36, 1) both;
}
.teams-share.settled {
  animation: teams-share-settle 650ms cubic-bezier(0.22, 1, 0.36, 1) both;
}
.capture-frame {
  transition: transform 650ms cubic-bezier(0.22, 1, 0.36, 1);
}
.capture-one .capture-frame {
  transform: translate(20px, 29px) scale(0.883, 0.717);
  transform-origin: 156px 108px;
}
.capture-one.visible .capture-frame {
  animation: capture-one-settle 650ms cubic-bezier(0.22, 1, 0.36, 1) both;
}
.capture-two .capture-frame {
  transform: translate(4px, 28px) scale(0.968, 0.689);
  transform-origin: 183px 104px;
}
.capture-two.settled .capture-frame {
  animation: capture-two-settle 650ms cubic-bezier(0.22, 1, 0.36, 1) both;
}
@keyframes capture-one-settle {
  from {
    transform: translate(20px, 29px) scale(0.883, 0.717);
  }
  to {
    transform: translate(0) scale(1);
  }
}
@keyframes projector-settle {
  from {
    transform: translate(-14px, -10px) scale(1.04);
  }
  to {
    transform: translate(0) scale(1);
  }
}
@keyframes capture-two-settle {
  from {
    transform: translate(4px, 28px) scale(0.968, 0.689);
  }
  to {
    transform: translate(0) scale(1);
  }
}
@keyframes teams-share-settle {
  from {
    transform: translate(-6px, -4px) scale(1.015);
  }
  to {
    transform: translate(0) scale(1);
  }
}
.call-grid {
  opacity: 1;
  transition: opacity 260ms ease;
}
.call-grid.hidden {
  opacity: 0;
}
.scene-label,
.status-strip {
  font-family: Inter, ui-sans-serif, system-ui, sans-serif;
}
.step-number,
.status-step {
  font-size: 11px;
  font-weight: 900;
}
.eyebrow {
  fill: #075985;
  font-size: 10px;
  font-weight: 850;
  letter-spacing: 0.07em;
}
.eyebrow.purple {
  fill: #5b21b6;
}
.window-title,
.teams-title {
  fill: #dbeafe;
  font-family: Inter, ui-sans-serif, system-ui, sans-serif;
  font-size: 9px;
  font-weight: 700;
}
.teams-title {
  fill: white;
  font-size: 10px;
}
.panel-label,
.hardware-label {
  fill: #94a3b8;
  font-family: Inter, ui-sans-serif, system-ui, sans-serif;
  font-size: 8px;
  font-weight: 800;
  letter-spacing: 0.08em;
}
.hardware-label {
  fill: #475569;
  font-size: 9px;
}
.tree,
.tab,
.line-no,
.code,
.console {
  font-family: "JetBrains Mono", ui-monospace, Consolas, monospace;
}
.tree {
  fill: #cbd5e1;
  font-size: 9px;
}
.tree.tiny {
  font-size: 7.5px;
}
.tree.muted {
  fill: #94a3b8;
}
.tree.bright {
  fill: #e0f2fe;
}
.tab {
  fill: #cbd5e1;
  font-size: 8px;
}
.tab.active {
  fill: #e0f2fe;
  font-weight: 700;
}
.line-no {
  fill: #475569;
  font-size: 8px;
  text-anchor: end;
}
.line-no.projected {
  font-size: 9px;
}
.code {
  fill: #e2e8f0;
  font-size: 9.5px;
}
.code.small {
  font-size: 7.4px;
}
.code.projected {
  font-size: 8.8px;
}
.code .kw {
  fill: #c4b5fd;
  font-weight: 700;
}
.code .type {
  fill: #67e8f9;
}
.code .comment {
  fill: #6ee7b7;
}
.console {
  fill: #cbd5e1;
  font-size: 8.5px;
}
.light-ide rect[fill="#0f172a"],
.light-ide rect[fill="#101827"] {
  fill: #ffffff;
}
.light-ide rect[fill="#20293a"] {
  fill: #e2e8f0;
}
.light-ide rect[fill="#111827"] {
  fill: #f1f5f9;
}
.light-ide rect[fill="#182235"] {
  fill: #e8eef5;
}
.light-ide rect[fill="#0b1220"] {
  fill: #f8fafc;
}
.light-ide rect[fill="#0e7490"] {
  fill: #bae6fd;
  opacity: 1;
}
.light-ide .window-title,
.light-ide .panel-label,
.light-ide .tree,
.light-ide .console {
  fill: #334155;
}
.light-ide .tree.muted {
  fill: #64748b;
}
.light-ide .tree.bright,
.light-ide .tab.active {
  fill: #075985;
}
.light-ide .line-no {
  fill: #94a3b8;
}
.light-ide .code {
  fill: #1e293b;
}
.light-ide .code .kw {
  fill: #7c3aed;
}
.light-ide .code .type {
  fill: #0369a1;
}
.light-ide .code .comment {
  fill: #047857;
}
.notes-title,
.notes-heading,
.note,
.empty-title,
.empty-subtitle,
.person,
.avatar,
.share-button,
.status-label,
.status-scene {
  font-family: Inter, ui-sans-serif, system-ui, sans-serif;
}
.notes-title {
  fill: #92400e;
  font-size: 9px;
  font-weight: 800;
}
.notes-heading {
  fill: #92400e;
  font-size: 8px;
  font-weight: 900;
  letter-spacing: 0.08em;
}
.note {
  fill: #78350f;
  font-size: 9px;
  font-weight: 650;
}
.note.hand {
  font-style: italic;
}
.empty-title {
  fill: #64748b;
  font-size: 14px;
  font-weight: 850;
  letter-spacing: 0.08em;
}
.empty-subtitle {
  fill: #94a3b8;
  font-size: 11px;
}
.avatar {
  fill: white;
  font-size: 12px;
  font-weight: 900;
}
.person {
  fill: #cbd5e1;
  font-size: 8px;
  font-weight: 700;
}
.person.group-name {
  font-size: 9px;
  font-weight: 800;
}
.share-button {
  fill: white;
  font-size: 8px;
  font-weight: 900;
  letter-spacing: 0.05em;
}
.status-label {
  fill: white;
  font-size: 11px;
  font-weight: 850;
  letter-spacing: 0.05em;
}
.status-scene {
  fill: #94a3b8;
  font-size: 9px;
  font-weight: 750;
  letter-spacing: 0.07em;
}
</style>
