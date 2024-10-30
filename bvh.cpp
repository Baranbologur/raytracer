#include "bvh.h"
#include <algorithm>
#include <limits>
#include <vector>



BVH_Tree::BVH_Tree(Scene& scene){
    configureHead(scene);
}

void BVH_Tree::configureHead(Scene& scene){
    vector<Triangle*> triangles;
    vector<Sphere*> spheres;
    for (Triangle& triangle : scene.triangles) {
        triangles.push_back(&triangle);
    }
    for (Sphere& sphere: scene.spheres){
        spheres.push_back(&sphere);
    }
    head = new Node(triangles, spheres, 1);
}

ClosestIntersectedObjectInfo BVH_Tree::getIntersectInfo(const Ray& r, bool backface_culling_enabled) const {
    ClosestIntersectedObjectInfo info;
    head->intersect(r, info, backface_culling_enabled);
    return info;
}

/* Prevents memory leak but decreases speed */
// void BVH_Tree::deleteTree(Node* node) {
//     if (node == nullptr) return;

//     deleteTree(node->left);
//     deleteTree(node->right);

//     delete node;
// }

// BVH_Tree::~BVH_Tree(){
//     deleteTree(head);
// }

// void BVH_Tree::print_main(){
//     this->head->print_tree();
// }

// void Node::print_tree(){
//     for(int i = 0; i < level; i++){
//         std::cout << "   ";
//     }
//     std::cout << level << " " << bbox.min_point << " " << bbox.max_point << "\n";
//     for (Triangle* triangle: triangles){
//         for(int i = 0; i < level; i++){
//             std::cout << "   ";
//         }
//         std::cout << "triangle" << triangle->a << " " << triangle->b << " " << triangle->c << "\n";
//     }
//     for (Sphere* sphere: spheres){
//         for(int i = 0; i < level; i++){
//             std::cout << "   ";
//         }
//         std::cout << "sphere" << sphere->center << " " << sphere->radius << "\n";
//     }
//     if (left) left->print_tree();
//     if (right) right->print_tree();
// }