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
        glm::vec3 nighttime;

        Light(glm::vec3 direction, glm::vec3 color){
            this->daytime = direction;
            this->nighttime = glm::vec3(0.0f,-direction.y,0.0f);
            this->color = color;
            this->position_light_color = glm::vec3(.5,.5,.5);
        }

        void setDaytime(){

            if (daytime.y < 0){
                light_direction = daytime;
                //daytime_value = (1-daytime.y);
                nighttime_value = (-light_direction.y);
                fog_color = glm::vec3(.5,.5 ,.75)* nighttime_value;
            } else {
                light_direction = glm::vec3(1-daytime.x,1-daytime.y, 0.0f);
                //daytime_value = (daytime.y);
                nighttime_value = (light_direction.y);
                fog_color = glm::vec3(.5,.5 ,.75)* -daytime.y;
            }
        };

    };


#endif //PROCEDURALWORLD_LIGHT_H
