/*!
 * @file
 * @brief This file contains implementation of rendering method that renders triangle stored in buffer.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/triangleBufferMethod.hpp>

/**
 * @brief This function is vertex shader of triangle buffer method
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void triangleBuffer_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  outVertex.gl_Position = glm::vec4(inVertex.attributes[0].v2,0.f,1.f);
}

/**
 * @brief This function is fragment shader of triangle buffer method
 *
 * @param outFragment output fragment
 * @param inFragment input fragment 
 * @param uniforms uniform variables
 */
void triangleBuffer_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  outFragment.gl_FragColor = glm::vec4(0.f,1.f,0.f,1.f);
}


TriangleBufferMethod::TriangleBufferMethod(){
  float const vertices[] = {
    -.5f,-.5f,
    -.1f,-.5f,
    -.5f,-.1f,

    +.1f,-.5f,
    +.5f,-.5f,
    +.5f,-.1f,

    -.5f,+.1f,
    -.1f,+.5f,
    -.5f,+.5f,

    +.1f,+.5f,
    +.5f,+.1f,
    +.5f,+.5f,
  };
  vbo = gpu.createBuffer(sizeof(vertices));
  gpu.setBufferData(vbo,0,sizeof(vertices),vertices);

  vao = gpu.createVertexPuller();
  gpu.setVertexPullerHead(vao,0,AttributeType::VEC2,sizeof(float)*2,0,vbo);
  gpu.enableVertexPullerHead(vao,0);

  prg = gpu.createProgram();
  gpu.attachShaders(prg,triangleBuffer_VS,triangleBuffer_FS);
}

TriangleBufferMethod::~TriangleBufferMethod(){
  gpu.deleteVertexPuller(vao);
  gpu.deleteProgram(prg);
}


void TriangleBufferMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  gpu.clear(0,0,0,0);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  gpu.drawTriangles(3*4);

  gpu.unbindVertexPuller();
}

