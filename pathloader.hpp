#ifndef PATHLOADER_H
#define PATHLOADER_H

#include <glm/glm.hpp>
#include <vector>

bool loadPath(const char* path, std::vector<glm::vec3>* vertices);

#endif // PATHLOADER_H
