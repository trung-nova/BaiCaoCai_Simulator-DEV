#include "include/GameManager.h"
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
    if (std::cin >> val && val >= minVal && val <= maxVal) {
      return val;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << RED << "Invalid input. Please try again." << RESET << "\n";
  }
}

int main() {
  while (true) {
    GameManager::clearScreen();
    std::cout << BOLD << BLUE << "========================================" << RESET << "\n";
    std::cout << BOLD << BLUE << "       BAI CAO CAI SIMULATOR v3.0       " << RESET << "\n";
    std::cout << BOLD << BLUE << "========================================" << RESET << "\n";
    std::cout << "1. Standard Simulation (AI vs AI)\n";
    std::cout << "2. Interactive Mode (You vs AI)\n";
    std::cout << "3. Random Mode (Continuous Research)\n";
    std::cout << "4. Log Mode (Detailed Analysis, 1 Round)\n";
    std::cout << "0. Exit\n";
    std::cout << "Selection: ";

    int choice;
    if (!(std::cin >> choice)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }
    if (choice == 0) break;

    GameManager manager;
    if (!manager.loadConfig("config.ini")) {
        std::cout << "Press Enter to exit...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return 1;
    }

    unsigned seed;
    if (manager.simulationSeed >= 0) {
        seed = (unsigned)manager.simulationSeed;
        std::cout << "[System] Using fixed seed from config: " << seed << "\n";
    } else {
        long long manualSeed = safeInput<long long>("Enter simulation seed (-1 for random): ", -1, 999999999999);
        if (manualSeed == -1) {
            seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::cout << "[System] Using random seed: " << seed << "\n";
        } else {
            seed = (unsigned)manualSeed;
            std::cout << "[System] Using manual seed: " << seed << "\n";
        }
    }
    manager.simulationSeed = (long long)seed;
    std::mt19937 gen(seed);

    if (choice == 1) {
      std::cout << "\nStandard Simulation Sub-modes:\n";
      std::cout << "1. Persistent Mode | 2. Reset Mode\n";
      int subMode = safeInput<int>("Selection: ", 1, 2);

      int numPlayers = safeInput<int>("Enter number of players (2-17): ", 2, 17);
      float minS = safeInput<float>("Min Skill (0-1): ", 0.0f, 1.0f);
      float maxS = safeInput<float>("Max Skill (0-1): ", minS, 1.0f);
      float conc = safeInput<float>("Skill Concentration (1-100): ", 1.0f, 100.0f);
      float stdDev = 1.0f / conc;
      float sPct = safeInput<float>("% Shark: ", 0.0f, 100.0f);
      float mPct = safeInput<float>("% Maniac: ", 0.0f, 100.0f - sPct);
      float nPct = 100.0f - sPct - mPct;

      std::vector<std::shared_ptr<Player>> players;
      std::discrete_distribution<int> archDist({sPct, mPct, nPct});
      manager.displayArchetypeConfigs();

      std::cout << "\nEnable Research Reports? (y/n): ";
      char exportToggle; std::cin >> exportToggle;
      manager.autoExport = (exportToggle == 'y' || exportToggle == 'Y');

      for (int i = 0; i < numPlayers; ++i) {
        int aChoice = archDist(gen);
        Archetype arch; float mS = 0.5f, mC = 0.0f;
        if (aChoice == 0) { arch = Archetype::SHARK; mS = maxS; mC = 0.2f; }
        else if (aChoice == 1) { arch = Archetype::MANIAC; mS = minS; mC = 0.8f; }
        else { arch = Archetype::NIT; mS = (minS + maxS) / 2.0f; mC = -0.5f; }

        std::normal_distribution<float> sD(mS, stdDev), cD(mC, stdDev);
        float skill = std::max(0.0f, std::min(1.0f, sD(gen)));
        float conf = std::max(-1.0f, std::min(1.0f, cD(gen)));

        auto& cfg = manager.archetypeConfigs[arch];
        players.push_back(std::make_shared<AIPlayer>("AI_" + std::to_string(i + 1), 10000,
                                       skill, conf, cfg.greedThreshold, cfg.k, cfg.gamma, arch, gen()));
      }

      manager.setPlayers(players);
      bool simActive = true;
      while (simActive) {
        if (subMode == 2) { for (auto& p : manager.players) p->resetStats(); manager.tiltLogs.clear(); manager.roundCount = 0; }
        int numR = safeInput<int>("Number of rounds: ", 1, 10000000);
        for (auto& p : manager.players) p->getBankrollHistory().reserve(p->getBankrollHistory().size() + numR + 1);

        std::ostringstream oss;
        oss << "Seed: " << manager.simulationSeed << "\nMode: Standard (" << (subMode == 1 ? "Persistent" : "Reset") << ")\nPlayers: " << players.size() << "\nRounds: " << numR;
        manager.simulationParams = oss.str();

        if (manager.autoExport) manager.startStreaming();
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.beginTransaction();
#endif
        auto tStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numR; ++i) {
          if (subMode == 2) { for (auto& p : manager.players) { p->setBalance(10000); p->setEliminated(false); } }
          int activeCount = 0;
          for (auto& p : manager.players) if (!p->getIsEliminated()) activeCount++;
          if (activeCount <= 1) break;
          if (i == numR - 1) manager.isFinalRound = true;
          manager.playRound();
#ifdef USE_SQLITE
          if ((i + 1) % 1000 == 0) { manager.db.endTransaction(); manager.db.beginTransaction(); }
#endif
          if ((i + 1) % 100 == 0 || i == numR - 1) {
              float progress = (float)(i + 1) / numR * 100.0f;
              std::cout << "\rProgress: " << (i + 1) << " / " << numR << " (" << std::fixed << std::setprecision(1) << progress << "%)" << std::flush;
          }
        }
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.endTransaction();
#endif
        if (manager.autoExport) manager.stopStreaming();
        auto tEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = tEnd - tStart;
        std::cout << "\nSimulation complete in " << elapsed.count() << "s.\n";
        manager.printSummary();

        std::cout << "\n1. Run more rounds | 2. Return to Main Menu\n";
        int postC = safeInput<int>("Selection: ", 1, 2);
        if (postC == 2) simActive = false;
      }
    } else if (choice == 2 || choice == 4) {
      bool isLog = (choice == 4);
      int nP = safeInput<int>("Number of players (2-17): ", 2, 17);
      std::vector<std::shared_ptr<Player>> players;
      if (!isLog) {
          std::string hName; std::cout << "Enter your name: "; std::cin >> hName;
          players.push_back(std::make_shared<HumanPlayer>(hName, 10000, -1.0f, 0.0f, 0.0f));
      }

      for (int i = players.size(); i < nP; ++i) {
          Archetype arch = (i % 3 == 0) ? Archetype::SHARK : (i % 3 == 1 ? Archetype::MANIAC : Archetype::NIT);
          auto& cfg = manager.archetypeConfigs[arch];
          players.push_back(std::make_shared<AIPlayer>("AI_" + std::to_string(i + 1), 10000, 0.5f, 0.0f, cfg.greedThreshold, cfg.k, cfg.gamma, arch, gen()));
      }
      manager.setPlayers(players);
      manager.logMode = true;
      int numR = isLog ? 1 : safeInput<int>("Number of rounds: ", 1, 100);
      for (int i = 0; i < numR; ++i) {
          if (i == numR - 1) manager.isFinalRound = true;
          manager.playRound();
      }
      manager.printSummary();
      std::cout << "Press Enter to return to menu..."; std::cin.ignore(1000, '\n'); std::cin.get();
    } else if (choice == 3) {
      int batches = safeInput<int>("Number of random batches to run: ", 1, 1000);
      std::cout << GREEN << "Running Random Mode (Continuous Research)..." << RESET << "\n";
      for (int b = 0; b < batches; ++b) {
          int nP = std::uniform_int_distribution<int>(2, 17)(gen);
          std::vector<std::shared_ptr<Player>> players;
          for (int i = 0; i < nP; ++i) {
              players.push_back(std::make_shared<AIPlayer>("AI_" + std::to_string(i + 1), 10000, 0.5f, 0.0f, 0.5f, 1.0f, 2.0f, Archetype::NORMAL, gen()));
          }
          manager.setPlayers(players);
          manager.isFinalRound = true; manager.playRound();
          if ((b + 1) % 10 == 0) std::cout << "\rBatch Progress: " << (b + 1) << " / " << batches << std::flush;
      }
      std::cout << "\nRandom Mode complete. Press Enter..."; std::cin.ignore(1000, '\n'); std::cin.get();
    }
  }
  return 0;
}
