/*
 *  Common utility functions
 */

#ifndef PROCEDURALWORLD_UTILS_H
#define PROCEDURALWORLD_UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include <src/vendor/PerlinNoise.hpp>

namespace utils {
	// https://stackoverflow.com/a/30099727
	// use ignoreResult(some_function_call()) to explicitly ignore
	// return values we don't need, and suppress compiler warnings
	template<typename T>
	inline void ignoreResult(const T &) {}

	// min is inclusive, max is exclusive
	inline float randomFloat(const float &min = 0.0f, const float &max = 1.0f) {
		// based on example: https://stackoverflow.com/a/27934300

		// random from 0 (inclusive) to 1 (exclusive)
		double r = ((double) (rand() % RAND_MAX) / RAND_MAX);
		// adjust to max/min range
		return float(r * (max - min) + min);
	}

	inline glm::vec3 getTranslationVector(const glm::mat4 &transform_matrix) {
		const float *t = glm::value_ptr(transform_matrix);
		return glm::vec3(t[12], t[13], t[14]);
	}

	inline glm::vec3 getScaleVector(const glm::mat4 &scale_matrix) {
		const float *t = glm::value_ptr(scale_matrix);
		return glm::vec3(t[0], t[5], t[10]);
	}

	// returns 3-octave perlin
	inline float getPerlinNoise(const double& x, const double& y)
	{
		static siv::PerlinNoise perlin(0);
		static std::int32_t octaves = 3;
		return (float)perlin.octaveNoise(x, y, octaves);
	}

	inline void printVec(const glm::vec3& vec)
	{
		std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]" << std::endl;
	}
}

#endif // PROCEDURALWORLD_UTILS_H
