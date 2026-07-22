# How to Use It
# Open PowerShell in your project root folder and load the script:
# . .\Invoke-EmBuild.ps1
# Usage Examples:
# 
#  Just build the project:
# 
#    Invoke-EmBuild
# 
#  Build and launch immediately in your browser:
# 
#    Invoke-EmBuild -Run
# 
#  Delete old build cache, recompile from scratch, and run:
# 
#    Invoke-EmBuild -Clean -Run
#
# Troubleshooting
# Make sure you have run source ./emsdk_env.ps1 (or emsdk_env.bat) in your PowerShell session beforehand so emcmake, emmake, and emrun are accessible in your environment path!
#
function Invoke-EmBuild {
    [CmdletBinding()]
    param(
        # Set this switch if you want to automatically serve/run the game after building
        [switch]$Run,

        # Set this switch if you want to clean the build directory before building
        [switch]$Clean,

        # Optional port for emrun (default: 8080)
        [int]$Port = 8080
    )

    # 1. Handle clean build if requested
    if ($Clean -and (Test-Path -Path "build")) {
        Write-Host "Cleaning build directory..." -ForegroundColor Yellow
        Remove-Item -Path "build" -Recurse -Force
    }

    # 2. Ensure build directory exists
    if (-not (Test-Path -Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }

    # 3. Enter build directory
    Push-Location "build"

    try {
        # 4. Configure with emcmake
        Write-Host "Configuring CMake with Emscripten..." -ForegroundColor Cyan
        emcmake cmake ../client
        if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed." }

        # 5. Build with emmake
        Write-Host "Building project..." -ForegroundColor Cyan
        emmake cmake --build .
        if ($LASTEXITCODE -ne 0) { throw "Build failed." }

        Write-Host "Build successful!" -ForegroundColor Green

        # 6. Run if requested
        if ($Run) {
            # Recursively find the first .html file generated within the build directory
            $htmlFile = Get-ChildItem -Filter "*.html" -Recurse | Select-Object -First 1
            if ($htmlFile) {
                Write-Host "Launching server for $($htmlFile.FullName)..." -ForegroundColor Cyan
                emrun --port $Port $htmlFile.FullName
            } else {
                Write-Warning "Build finished, but no .html output file was found to run."
            }
        }
    }
    catch {
        Write-Error $_
    }
    finally {
        # Always return back to the root project directory
        Pop-Location
    }
}