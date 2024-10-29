#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <cfloat>

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;

        Vec3f(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        static const Vec3f MAXVEC;
        static const Vec3f MINVEC;

        float calculateMagnitude() const {
            return pow(pow(x, 2) + pow(y, 2) + pow(z, 2), 0.5);
        }

        Vec3f getUnitVector() const {
            float magnitude = calculateMagnitude();
            return {x/magnitude, y/magnitude, z/magnitude};
        }

        float dotProductWith(const Vec3f &vec) const {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        Vec3f crossProductWith(const Vec3f &vec) const {
            Vec3f crossResult;
            crossResult.x = y * vec.z - z * vec.y;
            crossResult.y = z * vec.x - x * vec.z;
            crossResult.z = x * vec.y - y * vec.x;
            return crossResult;
        }

        Vec3f operator*(const float &multiplier) const {
            return {x*multiplier, y*multiplier, z*multiplier};
        }

        Vec3f operator*(const parser::Vec3f &vec) const {
            return Vec3f(x * vec.x, y * vec.y, z * vec.z);
        }

        Vec3f operator+(const Vec3f &vec) const {
            return {x + vec.x, y + vec.y, z + vec.z};
        }

        Vec3f operator-(const Vec3f &vec) const {
            return {x - vec.x, y - vec.y, z - vec.z};
        }

        Vec3f operator/(const float &div) const {
            return {x/div, y/div, z/div};
        }

        Vec3f operator-() const {
            return {-x, -y, -z};
        }

        bool operator<(const Vec3f &vec) const {
            return x < vec.x && y < vec.y && z < vec.z;
        }

        bool operator>(const Vec3f &vec) const {
            return x > vec.x && y > vec.y && z > vec.z;
        }

        bool operator<=(const Vec3f &vec) const {
            return x <= vec.x && y <= vec.y && z <= vec.z;
        }

        bool operator>=(const Vec3f &vec) const {
            return x >= vec.x && y >= vec.y && z >= vec.z;
        }

        bool operator==(const Vec3f &vec) const {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec3f& vec) {
            os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
            return os;
        }
        
        static float dotProduct(const Vec3f &vec1, const Vec3f &vec2){
            return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
        }
        
        static Vec3f crossProduct(const Vec3f &vec1, const Vec3f &vec2){
            Vec3f crossResult;
            crossResult.x = vec1.y * vec2.z - vec1.z * vec2.y;
            crossResult.y = vec1.z * vec2.x - vec1.x * vec2.z;
            crossResult.z = vec1.x * vec2.y - vec1.y * vec2.x;
            return crossResult;
        }

        static float cosOfAngelBetween(const Vec3f &vec1, const Vec3f &vec2) {
            float dotResult = dotProduct(vec1, vec2);
            return (dotResult / (vec1.calculateMagnitude() * vec2.calculateMagnitude()));
        }

        static float sinOfAngelBetween(const Vec3f &vec1, const Vec3f &vec2){
            return pow((1 - cosOfAngelBetween(vec1, vec2)), 0.5);
        }

        static float radOfAngelBetween(const Vec3f &vec1, const Vec3f &vec2){
            return acos(cosOfAngelBetween(vec1, vec2));
        }

        static float square_distance(const Vec3f &vec1, const Vec3f &vec2) {
            return pow(vec1.x - vec2.x, 2) + pow(vec1.y - vec2.y, 2) + pow(vec1.z - vec2.z, 2);
        }
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Plane
    {
        float left, right, bottom, top;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec3f u;
        Plane near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        bool is_mirror;
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };


    struct Triangle
    {
        int material_id;
        Vec3f a, b, c;
        Vec3f normal_vector;
        Vec3f centeroid;

        void calculateAndSetNormalVector(){
            normal_vector = (c-b).crossProductWith(a-b);
            centeroid = (a + b + c) / 3;
        }
    };

    struct Sphere
    {
        int material_id;
        Vec3f center;
        float radius;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        //Functions
        void loadFromXml(const std::string &filepath);
    };
}

#endif
