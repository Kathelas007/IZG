/*!
 * @file
 * @brief This file contains implementation of 3D triangle rendering method.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/triangle3DMethod.hpp>

/**
 * @brief This function represents vertex shader of 3D triangle rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void triangle3d_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  outVertex.gl_Position = glm::vec4(0.f,0.f,0.f,1.f);

  glm::mat4 viewMatrix       = uniforms.uniform[3].m4;
  glm::mat4 projectionMatrix = uniforms.uniform[2].m4;
 
  glm::mat4 mvp = projectionMatrix * viewMatrix;

  glm::vec4 pos;
  glm::vec4 col;

  if(inVertex.gl_VertexID == 0){
    outVertex.gl_Position = mvp*glm::vec4(-1.f,-1.f,0.f,1.f);
    outVertex.attributes[3].v4 = glm::vec4( 1.f, 0.f,0.f,1.f);
  }
  if(inVertex.gl_VertexID == 1){
    outVertex.gl_Position      = mvp*glm::vec4(1.f,-1.f,0.f,1.f);
    outVertex.attributes[3].v4 = glm::vec4(0.f, 1.f,0.f,1.f);
  }
  if(inVertex.gl_VertexID == 2){
    outVertex.gl_Position      = mvp*glm::vec4(-1.f,+1.f,0.f,1.f);
    outVertex.attributes[3].v4 = glm::vec4( 0.f, 0.f,1.f,1.f);
  }

}

/**
 * @brief This functionrepresents fragment shader of 3D triangle rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void triangle3d_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  outFragment.gl_FragColor = inFragment.attributes[3].v4;
}

/**
 * @brief Constructor
 */
Triangle3DMethod::Triangle3DMethod(){
  vao = gpu.createVertexPuller();
  prg = gpu.createProgram();
  gpu.attachShaders(prg,triangle3d_VS,triangle3d_FS);
  gpu.setVS2FSType(prg,3,AttributeType::VEC4);
}

/**
 * @brief This function is called every frame and should render 3D triangle
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void Triangle3DMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  gpu.clear(0,0,0,1);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);
  gpu.programUniformMatrix4f(prg,3,view);
  gpu.programUniformMatrix4f(prg,2,proj);
  gpu.drawTriangles(3);

  gpu.unbindVertexPuller();
}

/**
 * @brief Descturctor
 */
Triangle3DMethod::~Triangle3DMethod(){
  gpu.deleteProgram(prg);
  gpu.deleteVertexPuller(vao);
}
