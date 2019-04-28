#if !defined(VKX_FRAME_H)
#define VKX_FRAME_H

#pragma once

#include <glm/glm.hpp>

namespace Vkx
{
//! A frame of reference including translation, scale, and orientation.
class Frame
{
public:
    //! Constructor.
    Frame(glm::mat4x4 const & m = glm::mat4x4(1.0f))
        : m_(m)
    {
    }

    //! Constructor
    Frame(glm::vec3 const & translation,
          glm::quat const & rotation,
          glm::vec3 const & scale = glm::vec3(1.0f));

    //! Translates the frame and returns it.
    Frame & translate(glm::vec3 const & t);

    //! Rotates the frame by the given quaternion and returns it.
    Frame & rotate(glm::quat const & r);

    //! Scales the frame and returns it.
    Frame & scale(glm::vec3 const & s);

    //! Sets the frame's translation.
    void setTranslation(glm::vec3 const & t);

    //! Returns the frame's translation.
    glm::vec3 translation() const;

    //! Sets the frame's rotation according to the given quaternion.
    void setOrientation(glm::quat const & r);

    //! Sets the frame's orientation as a matrix.
    void setOrientation(glm::mat3x3 const & r);

    //! Returns the frame's rotation as a quaternion.
    glm::quat orientation() const;

    //! Returns the frame's orientation as a matrix.
    glm::mat3x3 orientationMatrix() const;

    //! Sets the frame's scale.
    void setScale(glm::vec3 const & s);

    //! Returns the frame's scale.
    glm::vec3 scale() const;

    //! Sets the frame's transformation matrix.
    void setTransformation(glm::mat4x4 const & m);

    //! Returns the frame's transformation matrix.
    glm::mat4x4 transformation() const;

    //! Returns the frame's unit X axis in global space.
    glm::vec3 xAxis() const;

    //! Returns the frame's unit Y axis in global space.
    glm::vec3 yAxis() const;

    //! Returns the frame's unit Z axis in global space.
    glm::vec3 zAxis() const;

    //! Returns an untransformed Frame.
    static Frame identity() { return Frame(); }

private:

    static glm::mat4x4 compose(glm::vec3 const & t, glm::mat4x4 const & r, glm::vec3 const & s);

    glm::mat4x4 m_;  //!< Transformation matrix
};
} // namespace Vkx

#endif // !defined(VKX_FRAME_H)
