#pragma once

#include <stack>
#include <vector>

#include <glad/gl.h>
#include <tiny_obj_loader.h>

#include <glm/mat4x4.hpp>

#include "glm/ext/matrix_float4x4.hpp"
#include "matrices.hpp"

class ObjModel {
    public:
        tinyobj::attrib_t                 attrib;
        std::vector<tinyobj::shape_t>     shapes;
        std::vector<tinyobj::material_t>  materials;

        size_t num_indices;
        GLuint vao_id;

        int object_id;

        ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);

        void compute_normals();

        void build_triangles();
        void draw();
};

class Object {
    public:
        ObjModel& model;
        glm::mat4 transformation_matrix;

        std::vector<Object*> children;

        Object(ObjModel& model, glm::mat4 transformation_matix = Matrix_Identity());

        void draw(GLint model_uniform, GLint object_id_uniform);

    protected:

};

class MatrixStack {
    private:
        std::stack<glm::mat4> stack;

    public:
        void push(glm::mat4 m);
        glm::mat4 top();
        void pop();
};

void draw_object(Object* root, MatrixStack matrix_stack, GLint model_uniform, GLint object_id_uniform);
