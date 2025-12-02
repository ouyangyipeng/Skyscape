Write-Host "Initializing Git repository..."
git init

Write-Host "Adding files..."
git add .

Write-Host "Committing..."
git commit -m "Initial commit"

Write-Host "Creating GitHub repository 'Skyscape' and pushing..."
# 使用单引号避免字符串转义问题
gh repo create Skyscape --public --source=. --remote=origin --push --description 'Skyscape - A Skybox Visualization Application'

Write-Host "Done! Repository is live at https://github.com/<your-username>/Skyscape"