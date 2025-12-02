$ErrorActionPreference = "Stop"

# Create vendor directory if it doesn't exist
$vendorDir = Join-Path $PSScriptRoot "vendor"
if (-not (Test-Path $vendorDir)) {
    New-Item -ItemType Directory -Path $vendorDir | Out-Null
}

function Download-And-Extract ($url, $name, $destName) {
    Write-Host "Processing $name..."
    $zipPath = Join-Path $vendorDir "$name.zip"
    $extractPath = Join-Path $vendorDir "$name-temp"
    $finalPath = Join-Path $vendorDir $destName

    if (Test-Path $finalPath) {
        Write-Host "$name already exists at $finalPath. Skipping."
        return
    }

    # Download
    Write-Host "Downloading $url..."
    Invoke-WebRequest -Uri $url -OutFile $zipPath

    # Extract
    Write-Host "Extracting..."
    Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force

    # Move to final destination
    # The zip usually contains a root folder like "glfw-3.3.8", we need to find it
    $subFolder = Get-ChildItem -Path $extractPath | Select-Object -First 1
    if ($subFolder) {
        Move-Item -Path $subFolder.FullName -Destination $finalPath
    } else {
        Write-Error "Could not find extracted folder for $name"
    }

    # Cleanup
    Remove-Item -Path $zipPath -Force
    Remove-Item -Path $extractPath -Recurse -Force
    Write-Host "$name setup complete."
}

Download-And-Extract "https://github.com/glfw/glfw/archive/refs/tags/3.3.8.zip" "glfw" "glfw"
Download-And-Extract "https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.zip" "glm" "glm"
Download-And-Extract "https://github.com/nothings/stb/archive/refs/heads/master.zip" "stb" "stb"

# GLAD Setup (Downloading pre-generated files from LearnOpenGL repo)
Write-Host "Setting up GLAD..."
$gladDir = Join-Path $vendorDir "glad"
$gladIncDir = Join-Path $gladDir "include"
$gladSrcDir = Join-Path $gladDir "src"
$gladGladInc = Join-Path $gladIncDir "glad"
$gladKhrInc = Join-Path $gladIncDir "KHR"

New-Item -ItemType Directory -Path $gladSrcDir -Force | Out-Null
New-Item -ItemType Directory -Path $gladGladInc -Force | Out-Null
New-Item -ItemType Directory -Path $gladKhrInc -Force | Out-Null

function Download-File ($url, $dest) {
    Write-Host "Downloading $url to $dest..."
    Invoke-WebRequest -Uri $url -OutFile $dest
}

Download-File "https://cdn.jsdelivr.net/gh/JoeyDeVries/LearnOpenGL@master/src/glad.c" (Join-Path $gladSrcDir "glad.c")
Download-File "https://cdn.jsdelivr.net/gh/JoeyDeVries/LearnOpenGL@master/includes/glad/glad.h" (Join-Path $gladGladInc "glad.h")
Download-File "https://cdn.jsdelivr.net/gh/JoeyDeVries/LearnOpenGL@master/includes/KHR/khrplatform.h" (Join-Path $gladKhrInc "khrplatform.h")

Write-Host "All dependencies downloaded successfully."