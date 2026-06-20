# Release Process

## Prerequisites

- All changes committed and pushed to `master`
- Project builds successfully and all tests pass

## 1. Rebuild CPack packages

```bash
cmake --preset=default -DCPACK_GENERATOR="ZIP;TGZ"
cmake --build build --config Release
cpack --config build/CPackConfig.cmake -G ZIP -B packages
cpack --config build/CPackConfig.cmake -G TGZ -B packages
```

On Windows (NSIS installer, requires NSIS installed):

```bash
cpack --config build/CPackConfig.cmake -G NSIS -B packages
```

## 2. Push to GitHub

```bash
git push origin master
```

## 3. Create GitHub Release

```bash
gh release create v<VERSION> --title "Cupboards v<VERSION>" --notes "Release notes" packages/*
```

Or manually:

1. Go to https://github.com/gordon0056/Cupboards/releases/new
2. Choose tag `v<VERSION>` (or create new)
3. Upload packages from `packages/` directory
4. Publish release

## 4. Verify Russian text on Windows

Run the game on a Windows machine **without** DejaVu Sans font installed.
Russian text should fall back to Nunito (bundled) or Tahoma/Segoe UI (system).
Check that all screens (MainMenu, Settings, Game, Win) display Cyrillic correctly.

## Version numbering

Version is defined in `CMakeLists.txt`:

```cmake
project(Cupboards VERSION <major>.<minor>.<patch> LANGUAGES CXX)
```

Update before release.
