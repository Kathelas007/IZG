/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include "vector"
#include "stack"


/**
 * @brief This class represent software GPU
 */
class GPU {
public:
    int test_num = -1;

    GPU();

    virtual ~GPU();

    //buffer object commands
    BufferID createBuffer(uint64_t size);

    void deleteBuffer(BufferID buffer);

    void setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const *data);

    void getBufferData(BufferID buffer, uint64_t offset, uint64_t size, void *data);

    bool isBuffer(BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID createVertexPuller();

    void deleteVertexPuller(VertexPullerID vao);

    void setVertexPullerHead(VertexPullerID vao, uint32_t head, AttributeType type, uint64_t stride, uint64_t offset,
                             BufferID buffer);

    void setVertexPullerIndexing(VertexPullerID vao, IndexType type, BufferID buffer);

    void enableVertexPullerHead(VertexPullerID vao, uint32_t head);

    void disableVertexPullerHead(VertexPullerID vao, uint32_t head);

    void bindVertexPuller(VertexPullerID vao);

    void unbindVertexPuller();

    bool isVertexPuller(VertexPullerID vao);

    //program object commands
    ProgramID createProgram();

    void deleteProgram(ProgramID prg);

    void attachShaders(ProgramID prg, VertexShader vs, FragmentShader fs);

    void setVS2FSType(ProgramID prg, uint32_t attrib, AttributeType type);

    void useProgram(ProgramID prg);

    bool isProgram(ProgramID prg);

    void programUniform1f(ProgramID prg, uint32_t uniformId, float const &d);

    void programUniform2f(ProgramID prg, uint32_t uniformId, glm::vec2 const &d);

    void programUniform3f(ProgramID prg, uint32_t uniformId, glm::vec3 const &d);

    void programUniform4f(ProgramID prg, uint32_t uniformId, glm::vec4 const &d);

    void programUniformMatrix4f(ProgramID prg, uint32_t uniformId, glm::mat4 const &d);

    //framebuffer functions
    void createFramebuffer(uint32_t width, uint32_t height);

    void deleteFramebuffer();

    void resizeFramebuffer(uint32_t width, uint32_t height);

    uint8_t *getFramebufferColor();

    float *getFramebufferDepth();

    uint32_t getFramebufferWidth();

    uint32_t getFramebufferHeight();

    //execution commands
    void clear(float r, float g, float b, float a);

    void drawTriangles(uint32_t nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{

// **************************************************************************

    struct buffersStructure {
        void **bufferArray{};
        u_int bufferSize = 0;
        u_int bufferCapacity = 0;
    };

    buffersStructure *Buffer = nullptr;

    buffersStructure *BDeleted = nullptr;

    static const u_int BSRESIZE = 150;

    buffersStructure *initBS(u_int32_t size = BSRESIZE);

    void resizeBS(buffersStructure *B);

    void popBS(buffersStructure *B, BufferID *popped_id);

    void pushBS(buffersStructure *B, uint64_t size, void *pushed_id = nullptr);

    void addBufferBS(buffersStructure *B, u_int64_t index, uint64_t size);

    void deleteBufferBS(buffersStructure *B, u_int64_t index);

    bool containBufferBS(buffersStructure *B, u_int size, void *value);

    void setDataBS(buffersStructure *B, u_int64_t index, uint64_t offset, uint64_t size, void const *data);

    void getDataBS(buffersStructure *B, u_int64_t index, uint64_t offset, uint64_t size, void *data);

    void deleteBS(buffersStructure *B);

    // *****************************************************************************

    struct headStructure {
        VertexPullerID vao;
        uint32_t head;
        AttributeType type;
        uint64_t stride;
        uint64_t offset;
        BufferID buffer;
        bool enabled;
    };

    struct indexingStructure {
        BufferID buffer{};
        IndexType type;
        bool enabled = false;
    };

    struct vertexPullerSettingStructure {
        u_int size = 0;
        headStructure **heads = nullptr;
        indexingStructure *indexing = nullptr;
    };

    struct vertexPullersStructure {
        u_int size = 0;
        u_int capacity = 0;
        vertexPullerSettingStructure **Pullers;
        VertexPullerID active = emptyID;
    };

    vertexPullersStructure *VP;

    buffersStructure *DVP;

    vertexPullersStructure *initVP();

    void deleteVP(vertexPullersStructure *VPS);

    void resizeVP(vertexPullersStructure *VPS);

    void pullVP(vertexPullerSettingStructure *puller, uint32_t inv_index, InVertex *inv);

    // *****************************************************************************

    struct programSettingStructure {
        VertexShader vs;
        FragmentShader fs;
        AttributeType v2f[maxAttributes];      // 16x
        Uniforms *uni; // 16x
    };

    struct programsStructure {
        u_int size = 0;
        u_int capacity = 0;
        programSettingStructure **Programs;
        ProgramID active = emptyID;
    };

    programsStructure *P;

    buffersStructure *DP;

    programsStructure *initP();

    void deleteP(programsStructure *PS);

    void resizeP(programsStructure *PS);

    // *****************************************************************************

    struct frameBufferStructure {
        buffersStructure *colorB;
        buffersStructure *depthB;
        uint32_t width;
        uint32_t height;
    };

    frameBufferStructure *FB;

    u_int32_t ColorPixelS = 4 * sizeof(uint8_t);
    u_int32_t DepthPixelS = 1 * sizeof(float);

    uint8_t convertColor(float value);

    // *****************************************************************************

    struct Assembly {
        OutVertex ov[3];
    };

    void getAssembly(programSettingStructure *program, vertexPullerSettingStructure *puller, uint32_t triangle_num, Assembly & a);

    void getTypes(vertexPullerSettingStructure *puller, AttributeType *attribute_types);

    void getV2FTypes(buffersStructure *buffer, AttributeType *v2f_types);

    std::vector<Assembly> clipAssembly(Assembly as, AttributeType types[maxAttributes]);

    OutVertex countOutVer(OutVertex A_OV, OutVertex B_OV, AttributeType types[maxAttributes]);

    float countLinCombination(float A, float B, float t);

    glm::vec2 countLinCombination(glm::vec2 A, glm::vec2 B, float t);

    glm::vec3 countLinCombination(glm::vec3 A, glm::vec3 B, float t);

    glm::vec4 countLinCombination(glm::vec4 A, glm::vec4 B, float t);

    Attribute countLinCombination(Attribute A, Attribute B, float t, AttributeType type);

    void perspectiveDivision(Assembly &ass);

    void viewPortTransformation(Assembly &ass, float width, float height);

    std::vector<InFragment> rasterize(Assembly ass, AttributeType *v2s_types);

    void getConvexCover(glm::vec4 A, glm::vec4 B, glm::vec4 C, uint64_t left_down[], uint64_t right_top[]);

    void getDeltas(glm::vec4 A, glm::vec4 B, glm::vec4 C, std::vector<glm::vec2> &deltas);

    void edgeFunction(glm::vec4 A, glm::vec4 B, glm::vec4 C, std::vector<glm::vec2> &deltas, float x, float y,
                      glm::vec3 &edges);

    void edgesRight(glm::vec3 &edges, std::vector<glm::vec2> &deltas);

    void edgesUp(glm::vec3 &edges, std::vector<glm::vec2> &deltas);

    void getBarycentricCoordinates(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec2 Point, glm::vec3 &Coordinates);

    float countCoordinates(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec2 Point);

    float perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, float a1, float a2, float a3);

    glm::vec2 perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, glm::vec2 a1, glm::vec2 a2, glm::vec2 a3);

    glm::vec3 perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, glm::vec3 a1, glm::vec3 a2, glm::vec3 a3);

    glm::vec4 perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, glm::vec4 a1, glm::vec4 a2, glm::vec4 a3);

    void putPixel(uint32_t x, uint32_t y, glm::vec4 color, float depth);

    /// @}
};

