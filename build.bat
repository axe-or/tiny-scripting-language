@echo off

start /b tcc -std=c11 -c kite.c -o build\kite.obj || goto :error
start /b tcc -std=c11 -c main.c -o build\main.obj || goto :error
tcc build\kite.obj build\main.obj -o build\kite.exe || goto :error
goto :EOF

:error
echo Failed to build.

