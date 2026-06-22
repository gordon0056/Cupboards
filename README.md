# Cupboards

[![Build](https://github.com/gordon0056/Cupboards/actions/workflows/build.yml/badge.svg)](https://github.com/gordon0056/Cupboards/actions/workflows/build.yml)
[![Windows](https://img.shields.io/badge/Windows-0078D6?logo=windows&logoColor=white)](https://github.com/gordon0056/Cupboards/releases)
[![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)](https://github.com/gordon0056/Cupboards/releases)
[![macOS](https://img.shields.io/badge/macOS-000000?logo=apple&logoColor=white)](https://github.com/gordon0056/Cupboards/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-blue)](https://github.com/gordon0056/Cupboards/releases/latest)
[![codecov](https://codecov.io/gh/gordon0056/Cupboards/graph/badge.svg)](https://codecov.io/gh/gordon0056/Cupboards)

> **Download:** [![Windows](https://img.shields.io/badge/Windows-.zip-0078D6?logo=windows)](https://github.com/gordon0056/Cupboards/releases/latest)
> [![Linux](https://img.shields.io/badge/Linux-.tar.gz-FCC624?logo=linux)](https://github.com/gordon0056/Cupboards/releases/latest)
> [![macOS](https://img.shields.io/badge/macOS-.tar.gz-000000?logo=apple)](https://github.com/gordon0056/Cupboards/releases/latest)
> [![Windows](https://img.shields.io/badge/Windows-Installer-0078D6?logo=windows)](https://github.com/gordon0056/Cupboards/releases/latest)

Read in: [English](#english) | [Русский](#русский)

---

## English

Cupboards is a puzzle game where you move pieces across a graph to their target positions. Each piece can only move to adjacent nodes, and other pieces block the path, so you have to plan the order of moves.

Loosely inspired by sokoban and sliding puzzles, but the graph structure changes how you think about it.

### How to play

1. Download the latest release for your OS (links above)
2. Unzip and run `cupboards-game`
3. Click a piece to select it
4. Click a highlighted node to move it there
5. Get all pieces to their targets to complete the level

| Action | Input |
|---|---|
| Select a piece | Left-click |
| Move a piece | Left-click highlighted node |
| Undo | Ctrl+Z |
| Debug info | F1 |
| Back to menu | Menu button |

### Features

- 17 levels
- Undo support (Ctrl+Z)
- Background music and sound effects, adjustable in settings
- Russian and English UI
- Dark and light themes
- Built-in BFS/IDA* solver (used during development for level design)

### Build from source

**Windows:**

```powershell
cmake -B build .
cmake --build build --config Release --parallel
.\build\game\Release\cupboards-game.exe
```

**Ubuntu / WSL:**

```bash
sudo apt-get install libsfml-dev build-essential cmake libfreetype-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/game/cupboards-game
```

**macOS:**

```bash
brew install cmake freetype
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)
./build/game/cupboards-game
```

Dependencies are fetched automatically via CMake FetchContent. You only need CMake and a C++17 compiler.

---

## Русский

Cupboards — головоломка, в которой нужно расставить фишки по целевым позициям на графе. Фишки перемещаются по рёбрам, другие фишки перекрывают путь, так что порядок ходов имеет значение.

Что-то вроде сокобана или пятнашек, но на графе — это меняет логику решения.

### Как играть

1. Скачайте последнюю версию для вашей ОС (ссылки вверху)
2. Распакуйте и запустите `cupboards-game`
3. Кликните на фишку, чтобы выбрать её
4. Кликните на подсвеченный узел, чтобы переместить фишку
5. Расставьте все фишки по целям, чтобы пройти уровень

| Действие | Управление |
|---|---|
| Выбрать фишку | ЛКМ |
| Переместить фишку | ЛКМ на подсвеченный узел |
| Отменить ход | Ctrl+Z |
| Отладка | F1 |
| Вернуться в меню | Кнопка «Menu» |

### Возможности

- 17 уровней
- Отмена ходов (Ctrl+Z)
- Музыка и звуки, настраиваются в меню
- Русский и английский интерфейс
- Тёмная и светлая темы
- Встроенный BFS/IDA* солвер (использовался при разработке уровней)

### Сборка из исходников

**Windows:**

```powershell
cmake -B build .
cmake --build build --config Release --parallel
.\build\game\Release\cupboards-game.exe
```

**Ubuntu / WSL:**

```bash
sudo apt-get install libsfml-dev build-essential cmake libfreetype-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/game/cupboards-game
```

**macOS:**

```bash
brew install cmake freetype
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)
./build/game/cupboards-game
```

Зависимости скачиваются через CMake FetchContent. Нужны только CMake и компилятор с поддержкой C++17.

[⬆ Наверх](#cupboards)
