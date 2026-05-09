#include "../include/GameManager.h"
#include "../include/ConcreteStates.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

GameManager::GameManager() : currentPot(0), currentState(nullptr), currentDealerIndex(0), isStreaming(false) {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0777);
#endif
}

GameManager::~GameManager() {
    if (isStreaming) stopStreaming();
    for (auto* p : players) {
        delete p;
    }
}

void GameManager::setPlayers(const std::vector<Player*>& p) {
    players = p;
    if (!players.empty()) {
        // Randomly assign the first dealer
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, players.size() - 1);
        
        currentDealerIndex = dist(gen);
        players[currentDealerIndex]->isDealer = true;
    }
}

void GameManager::changeState(GameState* newState) {
    currentState = newState;
}

void GameManager::playRound() {
    roundCount++;
    for (auto* p : players) {
        if (!p->isEliminated) {
            p->roundsPlayed++;
        }
        p->bankrollHistory.push_back(p->balance);
    }
    changeState(StateFactory::getBettingState());
    
    // The states will transition: Betting -> Dealing -> Trading -> Eval -> nullptr
    while (currentState != nullptr) {
        currentState->update(this);
    }
}

void GameManager::startStreaming() {
    if (isStreaming) return;
    currentSessionTS = getTimestamp();
    
#ifndef USE_SQLITE
    streamSwap.open("data/" + currentSessionTS + "_swap_decisions.csv");
    streamSwap << "RoundID,PlayerName,SwapTurn,Satisfaction,Desire,Probability,Swapped\n";
    
    streamRound.open("data/" + currentSessionTS + "_round_results.csv");
    streamRound << "RoundNum,Dealer,Pot,WinnersCount,Scores\n";
    
    streamHistory.open("data/" + currentSessionTS + "_bankroll_history.csv");
    streamHistory << "RoundNum";
    for (auto* p : players) streamHistory << "," << p->name;
    streamHistory << "\n";
#endif
    
    isStreaming = true;
#ifdef USE_SQLITE
    db.connect("data/" + currentSessionTS + "_simulation.db");
    db.initTables();
#endif
}

void GameManager::stopStreaming() {
    if (!isStreaming) return;
#ifndef USE_SQLITE
    streamSwap.close();
    streamRound.close();
    streamHistory.close();
#endif
    isStreaming = false;
#ifdef USE_SQLITE
    db.disconnect();
#endif
}

void GameManager::saveInitialState() {
    initialBalances.clear();
    for (const auto* p : players) {
        initialBalances.push_back(p->balance);
    }
}

std::string GameManager::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &now_tm);
    return std::string(buf);
}

void GameManager::exportResearchReports() {
    std::string ts = currentSessionTS.empty() ? getTimestamp() : currentSessionTS;

    // 1. Flush streaming files if still open
    if (isStreaming) {
        streamSwap.flush();
        streamRound.flush();
        streamHistory.flush();
    }

    // 4. Tilt Events
    std::ofstream fTilt("data/" + ts + "_tilt_events.txt");
    fTilt << "==========================================================\n";
    fTilt << "                 TILT EVENTS LOG\n";
    fTilt << "==========================================================\n";
    if (tiltLogs.empty()) {
        fTilt << "No tilt events occurred during this simulation.\n";
    } else {
        for (const auto& log : tiltLogs) {
            fTilt << log << "\n";
        }
    }
    fTilt.close();

    // 5. Simulation Summary
    std::ofstream fSummary("data/" + ts + "_simulation_summary.txt");
    fSummary << "==========================================================\n";
    fSummary << "                 SIMULATION PARAMETERS\n";
    fSummary << "==========================================================\n";
    fSummary << simulationParams << "\n\n";

    fSummary << "==========================================================\n";
    fSummary << "                 PLAYER START VS END\n";
    fSummary << "==========================================================\n";
    fSummary << std::left << std::setw(12) << "Player" 
             << std::setw(10) << "Skill" 
             << std::setw(12) << "Confidence"
             << std::setw(12) << "Start Bal"
             << std::setw(12) << "End Bal"
             << std::setw(12) << "Net Profit"
             << std::setw(10) << "Wins" 
             << std::setw(10) << "Rounds"
             << std::setw(10) << "Win Rate"
             << std::setw(18) << "Last Hand"
             << "Score\n";
    fSummary << "------------------------------------------------------------------------------------------------------\n";
    
    for (size_t i = 0; i < players.size(); ++i) {
        const auto* p = players[i];
        int startBal = (i < initialBalances.size()) ? initialBalances[i] : 10000;
        int netProfit = p->balance - startBal;
        float winRate = (p->roundsPlayed > 0) ? (float)p->wins / p->roundsPlayed : 0.0f;
        std::string nameStatus = p->name + (p->isEliminated ? " [X]" : "");
        
        std::string skillStr = (p->skillLevel < 0) ? "MANUAL" : std::to_string(p->skillLevel).substr(0, 4);
        
        std::stringstream wrss;
        wrss << std::fixed << std::setprecision(1) << (winRate * 100.0f) << "%";
        
        fSummary << std::left << std::setw(12) << nameStatus 
                 << std::setw(10) << skillStr
                 << std::setw(12) << p->confidenceLevel
                 << std::setw(12) << startBal
                 << std::setw(12) << p->balance
                 << std::showpos << std::setw(12) << netProfit << std::noshowpos
                 << std::setw(10) << p->wins 
                 << std::setw(10) << p->roundsPlayed
                 << std::setw(10) << wrss.str()
                 << std::setw(18) << p->lastHandPlain
                 << p->lastScore << "\n";
    }
    fSummary.close();
}

