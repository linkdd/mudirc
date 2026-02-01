param([switch]$Release)

$ROOTDIR = (Get-Location).Path
$BUILDDIR = Join-Path $ROOTDIR "__build__"
$DISTDIR  = Join-Path $ROOTDIR "dist"

$CC = if ($env:CC)  { $env:CC }  else { "clang" }

if ($env:MAKE) { $MAKE = $env:MAKE }
elseif ($IsWindows) { $MAKE = "mingw32-make" }
else { $MAKE = "make" }

$BUILD_TYPE = if ($Release) { "Release" } else { "Debug" }

$buildOut = Join-Path $BUILDDIR $BUILD_TYPE
New-Item -ItemType Directory -Force -Path $buildOut | Out-Null
Push-Location $buildOut

$GENERATOR = "MinGW Makefiles"

$cmakeArgs = @(
  "../..",
  "-G", $GENERATOR,
  "-DCMAKE_BUILD_TYPE=$BUILD_TYPE",
  "-DCMAKE_MAKE_PROGRAM=$MAKE",
  "-DCMAKE_C_COMPILER=$CC",
  "-DCMAKE_INSTALL_PREFIX=$([IO.Path]::Combine($DISTDIR,$BUILD_TYPE))"
)

& cmake @cmakeArgs
$exit = $LASTEXITCODE
Pop-Location
exit $exit
