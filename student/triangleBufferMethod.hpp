/*!
 * @file
 * @brief This file contains rendering method that renders triangle stored in buffer.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief This class represents triangle buffer rendering method
 */
class TriangleBufferMethod: public Method{
  public:
    TriangleBufferMethod();
    virtual ~TriangleBufferMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
    BufferID vbo;///< vertex buffer
};

