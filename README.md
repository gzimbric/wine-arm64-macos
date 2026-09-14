# Wine — experimental native macOS ARM64 branch

A private development branch of Wine for native Apple Silicon host processes and cross-architecture Windows execution. This repository contains Wine source and seven experimental Darwin patches. It contains no launcher or prebuilt runtime.

**Start here: [macOS ARM64 status and integration notes](docs/macos-arm64/README.md).**

- [Build requirements and remaining integration steps](docs/macos-arm64/BUILD.md)
- [Contribution guidance](docs/macos-arm64/CONTRIBUTING.md)
- [Original upstream Wine README](docs/macos-arm64/UPSTREAM-README.md)
- [Wine license](COPYING.LIB)

Based on upstream Wine `2550c238151a00e43908561f32b6e131603ec6f7`. Native ARM64 and external FEX/DXMT gameplay have been exercised on one development machine, but this is not a complete portable binary release. External integration and production-quality JIT memory handling remain open work.
