# Contributing to the native macOS experiment

Keep Wine changes in this repository. Changes to FEX, DXMT, a launcher, or provisioning services belong in their respective projects or separate integration repositories.

For each issue or pull request, include:

- macOS build, Xcode/SDK version, Apple chip, Wine commit and external component revisions.
- Reproduction steps using an isolated prefix and the relevant environment switches.
- Expected and actual behavior, plus focused logs with account details and personal paths removed.
- Validation of the fix and any known regressions.

Keep changes small enough to review. Preserve the relevant upstream copyright and LGPL notices. Do not add private signing material, provision profiles, Steam sessions, game files, save games, proprietary Apple runtime binaries, or complete machine logs.

Performance changes need repeated runs with the same scene, settings, camera and thermal conditions. Report CPU frame behavior and GPU timings separately. Upscaling, frame generation, changed quality settings, and CPU-worker changes must be disclosed. A faster smoke test is not evidence of faster gameplay.

For upstream contribution, follow Wine's own contribution process. This private branch is not an upstream submission, and no issue or patch is sent to external maintainers automatically.
