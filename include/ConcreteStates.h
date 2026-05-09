#ifndef CONCRETESTATES_H
#define CONCRETESTATES_H

#include "GameState.h"

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
    static GameState* getBettingState();
    static GameState* getDealingState();
    static GameState* getTradingState();
    static GameState* getEvalState();
};

#endif // CONCRETESTATES_H
