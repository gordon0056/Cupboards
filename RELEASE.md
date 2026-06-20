# Release Process

## Prerequisites

- All changes committed and pushed to `master`
- CI build passing, all tests green

## 1. Build packages

```bash
cmake --preset=default -DCPACK_GENERATOR="ZIP;TGZ"
cmake --build build --config Release
cpack --config build/CPackConfig.cmake -G ZIP -B packages
cpack --config build/CPackConfig.cmake -G TGZ -B packages
```

Windows installer (requires NSIS):

```bash
cpack --config build/CPackConfig.cmake -G NSIS -B packages
```

## 2. Push and tag

```bash
git push origin master
```

## 3. Create release on GitHub

```bash
gh release create v<VERSION> --title "Cupboards v<VERSION>" --notes "..." packages/*
```

Or manually through the GitHub UI: go to Releases, create a new tag, upload everything from `packages/`.

## 4. Smoke test on Windows

Run the game on a Windows machine without DejaVu Sans installed and check that Cyrillic renders correctly on all screens (main menu, settings, game, win screen). The fallback chain goes Nunito -> Tahoma/Segoe UI depending on what's available.

## Version

Defined in `CMakeLists.txt`:

```cmake
project(Cupboards VERSION <major>.<minor>.<patch> LANGUAGES CXX)
```

Bump before release.
