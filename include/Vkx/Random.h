#if !defined(VKX_RANDOM_H)
#define VKX_RANDOM_H

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <random>

//! @defgroup Random Random Number Generator Types
//! Miscellaneous types that can generate randomized mathematical entities.

namespace Vkx
{
//! A LCG pseudo-random number generator that generates directions.
//!
//! @ingroup Random

class RandomDirection
{
public:
    //! Constructor.
    RandomDirection(float phi = 0.0f, float theta = 0.0f);

    //! Returns a unit vector with a random direction limited to a pyramid centered on the X axis.
    glm::vec3 operator ()(std::minstd_rand & rng);

private:
    float phi_;
    float theta_;
    std::uniform_real_distribution<float> randomFloat_;
};

//! A LCG pseudo-random number generator that generates orientations.
//!
//! @ingroup Random

class RandomOrientation
{
public:
    //! Constructor.
    RandomOrientation(float phi = 0.0f, float theta = 0.0f, float psi = glm::pi<float>());

    //! Returns a limited random orientation .
    glm::quat operator ()(std::minstd_rand & rng);

private:
    RandomDirection randomDirection_;
    std::uniform_real_distribution<float> randomAngle_;
};
} // namespace Vkx

#endif // !defined(VKX_RANDOM_H)
