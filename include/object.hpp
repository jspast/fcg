#pragma once

#include <cmath>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <glad/gl.h>
#include <tiny_obj_loader.h>

#include <glm/mat4x4.hpp>

#include "gpu.hpp"
#include "matrices.hpp"

class AABB {
    public:
        float min_x = INFINITY;
        float min_y = INFINITY;
        float min_z = INFINITY;

        float max_x = -INFINITY;
        float max_y = -INFINITY;
        float max_z = -INFINITY;
};

class ObjModel {
    public:
        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;

        AABB aabb;

        ObjModel(std::string inputfile,
                 std::string mtl_search_path = "",
                 bool triangulate = true);

        void compute_normals();

        void build_triangles();
        void draw();

    private:
        size_t num_indices;
        GLuint vao_id;
};

class Object {
    public:
        using UniformValue = std::variant<float, int, glm::vec4, glm::mat4>;

        ObjModel& model;
        glm::mat4 transform = Matrix_Identity();

        std::vector<Object*> children;

        Object(ObjModel& model, GpuProgram& gpu_program);

        void set_transform(glm::mat4 transform);

        void set_uniform(std::string_view name, UniformValue value);
        void apply_uniforms();

        void draw(glm::mat4 parent_transform = Matrix_Identity());

    private:
        std::unordered_map<std::string, UniformValue> uniforms;
        GpuProgram& gpu_program;
};
