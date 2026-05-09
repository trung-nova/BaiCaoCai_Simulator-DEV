#include "../include/Player.h"
#include "../include/GameManager.h"
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

bool Player::isBaTien() const {
    return cachedBaTien;
}

int Player::getScore() const {
    return cachedScore;
}

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
    if (hand.size() == 3) {
        updateCachedValues();
    }
}

void Player::clearHand() {
    hand.clear();
    cachedScore = 0;
    cachedBaTien = false;
}

void Player::updateCachedValues() {
    if (hand.size() != 3) {
        cachedScore = 0;
        cachedBaTien = false;
        return;
    }

    // Check Ba Tien
    bool allFace = true;
    for (const auto& card : hand) {
        if (card.rank != Rank::JACK && card.rank != Rank::QUEEN && card.rank != Rank::KING) {
            allFace = false;
            break;
        }
    }
    cachedBaTien = allFace;

    if (cachedBaTien) {
        cachedScore = 10;
    } else {
        int sum = 0;
        for (const auto& card : hand) {
            sum += card.getModuloValue();
        }
        cachedScore = sum % 10;
    }
}

void Player::updateTiltStatus(GameManager* manager, int currentRound) {
    if (consecutiveLosses >= 5 || balance < startingBalance * 0.7f) {
        if (!isTilt) {
            isTilt = true;
            if (manager) {
                std::string archStr;
                switch (archetype) {
                    case Archetype::SHARK: archStr = "Shark"; break;
                    case Archetype::MANIAC: archStr = "Maniac"; break;
                    case Archetype::NIT: archStr = "Nit"; break;
                    default: archStr = "Normal"; break;
                }
                manager->tiltLogs.push_back("Round " + std::to_string(currentRound) + ", " + name + " (" + archStr + ") entered TILT state.");
            }
        }
        skillLevel = baseSkillLevel * 0.9f;
        confidenceLevel = std::min(1.0f, baseConfidenceLevel + 0.3f);
    } else {
        if (isTilt) {
            isTilt = false;
        }
        skillLevel = baseSkillLevel;
        confidenceLevel = baseConfidenceLevel;
    }
    updateSatisfactionTable();
}

void Player::updateSatisfactionTable() {
    float midpoint = 5.0f - (confidenceLevel * 2.0f);
    for (int i = 0; i <= 10; ++i) {
        satisfactionTable[i] = 1.0f / (1.0f + std::exp(-k * (i - midpoint)));
    }
}

// AIPlayer Implementation
AIPlayer::AIPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype)
    : Player(name, balance, skillLevel, confidenceLevel, tradeDesire, k, gamma, archetype) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng = std::mt19937(seed);
    dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

bool AIPlayer::wantsToTrade(int roundID, int swapTurn, bool logMode, bool showLogic, SwapRecord* outRecord) {
    if (isBaTien()) {
        if (logMode && showLogic) std::cout << "   [AI Logic] " << name << ": STAYS (Ba Tien - Unbeatable)\n";
        return false;
    }

    int score = getScore();
    float S = getSatisfaction(score);
    float D = tradeDesire;

    if (logMode && showLogic) {
        std::cout << "   [AI Logic] " << name << ": S=" << std::fixed << std::setprecision(2) << S << ", D=" << D;
    }

    // 1. Hard Limit: Max 3 successful swaps per round
    if (successfulSwapsCount >= 3) {
        if (logMode && showLogic) std::cout << " -> STAYS (Max swaps reached: " << successfulSwapsCount << ")\n";
        return false;
    }

    // 2. Strategic Stop: score >= 5 OR High Satisfaction (S > 0.75) after turn 1
    if (swapTurn > 1 && (score >= 5 || S > 0.75f)) {
        if (logMode && showLogic) {
            std::string reason = (score >= 5) ? "Score >= 5" : "High satisfaction";
            std::cout << " -> STAYS (" << reason << " constraint applied)\n";
        }
        return false;
    }

    // 3. Probabilistic Decision
    float p_psych = 0.5f * (1.0f - S) + 0.5f * D;
    
    // Add "Confidence Greed/Fear" factor
    float greedFactor = confidenceLevel * S * 0.3f; 
    p_psych += greedFactor;
    p_psych = std::max(0.0f, std::min(1.0f, p_psych));

    float ev_decision = (skillLevel > 0.5f) ? (1.0f - S) : 0.5f;
    float final_probability = (skillLevel * ev_decision) + ((1.0f - skillLevel) * p_psych);
    final_probability = std::max(0.05f, std::min(0.95f, final_probability));

    bool decision = dist(rng) < final_probability;

    if (logMode && showLogic) {
        std::cout << ", P_psych=" << p_psych 
                  << ", P_rational=" << ev_decision
                  << ", P_final=" << final_probability 
                  << (decision ? " -> WANTS TO TRADE\n" : " -> STAYS (Probability roll)\n");
    }

    if (outRecord) {
        *outRecord = {roundID, name, swapTurn, S, D, final_probability, decision};
    }

    return decision;
}

