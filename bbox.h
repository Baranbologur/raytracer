#ifndef __HW1__BBOX__
#define __HW1__BBOX__

#include "parser.h"
#include "ray.h"

using parser::Vec3f;
using std::min;
using std::max;

class BBox{
    public:
        Vec3f min_point;
        Vec3f max_point;

        bool rayIntersect(Ray ray);
};

#endif