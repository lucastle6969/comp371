/*
 *  Common utility functions
 */

#ifndef PACMAN3D_UTILS_H
#define PACMAN3D_UTILS_H

namespace utils {

    // https://stackoverflow.com/a/30099727
    // use ignore_result(some_function_call()) to explicitly ignore
    // return values we don't need, and suppress compiler warnings
    template<typename T>
    inline void ignore_result(T) {}

}

#endif // PACMAN3D_UTILS_H
