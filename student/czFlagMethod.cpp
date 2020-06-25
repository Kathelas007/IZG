/*!
 * @file
 * @brief This file contains implementation of czech flag rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/czFlagMethod.hpp>
#include <vector>

/**
 * @brief Czech flag vertex shader
 *
 * @param outVertex out vertex
 * @param inVertex in vertex
 * @param uniforms uniform variables
 */
void czFlag_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  auto const& pos   = inVertex.attributes[0].v2;
  auto const& coord = inVertex.attributes[1].v2;
  auto const& mvp   = uniforms.uniform[0].m4;

  auto time = uniforms.uniform[1].v1;
  
  auto z = (coord.x*0.5f)*glm::sin(coord.x*10.f + time);
  outVertex.gl_Position = mvp*glm::vec4(pos,z,1.f);

  outVertex.attributes[0].v2 = coord;
}

/**
 * @brief Czech flag fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void czFlag_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  auto const& vCoord = inFragment.attributes[0].v2;
  if(vCoord.y > vCoord.x && 1.f-vCoord.y>vCoord.x){
    outFragment.gl_FragColor = glm::vec4(0.f,0.f,1.f,1.f);
  }else{
    if(vCoord.y < 0.5f){
      outFragment.gl_FragColor = glm::vec4(1.f,0.f,0.f,1.f);
    }else{
      outFragment.gl_FragColor = glm::vec4(1.f,1.f,1.f,1.f);
    }
  }
}

CZFlagMethod::CZFlagMethod(){
  struct Vertex{
    glm::vec2 position;
    glm::vec2 texCoord;
  };
  std::vector<Vertex>vertices;

  for(size_t y=0;y<NY;++y)
    for(size_t x=0;x<NX;++x){
      glm::vec2 coord;
      coord.x = static_cast<float>(x) / static_cast<float>(NX-1);
      coord.y = static_cast<float>(y) / static_cast<float>(NY-1);
      auto const flagStart = glm::vec2(-1.5f,-1.f);
      auto const flagSize  = glm::vec2(+3.0f,+2.f);

      auto const position = flagStart + coord*flagSize;

      vertices.push_back({position,coord});
    }

  auto const verticesSize = sizeof(decltype(vertices)::value_type)*vertices.size();
  vbo = gpu.createBuffer(verticesSize);
  gpu.setBufferData(vbo,0,verticesSize,vertices.data());

  std::vector<uint32_t>indices;

  for(uint32_t y=0;y<NY-1;++y)
    for(uint32_t x=0;x<NX-1;++x){
      indices.push_back((y+0)*NX+(x+0));
      indices.push_back((y+0)*NX+(x+1));
      indices.push_back((y+1)*NX+(x+0));
      indices.push_back((y+1)*NX+(x+0));
      indices.push_back((y+0)*NX+(x+1));
      indices.push_back((y+1)*NX+(x+1));
    }

  auto const indicesSize = sizeof(decltype(indices)::value_type)*indices.size();
  ebo = gpu.createBuffer(indicesSize);
  gpu.setBufferData(ebo,0,indicesSize,indices.data());



  vao = gpu.createVertexPuller();
  gpu.setVertexPullerHead(vao,0,AttributeType::VEC2,sizeof(Vertex),0,vbo);
  gpu.enableVertexPullerHead(vao,0);
  gpu.setVertexPullerHead(vao,1,AttributeType::VEC2,sizeof(Vertex),sizeof(glm::vec2),vbo);
  gpu.enableVertexPullerHead(vao,1);

  gpu.setVertexPullerIndexing(vao,IndexType::UINT32,ebo);

  prg = gpu.createProgram();
  gpu.attachShaders(prg,czFlag_VS,czFlag_FS);
  gpu.setVS2FSType(prg,0,AttributeType::VEC2);
}

CZFlagMethod::~CZFlagMethod(){
  gpu.deleteVertexPuller(vao);
  gpu.deleteProgram(prg);
}

void CZFlagMethod::onUpdate(float dt){
  time += dt;
}

void CZFlagMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  gpu.clear(0,0,0,1);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  auto mvp = proj*view;
  gpu.programUniformMatrix4f(prg,0,mvp);
  gpu.programUniform1f      (prg,1,time);

  gpu.drawTriangles((NX-1)*(NY-1)*6);

  gpu.unbindVertexPuller();
}

