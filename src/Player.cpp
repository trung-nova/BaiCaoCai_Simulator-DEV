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
                manager->tiltLogs.push_back("Round " + std::to_string(currentRound) + ", " + getName() + " (" + archStr + ") entered TILT state.");
            }
        }
        skillLevel = baseSkillLevel * 0.9f;
        confidenceLevel = std::min(1.0f, baseConfidenceLevel + 0.3f);
    } else {
        isTilt = false;
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

AIPlayer::AIPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype, unsigned seed)
    : Player(name, balance, skillLevel, confidenceLevel, tradeDesire, k, gamma, archetype) {
    if (seed == 0) {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    rng = std::mt19937(seed);
    dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

bool AIPlayer::wantsToTrade(int roundID, int swapTurn, bool logMode, bool showLogic, SwapRecord* outRecord) {
    int score = getScore();
    float S = getSatisfaction(score);
    float D = tradeDesire;

    auto fillRecord = [&](bool dec, float prob) {
        if (outRecord) {
            outRecord->roundID = roundID;
            outRecord->playerName = name;
            outRecord->turn = swapTurn;
            outRecord->satisfaction = S;
            outRecord->desire = D;
            outRecord->probability = prob;
            outRecord->swapped = dec;
        }
    };

    if (isBaTien()) { fillRecord(false, 0.0f); return false; }
    if (successfulSwapsCount >= 3) { fillRecord(false, 0.0f); return false; }
    if (swapTurn > 1 && (score >= 5 || S > 0.75f)) { fillRecord(false, 0.0f); return false; }

    float p_psych = 0.5f * (1.0f - S) + 0.5f * D + (confidenceLevel * S * 0.3f);
    p_psych = std::max(0.0f, std::min(1.0f, p_psych));
    float ev_decision = (skillLevel > 0.5f) ? (1.0f - S) : 0.5f;
    float final_probability = (skillLevel * ev_decision) + ((1.0f - skillLevel) * p_psych);
    final_probability = std::max(0.05f, std::min(0.95f, final_probability));

    bool decision = dist(rng) < final_probability;
    fillRecord(decision, final_probability);
    return decision;
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
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].rank == Rank::JACK || hand[i].rank == Rank::QUEEN || hand[i].rank == Rank::KING || hand[i].rank == Rank::TEN)
            return &hand[i];
    }
    return &hand[0];
}

Player* AIPlayer::pickSwapPartner(const std::vector<Player*>& candidates) {
    if (candidates.empty()) return nullptr;
    std::uniform_int_distribution<int> selectDist(0, (int)candidates.size() - 1);
    return candidates[selectDist(rng)];
}

HumanPlayer::HumanPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma, Archetype archetype)
    : Player(name, balance, skillLevel, confidenceLevel, tradeDesire, k, gamma, archetype) { isHuman = true; }

bool HumanPlayer::wantsToTrade(int roundID, int swapTurn, bool logMode, bool showLogic, SwapRecord* outRecord) {
    std::cout << getName() << ", your hand: ";
    for (const auto& c : hand) std::cout << c.toString() << " ";
    std::cout << "(Score: " << getScore() << "). Want to trade? (y/n): ";
    char choice; std::cin >> choice;
    bool decision = choice == 'y' || choice == 'Y';
    if (outRecord) *outRecord = {roundID, getName(), swapTurn, getSatisfaction(getScore()), tradeDesire, 1.0f, decision};
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
