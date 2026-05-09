#include "../include/ConcreteStates.h"
#include "../include/GameManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>

static std::mt19937 state_rng(std::chrono::system_clock::now().time_since_epoch().count());

void BettingState::update(GameManager* manager) {
    if (manager->logMode) {
        GameManager::clearScreen();
        std::cout << BOLD << YELLOW << "========================================" << RESET << "\n";
        std::cout << BOLD << YELLOW << "            NEW ROUND STARTING          " << RESET << "\n";
        std::cout << BOLD << YELLOW << "========================================" << RESET << "\n";
    }

    int ante = 1; 
    for (auto* player : manager->players) {
        player->clearHand();
        player->successfulSwapsCount = 0;
        if (player->isEliminated) continue;

        if (!player->isDealer) {
            if (player->balance < ante) {
                player->isEliminated = true;
                continue;
            }
            player->balance -= ante;
            manager->currentPot += ante;
        }
    }
    manager->deck.reset();
    manager->changeState(StateFactory::getDealingState());
}

void DealingState::update(GameManager* manager) {
    if (manager->logMode) {
        std::cout << CYAN << "[System] Dealing cards..." << RESET << "\n";
    }
    for (int i = 0; i < 3; ++i) {
        for (auto* player : manager->players) {
            if (player->isEliminated) continue;
            player->receiveCard(manager->deck.drawCard());
        }
    }
    manager->changeState(StateFactory::getTradingState());
}

void TradingState::update(GameManager* manager) {
    if (manager->logMode) {
        std::cout << BOLD << MAGENTA << "\n--- TRADING PHASE (3 Turns) ---" << RESET << "\n";
    }

    bool hasHuman = false;
    for (auto* p : manager->players) if (p->isHuman) { hasHuman = true; break; }

    for (int swapTurn = 1; swapTurn <= 3; ++swapTurn) {
        if (manager->logMode) {
            std::cout << CYAN << "\n>> Swap Turn " << swapTurn << " starting..." << RESET << "\n";
        }

        std::vector<Player*> willingToTrade;
        
        for (auto* player : manager->players) {
            if (player->isEliminated) continue;
            
            // Update desire based on current dissatisfaction
            player->updateTradeDesire(swapTurn);
            
            bool isHuman = player->isHuman;

            if (manager->logMode) {
                // Show hands in log mode to track evolution across turns
                if (isHuman || !hasHuman) {
                    std::cout << (player->isDealer ? YELLOW : WHITE) << "Player: " << std::left << std::setw(5) << player->name << RESET 
                              << " | Hand: ";
                    for (const auto& c : player->hand) std::cout << c.toString() << " ";
                    std::cout << "| Score: " << BOLD << player->getScore() << RESET << "\n";
                }
            }

            int roundID = manager->roundCount; // Use manager->roundCount
            bool showLogic = manager->logMode && !hasHuman;
            
            SwapRecord sr;
            if (player->wantsToTrade(roundID, swapTurn, manager->logMode, showLogic, &sr)) {
                if (manager->logMode && !isHuman && hasHuman) {
                    std::cout << WHITE << "Player: " << std::left << std::setw(5) << player->name << RESET 
                              << " | Hand: [Hidden] (Wants to trade)\n";
                }
                willingToTrade.push_back(player);
            }
            
            if (manager->isStreaming) {
#ifndef USE_SQLITE
                manager->streamSwap << sr.roundID << "," << sr.playerName << "," << sr.turn << "," 
                                    << sr.satisfaction << "," << sr.desire << "," 
                                    << sr.probability << "," << (sr.swapped ? 1 : 0) << "\n";
#endif
#ifdef USE_SQLITE
                manager->db.insertSwap(sr.roundID, sr.playerName, sr.turn, sr.satisfaction, sr.desire, sr.probability, sr.swapped);
#endif
            }
        }
        
        if (willingToTrade.size() < 2) {
            if (manager->logMode) {
                if (willingToTrade.size() == 1) {
                    std::cout << CYAN << "[System] Only " << willingToTrade[0]->name << " wants to trade. Waiting for others..." << RESET << "\n";
                } else {
                    std::cout << CYAN << "[System] No one wants to trade this turn." << RESET << "\n";
                }
            }
            continue; // Move to next turn, desire will increase
        } else {
            std::vector<bool> paired(manager->players.size(), false);
            std::shuffle(willingToTrade.begin(), willingToTrade.end(), state_rng);

            for (auto* pA : willingToTrade) {
                // Find index of pA in manager->players
                int idxA = -1;
                for(int i=0; i<manager->players.size(); ++i) if(manager->players[i] == pA) { idxA = i; break; }
                
                if (idxA == -1 || paired[idxA]) continue;
                
                std::vector<Player*> candidates;
                for (auto* pB : willingToTrade) {
                    if (pB == pA) continue;
                    int idxB = -1;
                    for(int i=0; i<manager->players.size(); ++i) if(manager->players[i] == pB) { idxB = i; break; }
                    if (idxB != -1 && !paired[idxB]) {
                        candidates.push_back(pB);
                    }
                }
                
                if (candidates.empty()) continue;
                
                Player* chosenPartner = pA->pickSwapPartner(candidates);
                if (chosenPartner) {
                    int idxB = -1;
                    for(int i=0; i<manager->players.size(); ++i) if(manager->players[i] == chosenPartner) { idxB = i; break; }

                    if (manager->logMode && hasHuman) {
                         std::cout << GREEN << ">> ACTION: " << pA->name << " is trading cards with " << chosenPartner->name << RESET << "\n";
                    }

                    Card* cardA = pA->getCardToTrade();
                    Card* cardB = chosenPartner->getCardToTrade();
                    
                    if (manager->logMode && !hasHuman) {
                         std::cout << GREEN << ">> ACTION: " << pA->name << " swaps " << cardA->toString() 
                                   << " with " << chosenPartner->name << "'s " << cardB->toString() << RESET << "\n";
                    }
                    
                    Card copyA = *cardA;
                    Card copyB = *cardB;
                    
                    pA->swapCard(cardA, &copyB);
                    chosenPartner->swapCard(cardB, &copyA);
                    pA->successfulSwapsCount++;
                    chosenPartner->successfulSwapsCount++;
                    
                    paired[idxA] = true;
                    if (idxB != -1) paired[idxB] = true;
                }
            }
        }
    }
    
    manager->changeState(StateFactory::getEvalState());
}

