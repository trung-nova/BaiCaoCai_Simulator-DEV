#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <random>
#include "Card.h"

typedef std::vector<Card> Hand;

enum class TradeDecision { SKIP = 0, TRADE = 1, STAY = 2 };

struct SwapRecord {
    int roundID = 0;
    std::string playerName = "";
    int turn = 0;
    float satisfaction = 0.0f;
    float desire = 0.0f;
    float probability = 0.0f;
    TradeDecision decision = TradeDecision::SKIP;
    int scoreBefore = 0;
    int scoreAfter = 0;
    std::string cardOut = "";
    std::string cardIn = "";

    SwapRecord() = default;
};

enum class Archetype { SHARK, MANIAC, NIT, NORMAL };


class Player {
    friend class GameManager;
    friend class BettingState;
    friend class DealingState;
    friend class TradingState;
    friend class EvalState;

protected:
    std::string name;
    Hand hand;
    int balance;
    bool isDealer;
    float skillLevel;
    float confidenceLevel;
    float tradeDesire;
    float k;
    float gamma;
    int wins = 0;
    int roundsPlayed = 0;
    int successfulSwapsCount = 0;
    bool isEliminated = false;
    bool hasStayed = false;
    std::vector<int> bankrollHistory;

    Archetype archetype;
    float baseSkillLevel;
    float baseConfidenceLevel;
    bool isTilt = false;
    int consecutiveLosses = 0;
    int startingBalance;
    std::string lastHand = "---";
    std::string lastHandPlain = "---";
    int lastScore = 0;

    bool isHuman;
    int cachedScore = 0;
    bool cachedBaTien = false;
    float satisfactionTable[11];

public:
    Player(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k = 1.0f, float gamma = 2.0f, Archetype archetype = Archetype::NORMAL);
    virtual ~Player() {}

    // Getters
    std::string getName() const { return name; }
    int getBalance() const { return balance; }
    float getSkillLevel() const { return skillLevel; }
    float getConfidenceLevel() const { return confidenceLevel; }
    bool getIsEliminated() const { return isEliminated; }
    Archetype getArchetype() const { return archetype; }
    std::string getArchetypeString() const {
        switch (archetype) {
            case Archetype::SHARK: return "Shark";
            case Archetype::MANIAC: return "Maniac";
            case Archetype::NIT: return "Nit";
            default: return "Normal";
        }
    }
    bool isHumanPlayer() const { return isHuman; }
    std::vector<int>& getBankrollHistory() { return bankrollHistory; }

    // Set
    void setBalance(int b) { balance = b; }
    void setEliminated(bool e) { isEliminated = e; }
    void resetStats() { 
        wins = 0; 
        roundsPlayed = 0; 
        lastHand = "---"; 
        lastHandPlain = "---"; 
        lastScore = 0; 
    }

    int getScore() const;
    bool isBaTien() const;
    float getSatisfaction(int score) const;
    void swapCard(Card* out, Card* in);
    void receiveCard(Card c);
    void clearHand();
    void updateCachedValues();
    void updateSatisfactionTable();
    void updateTiltStatus(class GameManager* manager, int currentRound);

    // Polymorphic methods
    virtual TradeDecision wantsToTrade(int roundID, int swapTurn, bool logMode = false, bool showLogic = false, SwapRecord* outRecord = nullptr) = 0;
    virtual Card* getCardToTrade() = 0;
    virtual void updateTradeDesire(int swapTurn) = 0;
    virtual Player* pickSwapPartner(const std::vector<Player*>& candidates) = 0;
};

class AIPlayer : public Player {
private:
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

public:
    AIPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k = 1.0f, float gamma = 2.0f, Archetype archetype = Archetype::NORMAL, unsigned seed = 0);
    TradeDecision wantsToTrade(int roundID, int swapTurn, bool logMode = false, bool showLogic = false, SwapRecord* outRecord = nullptr) override;
    Card* getCardToTrade() override;
    void updateTradeDesire(int swapTurn) override;
    Player* pickSwapPartner(const std::vector<Player*>& candidates) override;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(const std::string& name, int balance, float skillLevel, float confidenceLevel, float tradeDesire, float k = 1.0f, float gamma = 2.0f, Archetype archetype = Archetype::NORMAL);
    TradeDecision wantsToTrade(int roundID, int swapTurn, bool logMode = false, bool showLogic = false, SwapRecord* outRecord = nullptr) override;
    Card* getCardToTrade() override;
    void updateTradeDesire(int swapTurn) override;
    Player* pickSwapPartner(const std::vector<Player*>& candidates) override;
};

#endif // PLAYER_H
