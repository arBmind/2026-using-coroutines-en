# Repository guidance

## Slide verification

- Use the prepared npm scripts; do not launch Playwright with ad-hoc `node -e` commands.
- Build with `npm run build`.
- Capture the complete deck with `npm run screenshots`.
- Review selected click states with, for example:
  `npm run screenshots -- --range 12-14 --clicks 0,1,2,3,4`
- Screenshot output is written to `screenshots/`. Selected click states use names such as `slide-12-click-02.png`.
- Visually inspect every changed slide at 1280 × 720, including the initial state and every meaningful click state.

## Editing

- Preserve unrelated staged and unstaged user changes.
- Keep click states synchronized with presenter-note `[click]` markers.
- Code-highlight ranges should begin with `0` when the initial state should have no highlighted lines.
- Do not use a final `all` highlight unless highlighting the complete block communicates a distinct idea.
