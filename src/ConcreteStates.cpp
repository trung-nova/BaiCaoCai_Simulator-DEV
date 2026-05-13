#include "ConcreteStates.h"
#include "GameManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>

static std::mt19937 state_rng(std::chrono::system_clock::now().time_since_epoch().count());

void BettingState::update(GameManager* manager) {
    if (manager->logMode) {
        GameManager::clearScreen();
        std::cout << "\033[1m\033[33m========================================\033[0m\n";
        std::cout << "\033[1m\033[33m            NEW ROUND STARTING          \033[0m\n";
        std::cout << "\033[1m\033[33m========================================\033[0m\n";
    }

    int ante = 1; 
    for (auto& player : manager->players) {
        player->clearHand();
        player->successfulSwapsCount = 0;
        if (player->getIsEliminated()) continue;

        if (!player->isDealer) {
            if (player->getBalance() < ante) {
                player->setEliminated(true);
                continue;
            }
            player->setBalance(player->getBalance() - ante);
            manager->currentPot += ante;
        }
    }
    manager->deck.reset();
    manager->changeState(StateFactory::getDealingState());
}

void DealingState::update(GameManager* manager) {
    if (manager->logMode) {
        std::cout << "\033[36m[System] Dealing cards...\033[0m\n";
    }
    for (int i = 0; i < 3; ++i) {
        for (auto& player : manager->players) {
            if (player->getIsEliminated()) continue;
            player->receiveCard(manager->deck.drawCard());
        }
    }
    manager->changeState(StateFactory::getTradingState());
}

