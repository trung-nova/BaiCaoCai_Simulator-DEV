#include "Player.h"
#include "GameManager.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>

Player::Player(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype)
    : name(name), balance(balance), isDealer(false), skillLevel(skillLevel), confidenceLevel(confidenceLevel), tradeDesire(tradeDesire), k(k), gamma(gamma),
      archetype(archetype), baseSkillLevel(skillLevel), baseConfidenceLevel(confidenceLevel), startingBalance(balance), consecutiveLosses(0), isTilt(false), isHuman(false) {
    updateSatisfactionTable();
}

bool Player::isBaTien() const { return cachedBaTien; }
int Player::getScore() const { return cachedScore; }

float Player::getSatisfaction(int score) const {
    if (score < 0) return 0.0f;
    if (score > 10) return 1.0f;
    return satisfactionTable[score];
}

void Player::swapCard(Card* out, Card* in) {
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].suit == out->suit && hand[i].rank == out->rank) {
            hand[i] = *in;
            updateCachedValues();
            return;
        }
    }
}

void Player::receiveCard(Card c) {
    hand.push_back(c);
    if (hand.size() == 3) updateCachedValues();
}

void Player::clearHand() {
    hand.clear();
    cachedScore = 0;
    cachedBaTien = false;
    hasStayed = false;
}

void Player::updateCachedValues() {
    if (hand.size() != 3) { cachedScore = 0; cachedBaTien = false; return; }
    bool allFace = true;
    for (const auto& card : hand) {
        if (card.rank != Rank::JACK && card.rank != Rank::QUEEN && card.rank != Rank::KING) {
            allFace = false;
            break;
        }
    }
    cachedBaTien = allFace;
    if (cachedBaTien) { cachedScore = 10; }
    else {
        int sum = 0;
        for (const auto& card : hand) sum += card.getModuloValue();
        cachedScore = sum % 10;
    }
}

void Player::updateTiltStatus(GameManager* manager, int currentRound) {
    if (manager && !manager->enableTilt) {
        if (isTilt) {
            isTilt = false;
            if (manager) {
                manager->tiltLogs.push_back("Round " + std::to_string(currentRound) + ", " + getName() + " (" + getArchetypeString() + ") exited TILT state (System Disabled).");
            }
        }
        skillLevel = baseSkillLevel;
        confidenceLevel = baseConfidenceLevel;
        return;
    }

    if (consecutiveLosses >= 5 || balance < startingBalance * 0.7f) {
        if (!isTilt) {
            isTilt = true;
            if (manager) {
                manager->tiltLogs.push_back("Round " + std::to_string(currentRound) + ", " + getName() + " (" + getArchetypeString() + ") entered TILT state.");
            }
        }
        skillLevel = baseSkillLevel * 0.9f;
        confidenceLevel = std::min(1.0f, baseConfidenceLevel + 0.3f);
    } else {
        if (isTilt) {
            isTilt = false;
            if (manager) {
                manager->tiltLogs.push_back("Round " + std::to_string(currentRound) + ", " + getName() + " (" + getArchetypeString() + ") exited TILT state.");
            }
        }
        skillLevel = baseSkillLevel;
        confidenceLevel = baseConfidenceLevel;
    }
    updateSatisfactionTable();
}

void Player::updateSatisfactionTable() {
    // gamma shifts the expectation: higher gamma (risk aversion) means higher scores needed for satisfaction
    float midpoint = (5.0f - (confidenceLevel * 2.0f)) + (gamma - 2.0f);
    for (int i = 0; i <= 10; ++i) {
        satisfactionTable[i] = 1.0f / (1.0f + std::exp(-k * (i - midpoint)));
    }
}

