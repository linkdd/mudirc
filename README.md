# MUD IRC Bot

## Build

Requirements:

 - CMake 3.22+
 - Git (used by CMake's `FetchContent`)
 - GNU Make (CMake generates *MinGW Makefiles*)
 - clang 21+

First, fetch submodules with:

```bash
git submodule update --init --recursive
```

Then, run:

 - `make` or `make debug`, to build in *debug* mode
 - `make release`, to build in *release* mode

> :warning: **NB:** On Windows, please use **PowerShell**.

## Run

Once built, the executable is available in the folder:

 - `dist/Debug/bin/` for the *debug* build
 - `dist/Release/bin/` for the *releae* build

## Debug

On Linux, you can use `gdb`, but on Windows the recommended debugger is
[WinDbg](https://learn.microsoft.com/fr-fr/windows-hardware/drivers/debugger/).
