# Запуск GLVMEngine в Windows через WSL

**Для обычного запуска на этом компьютере используйте нативную Windows-версию:**
`Run-GLVM.cmd`. Она использует Radeon напрямую. Подробности — в [WINDOWS_RU.md](WINDOWS_RU.md).
Ниже описан альтернативный Linux/WSL-режим.

Проект установлен в `D:\Projects\GLMV_ENGINE`. Используется Ubuntu в WSL и WSLg.

Дважды нажмите **Run-GLVM-WSL.cmd** в корне проекта. При изменении исходников он
пересоберёт оптимизированную версию и откроет демосцену. Лог сборки находится
в `build-release/build.log`.

Управление: WASD — движение, пробел — прыжок, I — инвентарь, O — отображение
коллизий, Esc или крестик окна — выход. Нажатие мышью внутри окна включает
захват указателя.

Из PowerShell:

```powershell
.\Run-GLVM-WSL.cmd
.\Run-GLVM-WSL.cmd --frames 30
.\Run-GLVM-WSL.cmd --debug --frames 30
```

Из Ubuntu/WSL:

```bash
cd /mnt/d/Projects/GLMV_ENGINE
bash scripts/run.sh
make -f MakefileLin test
python3 tests/check_shaders.py
```

Вариант `--debug` включает AddressSanitizer, UndefinedBehaviorSanitizer и
Vulkan validation layer. `--frames N` завершает работу после N кадров.
Обычный запуск использует отдельную папку `build-release`, чтобы флаги
санитайзеров не смешивались с оптимизированной сборкой.

Для установки зависимостей на другом компьютере с Ubuntu:

```bash
bash scripts/install-ubuntu.sh
```

Нужны Clang с поддержкой C++20,
Python 3, Wayland, Vulkan, ALSA и X11/XCB. Локально проверена Ubuntu 26.04,
Clang 21, Mesa 26.0.8. WSLg предоставляет звук через PulseAudio; движок выбирает
его автоматически, если задан `PULSE_SERVER`.

На этом компьютере Vulkan внутри WSL обнаруживает **llvmpipe**, программный
рендерер CPU. Это ограничивает FPS; наличие видеокарты в Windows само по себе
не означает, что она доступна Vulkan внутри WSL. Нативная Windows-версия проверена
на Radeon RX 7900 GRE. X11/XCB и OpenGL-варианты не проверялись.

Исходные модели и сцена сохранены. Изменения локальные, в GitHub они не отправлялись.

## Единая сборка

Поддерживаемая точка входа — `make CONFIG=Debug` или `make CONFIG=Release`.
`MakefileLin` остаётся совместимым входом. Старые Makefile в `make_files/`
перенаправляют в корень; копировать их поверх корневых файлов не нужно.
CI проверяет чистую сборку Linux Debug/Release и Windows Release.
Новые регрессии запускаются вместе с остальными через `make test`.
