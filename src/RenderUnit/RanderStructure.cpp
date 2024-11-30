#include "RanderStructure.h"

namespace RanderDatabase {
    const Material SLIVER_MATERIAL = {
        glm::vec3(0.19225f, 0.19225f, 0.19225f),
        glm::vec3(0.50754f, 0.50754f, 0.50754f),
        glm::vec3(0.508273f, 0.508273f, 0.508273f),
        51.2f
    };
    const Material WHITE_PLASTIC_MATERIAL = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.55f, 0.55f, 0.55f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        32.0f
    };
    const Material GREEN_PLASTIC_MATERIAL = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.1f, 0.35f, 0.1f),
        glm::vec3(0.45f, 0.55f, 0.45f),
        32.0f
    };

    const glm::vec3 YELLOW_COLOR = glm::vec3(0.5f, 0.5f, .1f);
    const glm::vec3 BLUE_COLOR = glm::vec3(.1f, .1f, .3f);
    const glm::vec3 WHITE_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
    const glm::vec3 LIGHT_GRAY_COLOR = glm::vec3(0.7f, 0.7f, 0.7f);
    const glm::vec3 GRAY_COLOR = glm::vec3(.3f, .3f, .3f);
    const glm::vec3 DARK_COLOR = glm::vec3(.1f, .1f, .1f);
    const glm::vec3 BLACK_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);
}