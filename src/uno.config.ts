import { defineConfig } from "unocss";

// Design tokens are tuned for Slidev's fixed 1280 × 720 canvas.
export default defineConfig({
  theme: {
    // Slidev exposes these as fontFamily, while Wind4 reads font.
    // Keep both the slide body and utility-marked labels on the deck fonts.
    font: {
      sans: '"Inter", ui-sans-serif, system-ui, sans-serif',
      mono: '"JetBrains Mono", ui-monospace, SFMono-Regular, Consolas, monospace',
    },
    colors: {
      ink: "#111827",
      muted: "#5d6675",
      paper: "#ffffff",
      soft: "#f2f5f8",
      line: "#d7dde5",
      "brand-blue": "#075985",
      "brand-cyan": "#0891b2",
      "brand-orange": "#ea580c",
      "brand-green": "#0f766e",
    },
    spacing: {
      footer: "42px",
      "slide-x": "3.7rem",
      "slide-top": "3.1rem",
      "slide-bottom": "calc(2.6rem + 42px)",
    },
    text: {
      "slide-title": { fontSize: "2.35rem", lineHeight: "1.05" },
      "section-title": { fontSize: "4rem", lineHeight: "1" },
      hero: { fontSize: "4.35rem", lineHeight: "0.94" },
    },
    shadow: {
      "slide-card": "0 10px 25px rgb(15 23 42 / 0.09)",
    },
  },
});
