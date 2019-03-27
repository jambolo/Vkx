#pragma once

#if !defined(VKX_LIGHT_H)
#define VKX_LIGHT_H

#include <glm/glm.hpp>

//! @defgroup Lights Light Types
//! Lights

namespace Vkx
{
//! A base class for lights
//!
//! @ingroup Lights

class Light
{
public:

    enum TypeId
    {
        CUSTOM = -1,
        AMBIENT,
        POINT,
        DIRECTIONAL,
        SPOT
    };

    //! Destructor
    virtual ~Light() = default;

    //! Returns the light's ID.
    int id() const { return id_; }

    //! Returns the light's type id.
    int type() const { return type_; }

    //! Returns true if the light is enabled
    bool isEnabled() const { return enabled_; }

    //! Enables or disables the light.
    void enable(bool enabled = true);

protected:

    //! Constructor
    Light() = delete;

    //! Constructor
    //!
    //! @param    id            An identifier for this light.
    //! @param    type          An type of this light.

    Light(int id, TypeId type)
        : id_(id)
        , type_(type)
        , enabled_(false)
    {
    }

private:

    int id_;
    TypeId type_;
    bool enabled_;
};

//! An ambient light.
//!
//! @ingroup Lights

class AmbientLight : public Light
{
public:

    //! Constructor
    AmbientLight(int id);

    //! Constructor
    AmbientLight(int               id,
                 glm::vec4 const & ambient);

    //! Destructor
    virtual ~AmbientLight() override = default;

    //! Sets the ambient color.
    void setAmbientColor(glm::vec4 const & ambient) { ambient_ = ambient; }

    //! Returns the ambient color
    glm::vec4 ambientColor() const { return ambient_; }

    //! Sets all the properties in one go.
    void set(glm::vec4 const & ambient);

private:

    glm::vec4 ambient_;
};

//! A point light
//
//! @ingroup Lights
//!

class PointLight : public Light
{
public:

    //! Constructor
    PointLight(int id);

    //! Constructor
    PointLight(int               id,
               glm::vec4 const & ambient,
               glm::vec4 const & diffuse,
               glm::vec4 const & specular,
               glm::vec3 const & position,
               float             range,
               float             attenuation0,
               float             attenuation1,
               float             attenuation2);

    //! Destructor
    virtual ~PointLight() override = default;

    //! Sets the ambient color.
    void setAmbientColor(glm::vec4 const & ambient) { ambient_ = ambient; }

    //! Returns the ambient color
    glm::vec4 ambientColor() const { return ambient_; }

    //! Sets the diffuse color.
    void setDiffuseColor(glm::vec4 const & diffuse) { diffuse_ = diffuse; }

    //! Returns the diffuse color.
    glm::vec4 diffuseColor() const { return diffuse_; }

    //! Sets the specular color.
    void setSpecularColor(glm::vec4 const & specular) { specular_ = specular; }

    //! Returns the specular color.
    glm::vec4 specularColor() const { return specular_; }

    //! Sets the light's position.
    void setPosition(glm::vec3 const & position) { position_ = position; }

    //! Returns the light's position
    glm::vec3 position() const { return position_; }

    //! Sets the range value.
    void setRange(float range) { range_ = range; }

    //! Returns the range value
    float range() const { return range_; }

    //! Sets the attenuation value.
    void setAttenuation(float a0, float a1, float a2)
    {
        attenuation0_ = a0;
        attenuation1_ = a1;
        attenuation2_ = a2;
    }

    //! Returns the attenuation values
    void getAttenuation(float * pA0, float * pA1, float * pA2) const
    {
        *pA0 = attenuation0_;
        *pA1 = attenuation1_;
        *pA2 = attenuation2_;
    }

    //! Sets all of the light's properties in one go
    void set(glm::vec4 const & ambient,
             glm::vec4 const & diffuse,
             glm::vec4 const & specular,
             glm::vec3 const & position,
             float             range,
             float             attenuation0,
             float             attenuation1,
             float             attenuation2);

private:

    glm::vec4 ambient_;
    glm::vec4 diffuse_;
    glm::vec4 specular_;
    glm::vec3 position_;
    float range_;
    float attenuation0_;
    float attenuation1_;
    float attenuation2_;
};

//! A directional light
//
//! @ingroup Lights
//!

class DirectionalLight : public Light
{
public:

