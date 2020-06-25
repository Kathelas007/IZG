/*!
 * @file
 * @brief This file contains 2D triangle rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief 2D Triangle rendering method
 */
class TriangleMethod: public Method{
  public:
    TriangleMethod();
    virtual ~TriangleMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
};