void EvalState::update(GameManager* manager) {
    Player* dealer = nullptr;
    for (auto* p : manager->players) {
        if (p->isDealer) {
            dealer = p;
            break;
        }
    }

    int dealerScore = dealer->getScore();
    std::vector<Player*> winners;
    
    bool captureHands = manager->logMode || manager->isFinalRound;
    
    if (captureHands) {
        for (auto* p : manager->players) {
            if (p->isEliminated) continue;
            std::string handStr = "";
            std::string handStrPlain = "";
            for (const auto& c : p->hand) {
                handStr += c.toString(true) + " ";
                handStrPlain += c.toString(false) + " ";
            }
            p->lastHand = handStr;
            p->lastHandPlain = handStrPlain;
            p->lastScore = p->getScore();

            if (manager->logMode) {
                std::cout << (p->isDealer ? YELLOW : WHITE) << (p->isDealer ? "Dealer " : "Player ") 
                          << std::left << std::setw(5) << p->name << " Hand: " << p->lastHand;
                std::cout << " (Score: " << p->lastScore << ")" << RESET << "\n";
            }
        }
    }

    std::string scoresSummary = "";
    if (manager->isStreaming) {
        scoresSummary = dealer->name + ":" + std::to_string(dealerScore);
        for (auto* p : manager->players) {
            if (!p->isDealer && !p->isEliminated) {
                int ps = p->getScore();
                scoresSummary += " " + p->name + ":" + std::to_string(ps);
                if (manager->logMode) {
                    std::cout << "Player " << std::left << std::setw(5) << p->name << " score: " << ps;
                    if (ps > dealerScore) {
                        std::cout << BOLD << GREEN << " (WINS)" << RESET;
                    } else {
                        std::cout << RED << " (LOSES)" << RESET;
                    }
                    std::cout << "\n";
                }
                if (ps > dealerScore) {
                    winners.push_back(p);
                    p->wins++;
                    p->consecutiveLosses = 0;
                } else {
                    p->consecutiveLosses++;
                }
            }
        }
    } else {
        for (auto* p : manager->players) {
            if (!p->isDealer && !p->isEliminated) {
                int ps = p->getScore();
                if (ps > dealerScore) {
                    winners.push_back(p);
                    p->wins++;
                    p->consecutiveLosses = 0;
                } else {
                    p->consecutiveLosses++;
                }
            }
        }
    }

    int roundNum = manager->roundCount;
    if (manager->isStreaming) {
#ifndef USE_SQLITE
        manager->streamRound << roundNum << "," << dealer->name << "," << manager->currentPot << "," 
                             << (int)winners.size() << "," << "\"" << scoresSummary << "\"\n";
        
        manager->streamHistory << roundNum;
        for (auto* p : manager->players) manager->streamHistory << "," << p->balance;
        manager->streamHistory << "\n";
        
        // Auto-flush for data integrity
        manager->streamRound.flush();
        manager->streamHistory.flush();
        manager->streamSwap.flush();
#endif
#ifdef USE_SQLITE
        manager->db.insertRound(roundNum, dealer->name, manager->currentPot, (int)winners.size(), scoresSummary);
#endif
    }

    int ante = 1; // Assuming constant ante for now
    int totalLosers = 0;
    for (auto* p : manager->players) {
        if (!p->isDealer && !p->isEliminated) {
            bool isWinner = false;
            for (auto* w : winners) if (w == p) { isWinner = true; break; }
            
            if (isWinner) {
                // Winner gets their ante back + 1 from dealer (if dealer has money)
                int payment = std::min(ante, dealer->balance);
                p->balance += payment + ante;
                dealer->balance -= payment;
                if (manager->logMode) {
                    if (payment < ante) {
                        std::cout << YELLOW << p->name << " wins, but Dealer is BANKRUPT! Takes remaining " << payment << " chips (Net " << (payment > 0 ? "+" : "") << (payment - ante + 1) << ")" << RESET << "\n";
                    } else {
                        std::cout << GREEN << p->name << " wins and takes 2 chips (Net +1)" << RESET << "\n";
                    }
                }
            } else {
                // Loser already lost their ante, dealer collects it
                dealer->balance += ante;
                totalLosers++;
                if (manager->logMode) std::cout << RED << p->name << " loses ante to Dealer" << RESET << "\n";
            }
        }
    }

    if (winners.empty()) {
        dealer->wins++;
        dealer->consecutiveLosses = 0;
        if (manager->logMode) std::cout << BOLD << YELLOW << "Dealer wins against everyone!" << RESET << "\n";
    } else {
        dealer->consecutiveLosses++;
    }

    manager->currentPot = 0;

    // Eliminate bankrupt players and check Tilt
    int currentRound = manager->roundCount;
    for (auto* p : manager->players) {
        if (p->balance <= 0) {
            p->isEliminated = true;
        } else {
            p->updateTiltStatus(manager, currentRound);
        }
    }

    // Rotate dealer to next ACTIVE player
    int oldDealerIndex = manager->currentDealerIndex;
    manager->players[oldDealerIndex]->isDealer = false;
    
    int nextDealer = (oldDealerIndex + 1) % manager->players.size();
    int searchCount = 0;
    while (manager->players[nextDealer]->isEliminated && searchCount < manager->players.size()) {
        nextDealer = (nextDealer + 1) % manager->players.size();
        searchCount++;
    }
    
    manager->currentDealerIndex = nextDealer;
    manager->players[manager->currentDealerIndex]->isDealer = true;

    if (manager->logMode) {
        std::cout << "\nNew Dealer: " << BOLD << YELLOW << manager->players[manager->currentDealerIndex]->name << RESET << "\n";
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }

    manager->changeState(nullptr); 
}

// StateFactory Implementation
static BettingState bettingState;
static DealingState dealingState;
static TradingState tradingState;
static EvalState evalState;

GameState* StateFactory::getBettingState() { return &bettingState; }
GameState* StateFactory::getDealingState() { return &dealingState; }
GameState* StateFactory::getTradingState() { return &tradingState; }
GameState* StateFactory::getEvalState() { return &evalState; }
