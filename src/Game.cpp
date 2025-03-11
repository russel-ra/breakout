#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Game.h"
#include "Shader.h"
#include "Texture.h"
#include "SpriteRenderer.h"
#include "Level.h"
#include "Ball.h"

SpriteRenderer* Renderer;

Object* Player;
const glm::vec2 PLAYER_SIZE(100.0f, 10.0f);
const float PLAYER_VELOCITY(500.0f);

const float BALL_RADIUS = 12.5f;
const glm::vec2 INITIAL_BALL_VELOCITY(200.0f, 300.0f);

Ball* BallObject;

Game::Game(unsigned int width, unsigned int height)
    : m_State(GAME_ACTIVE), m_Keys(), m_Width(width), m_Height(height)
{

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    Shader spriteShader("res/shaders/vertex.shader", "res/shaders/fragment.shader");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width),
        static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
    spriteShader.SetUniform1i("image", 0);
    spriteShader.SetUniformMat4f("projection", projection);
    Renderer = new SpriteRenderer(spriteShader);

    Level one;
    one.Load("res/levels/lvl1.txt", m_Width, m_Height / 2);
    m_Levels.push_back(one);
    m_CurrLevel = 0;

    Texture Paddle = Texture("res/textures/paddle.png");
    glm::vec2 playerPos = glm::vec2(
        m_Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        m_Height - PLAYER_SIZE.y
    );
    Player = new Object(playerPos, PLAYER_SIZE, Paddle, glm::vec3(1.0f));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
        -BALL_RADIUS * 2.0f);
    BallObject = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        Texture("res/textures/ball.png"));
}

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

typedef std::tuple<bool, Direction, glm::vec2> Collision;

bool CollisionCheck(Object& one, Object& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CollisionCheck(Ball& one, Object& two) // AABB - Circle collision
{
    // get center point circle first 
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    if (glm::length(difference) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::CheckCollisions()
{
    for (Object& box : m_Levels[m_CurrLevel].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CollisionCheck(*BallObject, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    BallObject->Velocity.x = -BallObject->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = BallObject->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        BallObject->Position.x += penetration; // move ball to right
                    else
                        BallObject->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    BallObject->Velocity.y = -BallObject->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = BallObject->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        BallObject->Position.y -= penetration; // move ball back up
                    else
                        BallObject->Position.y += penetration; // move ball back down
                }
            }
        }
    }

    Collision result = CollisionCheck(*BallObject, *Player);
    if (!BallObject->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (BallObject->Position.x + BallObject->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = BallObject->Velocity;
        BallObject->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //BallObject->Velocity.y = -BallObject->Velocity.y;
        BallObject->Velocity.y = -1.0f * abs(BallObject->Velocity.y); // THIS ONLY WORKS BECAUSE THE PADDLE IS AT THE BOTTOM
        BallObject->Velocity = glm::normalize(BallObject->Velocity) * glm::length(oldVelocity);
    }
}

void Game::Update(float dt)
{
    BallObject->Move(dt, m_Width);
    CheckCollisions();

    if (BallObject->Position.y >= m_Height) // did ball reach bottom edge?
    {
        ResetLevel();
        ResetPlayer();
    }
}

void Game::ResetLevel()
{
    // TODO: fix this for when we have multiple levels
    if (m_CurrLevel == 0)
        m_Levels[0].Load("res/levels/lvl1.txt", m_Width, m_Height / 2);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(m_Width / 2.0f - PLAYER_SIZE.x / 2.0f, m_Height - PLAYER_SIZE.y);
    BallObject->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

void Game::ProcessInput(float dt)
{
    if (m_State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        if (m_Keys[GLFW_KEY_A] || m_Keys[GLFW_KEY_LEFT])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (BallObject->Stuck)
                {
                    BallObject->Position.x -= velocity;
                }
            }
        }
        if (m_Keys[GLFW_KEY_D] || m_Keys[GLFW_KEY_RIGHT])
        {
            if (Player->Position.x + Player->Size.x <= m_Width)
            {
                Player->Position.x += velocity;
                if (BallObject->Stuck)
                {
                    BallObject->Position.x += velocity;
                }
            }
        }
        if (m_Keys[GLFW_KEY_SPACE])
        {
            BallObject->Stuck = false;
        }
        if (m_Keys[GLFW_KEY_R])
        {
            ResetLevel();
            ResetPlayer();
        }
    }
}

void Game::Render(float time)
{
    if (m_State == GAME_ACTIVE)
    {
        //std::cout << "active" << std::endl;
        m_Levels[m_CurrLevel].Draw(*Renderer);
        Player->Draw(*Renderer);
        BallObject->Draw(*Renderer);
    }
}

void Game::SetKey(int key, bool val)
{
    m_Keys[key] = val;
}