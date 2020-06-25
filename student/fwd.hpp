/*!
 * @file
 * @brief This file contains forward declarations and constants.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <glm/glm.hpp>
#include <cstdint>

//#define MAKE_STUDENT_RELEASE

uint32_t const maxAttributes = 16;///< maximum number of vertex/fragment attributes
uint32_t const maxUniforms   = 16;///< maximum number of uniform variables
uint32_t const emptyID       = 0xffffffff;///< empty object id (for buffers, programs and vertex pullers)

/**
 * @brief This enum represents vertex/fragment attribute type.
 */
enum class AttributeType{
  EMPTY = 0, ///< disabled attribute
  FLOAT = 1, ///< 1x 32-bit float
  VEC2  = 2, ///< 2x 32-bit floats
  VEC3  = 3, ///< 3x 32-bit floats
  VEC4  = 4, ///< 4x 32-bit floats
};

/**
 * @brief This union represents one vertex/fragment attribute
 */
union Attribute{
  Attribute(){}
  float     v1; ///< single float
  glm::vec2 v2; ///< vector of two floats
  glm::vec3 v3; ///< vector of three floats
  glm::vec4 v4 = glm::vec4(1.f); ///< vector of four floats
};

/**
 * @brief This struct represents input vertex of vertex shader.
 */
struct InVertex{
  Attribute attributes[maxAttributes]; ///< vertex attributes
  uint32_t  gl_VertexID              ; ///< vertex id
};

/**
 * @brief This struct represents output vertex of vertex shader.
 */
struct OutVertex{
  Attribute attributes[maxAttributes]; ///< vertex attributes
  glm::vec4 gl_Position              ; ///< clip space position
};

/**
 * @brief This struct represents input fragment.
 */
struct InFragment{
  Attribute attributes[maxAttributes]; ///< fragment attributes
  glm::vec4 gl_FragCoord             ; ///< fragment coordinates
};

/**
 * @brief This struct represents output fragment.
 */
struct OutFragment{
  glm::vec4 gl_FragColor; ///< fragment color
};

/**
 * @brief This union represents one uniform variable.
 */
union Uniform{
  Uniform(){}
  float     v1; ///< single float
  glm::vec2 v2; ///< two floats
  glm::vec3 v3; ///< three floats
  glm::vec4 v4; ///< four floats
  glm::mat4 m4 = glm::mat4(1.f); ///< 4x4 float matrix
};

/**
 * @brief This struct represents shader program uniform variables
 */
struct Uniforms{
  Uniform uniform[maxUniforms];///< uniform variables
};

/**
 * @brief This enum represents index type
 */
enum class IndexType{
  UINT8  = 1, ///< uin8_t type
  UINT16 = 2, ///< uin16_t type
  UINT32 = 4, ///< uint32_t type
};

/**
 * @brief Function type for vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
using VertexShader   = void(*)(
    OutVertex      &outVertex,
    InVertex  const&inVertex ,
    Uniforms  const&uniforms );

/**
 * @brief Function type for fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment 
 * @param uniforms uniform variables
 */
using FragmentShader = void(*)(
    OutFragment      &outFragment,
    InFragment  const&inFragment ,
    Uniforms    const&uniforms   );

using ObjectID       = uint64_t;///< object id (program, buffer, vertex puller)
using BufferID       = ObjectID;///< buffer id
using VertexPullerID = ObjectID;///< vertex puller id
using ProgramID      = ObjectID;///< shader program id

