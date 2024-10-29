#ifndef __HW1__RAY__
#define __HW1__RAY__

#include "parser.h"
#include "common.h"

class Node;
class BVH_Tree;

struct Ray
{
    parser::Vec3f start_position;
    parser::Vec3f direction;

    Ray(parser::Vec3f start_position, parser::Vec3f direction);
    intersectionInfo getIntersectionInfoWithTriangle(const parser::Triangle &triangle) const ;
    intersectionInfo getIntersectionInfoWithSphere(const parser::Sphere &sphere) const ;
    ClosestIntersectedObjectInfo findIntersectedObject(const Node &node, const bool &backface_culling_enabled) const ;
    RGB getcolor(const BVH_Tree &tree, const parser::Scene &scene, const int &depth) const ;
    RGB computeDiffuseColor(const parser::Vec3f &position, const parser::Vec3f &normal_vector, const parser::Material &material, const parser::PointLight &pointlight) const ;
    RGB computeAmbientColor(const parser::Material &material, const parser::Vec3f &ambient_light) const ;
    RGB computeSpecularColor(const parser::Vec3f &position, const parser::Vec3f &normal_vector, const parser::Material &material, const parser::PointLight &pointlight) const ;
};


#endif