void TradingState::update(GameManager* manager) {
    if (manager->logMode) {
        std::cout << "\033[1m\033[35m\n--- TRADING PHASE (3 Turns) ---\033[0m\n";
    }

    bool hasHuman = false;
    for (auto& p : manager->players) if (p->isHumanPlayer()) { hasHuman = true; break; }

    for (int swapTurn = 1; swapTurn <= 3; ++swapTurn) {
        if (manager->logMode) {
            std::cout << "\033[36m\n>> Swap Turn " << swapTurn << " starting...\033[0m\n";
        }

        std::vector<Player*> willingToTrade;
        std::map<Player*, SwapRecord> turnRecords;

        for (auto& player : manager->players) {
            if (player->getIsEliminated()) continue;
            
            // Nếu người chơi đã dằn (STAY) từ trước, bỏ qua lượt hỏi
            if (player->hasStayed) {
                SwapRecord sr;
                sr.roundID = manager->roundCount;
                sr.playerName = player->getName();
                sr.turn = swapTurn;
                sr.decision = TradeDecision::STAY;
                sr.scoreBefore = player->getScore();
                sr.scoreAfter = sr.scoreBefore;
                turnRecords[player.get()] = sr;
                continue;
            }

            player->updateTradeDesire(swapTurn);
            
            bool isHuman = player->isHumanPlayer();
            if (manager->logMode) {
                if (isHuman || !hasHuman) {
                    std::cout << (player->isDealer ? "\033[33m" : "\033[37m") << "Player: " << std::left << std::setw(5) << player->getName() << "\033[0m" 
                              << " | Hand: ";
                    for (const auto& c : player->hand) std::cout << c.toString() << " ";
                    std::cout << "| Score: \033[1m" << player->getScore() << "\033[0m\n";
                }
            }

            int roundID = manager->roundCount;
            bool showLogic = manager->logMode && !hasHuman;
            
            SwapRecord sr;
            TradeDecision decision = player->wantsToTrade(roundID, swapTurn, manager->logMode, showLogic, &sr);
            
            if (decision == TradeDecision::TRADE) {
                if (manager->logMode && !isHuman && hasHuman) {
                    std::cout << "\033[37mPlayer: " << std::left << std::setw(5) << player->getName() << "\033[0m" 
                              << " | Hand: [Hidden] (Wants to trade)\n";
                }
                willingToTrade.push_back(player.get());
            } else if (decision == TradeDecision::STAY) {
                player->hasStayed = true; 
                if (player->hasStayed) {
                std::cout << "\033[33m" << player->getName() << " has STAYED.\033[0m\n";
                continue;
                }
            } else {
                if (manager->logMode) {
                    std::cout << "\033[36m" << player->getName() << " chose to SKIP.\033[0m\n";
                }
            }

            turnRecords[player.get()] = sr;
        }
        
        if (willingToTrade.size() >= 2) {
            std::vector<Card*> outgoingCardPtrs;
            std::vector<Card> outgoingCardCopies;
            std::vector<std::string> outgoingCardStrs;
            
            for (auto* p : willingToTrade) {
                Card* c = p->getCardToTrade();
                outgoingCardPtrs.push_back(c);
                outgoingCardCopies.push_back(*c);
                outgoingCardStrs.push_back(c->toString(true));
            }

            std::vector<int> targetIndices(willingToTrade.size());
            std::iota(targetIndices.begin(), targetIndices.end(), 0);
            
            if (willingToTrade.size() == 2) {
                std::swap(targetIndices[0], targetIndices[1]);
            } else {
                bool isDerangement = false;
                while (!isDerangement) {
                    std::shuffle(targetIndices.begin(), targetIndices.end(), state_rng);
                    isDerangement = true;
                    for (size_t i = 0; i < targetIndices.size(); ++i) {
                        if (targetIndices[i] == (int)i) {
                            isDerangement = false;
                            break;
                        }
                    }
                }
            }

            if (manager->logMode) {
                std::cout << "\n\033[35m=== COMMON PLATE TRADING ===\033[0m\n";
                std::cout << "The following players put their cards in the common plate:\n";
                for (size_t i = 0; i < willingToTrade.size(); ++i) {
                    std::cout << "  - " << std::left << std::setw(8) << willingToTrade[i]->getName() << ": " << outgoingCardStrs[i] << "\n";
                }
                std::cout << "\033[35m-------------------------------------------\033[0m\n";
                std::cout << "Redistributing cards...\n";
            }

            for (size_t i = 0; i < willingToTrade.size(); ++i) {
                Player* p = willingToTrade[i];
                int fromIdx = targetIndices[i]; 
                
                Card newCard = outgoingCardCopies[fromIdx];
                std::string strOut = outgoingCardCopies[i].toString(false); 
                std::string strIn = newCard.toString(false); 

                if (manager->logMode) {
                    std::cout << "  \033[32m" << p->getName() << " receives " << newCard.toString(true) 
                              << " (from " << willingToTrade[fromIdx]->getName() << ")\033[0m\n";
                }

                p->swapCard(outgoingCardPtrs[i], &newCard);
                p->successfulSwapsCount++;

                turnRecords[p].cardOut = strOut;
                turnRecords[p].cardIn = strIn;
                turnRecords[p].scoreAfter = p->getScore();
            }
            
            if (manager->logMode) {
                std::cout << "\033[1m\033[35m===========================================\033[0m\n";
            }
        }
 else if (manager->logMode) {
            if (willingToTrade.size() == 1) {
                std::cout << "\033[36mOnly " << willingToTrade[0]->getName() << " wants to trade. Waiting for others...\033[0m\n";
            } else {
                std::cout << "\033[36mNo one wants to trade this turn.\033[0m\n";
            }
        }

        // Logging at the end of the turn
        if (manager->isStreaming) {
            for (auto& player : manager->players) {
                if (player->getIsEliminated()) continue;
                SwapRecord& sr = turnRecords[player.get()];
                if (sr.decision != TradeDecision::TRADE) sr.scoreAfter = sr.scoreBefore; // No change if skip/stay

#ifndef USE_SQLITE
                if (manager->isMode3) manager->streamSwap << manager->currentBatchID << ",";
                manager->streamSwap << sr.roundID << "," << sr.playerName << "," << sr.turn << "," 
                                    << sr.satisfaction << "," << sr.desire << "," 
                                    << sr.probability << "," << (int)sr.decision << ","
                                    << sr.scoreBefore << "," << sr.scoreAfter << ","
                                    << "\"" << sr.cardOut << "\",\"" << sr.cardIn << "\"\n";
#endif
#ifdef USE_SQLITE
                manager->db.insertSwap(sr.roundID, sr.playerName, sr.turn, sr.satisfaction, sr.desire, sr.probability, (int)sr.decision, sr.scoreBefore, sr.scoreAfter, sr.cardOut, sr.cardIn);
#endif
            }
        }
    }
    manager->changeState(StateFactory::getEvalState());
}

