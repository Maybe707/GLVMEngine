@echo off
if not exist "%~dp0build-win\GLVMEngine.exe" (
    echo Windows executable is missing. See docs\WINDOWS_RU.md.
    pause
    exit /b 1
)
start "" /D "%~dp0build-win" "%~dp0build-win\GLVMEngine.exe" %*
