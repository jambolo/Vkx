#include "Camera.h"

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_aligned.hpp>

#include <cassert>

namespace Vkx
{
//! @param	angleOfView		The angle between the bottom and top of the view frustum (in radians).
//! @param	nearDistance	The distance to the near clipping plane.
//! @param	farDistance		The distance to the far clipping plane.
//! @param	aspectRatio		View window w / h
//! @param	position		The camera's location.
//! @param	orientation		The camera's orientation.

Camera::Camera(float             angleOfView,
               float             nearDistance,
               float             farDistance,
               float             aspectRatio,
               glm::vec3 const & position,
               glm::quat const & orientation /* = glm::quat()*/)
    : angleOfView_(angleOfView)
    , nearDistance_(nearDistance)
    , farDistance_(farDistance)
    , aspectRatio_(aspectRatio)
    , frame_(position, orientation)
    , viewOffset_(0.0f, 0.0f)
{
    updateView();
    updateProjection();
    updateViewProjection();
}

//! @param	angleOfView		The angle between the bottom and top of the view frustum  (in radians).
//! @param	nearDistance	The distance to the near clipping plane.
//! @param	farDistance		The distance to the far clipping plane.
//! @param	aspectRatio		View window w / h
//! @param	frame			The camera's frame of reference.
//!
//! @warning    The frame must not be scaled.
Camera::Camera(float         angleOfView,
               float         nearDistance,
               float         farDistance,
               float         aspectRatio,
               Frame const & frame /* = Frame::Identity()*/)
    : angleOfView_(angleOfView)
    , nearDistance_(nearDistance)
    , farDistance_(farDistance)
    , aspectRatio_(aspectRatio)
    , frame_(frame)
    , viewOffset_(0.0f, 0.0f)
{
#if defined(_DEBUG)
    // Make sure that there is no scaling
    assert(glm::all(glm::epsilonEqual(frame.scale(), glm::vec3(1.0f), std::numeric_limits<float>::epsilon())));
#endif  // defined( _DEBUG )

    updateView();
    updateProjection();
    updateViewProjection();
}

//! @param    frame    New frame of reference
//!
//! @warning    The frame must not be scaled.

void Camera::setFrame(Frame const & frame)
{
#if defined(_DEBUG)
    // Make sure that there is no scaling
    assert(glm::all(glm::epsilonEqual(frame.scale(), glm::vec3(1.0f), std::numeric_limits<float>::epsilon())));
#endif  // defined( _DEBUG )
    frame_ = frame;
    updateView();
    updateViewProjection();
}

glm::vec3 Camera::position() const
{
    glm::mat4x4 frame = frame_.transformation();
    return glm::vec3(frame[3]);
}

glm::quat Camera::orientation() const
{
    glm::mat3x3 r(frame_.transformation());
    return glm::quat(r);
}

void Camera::lookAt(glm::vec3 const & to, glm::vec3 const & from, glm::vec3 const & up)
{
    glm::vec3 z(glm::normalize(from - to)); // z is opposite of the direction of view
    glm::vec3 x(glm::normalize(glm::cross(up, z)));
    glm::vec3 y(glm::cross(z, x));

    frame_ = glm::mat4x4(x.x, x.y, x.z, 0.0f,
                         y.x, y.y, y.z, 0.0f,
                         z.x, z.y, z.z, 0.0f,
                         from.x, from.y, from.z, 1.0f);
    updateView();
    updateViewProjection();
}

//! @param	angle	Angle of rotation (in radians)
//! @param	axis	Axis of rotation

void Camera::turn(float angle, glm::vec3 const & axis)
{
    turn(glm::angleAxis(angle, axis));
}

void Camera::updateView()
{
    // View matrix is just the inverse of the frame
    glm::mat4x4 frame = frame_.transformation();
    glm::mat3x3 r(frame);
    glm::vec3   t(frame[3]);

    glm::mat4x4 ir = glm::transpose(r);
    glm::mat4x4 it = glm::translate(glm::identity<glm::mat4x4>(), -t);

    // Compute the view matrix
    view_ = ir * it;
}

void Camera::updateProjection()
{
    projection_ = glm::perspective(angleOfView_, aspectRatio_, nearDistance_, farDistance_);
    // "GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted. The easiest way to
    // compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix. If you don't do
    // this, then the image will be rendered upside down."
    projection_[1][1] *= -1;   // This has got to go
}

void Camera::updateViewProjection()
{
    // Compute the view-projection matrix
    viewProjection_ = view_ * projection_;

    // Compute the view frustum
    computeViewFrustum(viewProjection_);
}

void Camera::computeViewFrustum(glm::mat4x4 const & m)
{
//     viewFrustum_.sides_[Frustum::LEFT_SIDE]   = Plane(-m[0][3] - m[0][0], -m[1][3] - m[1][0], -m[2][3] - m[2][0], -m[3][3] -
// m[3][0]);
//     viewFrustum_.sides_[Frustum::RIGHT_SIDE]  = Plane(-m[0][3] + m[0][0], -m[1][3] + m[1][0], -m[2][3] + m[2][0], -m[3][3] +
// m[3][0]);
//     viewFrustum_.sides_[Frustum::TOP_SIDE]    = Plane(-m[0][3] + m[0][1], -m[1][3] + m[1][1], -m[2][3] + m[2][1], -m[3][3] +
// m[3][1]);
//     viewFrustum_.sides_[Frustum::BOTTOM_SIDE] = Plane(-m[0][3] - m[0][1], -m[1][3] - m[1][1], -m[2][3] - m[2][1], -m[3][3] -
// m[3][1]);
//     viewFrustum_.sides_[Frustum::FRONT_SIDE]  = Plane(-m[0][2],           -m[1][2],           -m[2][2],           -m[3][2]);
//     viewFrustum_.sides_[Frustum::BACK_SIDE]   = Plane(-m[0][3] + m[0][2], -m[1][3] + m[1][2], -m[2][3] + m[2][2], -m[3][3] +
// m[3][2]);
}
} // namespace Vkx
