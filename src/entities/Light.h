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
        //for dayTime calculation
        float daytime_value;
        float nighttime_value;
        glm::vec3 fog_color;

        Light(glm::vec3 direction, glm::vec3 color){
            this->light_direction = direction;
            this->color = color;
            this->position_light_color = glm::vec3(.5,.5,.5);
        }

        void setDaytime(){

            if (light_direction.y < 0){
                daytime_value = (0.5);
                nighttime_value = (-light_direction.y);
                fog_color = glm::vec3(.5,.5 ,.75)* nighttime_value;
            } else {
                daytime_value = (light_direction.y);
                nighttime_value = (0.0);
                fog_color = glm::vec3(.5,.5 ,.75)* nighttime_value;
            }
        };

    };


#endif //PROCEDURALWORLD_LIGHT_H
