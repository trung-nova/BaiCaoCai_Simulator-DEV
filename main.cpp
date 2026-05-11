#include "include/GameManager.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
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
    std::mt19937 gen(seed);
    manager.simulationSeed = (long long)seed; // Store the actual seed used

    if (choice == 1) {
      std::cout << "\nStandard Simulation Sub-modes:\n";
      std::cout << "1. Persistent Mode (Accumulate balance across multiple runs)\n";
      std::cout << "2. Reset Mode (Reset balances to 10,000 before each run)\n";
      int subMode = safeInput<int>("Selection: ", 1, 2);

      int numPlayers = safeInput<int>("Enter number of players (2-17): ", 2, 17);
      float minSkill = safeInput<float>("Enter minimum skill (0-1): ", 0.0f, 1.0f);
      float maxSkill = safeInput<float>("Enter maximum skill (0-1): ", minSkill, 1.0f);
      float concentration = safeInput<float>("Enter skill concentration (1-100): ", 1.0f, 100.0f);
      float stdDev = 1.0f / concentration;
      float sharkPct = safeInput<float>("Enter % Shark: ", 0.0f, 100.0f);
      float maniacPct = safeInput<float>("Enter % Maniac: ", 0.0f, 100.0f - sharkPct);
      float nitPct = 100.0f - sharkPct - maniacPct;

      std::vector<std::shared_ptr<Player>> players;
      std::discrete_distribution<int> archetypeDist({sharkPct, maniacPct, nitPct});
      
      manager.displayArchetypeConfigs();

      std::cout << "\nEnable Research Reports (Data Export)? (y/n): ";
      char exportToggle;
      std::cin >> exportToggle;
      manager.autoExport = (exportToggle == 'y' || exportToggle == 'Y');

      for (int i = 0; i < numPlayers; ++i) {
        int archChoice = archetypeDist(gen);
        Archetype arch;
        float meanS = 0.5f, meanC = 0.0f;
        if (archChoice == 0) { arch = Archetype::SHARK; meanS = maxSkill; meanC = 0.2f; }
        else if (archChoice == 1) { arch = Archetype::MANIAC; meanS = minSkill; meanC = 0.8f; }
        else { arch = Archetype::NIT; meanS = (minSkill + maxSkill) / 2.0f; meanC = -0.5f; }

        std::normal_distribution<float> skillDist(meanS, stdDev);
        std::normal_distribution<float> confDist(meanC, stdDev);
        float skill = std::max(0.0f, std::min(1.0f, skillDist(gen)));
        float conf = std::max(-1.0f, std::min(1.0f, confDist(gen)));

        auto& cfg = manager.archetypeConfigs[arch];
        players.push_back(std::make_shared<AIPlayer>("AI_" + std::to_string(i + 1), 10000,
                                       skill, conf, cfg.greedThreshold, 
                                       (arch == Archetype::NORMAL ? 1.0f : cfg.k), 
                                       (arch == Archetype::NORMAL ? 2.0f : cfg.gamma), arch, gen()));
      }

      manager.setPlayers(players);
      bool simulationActive = true;
      while (simulationActive) {
        if (subMode == 2) {
          for (auto& p : manager.players) p->resetStats();
          manager.tiltLogs.clear();
          manager.roundCount = 0;
        }

        int numRounds = safeInput<int>("Enter number of rounds to run: ", 1, 33554332);
        for (auto& p : manager.players) p->getBankrollHistory().reserve(p->getBankrollHistory().size() + numRounds + 1);

        std::cout << "\n" << BOLD << CYAN << "--- Player Configuration ---" << RESET << "\n";
        for (const auto& p : manager.players) {
          std::cout << std::left << std::setw(10) << p->getName() << std::setw(10)
                    << (p->isHumanPlayer() ? "Human" : "AI") << std::fixed << std::setprecision(2)
                    << std::setw(10) << p->getSkillLevel() << std::setw(12) << p->getBalance() << "\n";
        }

        std::ostringstream oss;
        oss << "Used Seed: " << manager.simulationSeed << "\n"
            << "Mode: Standard Simulation (" << (subMode == 1 ? "Persistent" : "Reset") << ")\n"
            << "Players: " << players.size() << "\n"
            << "Rounds: " << numRounds;
        manager.simulationParams = oss.str();

        if (manager.autoExport) manager.startStreaming();
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.beginTransaction();
#endif
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numRounds; ++i) {
          if (subMode == 2) {
            for (auto& p : manager.players) { p->setBalance(10000); p->setEliminated(false); }
          }
          int activeCount = 0;
          for (auto& p : manager.players) if (!p->getIsEliminated()) activeCount++;
          if (activeCount <= 1) break;
          if (i == numRounds - 1) manager.isFinalRound = true;
          manager.playRound();
#ifdef USE_SQLITE
          if ((i + 1) % 1000 == 0) { manager.db.endTransaction(); manager.db.beginTransaction(); }
#endif
          // Real-time progress counter
          if ((i + 1) % 100 == 0 || i == numRounds - 1) {
              float progress = (float)(i + 1) / numRounds * 100.0f;
              std::cout << "\rProgress: " << (i + 1) << " / " << numRounds << " ("
                        << std::fixed << std::setprecision(1) << progress << "%)" << std::flush;
          }
        }
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.endTransaction();
#endif
        if (manager.autoExport) manager.stopStreaming();
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;
        std::cout << "\nSimulation complete in " << elapsed.count() << "s.\n";
        manager.printSummary();

        std::cout << "\n1. Run more rounds | 2. Return to Main Menu\n";
        int postChoice = safeInput<int>("Selection: ", 1, 2);
        if (postChoice == 2) simulationActive = false;
      }
    } else if (choice == 2) {
        // Interactive choice simplified logic
        std::cout << "Interactive Mode is active. (Simplified for testing)\n";
        std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();
    } else if (choice == 4) {
        // Log choice simplified logic
        std::cout << "Log Mode is active. (Simplified for testing)\n";
        std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();
    } else if (choice == 3) {
        std::cout << "Random Mode is active. (Simplified for testing)\n";
        std::cout << "Press Enter..."; std::cin.ignore(); std::cin.get();
    }
  }
  return 0;
}
