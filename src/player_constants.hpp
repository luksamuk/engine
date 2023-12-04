#ifndef PLAYER_CONSTANTS_HPP_INCLUDED
#define PLAYER_CONSTANTS_HPP_INCLUDED

namespace Player
{

    struct Constants
    {
        float gravity;
        float acceleration;
        float air_acceleration;
        float friction;
        float deceleration;
        float max_x_speed;
        float top_x_speed;
        float jump_strength;
        float min_jump_strength;
        float airdrag_division_factor;
        float airdrag_rem_factor;
        float airdrag_min_y_speed;
        float slope_factor;
        float rolling_friction;
        float rolling_deceleration;
        float rolling_top_x_speed;
        float rolling_min_x_speed;
        float unroll_min_x_speed;
    };

    const float BaseFrameRate = 60.0f;

    enum class Character : unsigned
    {
        Sonic,
        SuperSonic,
        Tails,
        Knuckles
    };

    enum class PhysicsMode : unsigned
    {
        Normal,
        Underwater,
        SpeedShoes
    };

    enum class ActionKind : unsigned
    {
        Idle,
        Jumping,
        Rolling,
        Skidding
    };

    struct State
    {
        ActionKind action;
        float      direction = 1.0f;
    };

    Constants GetConstants(Character, PhysicsMode);
}
    
#endif
