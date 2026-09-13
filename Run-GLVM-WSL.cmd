@echo off
wsl.exe -d Ubuntu --cd "%~dp0." --exec bash scripts/run.sh %*
if errorlevel 1 pause
