#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#define ASSIMP_BUILD_NO_EXPORT
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

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

//particle's attribute, can send into shader
struct Particle {
    glm::vec3 position;
    glm::vec3 color;
    float size;
};

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
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

    extern const float INIT_TIME_SCALE;
    extern const float BULLET_TIME_SCALE;
    //for the time scale, need multiple by every position calculate
    extern float timeScale;

    unsigned int loadTexture(const std::string path);
    unsigned int loadCubemap(const std::vector<std::string> faces);
}

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader* shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

class Model
{
public:
    Model(char* path)
    {
        loadModel(path);
    }
    void Draw(Shader* shader);
private:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};