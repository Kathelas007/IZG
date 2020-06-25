/*!
 * @file
 * @brief This file contains empty rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief Empty rendering method
 */
class EmptyMethod: public Method{
  public:
    EmptyMethod();
    virtual ~EmptyMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
};

EmptyMethod::EmptyMethod(){
}

EmptyMethod::~EmptyMethod(){
}

void EmptyMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  gpu.clear(0,0,0,1);
}
