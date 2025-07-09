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
        void draw(GpuProgram& gpu_program);

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

        void add_instance(glm::mat4 transform);
        void deactivate_instance(int instance_id);
        bool is_active(int instance_id);

        void set_transform(int instance_id, glm::mat4 transform);

        void set_uniform(std::string_view name, UniformValue value);
        void apply_uniforms();

        void draw(glm::mat4 parent_transform = Matrix_Identity());

    private:
        std::unordered_map<std::string, UniformValue> uniforms;
        GpuProgram& gpu_program;

        std::vector<glm::mat4> transforms;
        size_t num_instances = 0;
        std::vector<bool> inactive_instances;

        std::vector<std::shared_ptr<Object>> children;
};
