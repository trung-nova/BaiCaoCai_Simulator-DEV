#ifndef GAMESTATE_H
#define GAMESTATE_H

class GameManager; // Forward declaration

class GameState {
public:
    virtual ~GameState() {}
    virtual void update(GameManager* manager) = 0;
};

#endif // GAMESTATE_H
