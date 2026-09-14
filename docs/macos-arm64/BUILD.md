# Building this experimental branch

## Prerequisites

The successful local experiment used Xcode 27 beta on macOS 27 beta, ARM64 native build tools, and llvm-mingw `20260826` with ARM64EC support. Wine's normal build dependencies still apply; the local build used Homebrew Bison and GnuTLS. Install tools from their upstream sources rather than copying a development machine's binaries.

Select the appropriate Xcode with `DEVELOPER_DIR`. Keep `/usr/bin/clang` as the compiler for macOS objects; the cross compiler handles Windows PE objects. Make the llvm-mingw executables available in PATH along with a suitable Bison.

A starting component build, from an out-of-tree build directory:

```sh
export DEVELOPER_DIR=/Applications/Xcode-beta.app/Contents/Developer
# Add your installed llvm-mingw and Bison directories to PATH.
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
export CFLAGS='-O2 -g -arch arm64'
export CXXFLAGS='-O2 -g -arch arm64'
export LDFLAGS='-arch arm64'
/path/to/wine-arm64-macos/configure \
  --enable-archs=arm64ec,aarch64,i386,x86_64 \
  --without-x --without-gstreamer --without-vulkan --without-cups \
  --without-pulse --without-alsa \
  --prefix=/path/to/isolated/install
make -j6
```

This builds components. It does **not** produce the fully integrated, provisioned runtime used in the gameplay test. A clean-machine end-to-end recipe is still an open issue.

## Loader layout and signing

The current-SDK native test loader was linked from `loader/main.o` with:

```
-arch arm64
-Wl,-x86_64_layout_emulation,-no_fixup_chains,-no_huge,-image_base,0x170000000,-pagezero_size,0x170000000
```

The test app used an Apple-issued development provisioning profile authorizing `com.apple.developer.cross-architecture-support`. Each developer needs an appropriately entitled profile and matching signing identity/bundle ID. No credentials or profile from the development machine are included here. Restricted cross-architecture permissions were not obtained by merely ad-hoc signing; ordinary JIT permissions and the cross-architecture capability are different requirements.

References:
- [Apple cross-architecture compatibility documentation](https://developer.apple.com/documentation/bundleresources/entitlements/com.apple.developer.cross-architecture-support)
- Installed SDK headers `spawn.h`, `sys/mman.h`, `mach/mach_traps.h`, and `os/arch/arm64.h` describe the APIs used by the patch set.

## External runtime components

These are not bundled or automatically installed by this repository:

| Component | Tested base | Additional work required |
| --- | --- | --- |
| FEX | `5d6609a6b1182b8d972005befda8b7e338fd3644` | Darwin UnixLib adapter; per-thread hardware TSO setup in ARM64EC and WoW64 thread initialization; optional software-TSO diagnostic fallback. |
| DXMT | v0.80, `589adb780354b461645b29999cefaf533594ee99` | Native ARM64 UnixLib, ARM64EC PE build, Wine macdrv surface bridge, supporting build/atomic changes. |
| LLVM | 15.0.7 | Native ARM64 static libraries for DXMT shader translation. |

Upstreams: [FEX](https://github.com/FEX-Emu/FEX), [DXMT](https://github.com/3Shain/dxmt), [LLVM](https://github.com/llvm/llvm-project).

The external integration patches have not been included in this Wine-only repository. Until they are separately available, the full reported x86 gameplay setup is not reproducible from this repository alone. ARM64 Wine development and review of the Wine changes can proceed independently.

The working local layout supplied FEX's PE components as `xtajit64.dll` and `xtajit.dll`, paired with native UnixLibs through `WINEDLLPATH`, and external DXMT as native overrides for `d3d11,dxgi,d3d10core`. Exact installation and registration should be automated and independently tested before a binary release.

## Validation sources

Standalone diagnostic fixtures are in `tests/macos-arm64/`. They are experimental developer probes, not replacements for upstream Wine's test suites. Runtime tests must be run in the isolated provisioned environment; merely compiling the fixtures does not verify cross-architecture execution.
