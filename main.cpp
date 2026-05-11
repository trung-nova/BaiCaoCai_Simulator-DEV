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

std::shared_ptr<Player> createAI(int id, Archetype arch, float meanS, float meanC, float stdDev, GameManager& manager, std::mt19937& gen) {
    std::normal_distribution<float> sD(meanS, stdDev), cD(meanC, stdDev);
    float skill = std::max(0.0f, std::min(1.0f, sD(gen)));
    float conf = std::max(-1.0f, std::min(1.0f, cD(gen)));
    auto& cfg = manager.archetypeConfigs[arch];
    return std::make_shared<AIPlayer>("AI_" + std::to_string(id), 10000, skill, conf, cfg.greedThreshold, cfg.k, cfg.gamma, arch, gen());
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
    if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
    if (choice == 0) break;

    GameManager manager;
    if (!manager.loadConfig("config.ini")) {
        std::cout << "Press Enter to exit..."; std::cin.ignore(1000, '\n'); std::cin.get();
        return 1;
    }

    unsigned seed;
    if (manager.simulationSeed >= 0) { seed = (unsigned)manager.simulationSeed; std::cout << "[System] Using fixed seed from config: " << seed << "\n"; }
    else {
        long long manualSeed = safeInput<long long>("Enter simulation seed (-1 for random): ", -1, 999999999999);
        if (manualSeed == -1) { seed = std::chrono::system_clock::now().time_since_epoch().count(); std::cout << "[System] Using random seed: " << seed << "\n"; }
        else { seed = (unsigned)manualSeed; std::cout << "[System] Using manual seed: " << seed << "\n"; }
    }
    manager.simulationSeed = (long long)seed;
    std::mt19937 gen(seed);

    if (choice == 1) {
      std::cout << "\n1. Persistent Mode | 2. Reset Mode\nSelection: ";
      int subMode = safeInput<int>("", 1, 2);
      int nP = safeInput<int>("Enter number of players (2-17): ", 2, 17);
      float minS = safeInput<float>("Min Skill (0-1): ", 0.0f, 1.0f);
      float maxS = safeInput<float>("Max Skill (0-1): ", minS, 1.0f);
      float stdDev = 1.0f / safeInput<float>("Skill Concentration (1-100): ", 1.0f, 100.0f);
      float sP = safeInput<float>("% Shark: ", 0.0f, 100.0f), mP = safeInput<float>("% Maniac: ", 0.0f, 100.0f - sP);

      std::vector<std::shared_ptr<Player>> players;
      std::discrete_distribution<int> archDist({sP, mP, 100.0f - sP - mP});
      for (int i = 0; i < nP; ++i) {
        int aIdx = archDist(gen);
        Archetype arch = (aIdx == 0) ? Archetype::SHARK : (aIdx == 1 ? Archetype::MANIAC : Archetype::NIT);
        float mS = (arch == Archetype::SHARK) ? maxS : (arch == Archetype::MANIAC ? minS : (minS + maxS) / 2.0f);
        players.push_back(createAI(i + 1, arch, mS, (arch == Archetype::SHARK ? 0.2f : (arch == Archetype::MANIAC ? 0.8f : -0.5f)), stdDev, manager, gen));
      }
      manager.setPlayers(players);
      bool simActive = true;
      while (simActive) {
        if (subMode == 2) { for (auto& p : manager.players) p->resetStats(); manager.roundCount = 0; }
        int numR = safeInput<int>("Number of rounds: ", 1, 10000000);
        manager.simulationParams = "Seed: " + std::to_string(manager.simulationSeed) + "\nMode: Standard\nRounds: " + std::to_string(numR);
        auto tStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numR; ++i) {
          if (subMode == 2) { for (auto& p : manager.players) { p->setBalance(10000); p->setEliminated(false); } }
          int activeCount = 0; for (auto& p : manager.players) if (!p->getIsEliminated()) activeCount++;
          if (activeCount <= 1) break;
          if (i == numR - 1) manager.isFinalRound = true;
          manager.playRound();
          if ((i + 1) % 100 == 0 || i == numR - 1) {
              float progress = (float)(i + 1) / numR * 100.0f;
              std::cout << "\rProgress: " << (i + 1) << " / " << numR << " (" << std::fixed << std::setprecision(1) << progress << "%)" << std::flush;
          }
        }
        auto tEnd = std::chrono::high_resolution_clock::now();
        std::cout << "\nSimulation complete in " << std::chrono::duration<double>(tEnd - tStart).count() << "s.\n";
        manager.printSummary();
        std::cout << "\n1. Run more | 2. Menu\nSelection: ";
        if (safeInput<int>("", 1, 2) == 2) simActive = false;
      }
    } else if (choice == 2 || choice == 4) {
      bool isLog = (choice == 4);
      int nP = safeInput<int>("Number of players (2-17): ", 2, 17);
      std::vector<std::shared_ptr<Player>> players;
      if (!isLog) { std::string hN; std::cout << "Enter your name: "; std::cin >> hN; players.push_back(std::make_shared<HumanPlayer>(hN, 10000, -1.0f, 0.0f, 0.0f)); }
      for (int i = players.size(); i < nP; ++i) {
          Archetype arch = (i % 3 == 0) ? Archetype::SHARK : (i % 3 == 1 ? Archetype::MANIAC : Archetype::NIT);
          float mS = (arch == Archetype::SHARK) ? 0.8f : (arch == Archetype::MANIAC ? 0.2f : 0.5f);
          players.push_back(createAI(i + 1, arch, mS, 0.0f, 0.1f, manager, gen));
      }
      manager.setPlayers(players); manager.logMode = true;
      int numR = isLog ? 1 : safeInput<int>("Number of rounds: ", 1, 100);
      for (int i = 0; i < numR; ++i) { if (i == numR - 1) manager.isFinalRound = true; manager.playRound(); }
      manager.printSummary();
      std::cout << "Press Enter..."; std::cin.ignore(1000, '\n'); std::cin.get();
    } else if (choice == 3) {
      int batches = safeInput<int>("Number of batches: ", 1, 1000);
      for (int b = 0; b < batches; ++b) {
          int nP = std::uniform_int_distribution<int>(2, 17)(gen);
          std::vector<std::shared_ptr<Player>> players;
          for (int i = 0; i < nP; ++i) players.push_back(createAI(i + 1, Archetype::NORMAL, 0.5f, 0.0f, 0.2f, manager, gen));
          manager.setPlayers(players); manager.isFinalRound = true; manager.playRound();
          if ((b + 1) % 10 == 0) std::cout << "\rBatch Progress: " << (b + 1) << " / " << batches << std::flush;
      }
      std::cout << "\nDone. Press Enter..."; std::cin.ignore(1000, '\n'); std::cin.get();
    }
  }
  return 0;
}
