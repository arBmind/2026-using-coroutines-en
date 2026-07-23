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

function parseSlideRange(value) {
  if (!value)
    return Array.from({ length: slideCount }, (_, index) => index + 1)

  const selected = new Set()
  for (const part of value.split(',')) {
    const match = part.match(/^(\d+)(?:-(\d+))?$/)
    if (!match)
      throw new Error(`Invalid slide range: ${part}`)

    const start = Number(match[1])
    const end = Number(match[2] ?? match[1])
    if (start < 1 || end > slideCount || start > end)
      throw new Error(`Slide range outside 1-${slideCount}: ${part}`)

    for (let slide = start; slide <= end; slide++)
      selected.add(slide)
  }
  return [...selected].sort((left, right) => left - right)
}

function parseClicks(value) {
  if (!value)
    return [999]

  return value.split(',').map((part) => {
    if (!/^\d+$/.test(part))
      throw new Error(`Invalid click value: ${part}`)
    return Number(part)
  })
}

const selectedSlides = parseSlideRange(args.get('range'))
const selectedClicks = parseClicks(args.get('clicks'))
const explicitClickStates = args.has('clicks')
const capturePresenter = !args.has('range') && !explicitClickStates

function screenshotFilename(slide, clicks) {
  const clickSuffix = explicitClickStates
    ? `-click-${String(clicks).padStart(2, '0')}`
    : ''
  return `slide-${String(slide).padStart(2, '0')}${clickSuffix}.png`
}

await mkdir(outputDir, { recursive: true })
const targetFilenames = new Set(
  selectedSlides.flatMap(slide => selectedClicks.map(clicks => screenshotFilename(slide, clicks))),
)
for (const filename of await readdir(outputDir)) {
  if (targetFilenames.has(filename) || (capturePresenter && filename === 'presenter.png'))
    await unlink(path.join(outputDir, filename))
}

const browser = await chromium.launch({ headless: true })

try {
  const page = await browser.newPage({
    viewport: { width: 1280, height: 720 },
    deviceScaleFactor: 1,
  })

  const captureCount = selectedSlides.length * selectedClicks.length
  let captured = 0
  for (const slide of selectedSlides) {
    for (const clicks of selectedClicks) {
      await page.goto(`${baseUrl}/${slide}?clicks=${clicks}`, {
        waitUntil: 'domcontentloaded',
        timeout: 15_000,
      })
      await page.waitForFunction(() => {
        const layout = [...document.querySelectorAll('.slidev-layout')]
          .find(element => element.getBoundingClientRect().width > 0)
        return layout && !document.body.textContent?.includes('Loading slide...')
      })
      await page.waitForTimeout(settleDelay)

      const filename = screenshotFilename(slide, clicks)
      await page.screenshot({ path: path.join(outputDir, filename) })
      captured += 1
      process.stdout.write(`\rCaptured ${captured}/${captureCount}`)
    }
  }

  if (capturePresenter) {
    await page.setViewportSize({ width: 1440, height: 900 })
    await page.goto(`${baseUrl}/presenter/1`, {
      waitUntil: 'domcontentloaded',
      timeout: 15_000,
    })
    await page.waitForSelector('.slidev-presenter')
    await page.waitForTimeout(settleDelay)
    await page.screenshot({ path: path.join(outputDir, 'presenter.png') })
  }

  const presenterMessage = capturePresenter ? ' and presenter.png' : ''
  process.stdout.write(`\nSaved ${captureCount} slide screenshots${presenterMessage} to ${outputDir}\n`)
}
finally {
  await browser.close()
}
