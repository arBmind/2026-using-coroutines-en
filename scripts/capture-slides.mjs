import { parseSync } from '@slidev/parser'
import { chromium } from 'playwright-chromium'
import { mkdir, readFile, readdir, unlink } from 'node:fs/promises'
import path from 'node:path'

function readArguments(argv) {
  const values = new Map()
  for (let index = 0; index < argv.length; index += 2) {
    const key = argv[index]
    const value = argv[index + 1]
    if (!key?.startsWith('--') || value == null)
      throw new Error(`Invalid argument near ${key ?? '<end>'}`)
    values.set(key.slice(2), value)
  }
  return values
}

const args = readArguments(process.argv.slice(2))
const baseUrl = args.get('base-url')
const outputDir = path.resolve(args.get('output') ?? 'screenshots')
const slidesPath = path.resolve(args.get('slides') ?? 'src/slides.md')

if (!baseUrl)
  throw new Error('Missing --base-url')

const markdown = await readFile(slidesPath, 'utf8')
const slideCount = parseSync(markdown, slidesPath).slides.length
const settleDelay = Number(process.env.SLIDEV_SCREENSHOT_DELAY ?? 900)

await mkdir(outputDir, { recursive: true })
for (const filename of await readdir(outputDir)) {
  if (/^slide-\d+\.png$/.test(filename) || filename === 'presenter.png')
    await unlink(path.join(outputDir, filename))
}

const browser = await chromium.launch({ headless: true })

try {
  const page = await browser.newPage({
    viewport: { width: 1280, height: 720 },
    deviceScaleFactor: 1,
  })

  for (let slide = 1; slide <= slideCount; slide++) {
    await page.goto(`${baseUrl}/${slide}?clicks=999`, {
      waitUntil: 'domcontentloaded',
      timeout: 15_000,
    })
    await page.waitForFunction(() => {
      const layout = [...document.querySelectorAll('.slidev-layout')]
        .find(element => element.getBoundingClientRect().width > 0)
      return layout && !document.body.textContent?.includes('Loading slide...')
    })
    await page.waitForTimeout(settleDelay)

    const filename = `slide-${String(slide).padStart(2, '0')}.png`
    await page.screenshot({ path: path.join(outputDir, filename) })
    process.stdout.write(`\rCaptured ${slide}/${slideCount}`)
  }

  await page.setViewportSize({ width: 1440, height: 900 })
  await page.goto(`${baseUrl}/presenter/1`, {
    waitUntil: 'domcontentloaded',
    timeout: 15_000,
  })
  await page.waitForSelector('.slidev-presenter')
  await page.waitForTimeout(settleDelay)
  await page.screenshot({ path: path.join(outputDir, 'presenter.png') })

  process.stdout.write(`\nSaved ${slideCount} slide screenshots and presenter.png to ${outputDir}\n`)
}
finally {
  await browser.close()
}
