#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>
#include "Player.h"

// Note: This requires sqlite3.h and sqlite3.c to be in the project
#include "sqlite3.h"

class DatabaseManager {
private:
    sqlite3* db;
    bool connected;

public:
    DatabaseManager();
    ~DatabaseManager();

    bool connect(const std::string& dbName);
    void disconnect();
    
    bool initTables();
    
    bool beginTransaction();
    bool endTransaction();
    
    bool insertRound(int roundNum, const std::string& dealer, int pot, int winners, const std::string& scores);
    bool insertSwap(int roundID, const std::string& player, int turn, float satisfaction, float desire, float prob, bool swapped);
};

#endif // DATABASEMANAGER_H
