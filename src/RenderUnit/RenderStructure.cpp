#include "RenderStructure.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include <glad/glad.h>

namespace RenderDatabase {
    const Material SLIVER_MATERIAL = {
        glm::vec3(0.19225f, 0.19225f, 0.19225f),
        glm::vec3(0.50754f, 0.50754f, 0.50754f),
        glm::vec3(0.508273f, 0.508273f, 0.508273f),
        102.4f
    };
    const Material WHITE_PLASTIC_MATERIAL = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.55f, 0.55f, 0.55f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        64.0f
    };
    const Material GREEN_PLASTIC_MATERIAL = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.1f, 0.35f, 0.1f),
        glm::vec3(0.45f, 0.55f, 0.45f),
        64.0f
    };
    const Material RUBY_MATERIAL = {
        glm::vec3(0.1745f, 0.01175f, 0.01175f),
        glm::vec3(0.61424f, 0.04136f, 0.04136f),
        glm::vec3(0.727811f, 0.626959f, 0.626959),
        20.0f
    };

    const glm::vec3 YELLOW_COLOR = glm::vec3(0.5f, 0.5f, .1f);
    const glm::vec3 BLUE_COLOR = glm::vec3(.1f, .1f, .3f);
    const glm::vec3 WHITE_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
    const glm::vec3 LIGHT_GRAY_COLOR = glm::vec3(0.7f, 0.7f, 0.7f);
    const glm::vec3 GRAY_COLOR = glm::vec3(.3f, .3f, .3f);
    const glm::vec3 DARK_COLOR = glm::vec3(.1f, .1f, .1f);
    const glm::vec3 BLACK_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

    unsigned int loadTexture(const std::string path){
        unsigned int textureID;
        glGenTextures(1, &textureID);

        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB; //GL_RGB
            else if (nrComponents == 4)
                format = GL_RGBA; //GL_RGBA

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    unsigned int loadCubemap(const std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
}