/*
 *  Common utility functions
 */

#ifndef PACMAN3D_UTILS_H
#define PACMAN3D_UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils {

    // https://stackoverflow.com/a/30099727
    // use ignoreResult(some_function_call()) to explicitly ignore
    // return values we don't need, and suppress compiler warnings
    template<typename T>
    inline void ignoreResult(const T &) {}

    inline glm::vec3 getTranslationVector(const glm::mat4& transform_matrix)
    {
        const float* t = glm::value_ptr(transform_matrix);
        return glm::vec3(t[12], t[13], t[14]);
    }
}

#endif // PACMAN3D_UTILS_H
