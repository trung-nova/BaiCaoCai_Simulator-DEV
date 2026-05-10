#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <vector>
#include <map>
#include "Player.h"
#include "Deck.h"
#include "GameState.h"
#include <fstream>
#ifdef USE_SQLITE
#include "DatabaseManager.h"
#endif

struct RoundRecord {
    int roundNum;
    std::string dealerName;
    int pot;
    int winnersCount;
    std::string scoresSummary;
};

struct ArchetypeConfig {
    float k = 1.0f;
    float gamma = 2.0f;
    float greedThreshold = 0.5f;
    float minSkill = 0.0f;
    float maxSkill = 1.0f;
};


class GameManager {
public:
    std::vector<Player*> players;
    int currentPot;
    GameState* currentState;
    Deck deck;
    int currentDealerIndex;
    int roundCount = 0;

    // Streaming file handles for integrity and RAM optimization
    bool isStreaming = false;
    std::ofstream streamSwap;
    std::ofstream streamRound;
    std::ofstream streamHistory;
    std::string currentSessionTS;

    std::vector<std::string> tiltLogs;

    GameManager();
    ~GameManager();
    void setPlayers(const std::vector<Player*>& p);
    void changeState(GameState* newState);
    void playRound();
    void startStreaming();
    void stopStreaming();
    std::string getTimestamp();
    void exportResearchReports(); // Now handles Summary and Tilt logs
    void printSummary();
    void saveInitialState();

    static void clearScreen();

    bool logMode = false;
    bool autoExport = false;
    bool isFinalRound = false;
    std::string simulationParams;
    std::vector<int> initialBalances;

    std::map<Archetype, ArchetypeConfig> archetypeConfigs;

    void loadConfig(const std::string& filename);

#ifdef USE_SQLITE
    DatabaseManager db;
#endif
};

#endif // GAMEMANAGER_H
