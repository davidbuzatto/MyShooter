# Custom build script for webassembly (windows powershell).
#
# Usage:
#    .\buildwasm.ps1: clean, compile and run
#    .\buildwasm.ps1 -clean: clean compiled file
#    .\buildwasm.ps1 -cleanAndCompile: clean compiled file and compile the project
#    .\buildwasm.ps1 -compile: compile the project
#    .\buildwasm.ps1 -compileAndRun: compile the project and run the compiled file
#    .\buildwasm.ps1 -run: run the compiled file
#
# Author: Prof. Dr. David Buzatto

param(
    [switch]$clean,
    [switch]$cleanAndCompile,
    [switch]$compile,
    [switch]$compileAndRun,
    [switch]$run
);

$CurrentFolderName = Split-Path -Path (Get-Location) -Leaf
$CompiledFile = "$CurrentFolderName"
$BuildDir = "build"
$ServerDestination = "C:\xampp\htdocs\$CompiledFile"

$all = $false
if ( -not( $clean -or $cleanAndCompile -or $compile -or $compileAndRun -or $run ) ) {
    $all = $true
}

# clean
if ( $clean -or $cleanAndCompile -or $all ) {
    Write-Host "Cleaning..."
    if ( Test-Path $BuildDir ) {
        Remove-Item $BuildDir -Recurse -Force
    }
    if ( Test-Path $ServerDestination ) {
        Remove-Item $ServerDestination -Recurse -Force
    }
}

# compile
#--preload-file ./resources `
if ( $compile -or $cleanAndCompile -or $compileAndRun -or $all ) {
    Write-Host "Compiling..."
    New-Item -Path ".\$BuildDir" -ItemType Directory > $null
    emcc -o "./$BuildDir/$CompiledFile.html" `
         ./src/Block.c `
         ./src/Bullet.c `
         ./src/Enemy.c `
         ./src/GameWindow.c `
         ./src/GameWorld.c `
         ./src/main.c `
         ./src/Player.c `
         ./src/PowerUp.c `
         ./src/ResourceManager.c `
         ./src/utils.c `
         -Wall `
         -std=c99 `
         -D_DEFAULT_SOURCE `
         -Wno-missing-braces `
         -Wunused-result `
         -Os `
         -I. -I./include -I./src/include/ -I./src/include/raylib `
         -L. -L./lib/wasm/ `
         -s USE_GLFW=3 `
         -s ASYNCIFY `
         -s TOTAL_MEMORY=67108864 `
         -s FORCE_FILESYSTEM=1 `
         --preload-file ./resources `
         --shell-file ./src/wasm/minshell.html ./lib/wasm/libraylib.a `
         -DPLATFORM_WEB `
         -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' `
         -s EXPORTED_RUNTIME_METHODS=ccall
    Copy-Item -Path "$BuildDir" -Destination "$ServerDestination" -Recurse -Force
}

# run
if ( $run -or $compileAndRun -or $all ) {
    Write-Host "Running..."
    if ( Test-Path "$ServerDestination\$CompiledFile.html" ) {
        Start-Process "http://localhost/$CompiledFile/$CompiledFile.html"
    } else {
        Write-Host "$CompiledFile does not exists!"
    }
}