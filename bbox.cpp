#include "bbox.h"

bool BBox::rayIntersect(Ray ray) {
    float t_enter_x = FLT_MIN;
    float t_enter_y = FLT_MIN;
    float t_enter_z = FLT_MIN;
    float t_exit_x = FLT_MAX;
    float t_exit_y = FLT_MAX;
    float t_exit_z = FLT_MAX;

    if (ray.direction == Vec3f(0,0,0)){
        return false;
    }
    
    if (ray.direction.x != 0) {
        float t_x_min = (min_point.x - ray.start_position.x) / ray.direction.x;
        float t_x_max = (max_point.x - ray.start_position.x) / ray.direction.x;
        if (ray.direction.x > 0) {
            t_enter_x = t_x_min;
            t_exit_x = t_x_max;
        } else {
            t_enter_x = t_x_max;
            t_exit_x = t_x_min;
        }
    }

    if (ray.direction.y != 0) {
        float t_y_min = (min_point.y - ray.start_position.y) / ray.direction.y;
        float t_y_max = (max_point.y - ray.start_position.y) / ray.direction.y;
        if (ray.direction.y > 0) {
            t_enter_y = t_y_min;
            t_exit_y = t_y_max;
        } else {
            t_enter_y = t_y_max;
            t_exit_y = t_y_min;
        }
    }

    if (ray.direction.z != 0) {
        float t_z_min = (min_point.z - ray.start_position.z) / ray.direction.z;
        float t_z_max = (max_point.z - ray.start_position.z) / ray.direction.z;
        if (ray.direction.z > 0) {
            t_enter_z = t_z_min;
            t_exit_z = t_z_max;
        } else {
            t_enter_z = t_z_max;
            t_exit_z = t_z_min;
        }
    }

    float t_enter_largest = max(t_enter_x, max(t_enter_y, t_enter_z));
    float t_exit_smallest = min(t_exit_x, min(t_exit_y, t_exit_z));
    return (t_enter_largest <= t_exit_smallest);
}