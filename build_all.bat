echo BẮT ĐẦU KIỂM TRA HỆ THỐNG...
:: Kiểm tra sự tồn tại của các file
if exist main.cpp (echo - main.cpp: OK) else (echo - THIẾU main.cpp && pause && exit)
if exist sqlite3.c (echo - sqlite3.c: OK) else (echo - THIẾU sqlite3.c && pause && exit)
if exist sqlite3.h (echo - sqlite3.h: OK) else (echo - THIẾU sqlite3.h && pause && exit)

echo.
echo [BUỚC 1] Biên dịch SQLite (Đây là bước nặng nhất)...
g++ -x c -c sqlite3.c -o sqlite3.o
if %ERRORLEVEL% NEQ 0 (echo LOI O BUOC 1 && pause && exit)
echo SUCCESS: Da tao xong sqlite3.o
pause

echo.
echo [BƯỚC 2] Biên dịch Logic Game...
g++ -std=c++17 -DUSE_SQLITE -c main.cpp -I include -I . -o main.o
g++ -std=c++17 -DUSE_SQLITE -c src/Card.cpp -I include -I . -o Card.o
g++ -std=c++17 -DUSE_SQLITE -c src/ConcreteStates.cpp -I include -I . -o ConcreteStates.o
g++ -std=c++17 -DUSE_SQLITE -c src/Deck.cpp -I include -I . -o Deck.o
g++ -std=c++17 -DUSE_SQLITE -c src/GameManager.cpp -I include -I . -o GameManager.o
g++ -std=c++17 -DUSE_SQLITE -c src/Player.cpp -I include -I . -o Player.o
g++ -std=c++17 -DUSE_SQLITE -c src/DatabaseManager.cpp -I include -I . -o DatabaseManager.o
if %ERRORLEVEL% NEQ 0 (echo LOI O BUOC 2 && pause && exit)
echo SUCCESS: Da tao xong cac file .o cua Game
pause

echo.
echo [BƯỚC 3] Liên kết (Linking)...
g++ main.o Card.o ConcreteStates.o Deck.o GameManager.o Player.o DatabaseManager.o sqlite3.o -o game_sql.exe -lpthread -lws2_32 -static
if %ERRORLEVEL% NEQ 0 (echo LOI O BUOC 3 && pause && exit)

echo.
echo CHÚC MỪNG! game_sql.exe đã được tạo thành công.
del *.o
pause
