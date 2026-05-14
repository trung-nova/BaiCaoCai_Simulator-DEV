@echo off
SETLOCAL EnableDelayedExpansion

:: --- CONFIGURATION ---
SET "BIN_CSV=game.exe"
SET "CC=g++"
SET "FLAGS=-std=c++17 -I include -I . -O3 -static"

echo ============================================================
echo   BAI CAO CAI SIMULATOR - BUILD SYSTEM
echo ============================================================

:: 1. Check Prerequisite
echo [1/3] Checking source files...
if not exist main.cpp ( echo [ERROR] main.cpp missing! && pause && exit /b 1 )
echo OK.

:: 2. Build Game Version
echo [2/3] Building Game Version (%BIN_CSV%)...
%CC% %FLAGS% -c main.cpp -o main.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/Card.cpp -o Card.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/ConcreteStates.cpp -o ConcreteStates.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/Deck.cpp -o Deck.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/GameManager.cpp -o GameManager.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/Player.cpp -o Player.o
if !ERRORLEVEL! NEQ 0 goto :FAIL

%CC% main.o Card.o ConcreteStates.o Deck.o GameManager.o Player.o -o %BIN_CSV% -static
if !ERRORLEVEL! NEQ 0 goto :FAIL
echo SUCCESS: %BIN_CSV% created.

:: 3. Cleanup
echo [3/3] Cleaning up temporary files...
del *.o
echo OK.

echo.
echo ============================================================
echo   BUILD COMPLETE!
echo   - %BIN_CSV% (CSV Mode)
echo ============================================================
echo.
echo [FINISH] Binary is ready.
pause
exit /b 0

:FAIL
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo   BUILD FAILED! Please check the errors above.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
