#include <iostream>
#include "ray.h"
#include "common.h"
#include <algorithm> 
#include "bvh.h"

Ray::Ray(parser::Vec3f start_position, parser::Vec3f direction): start_position(start_position), direction(direction){;}

intersectionInfo Ray::getIntersectionInfoWithTriangle(const parser::Triangle &triangle) const {
    float epsilon = 1e-5;

    float a_x = triangle.a.x - triangle.b.x;
    float a_y = triangle.a.y - triangle.b.y;
    float a_z = triangle.a.z - triangle.b.z;

    float b_x = triangle.a.x - triangle.c.x;
    float b_y = triangle.a.y - triangle.c.y;
    float b_z = triangle.a.z - triangle.c.z;

    float d_x = direction.x;
    float d_y = direction.y;
    float d_z = direction.z;

    float r_x = triangle.a.x - start_position.x;
    float r_y = triangle.a.y - start_position.y;
    float r_z = triangle.a.z - start_position.z;

    float determinant = a_x * (b_y * d_z - b_z * d_y)
                      - a_y * (b_x * d_z - b_z * d_x)
                      + a_z * (b_x * d_y - b_y * d_x);

    if (determinant == 0) {
        return intersectionInfo(false);
    }

    float beta_numerator = r_x * (b_y * d_z - b_z * d_y)
                         - r_y * (b_x * d_z - b_z * d_x)
                         + r_z * (b_x * d_y - b_y * d_x);

    float beta = beta_numerator / determinant;

    float gamma_numerator = a_x * (r_y * d_z - r_z * d_y)
                          - a_y * (r_x * d_z - r_z * d_x)
                          + a_z * (r_x * d_y - r_y * d_x);

    float gamma = gamma_numerator / determinant;

    float t_numerator = a_x * (b_y * r_z - b_z * r_y)
                      - a_y * (b_x * r_z - b_z * r_x)
                      + a_z * (b_x * r_y - b_y * r_x);

    float t = t_numerator / determinant;

    if (beta >= -epsilon && gamma >= -epsilon && beta + gamma <= 1 + epsilon) {
        return intersectionInfo(true, t);
    }

    return intersectionInfo(false);
}

intersectionInfo Ray::getIntersectionInfoWithSphere(const parser::Sphere& sphere) const {
    parser::Vec3f tmpvec = start_position - sphere.center;
    float dirDotDir = direction.dotProductWith(direction);
    float tmpDotDir = direction.dotProductWith(tmpvec);
    float tmpDotTmp = tmpvec.dotProductWith(tmpvec);

    float discriminant = tmpDotDir * tmpDotDir - dirDotDir * (tmpDotTmp - sphere.radius * sphere.radius);

    if (discriminant < 0) {
        return intersectionInfo(false);
    }

    float sqrtDiscriminant = std::sqrt(discriminant);
    float t = (-tmpDotDir - sqrtDiscriminant) / dirDotDir;

    return intersectionInfo(true, t);
}

ClosestIntersectedObjectInfo Ray::findIntersectedObject(const Node& node, const bool& backface_culling_enabled) const {
    parser::Triangle* closestTriangle;
    parser::Sphere* closestSphere;
    float min_t = MAXFLOAT;
    bool found = false;
    bool sphere_found = false;

    for (parser::Triangle* triangle: node.triangles){ 
        if (backface_culling_enabled && direction.dotProductWith(triangle->unit_normal_vector) >= 0){
            continue;
        }
        intersectionInfo intersectionInfo = getIntersectionInfoWithTriangle(*triangle);
        if(intersectionInfo.isIntersected && intersectionInfo.t < min_t && intersectionInfo.t > 0){
            closestTriangle = triangle;
            min_t = intersectionInfo.t;
            found = true;
        }
    }
    for (parser::Sphere* sphere: node.spheres){
        if (backface_culling_enabled && direction.dotProductWith(start_position - sphere->center) >= 0){
            continue;
        }
        intersectionInfo intersectionInfo = getIntersectionInfoWithSphere(*sphere);
        if(intersectionInfo.isIntersected && intersectionInfo.t < min_t && intersectionInfo.t > 0){
            closestSphere = sphere;
            min_t = intersectionInfo.t;
            found = true;
            sphere_found = true;
        }
    }
    parser::Vec3f intersection_point = start_position + direction * min_t;

    if (found && sphere_found) return ClosestIntersectedObjectInfo(closestSphere, min_t, intersection_point);
    else if (found) return ClosestIntersectedObjectInfo(closestTriangle, min_t, intersection_point);
    else return ClosestIntersectedObjectInfo(false);
}

