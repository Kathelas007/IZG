/*!
 * @file
 * @brief This file contains triangle clipping 2 rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief Triangle clipping 2 rendering method
 */
class TriangleClip2Method: public Method{
  public:
    TriangleClip2Method();
    virtual ~TriangleClip2Method();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
};


