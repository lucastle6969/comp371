/*
 *  Common utility functions
 */

#ifndef PACMAN3D_UTILS_H
#define PACMAN3D_UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils {
    inline glm::vec3 getTranslationVector(const glm::mat4& transform_matrix)
    {
        const float* t = glm::value_ptr(transform_matrix);
        return glm::vec3(t[12], t[13], t[14]);
    }
}

#endif // PACMAN3D_UTILS_H
