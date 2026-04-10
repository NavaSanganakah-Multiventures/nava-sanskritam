# Nava Platform - Cloudflare Deployment Script
$projectDir = "nava-platform"

Write-Host "🚩 Starting Deployment for Nava Platform..." -ForegroundColor Gold

# 1. Build the Next.js App
Write-Host "🚩 Building Next.js Output..." -ForegroundColor Gold
cd $projectDir
npm install
npm run build

# 2. Verify WASM Assets
if (-not (Test-Path "public/nvc.wasm")) {
    Write-Host "⚠️ Warning: nvc.wasm not found in public directory. Build WASM first!" -ForegroundColor Yellow
}

# 3. Deploy to Cloudflare
Write-Host "🚩 Deploying to Cloudflare Workers with Assets..." -ForegroundColor Gold
npx wrangler deploy

cd ..
Write-Host "✅ Deployment Process Finished!" -ForegroundColor Green
