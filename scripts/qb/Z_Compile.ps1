# Path to the tmp file
$tmpFile = ".\last_compiletime.tmp"

# Define source and target roots (absolute paths!)
$sourceRoot   = (Resolve-Path ".\source").Path
$compiledRoot = Join-Path (Split-Path $sourceRoot -Parent) "compiled"

# Ensure source root ends with a backslash
if ($sourceRoot[-1] -ne '\') {
    $sourceRoot += '\'
}

# Path to the compiler executable
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$compilerExe = Join-Path $scriptDir "Z_QScript.QCompile.App.exe"

# Determine reference time
if (-not (Test-Path $tmpFile) -or ((Get-Content $tmpFile -ErrorAction SilentlyContinue) -eq "")) {
    # No timestamp → compile everything
    $refTime = [datetime]::MinValue
    Write-Host "No timestamp found. Compiling all files..."
} else {
    $refLine = Get-Content $tmpFile -TotalCount 1
    $refTime = [datetime]::ParseExact($refLine, "yyyyMMddHHmmss", $null)
}

# Find all .q files under source that were modified after the reference time
$files = Get-ChildItem -Path $sourceRoot -Recurse -Filter "*.q" | Where-Object {
    $_.LastWriteTimeUtc -gt $refTime
}

# Store current UTC time for next run
$now = (Get-Date).ToUniversalTime().ToString("yyyyMMddHHmmss")

foreach ($f in $files) {
    $inputFile = $f.FullName

    # Strip source root from full path
    $relativePath = $inputFile.Substring($sourceRoot.Length)

    # Build output path under compiled folder
    $outputFile = Join-Path $compiledRoot $relativePath
    $outputFile = [System.IO.Path]::ChangeExtension($outputFile, ".qb")

    # Ensure output directory exists
    $outputDir = Split-Path $outputFile -Parent
    if (-not (Test-Path $outputDir)) {
        New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
    }

    # Build arguments
    $args = @("-input", $inputFile, "-output", $outputFile, "-target", "thug1")

    Write-Host "Compiling $inputFile -> $outputFile"
    & $compilerExe @args
}

# Update the tmp file with the current time
$now | Out-File $tmpFile -Encoding UTF8

Write-Host "Done. Processed $($files.Count) file(s)."