#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <glad/gl.h>
#include <tiny_obj_loader.h>

#include <glm/mat4x4.hpp>

#include "gpu.hpp"
#include "matrices.hpp"
#include "collisions.hpp"

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
        void draw(GpuProgram& gpu_program, size_t num_instances = 1);

        void print_info();

        size_t num_indices;
        GLuint vao_id;
};

class Object {
    public:
        using UniformValue = std::variant<float, int, glm::vec2, glm::vec4, glm::mat4>;

        std::shared_ptr<ObjModel> model;

        Object(std::shared_ptr<ObjModel> model, GpuProgram& gpu_program);

        void add_child(std::shared_ptr<Object> child);

        void set_instances(size_t num_instances);

        void set_transform(glm::mat4 transform);

        void set_uniform(std::string_view name, UniformValue value);
        void apply_uniforms();

        void draw(glm::mat4 parent_transform = Matrix_Identity());

    private:
        std::unordered_map<std::string, UniformValue> uniforms;
        GpuProgram& gpu_program;

        glm::mat4 transform = Matrix_Identity();
        size_t num_instances = 1;

        std::vector<std::shared_ptr<Object>> children;
};
