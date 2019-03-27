#include "Camera.h"

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_aligned.hpp>

#include <cassert>

namespace Vkx
{
//! @param	angleOfView		The angle between the bottom and top of the view frustum (in degrees).
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
    : angleOfView_(glm::radians(angleOfView))
    , nearDistance_(nearDistance)
    , farDistance_(farDistance)
    , frame_(position, orientation)
    , aspectRatio_(aspectRatio)
    , viewOffset_(0.0f, 0.0f)
{
    syncInternalState();
}

//! @param	angleOfView		The angle between the bottom and top of the view frustum  (in degrees).
//! @param	nearDistance	The distance to the near clipping plane.
//! @param	farDistance		The distance to the far clipping plane.
//! @param	aspectRatio		View window w / h
//! @param	frame			The camera's frame of reference.

Camera::Camera(float         angleOfView,
               float         nearDistance,
               float         farDistance,
               float         aspectRatio,
               Frame const & frame /* = Frame::Identity()*/)
    : angleOfView_(glm::radians(angleOfView))
    , nearDistance_(nearDistance)
    , farDistance_(farDistance)
    , frame_(frame)
    , aspectRatio_(aspectRatio)
    , viewOffset_(0.0f, 0.0f)
{
    syncInternalState();
}

#if 0
void Camera::look() const
{
    HRESULT hr;

    hr = pDevice_->SetTransform(D3DTS_VIEW, &viewMatrix_);
    assert_succeeded(hr);
}

void Camera::reshape()
{
    HRESULT hr;

    hr = pDevice_->SetTransform(D3DTS_PROJECTION, &projectionMatrix_);
    assert_succeeded(hr);
}
#endif // if 0

glm::vec3 Camera::position() const
{
#if defined(_DEBUG)
    // Make sure that there is no scaling so we can simply extract the last row and avoid decomposition
    assert(glm::all(glm::epsilonEqual(frame_.scale(), glm::vec3(1.0f), std::numeric_limits<float>::epsilon())));
#endif  // defined( _DEBUG )

    glm::mat4x4 frame = frame_.transformation();
    return glm::vec3(frame[3][0], frame[3][1], frame[3][2]);
}

glm::quat Camera::orientation() const
{
#if defined(_DEBUG)
    // Make sure that there is no scaling so we can simply extract the upper left 3x3 and avoid decomposition
    assert(glm::all(glm::epsilonEqual(frame_.scale(), glm::vec3(1.0f), std::numeric_limits<float>::epsilon())));
#endif  // defined( _DEBUG )

    glm::mat4x4 frame = frame_.transformation();
    glm::mat3x3 r(frame[0][0], frame[0][1], frame[0][2], frame[1][0], frame[1][1], frame[1][2], frame[2][0], frame[2][1],
                  frame[2][2]);
    return glm::quat(r);
}

void Camera::lookAt(glm::vec3 const & to, glm::vec3 const & from, glm::vec3 const & up)
{
    frame_.setTransformation(glm::lookAtRH(from, to, up));
    syncInternalState();
}

//! @param	angle	Angle of rotation (in degrees)
//! @param	axis	Axis of rotation

void Camera::turn(float angle, glm::vec3 const & axis)
{
    turn(glm::angleAxis(glm::radians(angle), axis));
}

void Camera::syncViewMatrix()
{
// Yuck this is slow...there is a faster way
//
//	// Get the frame and invert it (because, in reality, the camera remains
//	// at the origin and the world is transformed).
//
//	glm::mat4x4	r;
//	D3DXMatrixInverse( r, NULL, frame_.GetTransformation() );

    // Rotate and translate
    //
    // Note the transformation is inverted (because, in reality, the camera remains at the origin and world space
    // is transformed). Also the inversion reverses the order of rotation and translation.

    // Get the rotation and invert it (by transposing).

#if defined(_DEBUG)
    // Make sure that there is no scaling so we don't have to worry about inverting being different from transposing.
    assert(glm::all(glm::epsilonEqual(frame_.scale(), glm::vec3(1.0f), std::numeric_limits<float>::epsilon())));
#endif  // defined( _DEBUG )

    glm::mat4x4 frame = frame_.transformation();
    glm::mat3x3 r(frame);
    glm::vec3   t(frame[3]);

    glm::mat4x4 ir = glm::transpose(r);
    glm::mat4x4 it = glm::translate(glm::identity<glm::mat4x4>(), -t);

    // Compute the view matrix
    viewMatrix_ = it * ir;
}

void Camera::syncProjectionMatrix()
{
    projectionMatrix_ = glm::perspectiveRH(angleOfView_, aspectRatio_, nearDistance_, farDistance_);
}

//!
//! @note	This function should be called whenever any value is modified directly.

void Camera::syncInternalState()
{
    // Sync the view matrix
    syncViewMatrix();

    // Sync the projection matrix
    syncProjectionMatrix();

    // Compute the view-projection matrix
    viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;

    // Compute the view frustum

    computeViewFrustum(viewProjectionMatrix_);
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