static int getVisibleLength(const std::string& s) {
    int len = 0;
    bool inEscape = false;
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] == '\033') { inEscape = true; continue; }
        if (inEscape) {
            if (s[i] == 'm') inEscape = false;
            continue;
        }
        len++;
    }
    return len;
}

void GameManager::printSummary() {
    int tableWidth = 105;
    std::string separator(tableWidth, '=');
    std::string subSeparator(tableWidth, '-');

    std::cout << "\n" << BOLD << YELLOW << separator << RESET << "\n";
    std::cout << BOLD << YELLOW << std::setw(tableWidth/2 + 12) << std::right << "FINAL SIMULATION REPORT" << RESET << "\n";
    std::cout << BOLD << YELLOW << separator << RESET << "\n";
    
    std::cout << std::left << std::setw(12) << "Player" 
              << std::setw(10) << "Skill" 
              << std::setw(12) << "Balance" 
              << std::setw(10) << "Wins" 
              << std::setw(10) << "Rounds"
              << std::setw(12) << "Win Rate"
              << std::setw(25) << "Last Hand"
              << "Score\n";
    std::cout << subSeparator << "\n";
    
    for (const auto* p : players) {
        float winRate = (p->roundsPlayed > 0) ? (float)p->wins / p->roundsPlayed : 0.0f;
        std::string balColor = (p->isEliminated) ? RESET : (p->balance >= 10000 ? GREEN : RED);
        std::string nameStatus = p->name + (p->isEliminated ? " [X]" : "");
        
        std::string skillStr = (p->skillLevel < 0) ? "MANUAL" : std::to_string(p->skillLevel).substr(0, 4);

        std::stringstream wrss;
        wrss << std::fixed << std::setprecision(1) << (winRate * 100.0f) << "%";

        std::cout << std::left << std::setw(12) << nameStatus 
                  << std::setw(10) << skillStr
                  << balColor << std::setw(12) << p->balance << RESET
                  << std::setw(10) << p->wins 
                  << std::setw(10) << p->roundsPlayed
                  << std::setw(12) << wrss.str();
        
        // Custom padding for colored hand string
        std::cout << p->lastHand;
        int handVisLen = getVisibleLength(p->lastHand);
        int pad = std::max(0, 25 - handVisLen);
        std::cout << std::string(pad, ' ');
        
        std::cout << p->lastScore << "\n";
    }
    std::cout << BOLD << YELLOW << separator << RESET << "\n";
}

void GameManager::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Config] Warning: Could not open " << filename << ". Using defaults.\n";
        return;
    }
    
    std::string line, currentSection;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') continue;
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;
        
        std::string key = line.substr(0, eqPos);
        float value = std::stof(line.substr(eqPos + 1));
        
        Archetype arch;
        if (currentSection == "SHARK") arch = Archetype::SHARK;
        else if (currentSection == "MANIAC") arch = Archetype::MANIAC;
        else if (currentSection == "NIT") arch = Archetype::NIT;
        else continue;
        
        if (key == "k") archetypeConfigs[arch].k = value;
        else if (key == "gamma") archetypeConfigs[arch].gamma = value;
        else if (key == "greed_threshold") archetypeConfigs[arch].greedThreshold = value;
        else if (key == "min_skill") archetypeConfigs[arch].minSkill = value;
        else if (key == "max_skill") archetypeConfigs[arch].maxSkill = value;
    }
    std::cout << "[Config] Successfully loaded archetype personalities from " << filename << "\n";
}


void GameManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}
