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

struct SimParams {
    int nP; float minS, maxS, conc, sP, mP;
};

SimParams getParams() {
    SimParams p;
    p.nP = safeInput<int>("Enter number of players (2-17): ", 2, 17);
    p.minS = safeInput<float>("Min Skill (0-1): ", 0.0f, 1.0f);
    p.maxS = safeInput<float>("Max Skill (0-1): ", p.minS, 1.0f);
    p.conc = safeInput<float>("Skill Concentration (1-100): ", 1.0f, 100.0f);
    p.sP = safeInput<float>("% Shark: ", 0.0f, 100.0f);
    p.mP = safeInput<float>("% Maniac: ", 0.0f, 100.0f - p.sP);
    return p;
}

int main() {
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
      
      SimParams p = getParams();
      float stdDev = 1.0f / p.conc;
      std::vector<std::shared_ptr<Player>> players;
      if (isInter) {
          std::string hN; std::cout << "Enter your name: "; std::cin >> hN;
          players.push_back(std::make_shared<HumanPlayer>(hN, 10000, -1.0f, 0.0f, 0.0f));
      }

      std::discrete_distribution<int> archDist({p.sP, p.mP, 100.0f - p.sP - p.mP});
      for (int i = players.size(); i < p.nP; ++i) {
        int aIdx = archDist(gen);
        Archetype arch = (aIdx == 0) ? Archetype::SHARK : (aIdx == 1 ? Archetype::MANIAC : Archetype::NIT);
        float mS = (arch == Archetype::SHARK) ? p.maxS : (arch == Archetype::MANIAC ? p.minS : (p.minS + p.maxS) / 2.0f);
        players.push_back(createAI(i + 1, arch, mS, (arch == Archetype::SHARK ? 0.2f : (arch == Archetype::MANIAC ? 0.8f : -0.5f)), stdDev, manager, gen));
      }
      manager.setPlayers(players);
      if (isInter || isLog) manager.logMode = true;

      std::cout << "\n" << BOLD << CYAN << "--- Player Configuration ---" << RESET << "\n";
      for (const auto& pl : manager.players) {
        std::cout << std::left << std::setw(10) << pl->getName() << std::setw(10)
                  << (pl->isHumanPlayer() ? "Human" : "AI") << std::fixed << std::setprecision(2)
                  << std::setw(10) << pl->getSkillLevel() << std::setw(12) << pl->getBalance() << "\n";
      }

      bool simActive = true;
      while (simActive) {
        if (subMode == 2) { for (auto& pl : manager.players) pl->resetStats(); manager.roundCount = 0; }
        int numR = (isLog) ? 1 : safeInput<int>("Number of rounds: ", 1, 10000000);
        manager.simulationParams = "Seed: " + std::to_string(manager.simulationSeed) + "\nPlayers: " + std::to_string(p.nP);
        
        auto tS = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numR; ++i) {
          if (subMode == 2) { for (auto& pl : manager.players) { pl->setBalance(10000); pl->setEliminated(false); } }
          int active = 0; for (auto& pl : manager.players) if (!pl->getIsEliminated()) active++;
          if (active <= 1) break;
          if (i == numR - 1) manager.isFinalRound = true;
          manager.playRound();
          if (!isInter && !isLog && ((i + 1) % 100 == 0 || i == numR - 1)) {
              std::cout << "\rProgress: " << (i + 1) << " / " << numR << " (" << std::fixed << std::setprecision(1) << (float)(i+1)/numR*100.0f << "%)" << std::flush;
          }
        }
        std::cout << "\nDone in " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - tS).count() << "s.\n";
        manager.printSummary();
        if (isLog) break;
        std::cout << "\n1. Run more | 2. Menu\nSelection: ";
        if (safeInput<int>("", 1, 2) == 2) simActive = false;
      }
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
