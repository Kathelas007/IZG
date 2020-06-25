/*!
 * @file
 * @brief This file contains implementation of 2D triangle rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/triangleMethod.hpp>

/**
 * @brief Vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void triangle_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  if(inVertex.gl_VertexID == 0)
    outVertex.gl_Position = glm::vec4(-1,-1,0,1);
  if(inVertex.gl_VertexID == 1)
    outVertex.gl_Position = glm::vec4(+1,-1,0,1);
  if(inVertex.gl_VertexID == 2)
    outVertex.gl_Position = glm::vec4(-1,+1,0,1);
}

/**
 * @brief Fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void triangle_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  outFragment.gl_FragColor = glm::vec4(1.f);
}


TriangleMethod::TriangleMethod(){
  vao = gpu.createVertexPuller();
  prg = gpu.createProgram();
  gpu.attachShaders(prg,triangle_VS,triangle_FS);
}

TriangleMethod::~TriangleMethod(){
  gpu.deleteVertexPuller(vao);
  gpu.deleteProgram(prg);
}


void TriangleMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  gpu.clear(0,0,0,1);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  gpu.drawTriangles(3);

  gpu.unbindVertexPuller();
}

