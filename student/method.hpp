/*!
 * @file
 * @brief This file contains rendering method interface
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <iostream>

#include <glm/glm.hpp>

#include <student/gpu.hpp>

/**
 * @brief This class represents rendering method.
 */
class Method{
  public:
    /**
     * @brief Constructor of rendering method
     */
    Method(){}
    /**
     * @brief Destructor of rendering method
     */
    virtual ~Method(){}
    /**
     * @brief This functino is called every frame.
     *
     * @param proj projection matrix
     * @param view view matrix
     * @param light light position
     * @param camera camera position
     */
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) = 0;
    /**
     * @brief This function is called on update
     *
     * @param dt delta time - time between frames
     */
    virtual void onUpdate(float dt){}
    GPU gpu; ///< graphic card
};

