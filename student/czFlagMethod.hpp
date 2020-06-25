/*!
 * @file
 * @brief This file contains czech flag rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/**
 * @brief Czech flag rendering method
 */
class CZFlagMethod: public Method{
  public:
    CZFlagMethod();
    virtual ~CZFlagMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    virtual void onUpdate(float dt) override;
    ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
    BufferID vbo;///< vertex buffer
    BufferID ebo;///< index buffer
    float time = 0.f;///< elapsed time
    uint32_t const NX = 100 ;///< nof vertices in x direction
    uint32_t const NY = 10 ;///< nof vertices in y direction

};

