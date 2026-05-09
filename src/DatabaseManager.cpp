#include "../include/DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager() : db(nullptr), connected(false) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) == SQLITE_OK) {
        connected = true;
        // Optimization for massive inserts
        sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
        sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, NULL);
        return true;
    }
    return false;
}

void DatabaseManager::disconnect() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    connected = false;
}

bool DatabaseManager::initTables() {
    if (!connected) return false;

    const char* sqlRounds = "CREATE TABLE IF NOT EXISTS rounds ("
                           "id INTEGER PRIMARY KEY,"
                           "dealer TEXT,"
                           "pot INTEGER,"
                           "winners INTEGER,"
                           "scores TEXT);";

    const char* sqlSwaps = "CREATE TABLE IF NOT EXISTS swaps ("
                          "round_id INTEGER,"
                          "player TEXT,"
                          "turn INTEGER,"
                          "satisfaction REAL,"
                          "desire REAL,"
                          "probability REAL,"
                          "swapped INTEGER);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sqlRounds, NULL, NULL, &errMsg) != SQLITE_OK) return false;
    if (sqlite3_exec(db, sqlSwaps, NULL, NULL, &errMsg) != SQLITE_OK) return false;

    return true;
}

bool DatabaseManager::beginTransaction() {
    if (!connected) return false;
    return sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL) == SQLITE_OK;
}

bool DatabaseManager::endTransaction() {
    if (!connected) return false;
    return sqlite3_exec(db, "COMMIT", NULL, NULL, NULL) == SQLITE_OK;
}

bool DatabaseManager::insertRound(int roundNum, const std::string& dealer, int pot, int winners, const std::string& scores) {
    if (!connected) return false;
    std::string sql = "INSERT INTO rounds VALUES (" + std::to_string(roundNum) + ", '" + 
                      dealer + "', " + std::to_string(pot) + ", " + std::to_string(winners) + ", '" + scores + "');";
    return sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL) == SQLITE_OK;
}

bool DatabaseManager::insertSwap(int roundID, const std::string& player, int turn, float satisfaction, float desire, float prob, bool swapped) {
    if (!connected) return false;
    std::string sql = "INSERT INTO swaps VALUES (" + std::to_string(roundID) + ", '" + 
                      player + "', " + std::to_string(turn) + ", " + std::to_string(satisfaction) + ", " + 
                      std::to_string(desire) + ", " + std::to_string(prob) + ", " + std::to_string(swapped ? 1 : 0) + ");";
    return sqlite3_exec(db, sql.c_str(), NULL, NULL, NULL) == SQLITE_OK;
}
