#include <student/triangleClip2Method.hpp>

/**
 * @brief Vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void triangleClip2_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);

  float z = 2.f/3.f;

  if(inVertex.gl_VertexID == 0)
    outVertex.gl_Position = glm::vec4(-z,-z,-z*e+f,+z);
  if(inVertex.gl_VertexID == 1)
    outVertex.gl_Position = glm::vec4(+z,-z,-z*e+f,+z);

  if(inVertex.gl_VertexID == 2)
    outVertex.gl_Position = glm::vec4(-2,+2,+2,+2);
}

/**
 * @brief Fragment shader
 * 
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void triangleClip2_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  outFragment.gl_FragColor = glm::vec4(1.f);
}


TriangleClip2Method::TriangleClip2Method(){
  vao = gpu.createVertexPuller();
  prg = gpu.createProgram();
  gpu.attachShaders(prg,triangleClip2_VS,triangleClip2_FS);
}

TriangleClip2Method::~TriangleClip2Method(){
  gpu.deleteVertexPuller(vao);
  gpu.deleteProgram(prg);
}

void TriangleClip2Method::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  gpu.clear(0,0,0,0);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  gpu.drawTriangles(3);

  gpu.unbindVertexPuller();
}

