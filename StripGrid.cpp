#include "Vkx.h"

#include <cstdint>

template <typename T>
int StripGrid_imp(int w, int h, T * pData)
{
    T * pStart = pData;         // The starting point (for keeping track)

    // Strip two rows of quads at a time

    for (int i = 0; i < h - 1; i += 2)
    {
        int r0 = i * w;     // Start of this row of vertices
        int r1 = r0 + w;    // Start of next row of vertices

        // Strip the odd row

        for (int j = 0; j < w; ++j)
        {
            *pData++ = T(r1 + j);
            *pData++ = T(r0 + j);
        }

        // If there is another row, then strip it

        if (i < h - 2)
        {
            int r2 = r1 + w;    // Start of third row of vertices
            int r3 = r2 + w;    // Start of fourth row of vertices

            // Do the degenerate triangles

            *pData++ = T(r0 + (w - 1));
            *pData++ = T(r1 + (w - 1));

            // Strip the even row backwards

            for (int j = w - 1; j >= 0; j--)
            {
                *pData++ = T(r1 + j);
                *pData++ = T(r2 + j);
            }

            // If there is another row, then do the degenerate triangles

            if (i < h - 3)
            {
                *pData++ = T(r2);
                *pData++ = T(r3);
            }
        }
    }

    int nIndexes = (int)(pData - pStart);

    // Make sure the expected number of indexes where generated.
    assert(nIndexes == (h - 1) * (2 * w + 2) - 2);

    return nIndexes;
}

namespace Vkx
{
//! This function generates an array of vertex indexes for stripping a w by h grid of vertexes. The strip is
//! created like this:
//!     - For every two rows in the grid:
//!         - Strip a row from left to right
//!         - Use degenerate triangles to continue the strip down to the end of the next row.
//!         - Continue stripping the next row from right to left.
//!         - Use degenerate triangles to continue the strip down to the beginning of the next row.
//!
//! The vertexes are stripped for a CW front-face.
//!
//! @param  w       Width of the grid (note: this is the number of vertexes -- the number of quads is w - 1)
//! @param  h       Height of the grid (note: this is the number of vertexes -- the number of quads is h - 1)
//! @param  pData   Where to place the indexes. Index values range 0 - 65535. The buffer must be big enough
//!                 to hold at least ( h - 1 ) * ( 2 * w + 2 ) - 2 indexes.
//!
//! @return     Number of vertex indexes generated: ( h - 1 ) * ( 2 * w + 2 ) - 2
//!
//! @warning    w * h must be less than 65536

int StripGrid(int w, int h, uint16_t * pData)
{
    assert(w * h <= 65536); // Indexes must fit in an unsigned 16-bit value
    return StripGrid_imp(w, h, pData);
}

//! This function generates an array of vertex indexes for stripping a w by h grid of vertexes. The strip is
//! created like this:
//!     - For every two rows in the grid:
//!         - Strip a row from left to right
//!         - Use degenerate triangles to continue the strip down to the end of the next row.
//!         - Continue stripping the next row from right to left.
//!         - Use degenerate triangles to continue the strip down to the beginning of the next row.
//!
//! The vertexes are stripped for a CW front-face.
//!
//! @param  w       Width of the grid (note: this is the number of vertexes -- the number of quads is w - 1)
//! @param  h       Height of the grid (note: this is the number of vertexes -- the number of quads is h - 1)
//! @param  pData   Where to place the indexes. The buffer must be big enough to hold at least
//!                 ( h - 1 ) * ( 2 * w + 2 ) - 2 indexes.
//!
//! @return         Number of vertex indexes generated: ( h - 1 ) * ( 2 * w + 2 ) - 2

int StripGrid(int w, int h, uint32_t * pData)
{
    return StripGrid_imp(w, h, pData);
}
} // namespace Vkx
