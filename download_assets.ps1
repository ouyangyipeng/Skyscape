$ErrorActionPreference = "Stop"

$assetsDir = Join-Path $PSScriptRoot "assets"
$texturesDir = Join-Path $assetsDir "textures"
$skyboxDir = Join-Path $texturesDir "skybox"

# Create directories
New-Item -ItemType Directory -Path $skyboxDir -Force | Out-Null

function Download-File ($url, $dest) {
    if (-not (Test-Path $dest)) {
        Write-Host "Downloading $url to $dest..."
        try {
            Invoke-WebRequest -Uri $url -OutFile $dest
        } catch {
            Write-Warning "Failed to download $url. Using a placeholder."
        }
    } else {
        Write-Host "$dest already exists."
    }
}

# Download Skybox (LearnOpenGL)
$baseUrl = "https://raw.githubusercontent.com/JoeyDeVries/LearnOpenGL/master/resources/textures/skybox"
$faces = @("right", "left", "top", "bottom", "front", "back")

foreach ($face in $faces) {
    Download-File "$baseUrl/$face.jpg" (Join-Path $skyboxDir "$face.jpg")
}

Write-Host "Assets download complete."
