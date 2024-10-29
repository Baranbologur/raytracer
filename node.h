#ifndef __HW1__NODE__
#define __HW1__NODE__

#include "bbox.h"
#include "parser.h"

using std::vector;
using std::min;
using std::max;
using parser::Triangle;
using parser::Sphere;
using parser::Scene;
using parser::Vec3f;

class Node{
    public:
        bool is_leaf;
        BBox bbox;
        Node* left;
        Node* right;
        vector<Triangle*> triangles;
        vector<Sphere*> spheres;
        int level;
        static int max_level;

        Node(vector<Triangle*> triangles, vector<Sphere*> spheres, int level);

        int getElementCount();

        void updateMinMaxTriangle(const Triangle* triangle);

        void updateMinMaxTriangleCorner(const Vec3f& vertex);

        void updateMinMaxSphere(const Sphere* sphere);

        void setMinAndMaxPoints();

        void setIsLeaf();

        void createChildNodes();

        Vec3f divideVolume();

        void sortTrianglesByCentroid();

        void sortSpheresByCenter();

        void intersect(const Ray& r, ClosestIntersectedObjectInfo& hitInfo, bool backface_culling_enabled);

        void print_tree();
};

#endif