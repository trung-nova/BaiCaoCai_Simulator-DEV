@echo off
SETLOCAL EnableDelayedExpansion

SET "FLAGS=-std=c++17 -I include -I . -O3 -static"
SET "CC=g++"

echo ============================================================
echo   BAI CAO CAI SIMULATOR - TEST BUILD SYSTEM
echo ============================================================

:: 1. Compile Core Objects (if not existing)
echo [1/3] Compiling core objects...
%CC% %FLAGS% -c src/Card.cpp -o Card.o
%CC% %FLAGS% -c src/Player.cpp -o Player.o
%CC% %FLAGS% -c src/Deck.cpp -o Deck.o
%CC% %FLAGS% -c src/GameManager.cpp -o GameManager.o
%CC% %FLAGS% -c src/ConcreteStates.cpp -o ConcreteStates.o
if !ERRORLEVEL! NEQ 0 goto :FAIL

:: 2. Build Tests
echo [2/3] Building Tests...
%CC% %FLAGS% tests/test_logic.cpp Card.o Player.o GameManager.o Deck.o ConcreteStates.o -o test_logic.exe
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% tests/test_trading.cpp Card.o Player.o GameManager.o Deck.o ConcreteStates.o -o test_trading.exe
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% tests/test_gamemanager.cpp Card.o Player.o GameManager.o Deck.o ConcreteStates.o -o test_gamemanager.exe
if !ERRORLEVEL! NEQ 0 goto :FAIL

:: 3. Run Tests
echo [3/3] Running tests...
echo.
test_logic.exe
if !ERRORLEVEL! NEQ 0 (
    echo.
    echo [ERROR] Logic Tests FAILED!
) else (
    echo.
    echo [SUCCESS] Logic Tests PASSED!
)

echo.
test_trading.exe
if !ERRORLEVEL! NEQ 0 (
    echo.
    echo [ERROR] Trading Tests FAILED!
) else (
    echo.
    echo [SUCCESS] Trading Tests PASSED!
)

echo.
test_gamemanager.exe
if !ERRORLEVEL! NEQ 0 (
    echo.
    echo [ERROR] GameManager Tests FAILED!
) else (
    echo.
    echo [SUCCESS] GameManager Tests PASSED!
)

:: Run Python Integration Test if python is available
echo.
echo Checking for Python Integration Test...
python --version >nul 2>&1
if !ERRORLEVEL! EQU 0 (
    echo Running System Test Suite...
    python tests/system_test_suite.py
) else (
    echo [SKIP] Python not found. Skipping system integration tests.
)

echo.
echo [DONE] Test cycle finished.
pause
exit /b 0

:FAIL
echo.
echo [ERROR] Build failed.
pause
exit /b 1
