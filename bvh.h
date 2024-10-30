#ifndef __HW1__BVH__
#define __HW1__BVH__

#include <vector>
#include "parser.h"
#include "ray.h"
#include "bbox.h"
#include "node.h"

using std::vector;
using std::min;
using std::max;
using parser::Triangle;
using parser::Sphere;
using parser::Scene;
using parser::Vec3f;

class BVH_Tree{
    public:
        Node* head;

        BVH_Tree(Scene& scene);

        /* Prevents memory leak but decreases speed */
        // ~BVH_Tree();
        // void deleteTree(Node* node);

        void configureHead(Scene& scene);
        void print_main();
        ClosestIntersectedObjectInfo getIntersectInfo(const Ray& r, bool backface_culling_enabled) const ;
};

#endif