AIPlayer::AIPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype, unsigned seed)
    : Player(name, balance, skillLevel, confidenceLevel, tradeDesire, k, gamma, archetype) {
    if (seed == 0) {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    rng = std::mt19937(seed);
    dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

TradeDecision AIPlayer::wantsToTrade(int roundID, int swapTurn, bool logMode, bool showLogic, SwapRecord* outRecord) {
    int score = getScore();
    float S = getSatisfaction(score);
    float D = tradeDesire;

    auto fillRecord = [&](TradeDecision dec, float prob) {
        if (outRecord) {
            outRecord->roundID = roundID;
            outRecord->playerName = name;
            outRecord->turn = swapTurn;
            outRecord->satisfaction = S;
            outRecord->desire = D;
            outRecord->probability = prob;
            outRecord->decision = dec;
            outRecord->scoreBefore = score;
        }
    };

    if (isBaTien()) { 
        if (showLogic) std::cout << "    [Logic] " << name << ": Ba Tiên -> STAY\n";
        fillRecord(TradeDecision::STAY, 0.0f); return TradeDecision::STAY; 
    }
    if (successfulSwapsCount >= 3) { 
        if (showLogic) std::cout << "    [Logic] " << name << ": Max Swaps -> STAY\n";
        fillRecord(TradeDecision::STAY, 0.0f); return TradeDecision::STAY; 
    }
    
    // Logic Dằn (STAY): Nếu bài quá đẹp (S > 0.9)
    if (S > 0.9f || score >= 9) {
        if (showLogic) std::cout << "    [Logic] " << name << ": S=" << S << " -> STAY\n";
        fillRecord(TradeDecision::STAY, 0.0f);
        return TradeDecision::STAY;
    }

    float p_psych = 0.5f * (1.0f - S) + 0.5f * D + (confidenceLevel * S * 0.3f);
    p_psych = std::max(0.0f, std::min(1.0f, p_psych));
    float ev_decision = (skillLevel > 0.5f) ? (1.0f - S) : 0.5f;
    float final_probability = (skillLevel * ev_decision) + ((1.0f - skillLevel) * p_psych);
    final_probability = std::max(0.05f, std::min(0.95f, final_probability));

    if (showLogic) {
        std::cout << "    [Logic] " << name << ": S=" << std::fixed << std::setprecision(2) << S 
                  << ", D=" << D << ", P=" << final_probability << " -> ";
    }

    if (dist(rng) < final_probability) {
        if (showLogic) std::cout << "TRADE\n";
        fillRecord(TradeDecision::TRADE, final_probability);
        return TradeDecision::TRADE;
    }

    if (showLogic) std::cout << "SKIP\n";
    fillRecord(TradeDecision::SKIP, final_probability);
    return TradeDecision::SKIP;
}

void AIPlayer::updateTradeDesire(int swapTurn) {
    float S = getSatisfaction(getScore());
    float aggression = 0.5f + (0.5f * confidenceLevel);
    if (swapTurn == 1) tradeDesire = (1.0f - S) * aggression;
    else {
        tradeDesire += 0.15f * (1.0f - S) * aggression;
        if (confidenceLevel > 0.7f && S > 0.5f) tradeDesire += 0.1f * S;
    }
    tradeDesire = std::max(0.0f, std::min(1.0f, tradeDesire));
}

Card* AIPlayer::getCardToTrade() {
    if (hand.empty()) return nullptr;

    int faceCount = 0;
    int nonFaceIdx = -1;
    bool hasAce = false;

    for (int i = 0; i < (int)hand.size(); ++i) {
        if (hand[i].isFaceCard()) faceCount++;
        else {
            nonFaceIdx = i;
            if (hand[i].isAce()) hasAce = true;
        }
    }

    // [New Logic] Ba Tien Hunting: If skill > 0.7 and has 2 faces, throw the 3rd card (if not Ace)
    if (skillLevel > 0.7f && faceCount == 2 && nonFaceIdx != -1 && !hasAce) {
        return &hand[nonFaceIdx];
    }

    // [Fallback] Old logic: Discard 0-value cards first (10, J, Q, K)
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].rank == Rank::JACK || hand[i].rank == Rank::QUEEN || hand[i].rank == Rank::KING || hand[i].rank == Rank::TEN)
            return &hand[i];
    }
    
    // Final fallback: discard the first card
    return &hand[0];
}

Player* AIPlayer::pickSwapPartner(const std::vector<Player*>& candidates) {
    if (candidates.empty()) return nullptr;
    std::uniform_int_distribution<int> selectDist(0, (int)candidates.size() - 1);
    return candidates[selectDist(rng)];
}

HumanPlayer::HumanPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype)
    : Player(name, balance, skillLevel, confidenceLevel, tradeDesire, k, gamma, archetype) { isHuman = true; }

TradeDecision HumanPlayer::wantsToTrade(int roundID, int swapTurn, bool logMode, bool showLogic, SwapRecord* outRecord) {
    if (hasStayed) return TradeDecision::STAY;
    std::cout << getName() << ", your hand: ";
    for (const auto& c : hand) std::cout << c.toString() << " ";
    std::cout << "(Score: " << getScore() << "). \n[T] Trade, [S] Skip turn, [D] Stay/Dằn: ";
    char choice; std::cin >> choice;
    
    TradeDecision decision = TradeDecision::SKIP;
    if (choice == 't' || choice == 'T') decision = TradeDecision::TRADE;
    else if (choice == 'd' || choice == 'D') {
        decision = TradeDecision::STAY;
        hasStayed = true;
    }

    if (outRecord) {
        outRecord->roundID = roundID;
        outRecord->playerName = getName();
        outRecord->turn = swapTurn;
        outRecord->satisfaction = getSatisfaction(getScore());
        outRecord->desire = tradeDesire;
        outRecord->probability = 1.0f;
        outRecord->decision = decision;
        outRecord->scoreBefore = getScore();
    }
    return decision;
}

Card* HumanPlayer::getCardToTrade() {
    std::cout << "Your current hand: ";
    for (size_t i = 0; i < hand.size(); ++i) std::cout << "[" << i << "] " << hand[i].toString() << "  ";
    std::cout << "\nWhich card to trade? (0-2): ";
    int index; std::cin >> index;
    if (index < 0 || index >= (int)hand.size()) index = 0;
    return &hand[index];
}

void HumanPlayer::updateTradeDesire(int swapTurn) {}

Player* HumanPlayer::pickSwapPartner(const std::vector<Player*>& candidates) {
    if (candidates.empty()) return nullptr;
    std::cout << "\nChoose a partner to trade with:\n";
    for (size_t i = 0; i < candidates.size(); ++i) std::cout << i << ". " << candidates[i]->getName() << "\n";
    std::cout << candidates.size() << ". Skip trade\nSelection: ";
    int choice; std::cin >> choice;
    if (choice >= 0 && choice < (int)candidates.size()) return candidates[choice];
    return nullptr;
}
