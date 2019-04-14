#if !defined(VKX_CAMERA_H)
#define VKX_CAMERA_H

#pragma once

#include <glm/gtc/quaternion.hpp>
#include <Vkx/Frame.h>
#include <Vkx/Vkx.h>

namespace Vkx
{
//! A camera.
//!
//! @ingroup	Vkx

class Camera
{
public:

    //! Constructor
    Camera(float         angleOfView,
           float         nearDistance,
           float         farDistance,
           float         aspectRatio,
           Frame const & frame = Frame::identity());

    //! Constructor
    Camera(float             angleOfView,
           float             nearDistance,
           float             farDistance,
           float             aspectRatio,
           glm::vec3 const & position,
           glm::quat const & orientation = glm::quat());

    // Destructor
    virtual ~Camera() = default;

#if 0
    //! Sets D3D's projection matrix based on the aspect ratio, near and far distance, and center.
    void reshape();

    //! Sets D3D's view matrix based on the current position and orientation of the camera.
    void look() const;
#endif

    //! Sets the camera's frame of reference.
    void setFrame(Vkx::Frame const & frame);

    //! Returns the camera's current frame of reference.
    Vkx::Frame frame() const;

    //! Sets the camera's current position.
    void setPosition(glm::vec3 const & position);

    //! Returns the camera's current position.
    glm::vec3 position() const;

    //! Sets the camera's current orientation.
    void setOrientation(glm::quat const & orientation);

    //! Returns the camera's current orientation.
    glm::quat orientation() const;

    //! Sets the camera's position and orientation
    void lookAt(glm::vec3 const & to, glm::vec3 const & from, glm::vec3 const & up);

    //! Sets the distance to the near clipping plane.
    void setNearDistance(float nearDistance);

    //! Returns the distance to the near clipping plane.
    float nearDistance() const;

    //! Sets the distance to the far clipping plane.
    void setFarDistance(float farDistance);

    //! Returns the distance to the far clipping plane.
    float farDistance() const;

    //! Sets the angle of view.
    void setAngleOfView(float angle);

    //! Returns the angle of view.
    float angleOfView() const;

    //! Sets the view window aspect ratio
    void setAspectRatio(float w, float h);

    //! Sets the view offset
    void setViewOffset(float x, float y);

    //! Rotates the camera.
    void turn(glm::quat const & rotation);

    //! Rotates the camera.
    void turn(float angle, glm::vec3 const & axis);

    //! Moves the camera.
    void move(glm::vec3 const & distance);

    //! Returns the direction vector
    glm::vec3 direction() const;

    //! Returns the up vector.
    glm::vec3 up() const;

    //! Returns the right vector.
    glm::vec3 right() const;

    //! Returns the view matrix
    glm::mat4x4 viewMatrix() const;

    //! Returns the projection matrix
    glm::mat4x4 projectionMatrix() const;

    //! Returns the view-projection matrix
    glm::mat4x4 viewProjectionMatrix() const;

    //! Returns the view frustum
//    Frustum viewFrustum() const;

protected:

    //! Syncs the internal state of the camera so that all values are consistent.
    void syncInternalState();

    float angleOfView_;                 //!< Angle of view of the height of the display (in radians)
    float nearDistance_;                //!< The distance to the near clipping plane
    float farDistance_;                 //!< The distance to the far clipping plane
    float aspectRatio_;                 //!< View window w / h
    Frame frame_;                       //!< View transformation
    glm::vec2 viewOffset_;              //!< View window offset
    glm::mat4x4 viewMatrix_;            //!< The current world-view transformation
    glm::mat4x4 projectionMatrix_;      //!< The current projection transformation
    glm::mat4x4 viewProjectionMatrix_;  //!< The concatenation of the view matrix and the projection matrix
//    Frustum viewFrustum_;               //!< View frustum

private:

    // Computes the projection matrix
    void syncProjectionMatrix();

    // Computes the view matrix
    void syncViewMatrix();

    // Computes the view frustum
    void computeViewFrustum(glm::mat4x4 const & m);
};
} // namespace Vkx

#include <glm/glm.hpp>
#include <Vkx/Frame.h>

namespace Vkx
{
//!
//! @param	frame	New frame of reference

inline void Camera::setFrame(Frame const & frame)
{
    frame_ = frame;
    syncInternalState();
}

inline Frame Camera::frame() const
{
    return frame_;
}

//!
//! @param	position	New position

inline void Camera::setPosition(glm::vec3 const & position)
{
    frame_.setTranslation(position);
    syncInternalState();
}

//!
//! @param	orientation		New orientation

inline void Camera::setOrientation(glm::quat const & orientation)
{
    frame_.setOrientation(orientation);
    syncInternalState();
}

//!
//! @param	nearDistance		Distance to the near clipping plane.

inline void Camera::setNearDistance(float nearDistance)
{
    nearDistance_ = nearDistance;
    syncInternalState();
}

inline float Camera::nearDistance() const
{
    return nearDistance_;
}

//!
//! @param	farDistance		Distance to the far clipping plane.

inline void Camera::setFarDistance(float farDistance)
{
    farDistance_ = farDistance;
    syncInternalState();
}

inline float Camera::farDistance() const
{
    return farDistance_;
}

//! The angle of view is the angle between the top and bottom of the view frustum.
//!
//! @param	angle	Angle of view (in degrees)

inline void Camera::setAngleOfView(float angle)
{
    angleOfView_ = glm::radians(angle);
    syncInternalState();
}

//! The angle of view is the angle between the top and bottom of the view frustum from the viewpoint.
//!

inline float Camera::angleOfView() const
{
    return angleOfView_;
}

//!
//! @param	rotation	A quaternion describing the rotation.

inline void Camera::turn(glm::quat const & rotation)
{
    frame_.rotate(rotation);
    syncInternalState();
}

//!
//! @param	distance	Amount to move the camera

inline void Camera::move(glm::vec3 const & distance)
{
    frame_.translate(distance);
    syncInternalState();
}

//! @param	w	Width of the image on the screen.
//! @param	h	Height of the image on the screen.
//!
//! @note	The units of @a w and @a h are not important. @a w and @a h are only used to compute the aspect ratio.
//!			If the pixels are square then @a w and @a h can simply be the width and height in pixels, otherwise
//!			they should be the actual width and height of the image on the screen.

inline void Camera::setAspectRatio(float w, float h)
{
    assert(h > 0 && w > 0);

    aspectRatio_ = w / h;
    syncInternalState();
}

//! @param	x	X-offset to the center of the near plane in view space.
//! @param	y	Y-offset to the center of the near plane in view space.

inline void Camera::setViewOffset(float x, float y)
{
    viewOffset_ = glm::vec2(x, y);
    syncInternalState();
}

inline glm::vec3 Camera::direction() const
{
    return frame_.zAxis();
}

inline glm::vec3 Camera::up() const
{
    return frame_.yAxis();
}

inline glm::vec3 Camera::right() const
{
    return frame_.xAxis();
}

inline glm::mat4x4 Camera::projectionMatrix() const
{
    return projectionMatrix_;
}

inline glm::mat4x4 Camera::viewMatrix() const
{
    return viewMatrix_;
}

inline glm::mat4x4 Camera::viewProjectionMatrix() const
{
    return viewProjectionMatrix_;
}

// inline Frustum Camera::viewFrustum() const
// {
//     return viewFrustum_;
// }
} // namespace Vkx

#endif // !defined(VKX_CAMERA_H)
