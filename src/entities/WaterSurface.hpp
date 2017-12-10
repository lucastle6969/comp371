#ifndef PROCEDURALWORLD_WATERSURFACE_HPP
#define PROCEDURALWORLD_WATERSURFACE_HPP

#include "Entity.hpp"
#include "DrawableEntity.hpp"

class WaterSurface : public DrawableEntity {
public:
	explicit WaterSurface(const GLuint& shader_program)
		: WaterSurface(shader_program, nullptr) {}
	WaterSurface(const GLuint& shader_program, Entity* parent);
	const std::vector<glm::vec3>& getVertices() const override;
	GLuint getVAO() override;
	const int getColorType() override;
};


#endif //PROCEDURALWORLD_WATERSURFACE_HPP
