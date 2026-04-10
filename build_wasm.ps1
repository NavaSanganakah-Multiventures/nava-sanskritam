# Nava Sanskritam WASM Build Script
$buildDir = "build-wasm"
$outputDir = "wasm"

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
Write-Host "🚩 Updating Web Playground..." -ForegroundColor Yellow
cp nvc.js "../wasm/nvc.js"
cp nvc.wasm "../wasm/nvc.wasm"

# Copy to Nava Studio Folder
Write-Host "🚩 Updating Nava Studio..." -ForegroundColor Yellow
if (Test-Path "../nava-studio/src") {
    cp nvc.js "../nava-studio/src/nvc.js"
    cp nvc.wasm "../nava-studio/src/nvc.wasm"
}

cd ..
Write-Host "✅ Engine Update Complete! All platforms synced." -ForegroundColor Green
