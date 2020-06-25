/*!
 * @file
 * @brief This file contains triangle clipping method 1
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief Triangle clipping method 1
 */
class TriangleClip1Method: public Method{
  public:
    TriangleClip1Method();
    virtual ~TriangleClip1Method();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
};


