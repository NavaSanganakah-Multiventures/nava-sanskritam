# Nava Sanskritam WASM Build Script
$buildDir = "build-wasm"
$outputDir = "nava-web"

if (-not (Test-Path $buildDir)) {
    mkdir $buildDir
}

cd $buildDir

# Configure with Emscripten
Write-Host "🚩 Configuring CMake for WebAssembly..." -ForegroundColor Gold
emcmake cmake .. -DEMSCRIPTEN=ON -DCMAKE_BUILD_TYPE=Release

# Build
Write-Host "🚩 Compiling NVC to WebAssembly..." -ForegroundColor Gold
emmake make -j4

# Copy to Web Folder
Write-Host "🚩 Bundling Assets..." -ForegroundColor Gold
cp nvc.js "../$outputDir/nvc.js"
cp nvc.wasm "../$outputDir/nvc.wasm"

cd ..
Write-Host "✅ WASM Build Complete! Open $outputDir/index.html to test." -ForegroundColor Green
