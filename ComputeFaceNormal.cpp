#include <Vkx/Vkx.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_aligned.hpp>

namespace Vkx
{
//! @param	v0,v1,v2	Vertexes. The order of the vertexes is important. They should be clockwise.
//! @param	pN			Location to store the normal (if not nullptr)
//!
//! @return     The face normal

glm::vec3 ComputeFaceNormal(glm::vec3 const & v0,
                            glm::vec3 const & v1,
                            glm::vec3 const & v2,
                            glm::vec3 *       pN /*= nullptr*/)
{
    glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v1));
    if (pN)
        *pN = normal;
    return normal;
}

//! @param	z1		Z of the +X vertex
//! @param	z2		Z of the +Y vertex
//! @param	z3		Z of the -X vertex
//! @param	z4		Z of the -Y vertex
//! @param	scale	Ratio of Sxy / Sz, where Sxy is the distance between vertices and Sz scales the specified Z
//!					values to the actual Z values.
//! @param	pN		Where to put the normal (if not nullptr)

glm::vec3 ComputeGridNormal(float       z1,
                            float       z2,
                            float       z3,
                            float       z4,
                            float       scale,
                            glm::vec3 * pN /*= nullptr*/)
{
//	The 4 adjacent points in a uniform grid: A, B, C, D
//
//	   B
//	   |
//	C--0--A
//	   |
//	   D
//
//
//	The ratio of XY-scale to Z-scale: s = Sxy / Sz
//	The desired normal: N = cross(A,B) + cross(B,C) + cross(C,D) + cross(D,A), (then normalize)
//
//	Nx = 2 * s * (Cz - Az)
//	Ny = 2 * s * (Dz - Bz)
//	Nz = 4 * s^2
//	N = normalize( N )
//
//	Since N is normalized in the end, (2 * s) can be removed from the component values:
//
//	Nx = Cz - Az
//	Ny = Dz - Bz
//	Nz = 2 * s
//	N = normalize( N )
//
    glm::vec3 normal = glm::normalize(glm::vec3(z3 - z1, z4 - z2, 2.0f * scale));
    if (pN)
        *pN = normal;
    return normal;
}
} // namespace Vkx
