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
        glm::vec3 daytime;

        Light(glm::vec3 direction, glm::vec3 color){
            this->daytime = direction;
            this->color = color;
            this->position_light_color = glm::vec3(.8, .7, .0);
        }

        void setDaytime(){

            if (daytime.y < 0){
                light_direction = daytime;
                color = glm::vec3(.5,.5,.5);
                nighttime_value = (-daytime.y);
                daytime_value = 1-nighttime_value;
                fog_color = glm::vec3(.5,.5 ,.75)* nighttime_value;
            } else {
                light_direction = -daytime;
                color = glm::vec3(.2,.2,.5);
                nighttime_value = (daytime.y);
                daytime_value = 1-nighttime_value;
                fog_color = glm::vec3(.1,.1 ,.15)* nighttime_value;
            }
        };

    };


#endif //PROCEDURALWORLD_LIGHT_H
