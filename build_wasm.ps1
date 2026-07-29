# Nava Sanskritam WASM Build Script
$buildDir = "build-wasm"
$outputDir = "nava-web"

if (-not (Test-Path $buildDir)) {
    mkdir $buildDir
}

cd $buildDir

# Configure with Emscripten
Write-Host "🚩 Configuring CMake for WebAssembly..." -ForegroundColor Yellow
emcmake cmake .. -DEMSCRIPTEN=ON -DCMAKE_BUILD_TYPE=Release

# Build
Write-Host "🚩 Compiling NVC to WebAssembly..." -ForegroundColor Yellow
emmake make -j4

# Copy to Web Playground Folder
Write-Host "🚩 Updating Web Playground (nava-web)..." -ForegroundColor Yellow
cp nvc.js "../nava-web/nvc.js"
cp nvc.wasm "../nava-web/nvc.wasm"

# Copy to Nava Studio Folder
Write-Host "🚩 Updating Nava Studio..." -ForegroundColor Yellow
if (Test-Path "../nava-studio/src") {
    cp nvc.js "../nava-studio/src/nvc.js"
    cp nvc.wasm "../nava-studio/src/nvc.wasm"
}

cd ..
Write-Host "✅ Engine Update Complete! All platforms synced." -ForegroundColor Green
