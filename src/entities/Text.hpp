#ifndef PROCEDURALWORLD_TEXT_HPP
#define PROCEDURALWORLD_TEXT_HPP
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h> // include GL Extension Wrangler
#endif

#include <string>
#include "Entity.hpp"
#include "DrawableEntity.hpp"

class Text : public DrawableEntity {
private:
    std::string message;
public:
    Text(
            const GLuint& shader_program,
            const std::string& message
    ) : Text(
            shader_program,
            message,
            nullptr
    ){}
    Text(
            const GLuint& shader_program,
            const std::string& message,
            Entity* parent
    );
    ~Text() override;
    const std::vector<glm::vec3>& getVertices() override;
    GLuint getVAO() override;
    const int getColorType() override;
    GLuint getTextureId() override;

};


#endif //PROCEDURALWORLD_TEXT_HPP
