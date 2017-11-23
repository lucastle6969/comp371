//
// Created by Andre on 2017-11-18.
//

#ifndef PROCEDURALWORLD_LIGHT_H
#include <glm/glm.hpp>

#define PROCEDURALWORLD_LIGHT_H

    class Light{
    public:
        glm::vec3 light_direction;
        glm::vec3 color;
        glm::vec3 light_position;
        glm::vec3 position_light_color;

        Light(glm::vec3 direction, glm::vec3 color){
            this->light_direction = direction;
            this->color = color;
            this->light_position = glm::vec3 (0,1,0);
            this->position_light_color = glm::vec3(.5,.5,.5);
        }
    };


#endif //PROCEDURALWORLD_LIGHT_H
