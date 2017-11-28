/*
 *  Common utility functions
 */

#ifndef PROCEDURALWORLD_UTILS_H
#define PROCEDURALWORLD_UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace utils {
	// platform-independent rand/srand implementations taken from C
	// standard 7.20.2 http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
	static unsigned long int next = 1;
	inline int rand() // RAND_MAX assumed to be 32767
	{
		next = next * 1103515245 + 12345;
		return (unsigned int)(next / 65536) % 32768;
	}
	inline void srand(unsigned int seed)
	{
		next = seed;
	}

	// https://stackoverflow.com/a/30099727
	// use ignoreResult(some_function_call()) to explicitly ignore
	// return values we don't need, and suppress compiler warnings
	template<typename T>
	inline void ignoreResult(const T &) {}

	// min is inclusive, max is exclusive
	inline float randomFloat(const float &min = 0.0f, const float &max = 1.0f) {
		// based on example: https://stackoverflow.com/a/27934300

		// random from 0 (inclusive) to 1 (exclusive)
		double r = ((double) (utils::rand() % RAND_MAX) / RAND_MAX);
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
}

#endif // PROCEDURALWORLD_UTILS_H
