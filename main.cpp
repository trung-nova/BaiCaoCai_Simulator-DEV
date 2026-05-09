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

double generate_skew_normal(double xi, double omega, double alpha,
                            std::mt19937 &gen) {
  std::normal_distribution<double> norm(0.0, 1.0);
  double u0 = norm(gen);
  double u1 = norm(gen);
  double x = (u1 > alpha * u0) ? u0 : -u0;
  return xi + omega * x;
}

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
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 gen(seed);

  while (true) {
    GameManager::clearScreen();
    std::cout << BOLD << BLUE
              << "========================================" << RESET << "\n";
    std::cout << BOLD << BLUE << "       BAI CAO CAI SIMULATOR v2.0       "
              << RESET << "\n";
    std::cout << BOLD << BLUE
              << "========================================" << RESET << "\n";
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

    if (choice == 0)
      break;

    if (choice == 1) {
      std::cout << "\nStandard Simulation Sub-modes:\n";
      std::cout
          << "1. Persistent Mode (Accumulate balance across multiple runs)\n";
      std::cout << "2. Reset Mode (Reset balances to 10,000 before each run)\n";
      int subMode = safeInput<int>("Selection: ", 1, 2);

      int numPlayers =
          safeInput<int>("Enter number of players (2-17): ", 2, 17);
      float minSkill =
          safeInput<float>("Enter minimum skill (0-1): ", 0.0f, 1.0f);
      float maxSkill =
          safeInput<float>("Enter maximum skill (0-1): ", minSkill, 1.0f);
      float concentration = safeInput<float>(
          "Enter skill concentration (1-100, higher = closer to average): ",
          1.0f, 100.0f);
      float stdDev = 1.0f / concentration;
      float sharkPct = safeInput<float>(
          "Enter % of Shark players (e.g. 20): ", 0.0f, 100.0f);
      float maniacPct = safeInput<float>(
          "Enter % of Maniac players (e.g. 30): ", 0.0f, 100.0f - sharkPct);
      float nitPct = 100.0f - sharkPct - maniacPct;

      std::vector<Player *> players;
      std::discrete_distribution<int> archetypeDist({sharkPct, maniacPct, nitPct});

      GameManager manager;
      manager.loadConfig("config.ini");

      std::cout << "\nEnable Research Reports (Data Export)? (y/n): ";
      char exportToggle;
      std::cin >> exportToggle;
      manager.autoExport = (exportToggle == 'y' || exportToggle == 'Y');

      for (int i = 0; i < numPlayers; ++i) {
        int archChoice = archetypeDist(gen);
        Archetype arch;
        float meanS = 0.5f, meanC = 0.0f;

        if (archChoice == 0) {
          arch = Archetype::SHARK;
          meanS = maxSkill;
          meanC = 0.2f;
        } else if (archChoice == 1) {
          arch = Archetype::MANIAC;
          meanS = minSkill;
          meanC = 0.8f;
        } else {
          arch = Archetype::NIT;
          meanS = (minSkill + maxSkill) / 2.0f;
          meanC = -0.5f;
        }

        std::normal_distribution<float> skillDist(meanS, stdDev);
        std::normal_distribution<float> confDist(meanC, stdDev);

        float skill = std::max(0.0f, std::min(1.0f, skillDist(gen)));
        float conf = std::max(-1.0f, std::min(1.0f, confDist(gen)));

        auto& cfg = manager.archetypeConfigs[arch];
        players.push_back(new AIPlayer("AI_" + std::to_string(i + 1), 10000,
                                       skill, conf, cfg.greedThreshold, 
                                       (arch == Archetype::NORMAL ? 1.0f : cfg.k), 
                                       (arch == Archetype::NORMAL ? 2.0f : cfg.gamma), arch));
      }

      manager.setPlayers(players);
      manager.logMode = false;

      bool simulationActive = true;
      while (simulationActive) {
        if (subMode == 2) {
          // Initial Reset for the batch
          for (auto *p : manager.players) {
            p->wins = 0;
            p->roundsPlayed = 0;
          }
          manager.tiltLogs.clear();
          manager.roundCount = 0;
        }

        int numRounds =
            safeInput<int>("Enter number of rounds to run: ", 1, 33554332);
        for (auto *p : manager.players)
          p->bankrollHistory.reserve(p->bankrollHistory.size() + numRounds + 1);

        // Display Player Info Table
        std::cout << "\n"
                  << BOLD << CYAN << "--- Player Configuration ("
                  << (subMode == 1 ? "Persistent" : "Reset") << ") ---" << RESET
                  << "\n";
        std::cout << std::left << std::setw(10) << "Name" << std::setw(10)
                  << "Type" << std::setw(10) << "Skill" << std::setw(12)
                  << "Confidence" << "Current Balance" << "\n";
        std::cout << "--------------------------------------------------\n";
        for (const auto *p : manager.players) {
          std::cout << std::left << std::setw(10) << p->name << std::setw(10)
                    << "AI" << std::fixed << std::setprecision(2)
                    << std::setw(10) << p->skillLevel << std::setw(12)
                    << p->confidenceLevel << std::setw(15) << p->balance << "\n";
        }
        std::cout << "--------------------------------------------------\n\n";

        std::cout << GREEN << "Starting simulation..." << RESET << "\n";
        std::ostringstream oss;
        oss << "Mode: Standard Simulation\n"
            << "SubMode: " << (subMode == 1 ? "Persistent" : "Reset") << "\n"
            << "Number of Rounds: " << numRounds << "\n"
            << "Players: " << numPlayers << "\n"
            << "Skill Configuration: "
            << "Min=" << std::fixed << std::setprecision(2) << minSkill
            << ", Max=" << maxSkill << ", Concentration=" << concentration
            << " (StdDev=" << stdDev << ")\n"
            << "Archetype Split: Shark=" << sharkPct
            << "%, Maniac=" << maniacPct << "%, Nit=" << nitPct << "%";
        manager.simulationParams = oss.str();
        manager.saveInitialState();
        if (manager.autoExport) manager.startStreaming();
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.beginTransaction();
#endif
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numRounds; ++i) {
          if (subMode == 2) {
            // Reset balances and status for EVERY round in Reset Mode
            for (auto *p : manager.players) {
              p->balance = 10000;
              p->isEliminated = false;
            }
          }

          // Check active players count
          int activeCount = 0;
          for (auto *p : manager.players)
            if (!p->isEliminated)
              activeCount++;

          if (activeCount <= 1) {
            std::cout << RED << "\n\n[NOTICE] Simulation stopped: Only "
                      << activeCount << " player(s) remain active." << RESET
                      << "\n";
            break;
          }

          if (i == numRounds - 1) {
            manager.isFinalRound = true;
          }
          manager.playRound();
#ifdef USE_SQLITE
          if ((i + 1) % 1000 == 0) {
            manager.db.endTransaction();
            manager.db.beginTransaction();
          }
#endif
          if ((i + 1) % 1000 == 0 || i == numRounds - 1) {
            float progress = (float)(i + 1) / numRounds * 100.0f;
            std::cout << "\rProgress: " << (i + 1) << " / " << numRounds << " ("
                      << std::fixed << std::setprecision(1) << progress << "%)"
                      << std::flush;
          }
        }
#ifdef USE_SQLITE
        if (manager.autoExport) manager.db.endTransaction();
#endif
        if (manager.autoExport) manager.stopStreaming();
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;
        std::cout << "\n\n"
                  << GREEN << "Simulation complete in " << std::fixed
                  << std::setprecision(2) << elapsed.count() << " seconds."
                  << RESET << "\n";

        manager.printSummary();

        if (manager.autoExport) {
          manager.exportResearchReports();
          std::cout << GREEN << "Reports generated automatically." << RESET << "\n";
        }

        std::cout << "\n1. Run more rounds | 3. Return to Main Menu\n";
        int postChoice = safeInput<int>("Selection: ", 1, 3);

        if (postChoice == 3)
          simulationActive = false;
      }
      continue; // Back to main menu while loop
    } else if (choice == 2) {
      int numPlayers =
          safeInput<int>("Enter number of players (2-17): ", 2, 17);
      std::vector<Player *> players;
      std::string humanName;
      std::cout << "Enter your name: ";
      std::cin >> humanName;

      // Human skill set to -1.0 to flag for "MANUAL" display
      players.push_back(new HumanPlayer(humanName, 10000, -1.0f, 0.0f, 0.0f));

      // Randomize simulation parameters for AI opponents
      std::uniform_real_distribution<float> minSDist(0.1f, 0.3f);
      std::uniform_real_distribution<float> maxSDist(0.6f, 0.9f);
      std::uniform_real_distribution<float> concDist(5.0f, 20.0f);

      float minSkill = minSDist(gen);
      float maxSkill = maxSDist(gen);
      float concentration = concDist(gen);
      float stdDev = 1.0f / concentration;

      std::uniform_real_distribution<float> pctDist(10.0f, 40.0f);
      float sharkPct = pctDist(gen);
      float maniacPct = pctDist(gen);
      float nitPct = 100.0f - sharkPct - maniacPct;

      std::discrete_distribution<int> archetypeDist(
          {sharkPct, maniacPct, nitPct});

      GameManager manager;
      manager.loadConfig("config.ini");

      for (int i = 1; i < numPlayers; ++i) {
        int archChoice = archetypeDist(gen);
        Archetype arch;
        float meanS = 0.5f, meanC = 0.0f;

        if (archChoice == 0) {
          arch = Archetype::SHARK;
          meanS = maxSkill;
          meanC = 0.2f;
        } else if (archChoice == 1) {
          arch = Archetype::MANIAC;
          meanS = minSkill;
          meanC = 0.8f;
        } else {
          arch = Archetype::NIT;
          meanS = (minSkill + maxSkill) / 2.0f;
          meanC = -0.5f;
        }

        std::normal_distribution<float> skillD(meanS, stdDev);
        std::normal_distribution<float> confD(meanC, stdDev);

        float skill = std::max(0.0f, std::min(1.0f, skillD(gen)));
        float conf = std::max(-1.0f, std::min(1.0f, confD(gen)));

        auto& cfg = manager.archetypeConfigs[arch];
        players.push_back(new AIPlayer("AI_" + std::to_string(i + 1), 10000,
                                       skill, conf, cfg.greedThreshold, 
                                       (arch == Archetype::NORMAL ? 1.0f : cfg.k), 
                                       (arch == Archetype::NORMAL ? 2.0f : cfg.gamma), arch));
      }

      manager.setPlayers(players);
      manager.logMode = true;

      int numRounds = safeInput<int>("Enter number of rounds: ", 1, 1000);

      std::ostringstream oss;
      oss << "Mode: Interactive Mode\n"
          << "AI Logic: Randomized GMM (Skill Range: " << std::fixed
          << std::setprecision(2) << minSkill << "-" << maxSkill
          << ", Conc=" << concentration << ")";
      manager.simulationParams = oss.str();
      manager.saveInitialState();

      std::cout << "\nEnable Research Reports (Data Export)? (y/n): ";
      char exportToggle;
      std::cin >> exportToggle;
      manager.autoExport = (exportToggle == 'y' || exportToggle == 'Y');

      for (int i = 0; i < numRounds; ++i) {
        GameManager::clearScreen();
        if (i == numRounds - 1) manager.isFinalRound = true;
        manager.playRound();
      }
      if (manager.autoExport) {
        manager.stopStreaming();
        manager.exportResearchReports();
        std::cout << GREEN << "Reports generated automatically." << RESET << "\n";
      }
      manager.printSummary();
      std::cout << "Press Enter to return to menu...";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cin.get();

    } else if (choice == 4) {
      int numPlayers =
          safeInput<int>("Enter number of players (2-17): ", 2, 17);
      float minSkill =
          safeInput<float>("Enter minimum skill (0-1): ", 0.0f, 1.0f);
      float maxSkill =
          safeInput<float>("Enter maximum skill (0-1): ", minSkill, 1.0f);
      float concentration =
          safeInput<float>("Enter skill concentration (1-100): ", 1.0f, 100.0f);
      float stdDev = 1.0f / concentration;
      float sharkPct = safeInput<float>("Enter % Shark: ", 0.0f, 100.0f);
      float maniacPct =
          safeInput<float>("Enter % Maniac: ", 0.0f, 100.0f - sharkPct);
      float nitPct = 100.0f - sharkPct - maniacPct;

      std::vector<Player *> players;
      std::discrete_distribution<int> archetypeDist(
          {sharkPct, maniacPct, nitPct});

      GameManager manager;
      manager.loadConfig("config.ini");

      for (int i = 0; i < numPlayers; ++i) {
        int archChoice = archetypeDist(gen);
        Archetype arch;
        float meanS = 0.5f, meanC = 0.0f;
        if (archChoice == 0) {
          arch = Archetype::SHARK;
          meanS = maxSkill;
          meanC = 0.2f;
        } else if (archChoice == 1) {
          arch = Archetype::MANIAC;
          meanS = minSkill;
          meanC = 0.8f;
        } else {
          arch = Archetype::NIT;
          meanS = (minSkill + maxSkill) / 2.0f;
          meanC = -0.5f;
        }

        std::normal_distribution<float> skillDist(meanS, stdDev);
        std::normal_distribution<float> confDist(meanC, stdDev);
        float skill = std::max(0.0f, std::min(1.0f, skillDist(gen)));
        float conf = std::max(-1.0f, std::min(1.0f, confDist(gen)));
        
        auto& cfg = manager.archetypeConfigs[arch];
        players.push_back(new AIPlayer("AI_" + std::to_string(i + 1), 10000,
                                       skill, conf, cfg.greedThreshold, 
                                       (arch == Archetype::NORMAL ? 1.0f : cfg.k), 
                                       (arch == Archetype::NORMAL ? 2.0f : cfg.gamma), arch));
      }

      manager.setPlayers(players);
      manager.logMode = true;
      std::cout << "\nEnable Research Reports (Data Export)? (y/n): ";
      char exportToggle;
      std::cin >> exportToggle;
      manager.autoExport = (exportToggle == 'y' || exportToggle == 'Y');
      
      manager.saveInitialState();

      if (manager.autoExport) manager.startStreaming();
      manager.isFinalRound = true;
      manager.playRound();
      if (manager.autoExport) {
        manager.stopStreaming();
        manager.exportResearchReports();
        std::cout << GREEN << "Reports generated automatically." << RESET << "\n";
      }
      
      manager.printSummary();
      std::cout << "Press Enter to return to menu...";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cin.get();

    } else if (choice == 3) {
      int numRounds = safeInput<int>(
          "Enter number of rounds for Random Mode: ", 1, 33554332);

      std::cout << "Running Random Mode...\n";
      std::uniform_int_distribution<int> pDist(2, 17);
      std::uniform_real_distribution<float> pctDist(0.0f, 100.0f);
      std::uniform_real_distribution<float> sdDist(0.01f, 0.2f);

      for (int r = 0; r < numRounds; ++r) {
        int n = pDist(gen);
        float sd = sdDist(gen);
        float sPct = pctDist(gen);
        float mPct =
            std::uniform_real_distribution<float>(0.0f, 100.0f - sPct)(gen);
        float nPct = 100.0f - sPct - mPct;

        std::discrete_distribution<int> archDist({sPct, mPct, nPct});
        std::vector<Player *> players;
        for (int i = 0; i < n; ++i) {
          int choice = archDist(gen);
          Archetype arch;
          float mS = 0.5f, mC = 0.0f;
          if (choice == 0) {
            arch = Archetype::SHARK;
            mS = 0.9f;
            mC = 0.2f;
          } else if (choice == 1) {
            arch = Archetype::MANIAC;
            mS = 0.2f;
            mC = 0.8f;
          } else {
            arch = Archetype::NIT;
            mS = 0.5f;
            mC = -0.5f;
          }

          std::normal_distribution<float> sD(mS, sd);
          std::normal_distribution<float> cD(mC, sd);
          float skill = std::max(0.0f, std::min(1.0f, sD(gen)));
          float conf = std::max(-1.0f, std::min(1.0f, cD(gen)));

          players.push_back(new AIPlayer("AI_" + std::to_string(i + 1), 10000,
                                         skill, conf, 0.0f, 1.0f, 2.0f, arch));
        }

        GameManager manager;
        manager.loadConfig("config.ini");

        std::cout << "\nEnable Research Reports (Data Export)? (y/n): ";
        char exportToggle;
        std::cin >> exportToggle;
        manager.autoExport = (exportToggle == 'y' || exportToggle == 'Y');

        manager.setPlayers(players);
        manager.isFinalRound = true;
        manager.playRound();

        if ((r + 1) % 100 == 0) {
          std::cout << "\rRandom Mode Progress: " << (r + 1) << " / "
                    << numRounds << std::flush;
        }
      }
      std::cout << "\nRandom Mode complete.\n";
      std::cout << "Press Enter to return to menu...";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cin.get();
    }
  }

  return 0;
}
