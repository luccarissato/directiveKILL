param()

Write-Host "Running workspace setup: verifying RAYLIB environment variable..."

$envUser = [Environment]::GetEnvironmentVariable('RAYLIB', 'User')
$envMachine = [Environment]::GetEnvironmentVariable('RAYLIB', 'Machine')
$envProcess = [Environment]::GetEnvironmentVariable('RAYLIB', 'Process')

if ($envProcess) { $found = $envProcess } elseif ($envUser) { $found = $envUser } elseif ($envMachine) { $found = $envMachine } else { $found = $null }

if ($found) {
    Write-Host "Found RAYLIB: $found"
    if (Test-Path (Join-Path $found 'src')) {
        Write-Host "Looks like a valid raylib root (has 'src' folder). No further action required."
        exit 0
    } else {
        Write-Host "The path $found does not contain 'src' subfolder. Please verify your RAYLIB variable."
        exit 1
    }
}

Write-Host "RAYLIB not set in environment. Creating a .env.example in workspace root to help you."
$examplePath = Join-Path -Path (Get-Location) -ChildPath ".env.example"
$example = @"
# Set RAYLIB to the root of your raylib installation. Example:
RAYLIB=C:\raylib

# Optionally set W64DEVKIT to the root of your w64devkit installation (if separate):
# W64DEVKIT=C:\w64devkit
"@

if (-not (Test-Path $examplePath)) {
    $example | Out-File -FilePath $examplePath -Encoding UTF8
    Write-Host "Created $examplePath. Update it with your real paths and optionally copy to .env."
} else {
    Write-Host ".env.example already exists at $examplePath. Please update it if needed."
}

Write-Host "After setting RAYLIB (user or system env var), restart VS Code so tasks pick it up." 
exit 0
