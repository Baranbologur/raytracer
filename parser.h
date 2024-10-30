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
    struct Vec3f {
        float x, y, z;

        Vec3f(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        static const Vec3f MAXVEC;
        static const Vec3f MINVEC;

        inline float calculateMagnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vec3f getUnitVector() const {
            float magnitude = calculateMagnitude();
            return (magnitude > 0) ? Vec3f(x / magnitude, y / magnitude, z / magnitude) : Vec3f(0, 0, 0);
        }

        inline float dotProductWith(const Vec3f& vec) const {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        Vec3f crossProductWith(const Vec3f& vec) const {
            return Vec3f(
                y * vec.z - z * vec.y,
                z * vec.x - x * vec.z,
                x * vec.y - y * vec.x
            );
        }

        inline Vec3f operator*(float multiplier) const {
            return Vec3f(x * multiplier, y * multiplier, z * multiplier);
        }

        inline Vec3f operator*(const Vec3f& vec) const {
            return Vec3f(x * vec.x, y * vec.y, z * vec.z);
        }

        inline Vec3f operator+(const Vec3f& vec) const {
            return Vec3f(x + vec.x, y + vec.y, z + vec.z);
        }

        inline Vec3f operator-(const Vec3f& vec) const {
            return Vec3f(x - vec.x, y - vec.y, z - vec.z);
        }

        inline Vec3f operator/(float div) const {
            return Vec3f(x / div, y / div, z / div);
        }

        inline Vec3f operator-() const {
            return Vec3f(-x, -y, -z);
        }

        inline bool operator<(const Vec3f& vec) const {
            return x < vec.x && y < vec.y && z < vec.z;
        }

        inline bool operator>(const Vec3f& vec) const {
            return x > vec.x && y > vec.y && z > vec.z;
        }

        inline bool operator<=(const Vec3f& vec) const {
            return x <= vec.x && y <= vec.y && z <= vec.z;
        }

        inline bool operator>=(const Vec3f& vec) const {
            return x >= vec.x && y >= vec.y && z >= vec.z;
        }

        inline bool operator==(const Vec3f& vec) const {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec3f& vec) {
            os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
            return os;
        }

        static inline float dotProduct(const Vec3f& vec1, const Vec3f& vec2) {
            return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
        }

        static Vec3f crossProduct(const Vec3f& vec1, const Vec3f& vec2) {
            return Vec3f(
                vec1.y * vec2.z - vec1.z * vec2.y,
                vec1.z * vec2.x - vec1.x * vec2.z,
                vec1.x * vec2.y - vec1.y * vec2.x
            );
        }

        static float cosOfAngleBetween(const Vec3f& vec1, const Vec3f& vec2) {
            float dotResult = dotProduct(vec1, vec2);
            float magnitudes = vec1.calculateMagnitude() * vec2.calculateMagnitude();
            return (magnitudes > 0) ? (dotResult / magnitudes) : 0.0f;
        }

        static float sinOfAngleBetween(const Vec3f& vec1, const Vec3f& vec2) {
            float cosAngle = cosOfAngleBetween(vec1, vec2);
            return std::sqrt(1 - cosAngle * cosAngle);
        }

        static float radOfAngleBetween(const Vec3f& vec1, const Vec3f& vec2) {
            return std::acos(cosOfAngleBetween(vec1, vec2));
        }

        static float squareDistance(const Vec3f& vec1, const Vec3f& vec2) {
            float dx = vec1.x - vec2.x;
            float dy = vec1.y - vec2.y;
            float dz = vec1.z - vec2.z;
            return dx * dx + dy * dy + dz * dz;
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

        Triangle(){
        }

        Triangle(Vec3f& a, Vec3f& b, Vec3f& c, int material_id): a(a), b(b), c(c), material_id(material_id){
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
