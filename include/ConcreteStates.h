#ifndef CONCRETESTATES_H
#define CONCRETESTATES_H

#include "GameState.h"
#include <memory>

class BettingState : public GameState {
public:
    void update(GameManager* manager) override;
};

class DealingState : public GameState {
public:
    void update(GameManager* manager) override;
};

class TradingState : public GameState {
public:
    void update(GameManager* manager) override;
};

class EvalState : public GameState {
public:
    void update(GameManager* manager) override;
};

class StateFactory {
public:
    static std::unique_ptr<GameState> getBettingState();
    static std::unique_ptr<GameState> getDealingState();
    static std::unique_ptr<GameState> getTradingState();
    static std::unique_ptr<GameState> getEvalState();
};

#endif // CONCRETESTATES_H
