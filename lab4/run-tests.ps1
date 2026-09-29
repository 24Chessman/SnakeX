param([ValidateSet('baseline', 'seam', 'all')][string]$Suite = 'all')
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$buildRoot = Join-Path $PSScriptRoot 'build'

function Invoke-Checked([string]$program, [string[]]$arguments) {
    & $program @arguments
    if ($LASTEXITCODE -ne 0) { throw "$program failed with exit code $LASTEXITCODE" }
}

function Run-Suite([string]$name) {
    $targetDir = Join-Path $buildRoot $name
    New-Item -ItemType Directory -Force $targetDir | Out-Null
    Get-ChildItem -LiteralPath $targetDir -Filter '*.gcda' | Remove-Item
    if ($name -eq 'baseline') {
        $baseSource = Join-Path $targetDir 'base-game.cpp'
        $lines = @(git -c "safe.directory=$($repoRoot.Replace('\', '/'))" -C $repoRoot show 'lab4-base:code/game.cpp')
        if ($LASTEXITCODE -ne 0) { throw 'Cannot read lab4-base:code/game.cpp' }
        [IO.File]::WriteAllLines($baseSource, [string[]]$lines, (New-Object Text.UTF8Encoding($false)))
    }
    Push-Location $targetDir
    try {
        $testFile = Join-Path $PSScriptRoot "tests/$name.cpp"
        Invoke-Checked 'g++' @('-std=c++17', '--coverage', '-O0', '-g', '-Wall', '-Wextra', '-I.', '-I', (Join-Path $PSScriptRoot 'tests'), '-c', $testFile, '-o', 'tests.o')
        Invoke-Checked 'g++' @('--coverage', 'tests.o', '-o', 'tests.exe')
        & ./tests.exe 2>&1 | Tee-Object 'test-results.txt'
        if ($LASTEXITCODE -ne 0) { throw "$name tests failed" }
        & gcov -b -c tests.gcno 2>&1 | Tee-Object 'coverage-summary.txt'
        if ($LASTEXITCODE -ne 0) { throw "$name coverage failed" }
    } finally { Pop-Location }
}

if ($Suite -eq 'all') { Run-Suite 'baseline'; Run-Suite 'seam' }
else { Run-Suite $Suite }
