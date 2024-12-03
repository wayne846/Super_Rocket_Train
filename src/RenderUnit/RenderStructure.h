#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff; // need set by cos(theta)
    float outerCutOff; //need set by cos(theta)

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Object {
    unsigned int VAO;
    unsigned int VBO[4];
    unsigned int EBO;
    unsigned int element_amount;
};

namespace RenderDatabase {
    extern const Material SLIVER_MATERIAL;
    extern const Material WHITE_PLASTIC_MATERIAL;
    extern const Material GREEN_PLASTIC_MATERIAL;
    extern const Material RUBY_MATERIAL;

    extern const glm::vec3 YELLOW_COLOR;
    extern const glm::vec3 BLUE_COLOR;
    extern const glm::vec3 WHITE_COLOR;
    extern const glm::vec3 LIGHT_GRAY_COLOR;
    extern const glm::vec3 GRAY_COLOR;
    extern const glm::vec3 DARK_COLOR;
    extern const glm::vec3 BLACK_COLOR;

    unsigned int loadTexture(const std::string path);
    unsigned int loadCubemap(const std::vector<std::string> faces);
}