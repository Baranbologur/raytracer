#include "node.h"

int Node::max_level = 0;
int MAX_ELEMENT_COUNT = 1;

Node::Node(vector<Triangle*> triangles, vector<Sphere*> spheres, int level): triangles(triangles), spheres(spheres), level(level), left(NULL), right(NULL){
    max_level = max(max_level, level);
    setMinAndMaxPoints();
    setIsLeaf();
    if (!is_leaf){
        createChildNodes();
    }
}

int Node::getElementCount(){
    return triangles.size() + spheres.size();
}

void Node::updateMinMaxTriangle(const Triangle* triangle) {
    updateMinMaxTriangleCorner(triangle->a);
    updateMinMaxTriangleCorner(triangle->b);
    updateMinMaxTriangleCorner(triangle->c);
}

void Node::updateMinMaxTriangleCorner(const Vec3f& vertex) {
    bbox.min_point.x = min(bbox.min_point.x, vertex.x);
    bbox.min_point.y = min(bbox.min_point.y, vertex.y);
    bbox.min_point.z = min(bbox.min_point.z, vertex.z);

    bbox.max_point.x = max(bbox.max_point.x, vertex.x);
    bbox.max_point.y = max(bbox.max_point.y, vertex.y);
    bbox.max_point.z = max(bbox.max_point.z, vertex.z);
}

void Node::updateMinMaxSphere(const Sphere* sphere) {
    bbox.min_point.x = min(bbox.min_point.x, sphere->center.x - sphere->radius);
    bbox.min_point.y = min(bbox.min_point.y, sphere->center.y - sphere->radius);
    bbox.min_point.z = min(bbox.min_point.z, sphere->center.z - sphere->radius);

    bbox.max_point.x = max(bbox.max_point.x, sphere->center.x + sphere->radius);
    bbox.max_point.y = max(bbox.max_point.y, sphere->center.y + sphere->radius);
    bbox.max_point.z = max(bbox.max_point.z, sphere->center.z + sphere->radius);
}

void Node::setMinAndMaxPoints(){
    bbox.min_point = Vec3f::MAXVEC;
    bbox.max_point = Vec3f::MINVEC;
    for (Triangle* triangle: triangles){
        updateMinMaxTriangle(triangle);
    }
    for (Sphere* sphere: spheres){
        updateMinMaxSphere(sphere);
    }
}

void Node::setIsLeaf(){
    is_leaf = (getElementCount() <= MAX_ELEMENT_COUNT);
}

void Node::createChildNodes(){
    sortTrianglesByCentroid();
    sortSpheresByCenter();

    vector<Triangle*> left_triangles;
    vector<Sphere*> left_spheres;
    vector<Triangle*> right_triangles;
    vector<Sphere*> right_spheres;

    left_triangles.assign(triangles.begin(), triangles.begin() + triangles.size()/2);
    right_triangles.assign(triangles.begin() + triangles.size()/2, triangles.end());

    left_spheres.assign(spheres.begin(), spheres.begin() + (spheres.size() + 1) / 2);
    right_spheres.assign(spheres.begin() + (spheres.size() + 1) / 2, spheres.end());

    triangles.clear();
    spheres.clear();

    if (left_spheres.empty() && left_triangles.empty()){
        left = NULL;
    }
    else{
        left = new Node(left_triangles, left_spheres, level + 1);
    }

    if (right_spheres.empty() && right_triangles.empty()){
        right = NULL;
    }
    else{
        right = new Node(right_triangles, right_spheres, level + 1);
    }
}

void Node::sortTrianglesByCentroid() {
    if (level % 3 == 0){
        std::sort(triangles.begin(), triangles.end(),
        [](Triangle* t1, Triangle* t2) {
            return t1->centeroid.x < t2->centeroid.x;
        });
    }
    else if (level % 3 == 1){
        std::sort(triangles.begin(), triangles.end(),
        [](Triangle* t1, Triangle* t2) {
            return t1->centeroid.y < t2->centeroid.y;
        });
    }
    else{
        std::sort(triangles.begin(), triangles.end(),
        [](Triangle* t1, Triangle* t2) {
            return t1->centeroid.z < t2->centeroid.z;
        });
    }
}

void Node::sortSpheresByCenter() {
    if (level % 3 == 0){
        std::sort(spheres.begin(), spheres.end(),
        [](Sphere* s1, Sphere* s2) {
            return s1->center.x < s2->center.x;
        });
    }
    else if (level % 3 == 10){
        std::sort(spheres.begin(), spheres.end(),
        [](Sphere* s1, Sphere* s2) {
            return s1->center.y < s2->center.y;
        });
    }
    else{
        std::sort(spheres.begin(), spheres.end(),
        [](Sphere* s1, Sphere* s2) {
            return s1->center.y < s2->center.y;
        });
    }
}

void Node::intersect(const Ray& r, ClosestIntersectedObjectInfo& hitInfo, bool backface_culling_enabled){
    if (bbox.rayIntersect(r) == false){
        hitInfo.isIntersectedWithAnyObject = false;
        return;
    }

    if (is_leaf) {
        hitInfo = r.findIntersectedObject(*this, backface_culling_enabled);
        return;
    }

    ClosestIntersectedObjectInfo hitInfo1, hitInfo2;

    if (left) {
        // std::cout << "left" << level << "\n";
        left->intersect(r, hitInfo1, backface_culling_enabled);
    }
    if (right){
        // std::cout << "right" << level << "\n";
        right->intersect(r, hitInfo2, backface_culling_enabled);
    }

    if(hitInfo1.isIntersectedWithAnyObject){
        hitInfo = hitInfo1;
    }

    if(hitInfo2.isIntersectedWithAnyObject){
        hitInfo = hitInfo2;
    }

    if(hitInfo2.isIntersectedWithAnyObject && hitInfo1.isIntersectedWithAnyObject){
        hitInfo = hitInfo1.t < hitInfo2.t ? hitInfo1 : hitInfo2;
    }
}