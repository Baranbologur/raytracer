#ifndef __HW1__COMMON__
#define __HW1__COMMON__

#include "parser.h"
#include <variant>
#include <algorithm> 

struct RGB{
    int r, g, b;

    RGB(int r, int g, int b): r(r), g(g), b(b) {}

    RGB(const parser::Vec3i& intvec) : r(intvec.x), g(intvec.y), b(intvec.z) {}

    RGB(const parser::Vec3f& floatvec) : r(std::round(floatvec.x)), g(std::round(floatvec.y)), b(std::round(floatvec.z)) {}

    inline RGB operator+(const RGB& color) const {
        return RGB(r + color.r, g + color.g, b + color.b);
    }

    inline RGB operator-(const RGB& color) const {
        return RGB(r - color.r, g - color.g, b - color.b);
    }

    inline RGB operator*(float mult) const {
        return RGB(r * mult, g * mult, b * mult);
    }

    inline RGB operator*(const parser::Vec3f& vec) const {
        return RGB(r * vec.x, g * vec.y, b * vec.z);
    }

    friend std::ostream& operator<<(std::ostream& os, const RGB& color) {
        os << "Color(" << color.r << ", " << color.g << ", " << color.b << ")";

        return os;
    }

    inline void truncate() {
        r = std::max(0, std::min(r, 255));
        g = std::max(0, std::min(g, 255));
        b = std::max(0, std::min(b, 255));
    }
};

struct intersectionInfo{
    bool isIntersected;
    float t;

    intersectionInfo(bool isIntersected): isIntersected(isIntersected), t(-1) {}
    intersectionInfo(bool isIntersected, float& t): isIntersected(isIntersected), t(t) {}
};

struct ClosestIntersectedObjectInfo{
    bool isIntersectedWithAnyObject;
    int material_id;
    parser::Vec3f unit_normal_vector;
    parser::Vec3f intersection_point;
    float t;

    ClosestIntersectedObjectInfo(parser::Triangle* triangle, float t, parser::Vec3f& intersection_point):
        isIntersectedWithAnyObject(true), material_id(triangle->material_id), t(t), intersection_point(intersection_point) {
        unit_normal_vector = triangle->normal_vector.getUnitVector();
    }
    ClosestIntersectedObjectInfo(parser::Sphere* sphere, float t, parser::Vec3f& intersection_point):
        isIntersectedWithAnyObject(true), material_id(sphere->material_id), t(t), intersection_point(intersection_point) {
        unit_normal_vector = (intersection_point - sphere->center).getUnitVector();
    }
    ClosestIntersectedObjectInfo(bool isIntersected): isIntersectedWithAnyObject(false), t(-1) {}
    ClosestIntersectedObjectInfo(): isIntersectedWithAnyObject(false), t(-1) {}
};

#endif