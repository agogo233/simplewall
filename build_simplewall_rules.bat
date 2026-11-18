@echo off

cd ..\builder
call build_simplewall_rules update %~dp0bin\profile_internal.xml %~dp0bin\profile_internal.xml

rem strip LZNT1 terminator appended by builder (RtlDecompressBuffer rejects it, profile hash check fails at startup)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0strip_lznt1_terminator.ps1"

pause