void EvalState::update(GameManager* manager) {
    Player* dealer = nullptr;
    for (auto& p : manager->players) {
        if (p->isDealer) { dealer = p.get(); break; }
    }

    int dealerScore = dealer->getScore();
    std::vector<Player*> winners;
    bool captureHands = manager->logMode || manager->isFinalRound;
    
    if (captureHands) {
        for (auto& p : manager->players) {
            if (p->getIsEliminated()) continue;
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
                std::cout << (p->isDealer ? "\033[33m" : "\033[37m") << (p->isDealer ? "Dealer " : "Player ") 
                          << std::left << std::setw(5) << p->getName() << " Hand: " << p->lastHand;
                std::cout << " (Score: " << p->lastScore << ")\033[0m\n";
            }
        }
    }

    std::string scoresSummary = "";
    scoresSummary = dealer->getName() + ":" + std::to_string(dealerScore);
    for (auto& p : manager->players) {
        if (!p->isDealer && !p->getIsEliminated()) {
            int ps = p->getScore();
            scoresSummary += " " + p->getName() + ":" + std::to_string(ps);
            if (manager->logMode) {
                std::cout << "Player " << std::left << std::setw(5) << p->getName() << " score: " << ps;
                if (ps > dealerScore) std::cout << "\033[1m\033[32m (WINS)\033[0m";
                else std::cout << "\033[31m (LOSES)\033[0m";
                std::cout << "\n";
            }
            if (ps > dealerScore) { winners.push_back(p.get()); p->wins++; p->consecutiveLosses = 0; }
            else { p->consecutiveLosses++; }
        }
    }

    int roundNum = manager->roundCount;
    if (manager->isStreaming) {
#ifndef USE_SQLITE
        if (manager->isMode3) manager->streamRound << manager->currentBatchID << ",";
        manager->streamRound << roundNum << "," << dealer->getName() << "," << manager->currentPot << "," 
                             << (int)winners.size() << "," << "\"" << scoresSummary << "\"\n";
        
        if (manager->isMode3) {
            for (auto& p : manager->players) {
                manager->streamHistory << manager->currentBatchID << "," << roundNum << "," << p->getName() << "," << p->getBalance() << "\n";
            }
        } else {
            manager->streamHistory << roundNum;
            for (auto& p : manager->players) manager->streamHistory << "," << p->getBalance();
            manager->streamHistory << "\n";
        }
        manager->streamRound.flush(); manager->streamHistory.flush(); manager->streamSwap.flush();
#endif

#ifdef USE_SQLITE
        manager->db.insertRound(roundNum, dealer->getName(), manager->currentPot, (int)winners.size(), scoresSummary);
#endif
    }

    int ante = 1;
    for (auto& p : manager->players) {
        if (!p->isDealer && !p->getIsEliminated()) {
            bool isWinner = false;
            for (auto* w : winners) if (w == p.get()) { isWinner = true; break; }
            
            if (isWinner) {
                int payment = std::min(ante, dealer->getBalance());
                p->setBalance(p->getBalance() + payment + ante);
                dealer->setBalance(dealer->getBalance() - payment);
            } else {
                dealer->setBalance(dealer->getBalance() + ante);
            }
        }
    }

    if (winners.empty()) { dealer->wins++; dealer->consecutiveLosses = 0; }
    else { dealer->consecutiveLosses++; }

    manager->currentPot = 0;
    int currentRound = manager->roundCount;
    for (auto& p : manager->players) {
        if (p->getBalance() <= 0) p->setEliminated(true);
        else p->updateTiltStatus(manager, currentRound);
    }

    int oldDealerIndex = manager->currentDealerIndex;
    manager->players[oldDealerIndex]->isDealer = false;
    int nextDealer = (oldDealerIndex + 1) % manager->players.size();
    int searchCount = 0;
    while (manager->players[nextDealer]->getIsEliminated() && searchCount < (int)manager->players.size()) {
        nextDealer = (nextDealer + 1) % manager->players.size();
        searchCount++;
    }
    manager->currentDealerIndex = nextDealer;
    manager->players[manager->currentDealerIndex]->isDealer = true;

    if (manager->logMode) {
        std::cout << "\nNew Dealer: \033[1m\033[33m" << manager->players[manager->currentDealerIndex]->getName() << "\033[0m\n";
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
    manager->changeState(nullptr); 
}

std::unique_ptr<GameState> StateFactory::getBettingState() { return std::make_unique<BettingState>(); }
std::unique_ptr<GameState> StateFactory::getDealingState() { return std::make_unique<DealingState>(); }
std::unique_ptr<GameState> StateFactory::getTradingState() { return std::make_unique<TradingState>(); }
std::unique_ptr<GameState> StateFactory::getEvalState() { return std::make_unique<EvalState>(); }
