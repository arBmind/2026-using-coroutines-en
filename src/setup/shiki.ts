import { defineShikiSetup } from '@slidev/types'
import {
  transformerNotationDiff,
  transformerNotationFocus,
  transformerNotationHighlight,
  transformerNotationWordHighlight,
} from '@shikijs/transformers'

export default defineShikiSetup(() => {
  return {
    themes: {
      dark: 'github-dark-default',
      light: 'github-light',
    },
    langs: ['cpp', 'bash', 'json', 'markdown', 'ts', 'vue', 'yaml'],
    transformers: [
      transformerNotationDiff(),
      transformerNotationHighlight(),
      transformerNotationFocus(),
      transformerNotationWordHighlight(),
    ],
  }
})
