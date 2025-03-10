#pragma once

#include "Level.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_END
};

class Game
{
private:
    GameState               m_State;
    unsigned int            m_Width, m_Height;
    bool                    m_Keys[1024];
    std::vector<Level>      m_Levels;
    unsigned int            m_CurrLevel;

    void ResetLevel();
    void ResetPlayer();
    void CheckCollisions();
public:
    Game(unsigned int width, unsigned int height);
    ~Game();
    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render(float time);
    void SetKey(int key, bool val);
};
