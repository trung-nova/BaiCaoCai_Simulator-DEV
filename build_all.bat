@echo off
SETLOCAL EnableDelayedExpansion

:: --- CONFIGURATION ---
SET "BIN_CSV=game.exe"
SET "BIN_SQL=game_sql.exe"
SET "CC=g++"
SET "FLAGS=-std=c++17 -I include -I . -O3 -static"
SET "SQL_FLAGS=-DUSE_SQLITE"

echo ============================================================
echo   BAI CAO CAI SIMULATOR - BUILD SYSTEM (ASCII)
echo ============================================================

:: 1. Check Prerequisite
echo [1/4] Checking source files...
if not exist main.cpp ( echo [ERROR] main.cpp missing! && pause && exit /b 1 )
if not exist sqlite3.c ( echo [ERROR] sqlite3.c missing! && pause && exit /b 1 )
echo OK.

:: 2. Build CSV Version (Lightweight)
echo [2/4] Building CSV Version (%BIN_CSV%)...
%CC% %FLAGS% -c main.cpp -o main_csv.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/Card.cpp -o Card_csv.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/ConcreteStates.cpp -o ConcreteStates_csv.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/Deck.cpp -o Deck_csv.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/GameManager.cpp -o GameManager_csv.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% -c src/Player.cpp -o Player_csv.o
if !ERRORLEVEL! NEQ 0 goto :FAIL

%CC% main_csv.o Card_csv.o ConcreteStates_csv.o Deck_csv.o GameManager_csv.o Player_csv.o -o %BIN_CSV% -static
if !ERRORLEVEL! NEQ 0 goto :FAIL
echo SUCCESS: %BIN_CSV% created.

:: 3. Build SQL Version (Heavy)
echo [3/4] Building SQL Version (%BIN_SQL%)...
echo   (Compiling SQLite3... this may take 30-60 seconds)
if not exist sqlite3.o (
    %CC% -x c -c sqlite3.c -o sqlite3.o -O3
    if !ERRORLEVEL! NEQ 0 goto :FAIL
)

%CC% %FLAGS% %SQL_FLAGS% -c main.cpp -o main_sql.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% %SQL_FLAGS% -c src/ConcreteStates.cpp -o ConcreteStates_sql.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% %SQL_FLAGS% -c src/GameManager.cpp -o GameManager_sql.o
if !ERRORLEVEL! NEQ 0 goto :FAIL
%CC% %FLAGS% %SQL_FLAGS% -c src/DatabaseManager.cpp -o DatabaseManager_sql.o
if !ERRORLEVEL! NEQ 0 goto :FAIL

:: Re-use common .o files if they don't depend on SQLITE flag
:: Note: Card, Deck, Player currently don't use USE_SQLITE, but we'll use CSV ones for safety
%CC% main_sql.o Card_csv.o ConcreteStates_sql.o Deck_csv.o GameManager_sql.o Player_csv.o DatabaseManager_sql.o sqlite3.o -o %BIN_SQL% -lpthread -lws2_32 -static
if !ERRORLEVEL! NEQ 0 goto :FAIL
echo SUCCESS: %BIN_SQL% created.

:: 4. Cleanup
echo [4/4] Cleaning up temporary files...
del *_csv.o *_sql.o 
:: We keep sqlite3.o to speed up next build
echo OK.

echo.
echo ============================================================
echo   BUILD COMPLETE!
echo   - %BIN_CSV% (CSV Only)
echo   - %BIN_SQL% (SQLite + CSV)
echo ============================================================
echo.
echo [FINISH] All binaries are ready.
pause
exit /b 0

:FAIL
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo   BUILD FAILED! Please check the errors above.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
