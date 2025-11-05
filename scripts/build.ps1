param()

# Pure PowerShell build script for the project (replaces build.bat)
# - Looks for RAYLIB env var
# - Detects w64devkit if available and picks x86_64-w64-mingw32-gcc
# - Adds libexec dir (where cc1.exe lives) to PATH if found
# - Compiles main.c + src/player.c and links raylib

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$projectRoot = Resolve-Path (Join-Path $scriptDir "..")

if (-not $env:RAYLIB) {
    Write-Error "RAYLIB environment variable is not set. Please set RAYLIB to your raylib root (e.g. C:\\raylib)"
    exit 1
}

$ray = $env:RAYLIB

# Candidate w64devkit roots
$candidates = @()
if ($env:W64DEVKIT) { $candidates += $env:W64DEVKIT }
$candidates += Join-Path $ray 'w64devkit'
$candidates += 'C:\w64devkit'

$w64root = $null
foreach ($c in $candidates) {
    if (Test-Path $c) { $w64root = (Resolve-Path $c).Path; break }
}

# Find compiler
$compiler = $null
if ($w64root) {
    $candidate1 = Join-Path $w64root 'bin\\x86_64-w64-mingw32-gcc.exe'
    $candidate2 = Join-Path $w64root 'bin\\gcc.exe'
    if (Test-Path $candidate1) { $compiler = $candidate1 }
    elseif (Test-Path $candidate2) { $compiler = $candidate2 }
}

if (-not $compiler) {
    # fallback: check common paths under RAY and PATH
    $try1 = Join-Path $ray 'w64devkit\\bin\\x86_64-w64-mingw32-gcc.exe'
    $try2 = Join-Path $ray 'w64devkit\\bin\\gcc.exe'
    if (Test-Path $try1) { $compiler = $try1 }
    elseif (Test-Path $try2) { $compiler = $try2 }
    else {
        $which = Get-Command x86_64-w64-mingw32-gcc.exe -ErrorAction SilentlyContinue
        if ($which) { $compiler = $which.Source }
        else {
            $which2 = Get-Command gcc.exe -ErrorAction SilentlyContinue
            if ($which2) { $compiler = $which2.Source }
        }
    }
}

if (-not $compiler) {
    Write-Error "Could not find a suitable gcc compiler (w64devkit). Set W64DEVKIT or ensure x86_64-w64-mingw32-gcc.exe is on PATH."
    exit 1
}

Write-Host "Using compiler: $compiler"

# If w64root exists, try to find libexec/cc1 and add it to PATH
if ($w64root) {
    $cc1 = Get-ChildItem -Path $w64root -Filter cc1.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($cc1) {
        $cc1Dir = $cc1.DirectoryName
        Write-Host "Prepending cc1 dir to PATH: $cc1Dir"
        $env:PATH = "$cc1Dir;$w64root\bin;$env:PATH"
    } else {
        Write-Host "cc1 not found under $w64root, adding $w64root\bin to PATH"
        $env:PATH = "$w64root\bin;$env:PATH"
    }
}

# Paths
$main = Join-Path $projectRoot 'main.c'
$player = Join-Path $projectRoot 'src\\player.c'
$enemy = Join-Path $projectRoot 'src\\enemy.c'
$projectile = Join-Path $projectRoot 'src\\projectile.c'
$out = Join-Path $projectRoot 'main.exe'
$include = Join-Path $projectRoot 'include'

# Build args
$args = @($main, $player, $enemy, $projectile, '-o', $out, '-I', (Join-Path $ray 'raylib\\src'), '-I', $include, '-L', (Join-Path $ray 'raylib\\src'), '-lraylib', '-lopengl32', '-lgdi32', '-lwinmm', '-static-libgcc', '-static-libstdc++')

Write-Host "Compiling: $($args -join ' ')"

& $compiler @args
$code = $LASTEXITCODE

if ($code -ne 0) {
    Write-Error "Build FAILED (exit code $code)"
    exit $code
}

Write-Host "Build succeeded: $out"
exit 0
