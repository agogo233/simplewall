@echo off

py -3 "%~dp0build_locale.py" --name-short=simplewall || python "%~dp0build_locale.py" --name-short=simplewall

pause
