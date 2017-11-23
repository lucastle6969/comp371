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

        Light(glm::vec3 direction, glm::vec3 color){
            this->light_direction = direction;
            this->color = color;
        }
    };


#endif //PROCEDURALWORLD_LIGHT_H
