#include "Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Vkx
{
/*==================================================================================================================*/
/*                                                  L I G H T                                                       */
/*==================================================================================================================*/

void Light::enable(bool enabled /* = true */)
{
    enabled_ = enabled;
}

#if 0
void setPointValues(glm::vec4 const & diffuse,
                    glm::vec4 const & specular,
                    glm::vec4 const & ambient,
                    glm::vec3 const & position,
                    float             range,
                    float             attenuation0,
                    float             attenuation1,
                    float             attenuation2)
{
    assert(range >= 0.0f && range <= sqrtf(FLT_MAX));
    assert(attenuation0 != 0.0f || attenuation1 != 0.0f || attenuation2 != 0.0f);

    // Set the values

    Type         = D3DLIGHT_POINT;
    Diffuse      = diffuse;
    Specular     = specular;
    Ambient      = ambient;
    Position     = position;
    Range        = range;
    Attenuation0 = attenuation0;
    Attenuation1 = attenuation1;
    Attenuation2 = attenuation2;

    // Tell D3D about the new values

    return pD3dDevice_->SetLight(id_, this);
}

//!
//! @param    all        Lighting parameters (See the docs for D3DLIGHT9)

void setDirectionalValues(glm::vec4 const & diffuse,
                          glm::vec4 const & specular,
                          glm::vec4 const & ambient,
                          glm::vec3 const & direction)
{
    assert(direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f);

    // Set the values

    Type      = D3DLIGHT_DIRECTIONAL;
    Diffuse   = diffuse;
    Specular  = specular;
    Ambient   = ambient;
    Direction = direction;

    // Tell D3D about the new values

    return pD3dDevice_->SetLight(id_, this);
}

//!
//! @param    all        Lighting parameters (See the docs for D3DLIGHT9)

void setSpotValues(glm::vec4 const & diffuse,
                   glm::vec4 const & specular,
                   glm::vec4 const & ambient,
                   glm::vec3 const & position,
                   glm::vec3 const & direction,
                   float             range,
                   float             falloff,
                   float             attenuation0,
                   float             attenuation1,
                   float             attenuation2,
                   float             theta,
                   float             phi)
{
    assert(direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f);
    assert(range >= 0.0f && range <= sqrtf(FLT_MAX));
    assert(attenuation0 != 0.0f || attenuation1 != 0.0f || attenuation2 != 0.0f);
    assert(theta > 0.0f && theta <= phi);
    assert(phi > 0.0f && phi < D3DX_PI);

    // Set the values

    Type         = D3DLIGHT_SPOT;
    Diffuse      = diffuse;
    Specular     = specular;
    Ambient      = ambient;
    Position     = position;
    Direction    = direction;
    Range        = range;
    Falloff      = falloff;
    Attenuation0 = attenuation0;
    Attenuation1 = attenuation1;
    Attenuation2 = attenuation2;
    Theta        = theta;
    Phi          = phi;

    // Tell D3D about the new values

    return pD3dDevice_->SetLight(id_, this);
}
#endif // if 0

/*==================================================================================================================*/
/*                                               A M B I E N T   L I G H T                                          */
/*==================================================================================================================*/

//! The default ambient light is initialized with the following properties:
//!
//!     - Ambient color          { 1.0f, 1.0f, 1.0f, 1.0f }
//!
//! @param  id      An identifier for this light.

AmbientLight::AmbientLight(int id)
    : Light(id, AMBIENT)
    , ambient_({ 1.0f, 1.0f, 1.0f, 1.0f })
{
}

//! @param  id          An identifier for this light.
//! @param  ambient     Ambient color

AmbientLight::AmbientLight(int               id,
                           glm::vec4 const & ambient)
    : Light(id, AMBIENT)
    , ambient_(ambient)
{
}

//!
//! @param    ambient        Ambient color

void AmbientLight::set(glm::vec4 const & ambient)
{
    setAmbientColor(ambient);
}

/*==================================================================================================================*/
/*                                               P O I N T   L I G H T                                              */
/*==================================================================================================================*/

//! The default constructor initializes a light with the following properties:
//!     - Diffuse        { 1.0f, 1.0f, 1.0f, 0.0f }
//!     - Specular       { 0.0f, 0.0f, 0.0f, 0.0f }
//!     - Ambient        { 0.0f, 0.0f, 0.0f, 0.0f }
//!     - Position       { 0.0f, 0.0f, 0.0f }
//!     - Range          sqrtf( FLT_MAX )
//!     - Attenuation0   1.0f
//!     - Attenuation1   0.0f
//!     - Attenuation2   0.0f
//!
//! @param  id      An identifier for this light.

PointLight::PointLight(int id)
    : Light(id, POINT)
    , ambient_({ 0.0f, 0.0f, 0.0f, 0.0f })
    , diffuse_({ 1.0f, 1.0f, 1.0f, 0.0f })
    , specular_({ 0.0f, 0.0f, 0.0f, 0.0f })
    , position_({ 0.0f, 0.0f, 0.0f })
    , range_(sqrtf(FLT_MAX))
    , attenuation0_(1.0f)
    , attenuation1_(0.0f)
    , attenuation2_(0.0f)
{
}

//! @param  id      An identifier for this light
//!
//! @note   Requirements for point lights:
//!             - 0.0f <= range <= sqrt(FLT_MAX).
//!             - One of the attenuation values must not be 0.

PointLight::PointLight(int               id,
                       glm::vec4 const & ambient,
                       glm::vec4 const & diffuse,
                       glm::vec4 const & specular,
                       glm::vec3 const & position,
                       float             range,
                       float             attenuation0,
                       float             attenuation1,
                       float             attenuation2)
    : Light(id, POINT)
    , ambient_(ambient)
    , diffuse_(diffuse)
    , specular_(specular)
    , position_(position)
    , range_(range)
    , attenuation0_(attenuation0)
    , attenuation1_(attenuation1)
    , attenuation2_(attenuation2)
{
    assert(range >= 0.0f && range <= sqrtf(FLT_MAX));
    assert(attenuation0 != 0.0f || attenuation1 != 0.0f || attenuation2 != 0.0f);
}

void PointLight::set(glm::vec4 const & ambient,
                     glm::vec4 const & diffuse,
                     glm::vec4 const & specular,
                     glm::vec3 const & position,
                     float             range,
                     float             attenuation0,
                     float             attenuation1,
                     float             attenuation2)
{
    assert(range >= 0.0f && range <= sqrtf(FLT_MAX));
    assert(attenuation0 != 0.0f || attenuation1 != 0.0f || attenuation2 != 0.0f);

    setAmbientColor(ambient);
    setDiffuseColor(diffuse);
    setSpecularColor(specular);
    setPosition(position);
    setRange(range);
    setAttenuation(attenuation0, attenuation1, attenuation2);
}

/*==================================================================================================================*/
/*                                       D I R E C T I O N A L   L I G H T                                          */
/*==================================================================================================================*/

//! The default constructor initializes a light with the following properties:
//!     - Ambient       { 0.0f, 0.0f, 0.0f, 0.0f }
//!     - Diffuse       { 1.0f, 1.0f, 1.0f, 0.0f }
//!     - Specular      { 0.0f, 0.0f, 0.0f, 0.0f }
//!     - Direction     { 0.0f, 0.0f, 1.0f }
//!
//! @param  id      An identifier for this light

DirectionalLight::DirectionalLight(int id)
    : Light(id, DIRECTIONAL)
    , ambient_({ 0.0f, 0.0f, 0.0f, 0.0f })
    , diffuse_({ 1.0f, 1.0f, 1.0f, 0.0f })
    , specular_({ 0.0f, 0.0f, 0.0f, 0.0f })
    , direction_({ 0.0f, 0.0f, 1.0f })
{
}

//! @param  id      An identifier for this light
//!
//! @note   Requirements for directional lights:
//!             - Direction must not be (0, 0, 0).

DirectionalLight::DirectionalLight(int               id,
                                   glm::vec4 const & ambient,
                                   glm::vec4 const & diffuse,
                                   glm::vec4 const & specular,
                                   glm::vec3 const & direction)
    : Light(id, DIRECTIONAL)
    , ambient_(ambient)
    , diffuse_(diffuse)
    , specular_(specular)
    , direction_(direction)
{
    assert(direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f);
}

//!
//! @param    all        Lighting parameters (See the docs for D3DLIGHT9)

void DirectionalLight::set(glm::vec4 const & ambient,
                           glm::vec4 const & diffuse,
                           glm::vec4 const & specular,
                           glm::vec3 const & direction)
{
    assert(direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f);

    setAmbientColor(ambient);
    setDiffuseColor(diffuse);
    setSpecularColor(specular);
    setDirection(direction);
}

/*==================================================================================================================*/
/*                                              S P O T   L I G H T                                                 */
/*==================================================================================================================*/

//! The default constructor initializes a light with the following properties:
//!     - Ambient            { 0.0f, 0.0f, 0.0f, 0.0f }
//!     - Diffuse            { 1.0f, 1.0f, 1.0f, 0.0f }
//!     - Specular           { 0.0f, 0.0f, 0.0f, 0.0f }
//!     - Position           { 0.0f, 0.0f, 0.0f }
//!     - Direction          { 0.0f, 0.0f, 1.0f }
//!     - Range              sqrtf( FLT_MAX )
//!     - Falloff            1.0f
//!     - Attenuation0       1.0f
//!     - Attenuation1       0.0f
//!     - Attenuation2       0.0f
//!     - Theta              pi/2
//!     - Phi                pi/2
//!
//! @param  id      An identifier for this light

SpotLight::SpotLight(int id)
    : Light(id, SPOT)
    , ambient_({ 0.0f, 0.0f, 0.0f, 0.0f })
    , diffuse_({ 1.0f, 1.0f, 1.0f, 0.0f })
    , specular_({ 0.0f, 0.0f, 0.0f, 0.0f })
    , position_({ 0.0f, 0.0f, 0.0f })
    , direction_({ 0.0f, 0.0f, 1.0f })
    , range_(sqrtf(FLT_MAX))
    , falloff_(1.0f)
    , attenuation0_(1.0f)
    , attenuation1_(0.0f)
    , attenuation2_(0.0f)
    , theta_(glm::half_pi<float>())
    , phi_(glm::half_pi<float>())
{
}

//! @param  id      An identifier for this light
//!
//! @note   Requirements for spot lights:
//!             - Direction must not be (0, 0, 0).
//!             - 0.0f <= range <= sqrt(FLT_MAX).
//!             - One of the attenuation values must not be 0.
//!             - 0.0f < theta <= phi
//!             - 0.0f < phi < pi.

SpotLight::SpotLight(int               id,
                     glm::vec4 const & ambient,
                     glm::vec4 const & diffuse,
                     glm::vec4 const & specular,
                     glm::vec3 const & position,
                     glm::vec3 const & direction,
                     float             range,
                     float             falloff,
                     float             attenuation0,
                     float             attenuation1,
                     float             attenuation2,
                     float             theta,
                     float             phi)
    : Light(id, SPOT)
    , ambient_(ambient)
    , diffuse_(diffuse)
    , specular_(specular)
    , position_(position)
    , direction_(direction)
    , range_(range)
    , falloff_(falloff)
    , attenuation0_(attenuation0)
    , attenuation1_(attenuation1)
    , attenuation2_(attenuation2)
    , theta_(theta)
    , phi_(phi)
{
    assert(direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f);
    assert(range >= 0.0f && range <= sqrtf(FLT_MAX));
    assert(attenuation0 != 0.0f || attenuation1 != 0.0f || attenuation2 != 0.0f);
    assert(theta > 0.0f && theta <= phi);
    assert(phi > 0.0f && phi < glm::pi<float>());
}

void SpotLight::set(glm::vec4 const & ambient,
                    glm::vec4 const & diffuse,
                    glm::vec4 const & specular,
                    glm::vec3 const & position,
                    glm::vec3 const & direction,
                    float             range,
                    float             falloff,
                    float             attenuation0,
                    float             attenuation1,
                    float             attenuation2,
                    float             theta,
                    float             phi)
{
    assert(direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f);
    assert(range >= 0.0f && range <= sqrtf(FLT_MAX));
    assert(attenuation0 != 0.0f || attenuation1 != 0.0f || attenuation2 != 0.0f);
    assert(theta > 0.0f && theta <= phi);
    assert(phi > 0.0f && phi < glm::pi<float>());

    setAmbientColor(ambient);
    setDiffuseColor(diffuse);
    setSpecularColor(specular);
    setPosition(position);
    setDirection(direction);
    setRange(range);
    setFalloff(falloff);
    setAttenuation(attenuation0, attenuation1, attenuation2);
    setTheta(theta);
    setPhi(phi);
}
} // namespace Vkx
