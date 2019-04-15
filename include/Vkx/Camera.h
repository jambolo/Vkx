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
//! The camera manages two transformations, one that transforms world space into view space and one that transforms view
//! space into projection (clip) space.
//!
//! The view transform is based on the position and orientation of the camera. The camera uses a right-handed coordinate
//! system with the camera facing in the -Z direction.
//!
//! The projection transform is based on the angle of view, aspect ratio, and the distances to the near and far planes.
//!
//! @ingroup	Vkx

class Camera
{
public:

    //! Constructor.
    Camera(float         angleOfView,
           float         nearDistance,
           float         farDistance,
           float         aspectRatio,
           Frame const & frame = Frame::identity());

    //! Constructor.
    Camera(float             angleOfView,
           float             nearDistance,
           float             farDistance,
           float             aspectRatio,
           glm::vec3 const & position,
           glm::quat const & orientation = glm::quat());

    virtual ~Camera() = default;

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

    //! Sets the camera's position and orientation.
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
    void setAspectRatio(float w, float h = 1.0f);

    //! Sets the view offset.
    void setViewOffset(float x, float y);

    //! Rotates the camera.
    void turn(glm::quat const & rotation);

    //! Rotates the camera.
    void turn(float angle, glm::vec3 const & axis);

    //! Moves the camera.
    void move(glm::vec3 const & distance);

    //! Returns the facing vector.
    glm::vec3 facing() const;

    //! Returns the up vector.
    glm::vec3 up() const;

    //! Returns the right vector.
    glm::vec3 right() const;

    //! Returns the view transform.
    glm::mat4x4 view() const;

    //! Returns the projection transform.
    glm::mat4x4 projection() const;

    //! Returns the view-projection transform.
    glm::mat4x4 viewProjection() const;

//    //! Returns the view frustum
//    Frustum viewFrustum() const;

protected:

    //! Updates the projection transform.
    void updateProjection();

    //! Updates the view transform.
    void updateView();

    //! Updates the view-projection transform.
    void updateViewProjection();

    float angleOfView_;             //!< Angle of view of the height of the display (in radians)
    float nearDistance_;            //!< The distance to the near clipping plane
    float farDistance_;             //!< The distance to the far clipping plane
    float aspectRatio_;             //!< View window w / h
    Frame frame_;                   //!< View transformation
    glm::vec2 viewOffset_;          //!< View window offset
    glm::mat4x4 view_;              //!< The current world-view transformation
    glm::mat4x4 projection_;        //!< The current projection transformation
    glm::mat4x4 viewProjection_;    //!< The concatenation of the view matrix and the projection matrix
//    Frustum viewFrustum_;           //!< View frustum

private:

    // Computes the view frustum
    void computeViewFrustum(glm::mat4x4 const & m);
};
} // namespace Vkx

#include <glm/glm.hpp>
#include <Vkx/Frame.h>

namespace Vkx
{
inline Frame Camera::frame() const
{
    return frame_;
}

//! @param	position	New position

inline void Camera::setPosition(glm::vec3 const & position)
{
    frame_.setTranslation(position);
    updateView();
    updateViewProjection();
}

//! @param	orientation		New orientation

inline void Camera::setOrientation(glm::quat const & orientation)
{
    frame_.setOrientation(orientation);
    updateView();
    updateViewProjection();
}

//! @param	nearDistance		Distance to the near clipping plane.

inline void Camera::setNearDistance(float nearDistance)
{
    nearDistance_ = nearDistance;
    updateProjection();
    updateViewProjection();
}

inline float Camera::nearDistance() const
{
    return nearDistance_;
}

//! @param	farDistance		Distance to the far clipping plane.

inline void Camera::setFarDistance(float farDistance)
{
    farDistance_ = farDistance;
    updateProjection();
    updateViewProjection();
}

inline float Camera::farDistance() const
{
    return farDistance_;
}

//! The angle of view is the angle between the top and bottom of the view frustum.
//!
//! @param	angle	Angle of view (in radians)

inline void Camera::setAngleOfView(float angle)
{
    angleOfView_ = angle;
    updateProjection();
    updateViewProjection();
}

//! The angle of view is the angle between the top and bottom of the view frustum from the viewpoint.

inline float Camera::angleOfView() const
{
    return angleOfView_;
}

//! @param	rotation	A quaternion describing the rotation.

inline void Camera::turn(glm::quat const & rotation)
{
    frame_.rotate(rotation);
    updateView();
    updateViewProjection();
}

//! @param	distance	Amount to move the camera

inline void Camera::move(glm::vec3 const & distance)
{
    frame_.translate(distance);
    updateView();
    updateViewProjection();
}

//! This function sets the projection's aspect ratio using width and height values as w / h. If the
//! height is omitted, then w is the aspect ratio.
//!
//! @param	w	Width of the image on the screen.
//! @param	h	Height of the image on the screen. (Default: 1.0f)
//!
//! @note	The units of w and h are not important. If the pixels are square then w and h can simply be the width and
//!         height in pixels, otherwise they should be the actual width and height of the image on the screen.
//!
//! @warning    The width and height must both be positive.

inline void Camera::setAspectRatio(float w, float h /* = 1.0f*/)
{
    assert(h > 0 && w > 0);

    aspectRatio_ = w / h;
    updateProjection();
    updateViewProjection();
}

//! @param	x	X-offset to the center of the near plane in view space.
//! @param	y	Y-offset to the center of the near plane in view space.

inline void Camera::setViewOffset(float x, float y)
{
    viewOffset_ = glm::vec2(x, y);
    updateProjection();
    updateViewProjection();
}

inline glm::vec3 Camera::facing() const
{
    return -frame_.zAxis();
}

inline glm::vec3 Camera::up() const
{
    return frame_.yAxis();
}

inline glm::vec3 Camera::right() const
{
    return frame_.xAxis();
}

inline glm::mat4x4 Camera::projection() const
{
    return projection_;
}

inline glm::mat4x4 Camera::view() const
{
    return view_;
}

inline glm::mat4x4 Camera::viewProjection() const
{
    return viewProjection_;
}

// inline Frustum Camera::viewFrustum() const
// {
//     return viewFrustum_;
// }
} // namespace Vkx

#endif // !defined(VKX_CAMERA_H)
