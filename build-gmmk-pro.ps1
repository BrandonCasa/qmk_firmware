<#
.SYNOPSIS
Build the fork's BrandonCasa firmware for the GMMK Pro Rev1 ANSI.
.EXAMPLE
.\build-gmmk-pro.ps1
.EXAMPLE
.\build-gmmk-pro.ps1 -Clean
.EXAMPLE
.\build-gmmk-pro.ps1 -Doctor
#>
[CmdletBinding()]
param(
    [switch]$Clean,
    [switch]$Doctor,
    [ValidateRange(1, 64)]
    [int]$Jobs = 8,
    [string]$QmkMsysRoot = (Join-Path $env:LOCALAPPDATA 'Programs\QMK_MSYS')
)

$ErrorActionPreference = 'Stop'
$qmkBash = Join-Path $QmkMsysRoot 'usr\bin\bash.exe'
if (-not (Test-Path -LiteralPath $qmkBash)) {
    throw "QMK MSYS was not found at $QmkMsysRoot. Specify its folder with -QmkMsysRoot."
}

$qmkArgs = @('compile', '-kb', 'gmmk/pro/rev1/ansi', '-km', 'BrandonCasa', '-j', "$Jobs")
if ($Doctor) {
    $qmkArgs = @('doctor', '-n')
} elseif ($Clean) {
    $qmkArgs += '--clean'
}

$qmkPreviousEnvironment = @{}
foreach ($qmkName in @('MSYSTEM', 'CHERE_INVOKING', 'MSYS2_PATH_TYPE')) {
    $qmkPreviousEnvironment[$qmkName] = [Environment]::GetEnvironmentVariable($qmkName, 'Process')
}

try {
    $env:MSYSTEM = 'MINGW64'
    $env:CHERE_INVOKING = '1'
    $env:MSYS2_PATH_TYPE = 'minimal'
    $qmkRepoPath = $PSScriptRoot.Replace('\', '/')
    & $qmkBash --login -c 'cd -- "$1" && shift && exec qmk "$@"' qmk-build $qmkRepoPath @qmkArgs
    $qmkExitCode = $LASTEXITCODE
} finally {
    foreach ($qmkName in $qmkPreviousEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($qmkName, $qmkPreviousEnvironment[$qmkName], 'Process')
    }
}

exit $qmkExitCode