RGB Ray::getcolor(const BVH_Tree &tree, const parser::Scene &scene, const int &depth) const {
    if (depth == -1){
        return RGB(0,0,0);
    }
    ClosestIntersectedObjectInfo objectInfo = tree.getIntersectInfo(*this, true);

    if (!objectInfo.isIntersectedWithAnyObject) {
        if (depth == scene.max_recursion_depth){
            return RGB(scene.background_color);
        } else{
            return RGB(0,0,0);
        }
    }

    parser::Material material = scene.materials[objectInfo.material_id - 1];
    RGB color = computeAmbientColor(material, scene.ambient_light);
    for(const parser::PointLight& pointlight: scene.point_lights){
        float cos = parser::Vec3f::cosOfAngleBetween(pointlight.position - objectInfo.intersection_point, objectInfo.unit_normal_vector);
        if (cos <= 0){
            continue;
        }
        parser::Vec3f new_ray_start_position = objectInfo.intersection_point + objectInfo.unit_normal_vector * scene.shadow_ray_epsilon;
        Ray ray_to_light = Ray(new_ray_start_position, pointlight.position - new_ray_start_position);
        ClosestIntersectedObjectInfo closestIntersectedObjectInfoForLightRay = tree.getIntersectInfo(ray_to_light, false);
        if (closestIntersectedObjectInfoForLightRay.isIntersectedWithAnyObject && closestIntersectedObjectInfoForLightRay.t < 1){
            continue;
        }
        color = color + computeDiffuseColor(objectInfo.intersection_point, objectInfo.unit_normal_vector, material, pointlight);
        color = color + computeSpecularColor(objectInfo.intersection_point, objectInfo.unit_normal_vector, material, pointlight);
    }

    if (material.is_mirror){
        parser::Vec3f new_ray_start_position = objectInfo.intersection_point + objectInfo.unit_normal_vector * scene.shadow_ray_epsilon;
        float cosTheta = parser::Vec3f::cosOfAngleBetween(-direction, objectInfo.unit_normal_vector);
        Ray new_ray = Ray(new_ray_start_position, (objectInfo.unit_normal_vector * 2 * cosTheta) + direction.getUnitVector());
        color = color + new_ray.getcolor(tree, scene, depth - 1) * material.mirror;
    }
    return color;
}

RGB Ray::computeDiffuseColor(const parser::Vec3f &position, const parser::Vec3f &normal_vector, const parser::Material &material, const parser::PointLight &pointlight) const {
    float cosTheta = parser::Vec3f::cosOfAngleBetween(pointlight.position - position, normal_vector);
    cosTheta = std::max(0.0f, cosTheta);
    return RGB((pointlight.intensity * material.diffuse * cosTheta) / (parser::Vec3f::squareDistance(pointlight.position, position)));
}

RGB Ray::computeAmbientColor(const parser::Material &material, const parser::Vec3f &ambient_light) const {
    return RGB(material.ambient * ambient_light);
}

RGB Ray::computeSpecularColor(const parser::Vec3f &position, const parser::Vec3f &normal_vector, const parser::Material &material, const parser::PointLight &pointlight) const {
    parser::Vec3f w_light = (pointlight.position - position).getUnitVector();
    parser::Vec3f w_camera = (-direction).getUnitVector();
    parser::Vec3f h = (w_light + w_camera).getUnitVector();
    float cosAlpha = parser::Vec3f::cosOfAngleBetween(h, normal_vector);
    cosAlpha = std::max(0.0f, cosAlpha);
    return RGB(material.specular * pointlight.intensity * pow(cosAlpha, material.phong_exponent) / (parser::Vec3f::squareDistance(pointlight.position, position)));
}