//
// Created by Andre on 2017-11-18.
//

#ifndef PROCEDURALWORLD_LIGHT_H
#include <glm/glm.hpp>

#define PROCEDURALWORLD_LIGHT_H

    class Light{
    public:
        glm::vec3 light_direction;
        glm::vec3 light_position;

        Light(glm::vec3 direction, glm::vec3 position){
            this->light_direction = direction;
            this->light_position = position;
        }
    };


#endif //PROCEDURALWORLD_LIGHT_H
