import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";

import {
  presetAttributify,
  presetIcons,
  presetTypography,
  presetWind4,
} from "unocss";
import { defineConfig } from "vite";

const slidevLogo = fileURLToPath(
  new URL("../node_modules/@slidev/client/assets/logo.svg", import.meta.url),
);

export default defineConfig({
  slidev: {
    // Slidev currently defaults to Wind3. Supplying presets here replaces that
    // array, while retaining Slidev's shortcuts, variants, and transformers.
    unocss: {
      presets: [
        presetWind4({ preflights: { reset: false } }),
        presetAttributify(),
        presetTypography(),
        presetIcons({
          collections: {
            slidev: {
              logo: () => readFileSync(slidevLogo, "utf8"),
            },
          },
        }),
      ],
    },
  },
});
