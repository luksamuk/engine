#include "player_constants.hpp"

const Player::Constants default_values = {
    0.21875f,   // Gravity
    0.046875f,  // Acceleration
    0.09375f,   // Air Acceleration
    0.046875f,  // Friction
    0.5f,       // Deceleration
    12.0f,      // Max X Speed
    6.0f,       // Top X Speed
    -6.5f,      // Jump Strength
    -4.0f,      // Minimum Jump Strength
    0.96875f,   // Air Drag Factor
    0.0125f,    // Air Drag Minimum X value
    -4.0f,      // Air Drag Minimum Y value
    0.125f,     // Slope Factor

    0.0234375f, // Rolling Friction
    0.125f,     // Rolling Deceleration
    16.0f,      // Rolling Top X Speed
    1.03125f,   // Rolling Minimum X Speed
    0.046875f   // Unroll Minimum X Speed
};

// Speed Shoes
const Player::Constants speedshoes_values = {
    0.21875f,
    0.09375f,    // Acceleration
    0.1875f,     // Air Acceleration
    0.09375f,    // Friction
    0.5f,
    12.0f,
    12.0f,
    -6.5f,
    -4.0f,
    0.96875f,
    0.0125f,
    -4.0f,
    0.125f,

    0.046875f,  // Rolling Friction
    0.125f,
    16.0f,
    1.03125f,
    0.046875f
};

const Player::Constants underwater_values = {
    0.0625f,     // Gravity
    0.0234375f,  // Acceleration
    0.046875f,   // Air Acceleration
    0.0234375f,  // Friction
    0.25f,       // Deceleration
    12.0f,	     
    3.0f,        // Top X Speed
    -3.5f,
    -2.0f,
    0.96875f,
    0.0125f,
    -4.0f,
    0.125f,

    0.01171875f, // Rolling Friction
    0.125f,
    16.0f,
    1.03125f,
    0.046875f
};

const Player::Constants super_values = {
    0.21875f,
    0.1875f,   // Acceleration
    0.375f,    // Air Acceleration
    0.046875f,
    1.0f,      // Deceleration
    12.0f,
    10.0f,     // Top X Speed
    -8.0f,     // Jump Strength
    -4.0f,
    0.96875f,
    0.0125f,
    -4.0f,
    0.125f,

    0.0234375f,
    0.125f,
    16.0f,
    1.03125f,
    0.046875f
};

const Player::Constants super_underwater_values = {
    0.0625f,   // Gravity
    0.09375f,  // Acceleration
    0.1875f,   // Air Acceleration
    0.046875f, // Friction
    0.5f,      // Deceleration
    12.0f,
    5.0f,      // Top X Speed
    -3.5f,
    -2.0f,
    0.96875f,
    0.0125f,
    -4.0f,
    0.125f,

    0.0234375f,
    0.125f,
    16.0f,
    1.03125f,
    0.046875f
};

namespace Player
{
    Constants
    GetConstants(Character c, PhysicsMode m)
    {
        switch(c) {
        case Character::Sonic:
            switch(m) {
            case PhysicsMode::Normal:     return default_values;
            case PhysicsMode::Underwater: return underwater_values;
            case PhysicsMode::SpeedShoes: return speedshoes_values;
            };
            break;
        case Character::SuperSonic:
            switch(m) {
            case PhysicsMode::Normal:     return super_values;
            case PhysicsMode::Underwater: return super_underwater_values;
            case PhysicsMode::SpeedShoes: return speedshoes_values;
            };
            break;
        case Character::Tails:
            switch(m) {
            case PhysicsMode::Normal:     return default_values;
            case PhysicsMode::Underwater: return underwater_values;
            case PhysicsMode::SpeedShoes: return speedshoes_values;
            };
            break;
        case Character::Knuckles:
        {
            Constants c;
            switch(m) {
            case PhysicsMode::Normal:     c = default_values;    break;
            case PhysicsMode::Underwater: c = underwater_values; break;
            case PhysicsMode::SpeedShoes: c = speedshoes_values; break;
            };

            // Reduce jump strength (which is negative) by 0.5
            c.jump_strength += .5f;
            return c;
        }
        break;
        };

        // Return all constants as zero if reaching here
        return {};
    }
}
