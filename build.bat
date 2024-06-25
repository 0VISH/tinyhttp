@echo off

if not exist bin\ (
    mkdir bin\
)

cl /nologo src/main.c /Zi /Fo:bin/tinyhttp.obj /Fe:bin/tinyhttp.exe /Fd:bin/tinyhttp.pdb

if %errorlevel% equ 0 (
    bin\tinyhttp.exe
)