void AIPlayer::updateTradeDesire(int swapTurn) {
    float S = getSatisfaction(getScore());
    
    // Confidence-based aggression: high confidence players want to swap more to reach perfection
    float aggression = 0.5f + (0.5f * confidenceLevel);
    
    if (swapTurn == 1) {
        // Initial desire: inverse to satisfaction, scaled by aggression
        tradeDesire = (1.0f - S) * aggression;
    } else {
        // Incremental desire: builds faster for aggressive (confident) players
        tradeDesire += 0.15f * (1.0f - S) * aggression;
        
        // "Greed" factor: even if somewhat satisfied, high confidence players keep pushing
        if (confidenceLevel > 0.7f && S > 0.5f) {
            tradeDesire += 0.1f * S; // Greedily seeking perfection (Ba Tien)
        }
    }
    
    // Clamp to [0, 1]
    tradeDesire = std::max(0.0f, std::min(1.0f, tradeDesire));
}

Card* AIPlayer::getCardToTrade() {
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].rank == Rank::JACK || hand[i].rank == Rank::QUEEN || 
            hand[i].rank == Rank::KING || hand[i].rank == Rank::TEN) {
            return &hand[i];
        }
    }
    return &hand[0];
}

Player* AIPlayer::pickSwapPartner(const std::vector<Player*>& candidates) {
    if (candidates.empty()) return nullptr;
    std::uniform_int_distribution<int> selectDist(0, candidates.size() - 1);
    return candidates[selectDist(rng)];
}

// HumanPlayer Implementation
HumanPlayer::HumanPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype)
    : Player(name, balance, skillLevel, confidenceLevel, tradeDesire, k, gamma, archetype) {
    isHuman = true;
}

bool HumanPlayer::wantsToTrade(int roundID, int swapTurn, bool logMode, bool showLogic, SwapRecord* outRecord) {
    std::cout << name << ", your hand: ";
    for (const auto& c : hand) std::cout << c.toString() << " ";
    std::cout << "(Score: " << getScore() << "). Want to trade? (y/n): ";
    char choice;
    std::cin >> choice;
    bool decision = choice == 'y' || choice == 'Y';
    if (outRecord) {
        *outRecord = {roundID, name, swapTurn, getSatisfaction(getScore()), tradeDesire, 1.0f, decision};
    }
    return decision;
}

Card* HumanPlayer::getCardToTrade() {
    std::cout << "Your current hand: ";
    for (size_t i = 0; i < hand.size(); ++i) {
        std::cout << "[" << i << "] " << hand[i].toString() << "  ";
    }
    std::cout << "\nWhich card to trade? (0-2): ";
    int index;
    std::cin >> index;
    if (index < 0 || index >= (int)hand.size()) index = 0;
    return &hand[index];
}

void HumanPlayer::updateTradeDesire(int swapTurn) {
    // Humans manage their own desire
}

Player* HumanPlayer::pickSwapPartner(const std::vector<Player*>& candidates) {
    if (candidates.empty()) return nullptr;
    std::cout << "\nChoose a partner to trade with:\n";
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::cout << i << ". " << candidates[i]->name << "\n";
    }
    std::cout << candidates.size() << ". Skip trade\n";
    std::cout << "Selection: ";
    int choice;
    std::cin >> choice;
    if (choice >= 0 && choice < (int)candidates.size()) {
        return candidates[choice];
    }
    return nullptr;
}
