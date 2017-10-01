#include <vector>
#include <stdio.h>
#include <string.h>
#include <string>

#include <glm/glm.hpp>

#include "pathloader.hpp"
#include "utils.hpp"

// returns true if load is successful, false otherwise
bool loadPath(const char* path, std::vector<glm::vec3>* vertices)
{
	FILE* file = fopen(path, "r");
	if (file == nullptr) {
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}

	int count;
	utils::ignoreResult(fscanf(file, "%d\n", &count));

	char stupidBuffer[1000];
	for (int i = count; i--; ) {
		float x, y, z;
		// read x, y and z values
		int matches = fscanf(file, "%f, %f, %f", &x, &y, &z);
		if (matches != 3) {
			printf("Invalid format!\n");
			return false;
		}

		// create new vertex from x, y and z values
		vertices->emplace_back(x, y, z);

		// eat the rest of the line
		utils::ignoreResult(fgets(stupidBuffer, 1000, file));
	}

	return true;
}
