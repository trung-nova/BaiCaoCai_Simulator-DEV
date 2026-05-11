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
    // Smart pointers handle deletion
}

void GameManager::setPlayers(const std::vector<std::shared_ptr<Player>>& p) {
    players = p;
    if (!players.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, (int)players.size() - 1);
        
        currentDealerIndex = dist(gen);
        players[currentDealerIndex]->isDealer = true;
    }

    initialBalances.clear();
    for (auto& pl : players) {
        initialBalances.push_back(pl->getBalance());
    }
}

void GameManager::changeState(std::unique_ptr<GameState> newState) {
    currentState = std::move(newState);
}

void GameManager::playRound() {
    roundCount++;
    for (auto& p : players) {
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
    currentSessionTS = getTimestamp();
    streamSwap.open("data/" + currentSessionTS + "_swap_decisions.csv");
    streamRound.open("data/" + currentSessionTS + "_round_results.csv");
    streamHistory.open("data/" + currentSessionTS + "_bankroll_history.csv");
    
    streamSwap << "RoundID,PlayerName,SwapTurn,Satisfaction,Desire,Probability,Swapped\n";
    streamRound << "RoundNum,Dealer,Pot,WinnersCount,ScoresSummary\n";
    
    // Header for bankroll
    streamHistory << "Round";
    for(auto& p : players) streamHistory << "," << p->getName();
    streamHistory << "\n";

#ifdef USE_SQLITE
    db.connect("data/simulation_results.db");
    db.initTables();
#endif
    isStreaming = true;
}

void GameManager::stopStreaming() {
    if (streamSwap.is_open()) streamSwap.close();
    if (streamRound.is_open()) streamRound.close();
    if (streamHistory.is_open()) streamHistory.close();
#ifdef USE_SQLITE
    db.disconnect();
#endif
    isStreaming = false;
}

std::string GameManager::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", parts);
    return std::string(buf);
}

void GameManager::printSummary() {
    std::cout << "\n" << BOLD << BLUE << "=========================================================================================================" << RESET << "\n";
    std::cout << BOLD << BLUE << "                                         FINAL SIMULATION REPORT                                         " << RESET << "\n";
    std::cout << BOLD << BLUE << "=========================================================================================================" << RESET << "\n";
    std::cout << std::left << std::setw(12) << "Player" 
              << std::setw(10) << "Skill" 
              << std::setw(12) << "Balance" 
              << std::setw(10) << "Wins" 
              << std::setw(10) << "Rounds" 
              << std::setw(12) << "Win Rate" 
              << std::setw(25) << "Last Hand" 
              << "Score" << "\n";
    std::cout << "---------------------------------------------------------------------------------------------------------\n";
    
    for (auto& p : players) {
        float winRate = (p->roundsPlayed > 0) ? (float)p->wins / p->roundsPlayed * 100.0f : 0.0f;
        std::cout << std::left << std::setw(12) << p->getName()
                  << std::fixed << std::setprecision(2) << std::setw(10) << p->getSkillLevel()
                  << std::setw(12) << p->getBalance()
                  << std::setw(10) << p->wins
                  << std::setw(10) << p->roundsPlayed;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << winRate << "%";
        std::cout << std::left << std::setw(12) << ss.str();
        std::cout << "        " << p->lastHand;
        int visibleLen = p->lastHandPlain.length();
        int pad = std::max(0, 25 - visibleLen);
        std::cout << std::string(pad, ' ') << p->lastScore << "\n";
    }
    std::cout << BOLD << BLUE << "=========================================================================================================" << RESET << "\n";
}

void GameManager::exportResearchReports() {
    std::string filename = "data/" + currentSessionTS + "_summary.txt";
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << "SIMULATION RESEARCH SUMMARY\n";
    out << "Timestamp: " << currentSessionTS << "\n";
    out << "Parameters:\n" << simulationParams << "\n\n";
    out << "Final Results:\n";
    out << std::left << std::setw(12) << "Player" << std::setw(10) << "Skill" << std::setw(12) << "Balance" << "\n";
    for (auto& p : players) {
        out << std::left << std::setw(12) << p->getName() << std::setw(10) << p->getSkillLevel() << std::setw(12) << p->getBalance() << "\n";
    }

    if (!tiltLogs.empty()) {
        out << "\nAI TILT EVENTS LOG:\n";
        for (const auto& log : tiltLogs) out << log << "\n";
    }
    out.close();
}

void GameManager::saveInitialState() {
    // Already handled in startStreaming for history file header
}

bool GameManager::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "\n" << BOLD << RED << "[ERROR] CRITICAL: Could not open " << filename << RESET << "\n";
        std::cerr << YELLOW << "The simulation requires " << filename << " to define AI behavioral models.\n";
        std::cerr << "Please ensure the file is in the same directory as the executable." << RESET << "\n\n";
        return false;
    }
    
    std::string line, currentSection;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;
        
        std::string key = line.substr(0, eqPos);
        float value = std::stof(line.substr(eqPos + 1));
        
        if (currentSection == "GLOBAL") {
            if (key == "seed") simulationSeed = (long long)value;
            continue;
        }

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
    return true;
}

void GameManager::displayArchetypeConfigs() {
    std::cout << BOLD << CYAN << "\n--- AI BEHAVIORAL PROFILES (From Config) ---" << RESET << "\n";
    std::cout << std::left << std::setw(10) << "Type" 
              << std::setw(10) << "k" 
              << std::setw(10) << "gamma" 
              << std::setw(10) << "Greed" 
              << std::setw(15) << "Skill Range" << "\n";
    std::cout << "--------------------------------------------------------\n";
    
    auto printArch = [&](const std::string& name, Archetype a) {
        auto& c = archetypeConfigs[a];
        std::cout << std::left << std::setw(10) << name 
                  << std::fixed << std::setprecision(2)
                  << std::setw(10) << c.k 
                  << std::setw(10) << c.gamma 
                  << std::setw(10) << c.greedThreshold
                  << "[" << c.minSkill << " - " << c.maxSkill << "]\n";
    };

    printArch("SHARK", Archetype::SHARK);
    printArch("MANIAC", Archetype::MANIAC);
    printArch("NIT", Archetype::NIT);
    std::cout << "--------------------------------------------------------\n\n";
}

void GameManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}
