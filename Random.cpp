#include "Random.h"

// #include <Misc/Assertx.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <random>

namespace Vkx
{
//! If both phi and theta are 0, then unlimited directions are generated.
//! If theta is 0, then directions within phi radians from the X axis are generated.
//! If both phi and theta are not 0, then directions within phi radians from X in the XY plane and within theta radians from X in
//! the XZ plane are generated.
//!
//! @param  phi     Max angle from X axis (Valid range is [0, pi], default: 0)
//! @param  theta   Max angle from X axis in radians in the XZ plane (Valid range is [0, pi], default: 0)

RandomDirection::RandomDirection(float phi /*= 0.0f*/, float theta /*= 0.0f*/)
    : phi_((phi == 0.0f && theta == 0.0f) ? glm::pi<float>() : phi)
    , theta_(theta)
    , randomFloat_(0.0f, 1.0f)
{
//     assert_limits(0.0f, phi, glm::pi<float>());
//     assert_limits(0.0f, theta, glm::pi<float>());
}

//! Source: http://mathworld.wolfram.com/SpherePointPicking.html
//!
//!      To obtain points such that any small area on the sphere is expected to contain the same number of points
//!      (right figure above), choose U and V to be random variates on (0,1). Then
//!
//!          theta = 2pi u
//!          phi = acos(2v - 1)
//!
//! @param 	rng     Source of entropy
//!
//! @return     Random direction

glm::vec3 RandomDirection::operator ()(std::minstd_rand & rng)
{

    float u = randomFloat_(rng);
    float v = randomFloat_(rng);

    glm::vec3 d;

    //  if theta is 0, then generate directions in a cone around the X axis
    if (theta_ == 0.0f)
    {
        float t  = glm::two_pi<float>() * u;
        float st = sin(t);
        float ct = cos(t);

        float cp = 1.0f - (1.0f - cos(phi_)) * v;
        float sp = sqrtf(1.0f - cp * cp);
        d = { cp, ct * sp, st * sp };
    }

    // Otherwise, generate directions in a section around the X axis limited by phi in the XY plane and theta in the XZ plane
    else
    {
        float t  = theta_ * (2.0f * u - 1.0f);
        float st = sin(t);
        float ct = cos(t);

        float cp = sin(phi_) * (2.0f * v - 1.0f);
        float sp = sqrtf(1.0f - cp * cp);
        d = { ct * sp, st * sp, cp };
    }

    return d;
}

//! If both phi and theta are 0, then unlimited directions are generated.
//! If theta is 0, then directions within phi radians from the X axis are generated.
//! If both phi and theta are not 0, then directions within phi radians from X in the XY plane and within theta radians from X in
//! the XZ plane are generated.
//!
//! @param  phi     Max angle from X axis (Valid range is [0, pi], default: 0)
//! @param  theta   Max angle from X axis in radians in the XZ plane (Valid range is [0, pi], default: 0)
//! @param  psi     Max angle in radians around the axis in each direction (Valid range is [0, pi], default: 0)

RandomOrientation::RandomOrientation(float phi /*= 0.0f*/, float theta /*= 0.0f*/, float psi /*= glm::pi<float>()*/)
    : randomDirection_(phi, theta)
    , randomAngle_(-psi, psi)
{
//     assert_limits(0.0f, psi, glm::pi<float>());
}

//! @param 	rng     Source of entropy
//!
//! @return     Random direction

glm::quat RandomOrientation::operator ()(std::minstd_rand & rng)
{
    return glm::angleAxis(randomAngle_(rng), randomDirection_(rng));
}
} // namespace Vkx
