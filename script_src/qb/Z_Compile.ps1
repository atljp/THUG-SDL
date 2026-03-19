# Path to the tmp file
$tmpFile = ".\last_compiletime.tmp"

# Path to the compiler executable (im gleichen Ordner wie das Skript)
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$compilerExe = Join-Path $scriptDir "QScript.QCompile.App.exe"

# If the tmp file doesn't exist or is empty, create it with current UTC time
if (-not (Test-Path $tmpFile) -or ((Get-Content $tmpFile -ErrorAction SilentlyContinue) -eq "")) {
    (Get-Date).ToUniversalTime().ToString("yyyyMMddHHmmss") | Out-File $tmpFile
    return
}

# Read the first line as the reference timestamp
$refLine = Get-Content $tmpFile -TotalCount 1
$refTime = [datetime]::ParseExact($refLine, "yyyyMMddHHmmss", $null)

# Find all .q files recursively that were modified after the reference time
$files = Get-ChildItem -Path . -Recurse -Filter "*.q" | Where-Object {
    $_.LastWriteTimeUtc -gt $refTime
}

# Store current UTC time for next run
$now = (Get-Date).ToUniversalTime().ToString("yyyyMMddHHmmss")

# Process each file
foreach ($f in $files) {
    $inputFile = $f.FullName
    $outputFile = [System.IO.Path]::ChangeExtension($inputFile, ".qb")

    # Build arguments as an array, each part separate
    $args = @("-input", $inputFile, "-output", $outputFile, "-target", "thug1")

    # Call the compiler
    Write-Host "Compiling $inputFile -> $outputFile"
    & $compilerExe @args
}

# Update the tmp file with the current time for the next run
$now | Out-File $tmpFile -Encoding UTF8

Write-Host "Done. Processed $($files.Count) file(s)."
