#pragma once

namespace Graphic
{

void stackblur(
    unsigned char *src,  ///< input image data
    unsigned int w,      ///< image width
    unsigned int h,      ///< image height
    unsigned int radius, ///< blur intensity (should be in 2..254 range)
    unsigned int minX,
    unsigned int maxX,
    unsigned int minY,
    unsigned int maxY);

};