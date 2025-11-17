param()

# Script de build em PowerShell para o projeto (substitui build.bat)
# - Procura pela variável de ambiente RAYLIB
# - Detecta w64devkit se disponível e usa x86_64-w64-mingw32-gcc
# - Adiciona o diretório libexec (onde cc1.exe pode estar) ao PATH se encontrado
# - Compila main.c + src/player.c e linka com raylib

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$projectRoot = Resolve-Path (Join-Path $scriptDir "..")

if (-not $env:RAYLIB) {
    Write-Error "RAYLIB environment variable is not set. Please set RAYLIB to your raylib root (e.g. C:\\raylib)"
    exit 1
}

$ray = $env:RAYLIB

# Candidatos a raízes do w64devkit
$candidates = @()
if ($env:W64DEVKIT) { $candidates += $env:W64DEVKIT }
$candidates += Join-Path $ray 'w64devkit'
$candidates += 'C:\w64devkit'

$w64root = $null
foreach ($c in $candidates) {
    if (Test-Path $c) { $w64root = (Resolve-Path $c).Path; break }
}

# Localiza o compilador
$compiler = $null
if ($w64root) {
    $candidate1 = Join-Path $w64root 'bin\\x86_64-w64-mingw32-gcc.exe'
    $candidate2 = Join-Path $w64root 'bin\\gcc.exe'
    if (Test-Path $candidate1) { $compiler = $candidate1 }
    elseif (Test-Path $candidate2) { $compiler = $candidate2 }
}

if (-not $compiler) {
    # fallback: verifica caminhos comuns sob RAY e PATH
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

# Se w64root existir, tenta encontrar libexec/cc1 e adicioná-lo ao PATH
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

# Caminhos
$main = Join-Path $projectRoot 'main.c'
$player = Join-Path $projectRoot 'src\\player.c'
$enemy = Join-Path $projectRoot 'src\\enemy.c'
$projectile = Join-Path $projectRoot 'src\\projectile.c'
$game = Join-Path $projectRoot 'src\\game.c'
$out = Join-Path $projectRoot 'main.exe'
$include = Join-Path $projectRoot 'include'
$gui = Join-Path $projectRoot 'src\\gui.c'

# Argumentos de compilação
$args = @($main, $player, $enemy, $projectile, '-o', $out, '-I', (Join-Path $ray 'raylib\\src'), '-I', $include, '-L', (Join-Path $ray 'raylib\\src'), '-lraylib', '-lopengl32', '-lgdi32', '-lwinmm', '-static-libgcc', '-static-libstdc++')
 # monta as fontes e adiciona gui.c se presente
 $sources = @($main, $player, $enemy, $projectile)
if (Test-Path $game) { $sources += $game }
if (Test-Path $gui) { $sources += $gui }

$args = $sources + @('-o', $out, '-I', (Join-Path $ray 'raylib\\src'), '-I', $include, '-L', (Join-Path $ray 'raylib\\src'), '-lraylib', '-lopengl32', '-lgdi32', '-lwinmm', '-static-libgcc', '-static-libstdc++')

Write-Host "Compiling: $($args -join ' ')"

# Se o executável já existe, tenta removê-lo primeiro para evitar "Permission denied" do linker
# Se um processo `main` estiver em execução, tenta finalizá-lo para que o linker possa escrever o output
$running = Get-Process -Name main -ErrorAction SilentlyContinue
if ($running) {
    Write-Host "Found running 'main' process(es). Attempting to stop them..."
    foreach ($p in $running) {
        try {
            Stop-Process -Id $p.Id -Force -ErrorAction Stop
            Write-Host "Stopped process Id $($p.Id)"
        } catch {
            Write-Host "Could not stop process Id $($p.Id): $($_.Exception.Message)"
        }
    }
}

# Tenta remover o arquivo de saída existente se presente
if (Test-Path $out) {
    try {
        Remove-Item $out -Force -ErrorAction Stop
        Write-Host "Removed existing output: $out"
    } catch {
        Write-Error "Cannot remove existing output $out. Ensure the program is not running and you have permission to delete it. Close the running executable and try again."
        exit 1
    }
}

& $compiler @args
$code = $LASTEXITCODE

if ($code -ne 0) {
    Write-Error "Build FAILED (exit code $code)"
    exit $code
}

Write-Host "Build succeeded: $out"
exit 0
