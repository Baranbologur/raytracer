#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "ray.h"
#include "common.h"
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>
#include "bvh.h"

void render_section(int start_row, int end_row, unsigned char* image, parser::Camera& camera, parser::Scene& scene, BVH_Tree& tree, parser::Vec3f top_left_point, parser::Vec3f right_vector_per_pixel, parser::Vec3f top_vector_per_pixel) {
    int img = start_row * camera.image_width * 3;
    for (int j = start_row; j < end_row; j++) {
        for (int i = 0; i < camera.image_width; i++) {
            parser::Vec3f pixel_point = top_left_point + right_vector_per_pixel * (i + 0.5) - top_vector_per_pixel * (j + 0.5);
            parser::Vec3f ray_direction = pixel_point - camera.position;
            Ray ray = Ray(camera.position, ray_direction);
            RGB rgb = ray.getcolor(tree, scene, scene.max_recursion_depth);
            rgb.truncate();
            image[img++] = rgb.r;
            image[img++] = rgb.g;
            image[img++] = rgb.b;
        }
    }
}

int main(int argc, char* argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    auto start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::cout << "Execution is started at " 
              << std::put_time(std::localtime(&start_time), "%H:%M:%S") 
              << std::endl;

    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    BVH_Tree tree = BVH_Tree(scene);

    for (parser::Camera camera : scene.cameras) {
        parser::Vec3f center_point = camera.position + camera.gaze * camera.near_distance;
        parser::Vec3f top_left_point = center_point + camera.u * camera.near_plane.left + camera.up * camera.near_plane.top;
        float index_width = (camera.near_plane.right - camera.near_plane.left) / camera.image_width;
        float index_height = (camera.near_plane.top - camera.near_plane.bottom) / camera.image_height;
        parser::Vec3f right_vector_per_pixel = camera.u * index_width;
        parser::Vec3f top_vector_per_pixel = camera.up * index_height;

        unsigned char* image = new unsigned char[camera.image_height * camera.image_width * 3];
        int num_threads = 16;
        int rows_per_thread = camera.image_height / num_threads;
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            int start_row = t * rows_per_thread;
            int end_row = (t == num_threads - 1) ? camera.image_height : start_row + rows_per_thread;
            threads.emplace_back(render_section, start_row, end_row, image, std::ref(camera), std::ref(scene), std::ref(tree), top_left_point, right_vector_per_pixel, top_vector_per_pixel);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        write_ppm(camera.image_name.c_str(), image, camera.image_width, camera.image_height);
        delete[] image;

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        long hours = duration.count() / 3600000;
        long minutes = (duration.count() % 3600000) / 60000;
        long seconds = (duration.count() % 60000) / 1000;
        long milliseconds = duration.count() % 1000;

        std::cout << "Execution time of " << camera.image_name << ": " << hours << " hours, "
                  << minutes << " minutes, "
                  << seconds << " seconds, "
                  << milliseconds << " milliseconds\n";

        start = end;
    }

    return 0;
}