    //! Constructor
    DirectionalLight(int id);

    //! Constructor
    DirectionalLight(int               id,
                     glm::vec4 const & ambient,
                     glm::vec4 const & diffuse,
                     glm::vec4 const & specular,
                     glm::vec3 const & direction);

    //! Destructor
    virtual ~DirectionalLight() override = default;

    //! Sets the ambient color.
    void setAmbientColor(glm::vec4 const & ambient) { ambient_ = ambient; }

    //! Returns the ambient color
    glm::vec4 ambientColor() const { return ambient_; }

    //! Sets the diffuse color.
    void setDiffuseColor(glm::vec4 const & diffuse) { diffuse_ = diffuse; }

    //! Returns the diffuse color.
    glm::vec4 diffuseColor() const { return diffuse_; }

    //! Sets the specular color.
    void setSpecularColor(glm::vec4 const & specular) { specular_ = specular; }

    //! Returns the specular color.
    glm::vec4 specularColor() const { return specular_; }

    //! Sets the light's direction.
    void setDirection(glm::vec3 const & direction) { direction_ = direction; }

    //! Returns the light's direction
    glm::vec3 direction() const { return direction_; }

    //! Sets the values specific to directional lights.
    void set(glm::vec4 const & ambient,
             glm::vec4 const & diffuse,
             glm::vec4 const & specular,
             glm::vec3 const & direction);

private:

    glm::vec4 ambient_;
    glm::vec4 diffuse_;
    glm::vec4 specular_;
    glm::vec3 direction_;
};

//! A spot light
//
//! @ingroup Lights
//!

class SpotLight : public Light
{
public:

    //! Constructor
    SpotLight(int id);

    //! Constructor
    SpotLight(int               id,
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
              float             phi);

    //! Destructor
    virtual ~SpotLight() override;

    //! Sets the ambient color.
    void setAmbientColor(glm::vec4 const & ambient) { ambient_ = ambient; }

    //! Returns the ambient color
    glm::vec4 ambientColor() const { return ambient_; }

    //! Sets the diffuse color.
    void setDiffuseColor(glm::vec4 const & diffuse) { diffuse_ = diffuse; }

    //! Returns the diffuse color.
    glm::vec4 diffuseColor() const { return diffuse_; }

    //! Sets the specular color.
    void setSpecularColor(glm::vec4 const & specular) { specular_ = specular; }

    //! Returns the specular color.
    glm::vec4 specularColor() const { return specular_; }

    //! Sets the light's position.
    void setPosition(glm::vec3 const & position) { position_ = position; }

    //! Returns the light's position
    glm::vec3 position() const { return position_; }

    //! Sets the light's direction.
    void setDirection(glm::vec3 const & direction) { direction_ = direction; }

    //! Returns the light's direction
    glm::vec3 const & direction() const { return direction_; }

    //! Sets the range value.
    void setRange(float range) { range_ = range; }

    //! Returns the range value
    float range() const { return range_; }

    //! Sets the falloff value.
    void setFalloff(float falloff) { falloff_ = falloff; }

    //! Returns the falloff value
    float falloff() const { return falloff_; }

    //! Sets the attenuation value.
    void setAttenuation(float a0, float a1, float a2)
    {
        attenuation0_ = a0;
        attenuation1_ = a1;
        attenuation2_ = a2;
    }

    //! Returns the attenuation values
    void getAttenuation(float * pA0, float * pA1, float * pA2) const
    {
        *pA0 = attenuation0_;
        *pA1 = attenuation1_;
        *pA2 = attenuation2_;
    }

    //! Sets the theta value.
    void setTheta(float theta) { theta_ = theta; }

    //! Returns the theta value
    float theta() const { return theta_; }

    //! Sets the phi value.
    void setPhi(float phi) { phi_ = phi; }

    //! Returns the phi value
    float phi() const { return phi_; }

    //! Sets all of the properties in one go.
    void set(glm::vec4 const & ambient,
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
             float             phi);

private:

    glm::vec4 ambient_;
    glm::vec4 diffuse_;
    glm::vec4 specular_;
    glm::vec3 direction_;
    glm::vec3 position_;
    float range_;
    float falloff_;
    float attenuation0_;
    float attenuation1_;
    float attenuation2_;
    float theta_;
    float phi_;
};
} // namespace Vkx

#endif // !defined(VKX_LIGHT_H)
