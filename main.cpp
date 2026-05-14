#include "GameManager.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#endif

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

template <typename T>
T safeInput(const std::string &prompt, T minVal, T maxVal) {
  T val;
  while (true) {
    std::cout << prompt;
    if (std::cin >> val && val >= minVal && val <= maxVal) return val;
    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << RED << "Invalid input." << RESET << "\n";
  }
}

std::shared_ptr<Player> createAI(int id, Archetype arch, float meanS, float meanC, float stdDev, GameManager& manager, std::mt19937& gen) {
    std::normal_distribution<float> sD(meanS, stdDev), cD(meanC, stdDev);
    float skill = std::max(0.0f, std::min(1.0f, sD(gen)));
    float conf = std::max(-1.0f, std::min(1.0f, cD(gen)));
    auto& cfg = manager.archetypeConfigs[arch];
    return std::make_shared<AIPlayer>("AI_" + std::to_string(id), 10000, skill, conf, cfg.greedThreshold, cfg.k, cfg.gamma, arch, gen());
}

struct SimParams { int nP; float minS, maxS, conc, sP, mP; bool useCount; int sC, mC, nC; };
SimParams getParams() {
    SimParams p;
    std::cout << "\n" << BOLD << CYAN << "--- Distribution Mode ---" << RESET << "\n";
    std::cout << "1. Exact Count (Default) | 2. Percentage Distribution\nSelection: ";
    int mode; 
    std::string input;
    std::getline(std::cin >> std::ws, input);
    if (input == "2") p.useCount = false;
    else p.useCount = true;

    if (p.useCount) {
        p.sC = safeInput<int>("Number of Sharks: ", 0, 17);
        p.mC = safeInput<int>("Number of Maniacs: ", 0, 17 - p.sC);
        p.nC = safeInput<int>("Number of Nits: ", 0, 17 - p.sC - p.mC);
        p.nP = p.sC + p.mC + p.nC;
        if (p.nP < 2) { 
            std::cout << YELLOW << "[System] Total players < 2. Adding Nits to reach minimum 2.\n" << RESET; 
            p.nC += (2 - p.nP); p.nP = 2; 
        }
    } else {
        p.nP = safeInput<int>("Enter total number of players (2-17): ", 2, 17);
        p.sP = safeInput<float>("% Shark: ", 0.0f, 100.0f);
        p.mP = safeInput<float>("% Maniac: ", 0.0f, 100.0f - p.sP);
    }

    std::cout << "\n" << BOLD << CYAN << "--- Global Skill Constraints ---" << RESET << "\n";
    p.minS = safeInput<float>("Min Skill (0-1): ", 0.0f, 1.0f);
    p.maxS = safeInput<float>("Max Skill (0-1): ", p.minS, 1.0f);
    p.conc = safeInput<float>("Skill Concentration (1-100): ", 1.0f, 100.0f);
    return p;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
  while (true) {
    GameManager::clearScreen();
    std::cout << BOLD << BLUE << "========================================" << RESET << "\n";
    std::cout << BOLD << BLUE << "       BAI CAO CAI SIMULATOR v3.0       " << RESET << "\n";
    std::cout << BOLD << BLUE << "========================================" << RESET << "\n";
    std::cout << "1. Standard Simulation | 2. Interactive | 3. Random | 4. Log Mode | 0. Exit\nSelection: ";
    int choice; if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
    if (choice == 0) break;
    GameManager manager; if (!manager.loadConfig("config.ini")) { std::cout << "Config error. Press Enter..."; std::cin.ignore(1000, '\n'); std::cin.get(); return 1; }

    unsigned seed;
    if (manager.simulationSeed >= 0) { seed = (unsigned)manager.simulationSeed; std::cout << "[System] Seed from config: " << seed << "\n"; }
    else {
        long long mSeed = safeInput<long long>("Enter seed (-1 for random): ", -1, 999999999999);
        seed = (mSeed == -1) ? std::chrono::system_clock::now().time_since_epoch().count() : (unsigned)mSeed;
        std::cout << "[System] Used seed: " << seed << "\n";
    }
    manager.simulationSeed = (long long)seed; std::mt19937 gen(seed);

    if (choice == 1 || choice == 2 || choice == 4) {
      bool isInter = (choice == 2), isLog = (choice == 4);
      int subMode = 1; if (choice == 1) subMode = safeInput<int>("1. Persistent | 2. Reset\nSelection: ", 1, 2);
      SimParams p = getParams(); float stdDev = 1.0f / p.conc;
      manager.displayArchetypeConfigs();
      std::vector<std::shared_ptr<Player>> players;
      if (isInter) { std::string hN; std::cout << "Enter your name: "; std::cin >> hN; players.push_back(std::make_shared<HumanPlayer>(hN, 10000, -1.0f, 0.0f, 0.0f)); }
      
      if (p.useCount) {
          int sAdded = 0, mAdded = 0, nAdded = 0;
          while (players.size() < (size_t)p.nP) {
              Archetype arch;
              if (sAdded < p.sC) { arch = Archetype::SHARK; sAdded++; }
              else if (mAdded < p.mC) { arch = Archetype::MANIAC; mAdded++; }
              else { arch = Archetype::NIT; nAdded++; }

              auto& cfg = manager.archetypeConfigs[arch];
              float mS = (cfg.minSkill + cfg.maxSkill) / 2.0f;
              float mC = (arch == Archetype::SHARK ? 0.2f : (arch == Archetype::MANIAC ? 0.8f : -0.5f));
              players.push_back(createAI(players.size() + 1, arch, mS, mC, stdDev, manager, gen));
          }
      } else {
          std::discrete_distribution<int> archDist({p.sP, p.mP, 100.0f - p.sP - p.mP});
          for (int i = (int)players.size(); i < p.nP; ++i) {
              int aIdx = archDist(gen); Archetype arch = (aIdx == 0) ? Archetype::SHARK : (aIdx == 1 ? Archetype::MANIAC : Archetype::NIT);
              auto& cfg = manager.archetypeConfigs[arch];
              float mS = (cfg.minSkill + cfg.maxSkill) / 2.0f;
              float mC = (arch == Archetype::SHARK ? 0.2f : (arch == Archetype::MANIAC ? 0.8f : -0.5f));
              players.push_back(createAI(i + 1, arch, mS, mC, stdDev, manager, gen));
          }
      }
      manager.setPlayers(players); if (isInter || isLog) manager.logMode = true;
      std::cout << "\n" << BOLD << CYAN << "--- Player Configuration ---" << RESET << "\n";
      std::cout << std::left << std::setw(12) << "Player" 
                << std::setw(10) << "Type" 
                << std::setw(12) << "Archetype" 
                << std::setw(10) << "Skill" 
                << std::setw(12) << "Confidence" 
                << "Balance" << "\n";
      std::cout << "----------------------------------------------------------------------------\n";
      for (const auto& pl : manager.players) {
        std::cout << std::left << std::setw(12) << pl->getName() 
                  << std::setw(10) << (pl->isHumanPlayer() ? "Human" : "AI") 
                  << std::setw(12) << (pl->isHumanPlayer() ? "---" : pl->getArchetypeString())
                  << std::fixed << std::setprecision(2)
                  << std::setw(10) << pl->getSkillLevel() 
                  << std::setw(12) << pl->getConfidenceLevel() 
                  << pl->getBalance() << "\n";
      }
      
      std::cout << "\nEnable Research Reports (Data Export)? (y/n): ";
      char exT; std::cin >> exT; manager.autoExport = (exT == 'y' || exT == 'Y');

      bool simActive = true;
      while (simActive) {
        if (subMode == 2) { for (auto& pl : manager.players) pl->resetStats(); manager.roundCount = 0; }
        int numR = (isLog) ? 1 : safeInput<int>("Number of rounds: ", 1, 10000000);
        std::stringstream ss;
        ss << "Mode: " << (isInter ? "Interactive" : (isLog ? "Log Mode" : "Standard")) << "\n"
           << "Submode: " << (subMode == 1 ? "Persistent" : "Reset Every Round") << "\n"
           << "Used Seed: " << manager.simulationSeed << "\n"
           << "Players: " << p.nP << "\n";
        if (p.useCount) {
            ss << "Distribution: Exact Count (S:" << p.sC << ", M:" << p.mC << ", N:" << p.nC << ")\n";
        } else {
            ss << "Distribution: Percentage (S:" << p.sP << "%, M:" << p.mP << "%, N:" << (100.0f - p.sP - p.mP) << "%)\n";
        }
        ss << "Global Min Skill: " << std::fixed << std::setprecision(2) << p.minS << "\n"
           << "Global Max Skill: " << p.maxS << "\n"
           << "Skill Concentration: " << p.conc << "\n"
           << "Rounds: " << numR;
        manager.simulationParams = ss.str();
        if (manager.autoExport) manager.startStreaming();
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.beginTransaction();
#endif
        auto tS = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numR; ++i) {
          if (subMode == 2) { for (auto& pl : manager.players) { pl->setBalance(10000); pl->setEliminated(false); } }
          int active = 0; for (auto& pl : manager.players) if (!pl->getIsEliminated()) active++;
          if (active <= 1) break;
          if (i == numR - 1) manager.isFinalRound = true;
          manager.playRound();
#ifdef USE_SQLITE
          if (manager.autoExport && (i + 1) % 1000 == 0) { manager.db.endTransaction(); manager.db.beginTransaction(); }
#endif
          if (!isInter && !isLog && ((i + 1) % 100 == 0 || i == numR - 1)) std::cout << "\rProgress: " << (i + 1) << " / " << numR << " (" << std::fixed << std::setprecision(1) << (float)(i+1)/numR*100.0f << "%)" << std::flush;
        }
        if (manager.autoExport) {
#ifdef USE_SQLITE
            manager.db.endTransaction();
#endif
            manager.stopStreaming();
            manager.exportResearchReports();
        }
        std::cout << "\nDone in " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - tS).count() << "s.\n";
        manager.printSummary();
        if (isLog) { std::cout << "\n" << YELLOW << "[Log Mode] Press Enter to return to menu..." << RESET; std::cin.ignore(1000, '\n'); std::cin.get(); break; }
        std::cout << "\n1. Run more | 2. Menu\nSelection: "; if (safeInput<int>("", 1, 2) == 2) simActive = false;
      }
    } else if (choice == 3) {
      int batches = safeInput<int>("Number of batches: ", 1, 1000);
      std::cout << "Enable Data Export? (y/n): "; char exT; std::cin >> exT; 
      manager.autoExport = (exT == 'y' || exT == 'Y');
      manager.isMode3 = true;

      if (manager.autoExport) {
          manager.startStreaming();
#ifdef USE_SQLITE
          manager.db.beginTransaction();
#endif
      }

      std::stringstream ss;
      ss << "Mode: Random (Continuous Research)\n"
         << "Used Seed: " << manager.simulationSeed << "\n"
         << "Batches: " << batches;
      manager.simulationParams = ss.str();

      std::cout << GREEN << "Running Random Mode (Continuous Research)..." << RESET << "\n";
      for (int b = 0; b < batches; ++b) {
          manager.currentBatchID = b + 1;
          int nP = std::uniform_int_distribution<int>(2, 17)(gen);
          std::vector<std::shared_ptr<Player>> players;
          for (int i = 0; i < nP; ++i) players.push_back(createAI(i + 1, Archetype::NORMAL, 0.5f, 0.0f, 0.2f, manager, gen));
          manager.setPlayers(players); 
          if (manager.autoExport) manager.logAIConfigs();
          manager.isFinalRound = true; 
          manager.playRound();
#ifdef USE_SQLITE
          if (manager.autoExport && (b + 1) % 100 == 0) { manager.db.endTransaction(); manager.db.beginTransaction(); }
#endif
          if ((b + 1) % 10 == 0) std::cout << "\rBatch Progress: " << (b + 1) << " / " << batches << std::flush;
      }

      if (manager.autoExport) {
#ifdef USE_SQLITE
          manager.db.endTransaction();
#endif
          manager.stopStreaming();
          manager.exportResearchReports();
      }
      std::cout << "\nDone. Press Enter..."; std::cin.ignore(1000, '\n'); std::cin.get();
    }
  }
  return 0;
}
