from playwright.sync_api import sync_playwright

with sync_playwright() as p:
    browser = p.chromium.launch()
    page = browser.new_page(viewport={"width": 1280, "height": 3000})
    page.goto('http://localhost:8080/docs.html')
    # Wait for the font to load and render
    page.wait_for_timeout(2000)
    page.screenshot(path='docs_updated.png', full_page=True)
    browser.close()
