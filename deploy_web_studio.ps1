# Nava Studio Web - Cloudflare Deployment Script
$projectDir = "nava-web"

Write-Host "🚩 Starting Deployment for Nava Studio (Web Edition)..." -ForegroundColor Gold

cd $projectDir

# 1. Verify Assets
if (-not (Test-Path "nvc.wasm")) {
    Write-Host "⚠️ Warning: nvc.wasm not found. Build WASM first!" -ForegroundColor Yellow
}

# 2. Deploy to Cloudflare
# Note: Project name 'nava-studio' is picked up from wrangler.json
Write-Host "🚩 Deploying to Cloudflare Workers with Assets (Project: nava-studio)..." -ForegroundColor Gold
npx wrangler deploy

cd ..
Write-Host "✅ Deployment Process Finished!" -ForegroundColor Green
