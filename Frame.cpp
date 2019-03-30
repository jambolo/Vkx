#include "Frame.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cassert>

namespace Vkx
{
//! The frame's transformation is computed as M = S * R * T
//!
//! @param	translation		Translation component as a 3D vector
//! @param	rotation		Rotation component as a quaternion
//! @param	scale			Scale component as a 3D vector

Frame::Frame(glm::vec3 const & translation,
             glm::quat const & rotation,
             glm::vec3 const & scale /* = glm::vec3(1.0f)*/)
    : m_(compose(translation, glm::toMat4(rotation), scale))
{
}

//! M' =  T * M
//!
//! @param	t	translation (in local space)

Frame & Frame::translate(glm::vec3 const & t)
{
    m_ = glm::translate(glm::identity<glm::mat4x4>(), t) * m_;
    return *this;
}

//! M' = R * M
//!
//! @param	r	rotation (in local space)

Frame & Frame::rotate(glm::quat const & r)
{
    m_ = glm::toMat4(r) * m_;
    return *this;
}

//! M' = S * M
//!
//! @param	s	scale (in local space)

Frame & Frame::scale(glm::vec3 const & s)
{
    assert(glm::all(glm::epsilonNotEqual(s, glm::vec3(0.0f), std::numeric_limits<float>::epsilon() * glm::length(s))));

    m_ = glm::scale(glm::identity<glm::mat4x4>(), s) * m_;
    return *this;
}

//! M' = T * M
//!
//! @param	t	translation

void Frame::setTranslation(glm::vec3 const & t)
{
    glm::vec3 s;
    glm::quat r;
    glm::vec3 old_t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, s, r, old_t, k, p);

    // Replace translation
    m_ = compose(t, glm::toMat4(r), s);
}

glm::vec3 Frame::translation() const
{
    glm::vec3 s;
    glm::quat r;
    glm::vec3 t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, s, r, t, k, p);

    return t;
}

//!
//! @param	r	rotation

void Frame::setOrientation(glm::mat3x3 const & r)
{
    glm::vec3 s;
    glm::quat old_r;
    glm::vec3 t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, s, old_r, t, k, p);

    // Replace orientation
    m_ = compose(t, glm::mat4x4(r), s);
}

//!
//! @param	r	Orientation value.

void Frame::setOrientation(glm::quat const & r)
{
    glm::vec3 s;
    glm::quat old_r;
    glm::vec3 t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, s, old_r, t, k, p);

    // Replace orientation
    m_ = compose(t, glm::toMat4(r), s);
}

glm::quat Frame::orientation() const
{
    glm::vec3 s;
    glm::quat r;
    glm::vec3 t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, s, r, t, k, p);

    return r;
}

glm::mat3x3 Frame::orientationMatrix() const
{
    return glm::toMat3(orientation());
}

//!
//! @param	s	scale

void Frame::setScale(glm::vec3 const & s)
{
    assert(glm::all(glm::epsilonNotEqual(s, glm::vec3(0.0f), std::numeric_limits<float>::epsilon() * glm::length(s))));

    glm::vec3 old_s;
    glm::quat r;
    glm::vec3 t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, old_s, r, t, k, p);

    // Replace scale
    m_ = compose(t, glm::toMat4(r), s);
}

glm::vec3 Frame::scale() const
{
    glm::vec3 s;
    glm::quat r;
    glm::vec3 t;
    glm::vec3 k;
    glm::vec4 p;
    glm::decompose(m_, s, r, t, k, p);

    return s;
}

glm::mat4x4 Frame::transformation() const
{
    return m_;
}

//!
//! @param	m	Value to set the transformation matrix to.

void Frame::setTransformation(glm::mat4x4 const & m)
{
    // Make sure that none of the scales are 0

    //	assert( !Math::IsCloseToZero( Vector3d( m[0][0], m[0][1], m[0][2] ).Length2() ) );
    //	assert( !Math::IsCloseToZero( Vector3d( m[1][0], m[1][1], m[1][2] ).Length2() ) );
    //	assert( !Math::IsCloseToZero( Vector3d( m[2][0], m[2][1], m[2][2] ).Length2() ) );
    //	assert( m[0][2] == 0.f );
    //	assert( m[1][2] == 0.f );
    //	assert( m[2][2] == 0.f );
    //	assert( m[3][2] == 1.f );

    m_ = m;
}

glm::vec3 Frame::xAxis() const
{
    glm::vec4 axis = glm::normalize(m_[0]);
    return { axis.x, axis.y, axis.z };
}

glm::vec3 Frame::yAxis() const
{
    glm::vec4 axis = glm::normalize(m_[1]);
    return { axis.x, axis.y, axis.z };
}

glm::vec3 Frame::zAxis() const
{
    glm::vec4 axis = glm::normalize(m_[2]);
    return { axis.x, axis.y, axis.z };
}

glm::mat4x4 Frame::compose(glm::vec3 const & t, glm::mat4x4 const & r, glm::vec3 const & s)
{
    return glm::scale(glm::translate(glm::identity<glm::mat4x4>(), t) * r, s);
}
} // namespace Vkx
