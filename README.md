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

Read in: [English](#-english) | [Русский](#-русский)

---

## English

Cupboards is a puzzle game where you slide pieces across a board to reach their target positions.
Think of it as a mix between Sokoban and a sliding puzzle — but on a graph.

No timers, no enemies — just your brain and 9 handcrafted levels.

### How to play

1. **Download** the latest version for your OS above
2. **Unzip** and run `cupboards-game`
3. **Click** a piece to select it
4. **Click** a highlighted node to move it there
5. Reach all target positions to **win**

| Action | Input |
|---|---|
| Select a piece | Left-click |
| Move a piece | Left-click highlighted node |
| Undo last move | Ctrl+Z |
| Toggle debug info | F1 |
| Back to menu | Menu button |

### Features

- **9 levels** — from easy warm-ups to brain-melters
- **Smooth animations** — pieces glide between nodes
- **Undo** — Ctrl+Z your way back
- **Background music & SFX** — adjustable in settings
- **Russian & English UI**
- **Dark theme** — easy on the eyes

### Build from source

> You only need this if you want to compile the game yourself.
> Otherwise, grab a ready-to-run build above.

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

All dependencies are downloaded automatically — you only need CMake and a compiler.

---

## Русский

Cupboards — это головоломка, в которой вы перемещаете фишки по игровому полю в нужные позиции.
Что-то среднее между «сокобаном» и пятнашками, только на графе.

Никаких таймеров и врагов — только ваш мозг и 9 уровней.

### Как играть

1. **Скачайте** последнюю версию для вашей ОС (ссылки вверху)
2. **Распакуйте** и запустите `cupboards-game`
3. **Кликните** на фишку, чтобы выбрать её
4. **Кликните** на подсвеченный узел, чтобы переместить фишку
5. Расставьте все фишки по целям, чтобы **победить**

| Действие | Управление |
|---|---|
| Выбрать фишку | ЛКМ |
| Переместить фишку | ЛКМ на подсвеченный узел |
| Отменить ход | Ctrl+Z |
| Отладка | F1 |
| Вернуться в меню | Кнопка «Menu» |

### Возможности

- **9 уровней** — от разминки до мозголомок
- **Плавная анимация** — фишки перекатываются между узлами
- **Отмена ходов** — Ctrl+Z
- **Музыка и звуки** — настраиваются в меню
- **Русский и английский интерфейс**
- **Тёмная тема** — не устают глаза

### Сборка из исходников

> Это нужно только если хотите скомпилировать игру самостоятельно.
> Иначе скачайте готовую сборку по ссылкам вверху.

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

Все зависимости скачиваются автоматически — нужны только CMake и компилятор.

[⬆ Наверх](#cupboards)
