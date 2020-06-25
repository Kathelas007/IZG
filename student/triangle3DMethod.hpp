/*!
 * @file
 * @brief This file contains 3D triangle rendering method.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief This class represents 3D triagnle rendering method
 */
class Triangle3DMethod: public Method{
  public:
    Triangle3DMethod();
    virtual ~Triangle3DMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
};

