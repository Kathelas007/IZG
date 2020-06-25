/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include "cstring"
#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include <X11/Xmd.h>
#include "vector"


/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU() {
    this->Buffer = this->initBS();
    this->BDeleted = this->initBS();

    this->VP = this->initVP();
    this->DVP = this->initBS();

    this->P = this->initP();
    this->DP = this->initBS();

    this->FB = new frameBufferStructure;
    this->FB->depthB = nullptr;
    this->FB->colorB = nullptr;
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU() {
    this->deleteBS(this->Buffer);
    this->deleteBS(this->BDeleted);

    this->deleteVP(this->VP);
    this->deleteBS(this->DVP);

    this->deleteP(this->P);
    this->deleteBS(this->DP);

    this->deleteFramebuffer();
    delete this->FB;
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) {
    /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
    /// Velikost bufferu je v parameteru size (v bajtech).<br>
    /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
    /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>

    if (this->Buffer == nullptr) return emptyID;

    BufferID id = 0;
    if (this->BDeleted->bufferSize == 0) {
        id = this->Buffer->bufferSize;
        this->pushBS(this->Buffer, size);
    } else {
        this->popBS(this->BDeleted, &id);
        this->addBufferBS(this->Buffer, id, size);
    }

    return id;

}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
    /// \todo Tato funkce uvolní buffer na grafické kartě.
    /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
    /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.

    if (!GPU::isBuffer(buffer)) return;

//    std::cout << "** deleting ID: " << buffer << "\n";

    this->deleteBufferBS(this->Buffer, buffer);
    this->pushBS(this->BDeleted, sizeof(BufferID), &buffer);

//    std::cout << " ok\n";
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const *data) {
    /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
    /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
    /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
    /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
    /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>



    if (!GPU::isBuffer(buffer)) return;

    //this->setDataBS(buffer, offset, size, data);

    void *b = this->Buffer->bufferArray[buffer];
    memcpy(b + offset, data, size);
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer, uint64_t offset, uint64_t size, void *data) {
    /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
    /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
    /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
    /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
    /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>

    if (!GPU::isBuffer(buffer)) return;

    this->getDataBS(this->Buffer, buffer, offset, size, data);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) {
    /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
    /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
    /// Pro emptyId vrací false.<br>

    if (this->Buffer == nullptr) return false;

    if (buffer == emptyID) return false;

    if (this->Buffer->bufferSize == 0) return false;

    if (this->Buffer->bufferArray[buffer] == nullptr) return false;

    if (buffer >= this->Buffer->bufferSize or buffer < 0) return false;

    bool res = this->containBufferBS(this->BDeleted, sizeof(Buffer), &buffer);
    return !res;

}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller() {
    /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
    /// Funkce by měla vrátit identifikátor nové tabulky.
    /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.

    auto new_vp = new vertexPullerSettingStructure;
    VertexPullerID id = 0;

    if (this->DVP->bufferSize == 0) {
        this->resizeVP(this->VP);
        id = this->VP->size;
        this->VP->size++;
    } else {
        this->popBS(this->DVP, &id);
    }
    new_vp->size = 0;

    new_vp->indexing = new indexingStructure;
    new_vp->indexing->enabled = false;

    new_vp->heads = new headStructure *[maxAttributes];

    for (u_int i = 0; i < maxAttributes; i++) {
        new_vp->heads[i] = new headStructure;
        new_vp->heads[i]->enabled = false;
        new_vp->heads[i]->vao = emptyID;
        new_vp->heads[i]->type = AttributeType::EMPTY;
    }

    this->VP->Pullers[id] = new_vp;

    return id;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void GPU::deleteVertexPuller(VertexPullerID vao) {
    /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
    /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
    /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
    if (!GPU::isVertexPuller(vao)) {
        return;
    }
    delete this->VP->Pullers[vao]->indexing;

    for (u_int i = 0; i < maxAttributes; i++) {
        delete this->VP->Pullers[vao]->heads[i];
    }

    delete[] this->VP->Pullers[vao]->heads;

    delete this->VP->Pullers[vao];

    this->VP->Pullers[vao] = nullptr;
    this->pushBS(this->DVP, sizeof(BufferID), &vao);
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void GPU::setVertexPullerHead(VertexPullerID vao, uint32_t head, AttributeType type, uint64_t stride, uint64_t offset,
                              BufferID buffer) {
    /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
    /// Parametr "vao" vybírá tabulku s nastavením.<br>
    /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
    /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
    /// Parametr "stride" nastaví krok čtecí hlavy.<br>
    /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
    /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>

    if (!GPU::isVertexPuller(vao)) return;

    if (head > maxAttributes) return;

    this->VP->Pullers[vao]->heads[head]->head = head;
    this->VP->Pullers[vao]->heads[head]->type = type;
    this->VP->Pullers[vao]->heads[head]->stride = stride;
    this->VP->Pullers[vao]->heads[head]->offset = offset;
    this->VP->Pullers[vao]->heads[head]->buffer = buffer;
    this->VP->Pullers[vao]->heads[head]->vao = vao; //???
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void GPU::setVertexPullerIndexing(VertexPullerID vao, IndexType type, BufferID buffer) {
    /// \todo Tato funkce nastaví indexování vertex pulleru.
    /// Parametr "vao" vybírá tabulku s nastavením.<br>
    /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
    /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
    if (!GPU::isVertexPuller(vao)) return;

    this->VP->Pullers[vao]->indexing->enabled = true; // ??
    this->VP->Pullers[vao]->indexing->type = type;
    this->VP->Pullers[vao]->indexing->buffer = buffer;
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void GPU::enableVertexPullerHead(VertexPullerID vao, uint32_t head) {
    /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
    /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
    /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
    /// Parametr "head" volí čtecí hlavu.<br>

    if (!GPU::isVertexPuller(vao)) return;
    if (head > maxAttributes) return;

    this->VP->Pullers[vao]->heads[head]->enabled = true;
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void GPU::disableVertexPullerHead(VertexPullerID vao, uint32_t head) {
    /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
    /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
    /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>

    if (!GPU::isVertexPuller(vao)) return;
    if (head > maxAttributes) return;

    this->VP->Pullers[vao]->heads[head]->enabled = false;

}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void GPU::bindVertexPuller(VertexPullerID vao) {
    /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
    /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>

    if (!GPU::isVertexPuller(vao)) return;

    this->VP->active = vao;

}

/**
 * @brief This function deactivates vertex puller.
 */
void GPU::unbindVertexPuller() {
    /// \todo Tato funkce deaktivuje vertex puller.
    /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.

    this->VP->active = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool GPU::isVertexPuller(VertexPullerID vao) {
    /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
    /// Pokud ano, funkce vrací true.

    if (vao < 0 or vao >= this->VP->size) return false;

    if (vao == emptyID) return false;

    if (this->VP->Pullers[vao] == nullptr) return false;

    if (this->containBufferBS(this->DVP, sizeof(VertexPullerID), &vao)) return false;

    return true;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID GPU::createProgram() {
    /// \todo Tato funkce by měla vytvořit nový shader program.<br>
    /// Funkce vrací unikátní identifikátor nového proramu.<br>
    /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
    /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>

    auto newp_prog = new programSettingStructure;
    ProgramID id = 0;

    if (this->DP->bufferSize == 0) {
        this->resizeP(this->P);
        id = this->P->size;
        this->P->size++;
    } else {
        this->popBS(this->DP, &id);
    }

    newp_prog->fs = nullptr;
    newp_prog->vs = nullptr;

    newp_prog->v2f;

    for (u_int i = 0; i < maxAttributes; i++) {
        newp_prog->v2f[i] = AttributeType::EMPTY;
    }

    newp_prog->uni = new Uniforms;

    this->P->Programs[id] = newp_prog;

    return id;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void GPU::deleteProgram(ProgramID prg) {
    /// \todo Tato funkce by měla smazat vybraný shader program.<br>
    /// Funkce smaže nastavení shader programu.<br>
    /// Identifikátor programu se stane volným a může být znovu využit.<br>

    if (!GPU::isProgram(prg)) return;

    delete this->P->Programs[prg]->uni;

    delete this->P->Programs[prg];
    this->P->Programs[prg] = nullptr;

    this->pushBS(this->DP, sizeof(ProgramID), &prg);
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void GPU::attachShaders(ProgramID prg, VertexShader vs, FragmentShader fs) {
    /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.

    if (!GPU::isProgram(prg)) return;

    this->P->Programs[prg]->vs = vs;
    this->P->Programs[prg]->fs = fs;
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void GPU::setVS2FSType(ProgramID prg, uint32_t attrib, AttributeType type) {
    /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
    /// V průběhu rasterizace vznikají fragment.<br>
    /// Fragment obsahují fragment atributy.<br>
    /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
    /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
    /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>

    if (!GPU::isProgram(prg)) return;

    if (attrib >= maxAttributes) {
       // std::cout << " !!!!!!!!!! WARNING setVS2FSType(), big attribute\n";
        return;
    }
    this->P->Programs[prg]->v2f[attrib] = type;
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void GPU::useProgram(ProgramID prg) {
    /// \todo tato funkce by měla vybrat aktivní shader program.

    if (!GPU::isProgram(prg)) return;

    this->P->active = prg;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool GPU::isProgram(ProgramID prg) {
    /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
    /// Funkce vráti true, pokud program existuje.<br>

    if (prg < 0 or prg >= this->P->size) return false;

    if (prg == emptyID) return false;

    if (this->P->Programs[prg] == nullptr) return false;

    if (this->containBufferBS(this->DP, sizeof(ProgramID), &prg)) return false;

    return true;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform1f(ProgramID prg, uint32_t uniformId, float const &d) {
    /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
    /// Parametr "prg" vybírá shader program.<br>
    /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
    /// Parametr "d" obsahuje data (1 float).<br>

    if (!GPU::isProgram(prg)) return;

    if (uniformId >= maxUniforms) return;

    memcpy(&(this->P->Programs[prg]->uni->uniform[uniformId]), &d, sizeof(float));
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform2f(ProgramID prg, uint32_t uniformId, glm::vec2 const &d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává 2 floaty.

    if (!GPU::isProgram(prg)) return;

    if (uniformId >= maxUniforms) return;

    memcpy(&(this->P->Programs[prg]->uni->uniform[uniformId]), &d, sizeof(glm::vec2));
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform3f(ProgramID prg, uint32_t uniformId, glm::vec3 const &d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává 3 floaty.

    if (!GPU::isProgram(prg)) return;

    if (uniformId >= maxUniforms) return;

    memcpy(&(this->P->Programs[prg]->uni->uniform[uniformId]), &d, sizeof(glm::vec3));
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform4f(ProgramID prg, uint32_t uniformId, glm::vec4 const &d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává 4 floaty.

    if (!GPU::isProgram(prg)) return;

    if (uniformId >= maxUniforms) return;

    memcpy(&(this->P->Programs[prg]->uni->uniform[uniformId]), &d, sizeof(glm::vec4));
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniformMatrix4f(ProgramID prg, uint32_t uniformId, glm::mat4 const &d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává matici 4x4 (16 floatů).

    if (!GPU::isProgram(prg)) return;

    if (uniformId >= maxUniforms) return;

    memcpy(&(this->P->Programs[prg]->uni->uniform[uniformId]), &d, sizeof(glm::mat4));
}

/// @}


/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer(uint32_t width, uint32_t height) {
    /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
    /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
    /// Buffery obsahují width x height pixelů.<br>
    /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
    /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
    /// Nultý pixel framebufferu je vlevo dole.<br>

    if (this->FB->depthB != nullptr or this->FB->colorB != nullptr) {
        this->deleteFramebuffer();
    }

    if (this->FB->depthB != nullptr or this->FB->colorB != nullptr) {
        return;
    }

    this->FB->width = width;
    this->FB->height = height;

    u_long size = (DepthPixelS * width * height) + 3;
    this->FB->depthB = this->initBS(size);

    size = (ColorPixelS * width * height) + 3;
    this->FB->colorB = this->initBS(size);
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer() {
    /// \todo tato funkce by měla dealokovat framebuffer.

    if (this->FB->colorB != nullptr) {
        this->deleteBS(this->FB->colorB);
        this->FB->colorB = nullptr;
    }

    if (this->FB->depthB != nullptr) {
        this->deleteBS(this->FB->depthB);
        this->FB->depthB = nullptr;
    }
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void GPU::resizeFramebuffer(uint32_t width, uint32_t height) {
    /// \todo Tato funkce by měla změnit velikost framebuffer.
    u_long new_size_c = width * height * ColorPixelS + 3;
    u_long new_size_d = width * height * DepthPixelS + 3;

    buffersStructure *new_cb = this->initBS(new_size_c);
    buffersStructure *new_db = this->initBS(new_size_d);

    this->deleteBS(this->FB->colorB);
    this->deleteBS(this->FB->depthB);

    this->FB->colorB = new_cb;
    this->FB->depthB = new_db;

    this->FB->width = width;
    this->FB->height = height;
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t *GPU::getFramebufferColor() {
    /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
    if (this->FB->colorB == nullptr) return nullptr;

    uint8_t *index;
    index = (uint8_t *) &(this->FB->colorB->bufferArray[0]);
    return index;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float *GPU::getFramebufferDepth() {
    /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
    if (this->FB->depthB == nullptr) return nullptr;

    float *index;
    index = (float *) &(this->FB->depthB->bufferArray[0]);
    return index;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth() {
    /// \todo Tato funkce by měla vrátit šířku framebufferu.
    if (this->FB->colorB == nullptr or this->FB->depthB == nullptr) return 0;
    return this->FB->width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight() {
    /// \todo Tato funkce by měla vrátit výšku framebufferu.

    if (this->FB->colorB == nullptr or this->FB->depthB == nullptr) return 0;
    return this->FB->height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void GPU::clear(float r, float g, float b, float a) {
    /// \todo Tato funkce by měla vyčistit framebuffer.<br>
    /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
    /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
    /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
    /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>

    if (this->FB->depthB == nullptr or this->FB->colorB == nullptr) return;

    std::vector<float> colors_float{r, g, b, a};
    uint8_t color_byte[4];

    for (uint8_t i = 0; i < 4; i++) {
        color_byte[i] = convertColor(colors_float[i]);
    }

    float dep = 1.1f;

    uint64_t maxId = this->FB->height * this->FB->width;
    void *color_pointer = this->FB->colorB->bufferArray;
    void *depth_pointer = this->FB->depthB->bufferArray;

    for (uint64_t index = 0; index < maxId; index++) {
        memcpy(color_pointer + (ColorPixelS * index), &color_byte, ColorPixelS);
        memcpy(depth_pointer + (DepthPixelS * index), &dep, DepthPixelS);
    }
}


void GPU::drawTriangles(uint32_t nofVertices) {
    /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
    /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
    /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
    /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

    if (nofVertices < 3) return;
    if (nofVertices % 3 != 0) return;
    if (this->VP->active == emptyID) return;
    if (this->P->active == emptyID) return;
    if (!this->isVertexPuller(this->VP->active)) return;
    if (!this->isProgram(this->P->active)) return;

    vertexPullerSettingStructure *current_puller = this->VP->Pullers[this->VP->active];
    programSettingStructure *current_program = this->P->Programs[this->P->active];

    if (current_program == nullptr or current_puller == nullptr) return;

    std::vector<Assembly> assemblies;
    uint32_t triangle_num = nofVertices / 3;

    AttributeType attribute_types[maxAttributes];

    std::vector<Assembly> clipped_assemblies;

    auto frame_width = (float) getFramebufferWidth();
    auto frame_height = (float) getFramebufferHeight();

    std::vector<InFragment> in_fragments;
    std::vector<InFragment> new_in_fragments;

    OutFragment out_frag{};
    for (uint8_t i = 0; i < 4; i++) {
        out_frag.gl_FragColor[i] = 0;
    }

    // vertex processor
    Assembly a{};
    InVertex iv{};
    OutVertex ov{};

    for (uint32_t tr_num = 0; tr_num < triangle_num; tr_num++) {
        for (uint32_t i = 0; i < 3; i++) {
            this->pullVP(current_puller, 3 * tr_num + i, &iv);
            current_program->vs(ov, iv, *(current_program->uni));
            a.ov[i] = ov;
        }
//        getAssembly(current_program, current_puller, tr_num, a);
        assemblies.push_back(a);
    }

    // clipping
    clipped_assemblies.reserve(assemblies.size() * 2);
    getTypes(current_puller, attribute_types);

    for (auto assembly: assemblies) {
        auto new_assemblies = clipAssembly(assembly, attribute_types);
        clipped_assemblies.insert(clipped_assemblies.end(), new_assemblies.begin(), new_assemblies.end());
    }

    // perspective division
    for (uint64_t i = 0; i < clipped_assemblies.size(); i++) {
        perspectiveDivision(clipped_assemblies[i]);
    }

    // viewport transformation
    for (uint64_t i = 0; i < clipped_assemblies.size(); i++) {
        viewPortTransformation(clipped_assemblies[i], frame_width, frame_height);
    }

    // rasterization
    for (auto assembly: clipped_assemblies) {
        new_in_fragments = rasterize(assembly, current_program->v2f);
        in_fragments.insert(in_fragments.end(), new_in_fragments.begin(), new_in_fragments.end());
    }

    // fragment processor + per fragment
    for (auto &in_frag: in_fragments) {
        current_program->fs(out_frag, in_frag, *(current_program->uni));
        putPixel((uint32_t) in_frag.gl_FragCoord[0], (uint32_t) in_frag.gl_FragCoord[1], out_frag.gl_FragColor,
                 in_frag.gl_FragCoord[2]);
    }
//    printf("Frag count: ");
//    std::cout << in_fragments.size() << "\n";
}


// ***************************************************************************

GPU::buffersStructure *GPU::initBS(u_int32_t size) {
    void **b_array = new void *[size];

    auto *B = (GPU::buffersStructure *) malloc(sizeof(GPU::buffersStructure));
    B->bufferSize = 0;
    B->bufferCapacity = size;
    B->bufferArray = b_array;

    return B;

}

void GPU::resizeBS(buffersStructure *B) {
    if (B == nullptr) return;
    u_int new_cap = 0;

    if (B->bufferCapacity == B->bufferSize) {
        new_cap = B->bufferCapacity + BSRESIZE;
    } else {
        return;
    }

    void **new_array = new void *[new_cap];

    for (u_int i = 0; i < B->bufferSize; i++) {
        B->bufferArray[i] = new_array[i];
    }
    delete[] B->bufferArray;

    B->bufferArray = new_array;
    B->bufferCapacity = new_cap;
}

void GPU::popBS(buffersStructure *B, BufferID *popped_id = nullptr) {
    if (B == nullptr) return;
    this->resizeBS(B);

    if (B->bufferSize == 0) return;

    if (popped_id != nullptr) {
        BufferID val;
        memcpy(&val, B->bufferArray[B->bufferSize - 1], sizeof(BufferID));
        *popped_id = val;
    }

    free(B->bufferArray[B->bufferSize - 1]);
    B->bufferArray[B->bufferSize - 1] = nullptr;
    B->bufferSize--;
}

void GPU::pushBS(buffersStructure *B, uint64_t size, void *pushed_id) {
    if (B == nullptr) return;

    this->resizeBS(B);

    void *bfr = (void *) malloc(size);
    B->bufferArray[B->bufferSize] = bfr;

    if (pushed_id != nullptr) {
        memcpy(B->bufferArray[B->bufferSize], pushed_id, size);
    }

    B->bufferSize++;

    if (pushed_id != nullptr and !GPU::containBufferBS(B, size, pushed_id)) {
        //std::cout << "** !!! ups not pushed" << "\n";
    }
}

void GPU::addBufferBS(buffersStructure *B, u_int64_t index, uint64_t size) {
    if (B == nullptr) return;

    this->resizeBS(B);

    if (index >= B->bufferSize or index < 0) return;

    void *bfr = (void *) malloc(size);
    B->bufferArray[index] = bfr;
}

void GPU::deleteBufferBS(buffersStructure *B, u_int64_t index) {
    if (B == nullptr) return;
    this->resizeBS(B);

    if (index >= B->bufferSize or index < 0) return;

    if (B->bufferArray[index] != nullptr) {
        free(B->bufferArray[index]);
        B->bufferArray[index] = nullptr;
    }
}

void GPU::deleteBS(buffersStructure *B) {
    if (B == nullptr) return;

    for (u_int i = 0; i < B->bufferSize; i++) {
        this->deleteBufferBS(B, i);
    }

    delete[] B->bufferArray;

    free(B);
    B = nullptr;
}

bool GPU::containBufferBS(buffersStructure *B, u_int size, void *value) {
    if (B == nullptr) return false;

    if (B->bufferSize == 0) return false;

    for (int i = 0; i < B->bufferSize; i++) {
        if (memcmp(value, B->bufferArray[i], size) == 0) {
            return true;
        }
    }
    return false;
}

void GPU::setDataBS(GPU::buffersStructure *B, u_int64_t index, uint64_t offset, uint64_t size, const void *data) {
    if (B->bufferSize == 0 or index >= B->bufferSize) return;

    void *b = B->bufferArray[index];
    memcpy(b + offset, data, size);
}

void GPU::getDataBS(GPU::buffersStructure *B, u_int64_t index, uint64_t offset, uint64_t size, void *data) {
    if (B->bufferSize == 0 or index >= B->bufferSize) return;

    void *b = B->bufferArray[index];
    memcpy(data, b + offset, size);
}


//********************************************************************

GPU::vertexPullersStructure *GPU::initVP() {
    auto *vps = new vertexPullersStructure;
    vps->Pullers = new vertexPullerSettingStructure *[BSRESIZE];
    vps->size = 0;
    vps->capacity = BSRESIZE;
    vps->active = emptyID;
    return vps;
}

void GPU::deleteVP(GPU::vertexPullersStructure *VPS) {
    for (u_int i = 0; i < VPS->size; i++) {
        GPU::deleteVertexPuller(i);
    }

    delete[] this->VP->Pullers;
    delete VPS;
}

void GPU::resizeVP(GPU::vertexPullersStructure *VPS) {
    if (VPS->capacity - 2 > VPS->size) return;

    u_int new_capa = BSRESIZE + VPS->capacity;
    auto new_pullers = new vertexPullerSettingStructure *[new_capa];

    for (u_int i = 0; i < VPS->size; i++) {
        new_pullers[i] = VPS->Pullers[i];
    }

    delete[] VPS->Pullers;
    VPS->Pullers = new_pullers;
}

void GPU::pullVP(GPU::vertexPullerSettingStructure *puller, uint32_t inv_index, InVertex *inv) {
    if (puller == nullptr) return;

    bool indexing = puller->indexing->enabled;
    uint32_t index;

    for (uint32_t i = 0; i < maxAttributes; i++) {
        if (indexing) {
            uint8_t index_8;
            uint16_t index_16;
            uint32_t index_32;

            switch (puller->indexing->type) {
                case IndexType::UINT8:
                    this->getDataBS(this->Buffer, puller->indexing->buffer, sizeof(index_8) * inv_index,
                                    sizeof(index_8),
                                    &index_8);
                    index = index_8;
                    break;
                case IndexType::UINT16:
                    this->getDataBS(this->Buffer, puller->indexing->buffer, sizeof(index_16) * inv_index,
                                    sizeof(index_16),
                                    &index_16);
                    index = index_16;
                    break;
                case IndexType::UINT32:
                    this->getDataBS(this->Buffer, puller->indexing->buffer, sizeof(index_32) * inv_index,
                                    sizeof(index_32),
                                    &index_32);
                    index = index_32;
                    break;
            }

        } else {
            index = inv_index;
        }

        inv->gl_VertexID = index;

        headStructure head = *(puller->heads[i]);
        if (!head.enabled) continue;

        uint64_t size = (uint64_t) head.type * sizeof(float);
        if (size == 0) continue;

        this->getDataBS(this->Buffer, head.buffer, head.offset + head.stride * index, size, &(inv->attributes[i]));
    }
}

//********************************************************************

GPU::programsStructure *GPU::initP() {
    auto *ps = new programsStructure;
    ps->size = 0;
    ps->capacity = BSRESIZE;
    ps->Programs = new programSettingStructure *[BSRESIZE];
    ps->active = emptyID;
    return ps;
}

void GPU::deleteP(GPU::programsStructure *PS) {
    for (u_int i = 0; i < PS->size; i++) {
        GPU::deleteProgram(i);
    }

    delete[] this->P->Programs;
    delete PS;
}

void GPU::resizeP(GPU::programsStructure *PS) {

    if (PS->capacity - 2 > PS->size) return;;

    u_int new_capa = BSRESIZE + PS->capacity;
    auto new_programs = new programSettingStructure *[new_capa];

    for (u_int i = 0; i < PS->size; i++) {
        new_programs[i] = PS->Programs[i];
    }

    delete[] P->Programs;
    P->Programs = new_programs;
}

//********************************************************************

uint8_t GPU::convertColor(float value) {
    if (value >= 1) return 255;
    if (value <= 0) return 0;

    value = value * 255 + 0.5f;

    if (value >= 255) return 255;
    if (value <= 0) return 0;

    value = (uint8_t) value;

    return value;
}

void GPU::getAssembly(GPU::programSettingStructure *program, GPU::vertexPullerSettingStructure *puller,
                      uint32_t triangle_num, GPU::Assembly &a) {
    InVertex *iv;
    OutVertex *ov;

    for (uint32_t i = 0; i < 3; i++) {
        this->pullVP(puller, 3 * triangle_num + i, iv);
        program->vs(*ov, *iv, *(program->uni));
        a.ov[i] = *ov;
    }
}

void GPU::getTypes(GPU::vertexPullerSettingStructure *puller, AttributeType *attribute_types) {
    for (uint8_t i = 0; i < maxAttributes; i++) {
        if (puller->heads[i]->enabled) {
            attribute_types[i] = puller->heads[i]->type;
        } else {
            attribute_types[i] = AttributeType::EMPTY;
        }
    }
}

std::vector<GPU::Assembly> GPU::clipAssembly(GPU::Assembly as, AttributeType types[maxAttributes]) {
    std::vector<Assembly> out_av;

    std::vector<OutVertex> OK_points;
    std::vector<OutVertex> F_points;

    for (uint8_t i = 0; i < 3; i++) {
        if (-as.ov[i].gl_Position[3] <= as.ov[i].gl_Position[2]) {
            OK_points.push_back(as.ov[i]);
        } else {
            F_points.push_back(as.ov[i]);
        }
    }

    if (OK_points.empty()) {
        return out_av;
    }

    if (OK_points.size() == 3) {
        out_av.push_back(as);
        return out_av;
    }

    if (OK_points.size() == 2) {
        for (uint8_t i = 0; i < maxAttributes; i++) {
            if (types[i] == AttributeType::EMPTY) continue;
        }

        auto new_point_0 = countOutVer(OK_points[0], F_points[0], types);
        auto new_point_1 = countOutVer(OK_points[1], F_points[0], types);

        Assembly ass;
        ass.ov[0] = OK_points[0];
        ass.ov[1] = new_point_0;
        ass.ov[2] = new_point_1;
        out_av.push_back(ass);

        ass.ov[0] = OK_points[0];
        ass.ov[1] = OK_points[1];
        ass.ov[2] = new_point_1;
        out_av.push_back(ass);
    }

    if (OK_points.size() == 1) {
        auto new_point_0 = countOutVer(OK_points[0], F_points[0], types);
        auto new_point_1 = countOutVer(OK_points[0], F_points[1], types);

        Assembly ass;
        ass.ov[0] = OK_points[0];
        ass.ov[1] = new_point_0;
        ass.ov[2] = new_point_1;
        out_av.push_back(ass);
    }

    return out_av;
}


OutVertex GPU::countOutVer(OutVertex A_OV, OutVertex B_OV, AttributeType types[maxAttributes]) {
    OutVertex O_OV;

    int zp = 2;
    int wp = 3;

    auto A = A_OV.gl_Position;
    auto B = B_OV.gl_Position;

    float t = (-A[wp] - A[zp]) / (B[wp] - A[wp] + B[zp] - A[zp]);

    O_OV.gl_Position = countLinCombination(A, B, t);

    for (uint8_t i = 0; i < maxAttributes; i++) {
        if (types[i] == AttributeType::EMPTY) continue;
        O_OV.attributes[i] = countLinCombination(A_OV.attributes[i], B_OV.attributes[i], t, types[i]);
    }

    return O_OV;
}

float GPU::countLinCombination(float A, float B, float t) {
    return A + t * (B - A);
}

glm::vec2 GPU::countLinCombination(glm::vec2 A, glm::vec2 B, float t) {
    glm::vec2 ret;
    for (uint i = 0; i < 2; i++)
        ret[i] = countLinCombination(A[i], B[i], t);
    return ret;
}

glm::vec3 GPU::countLinCombination(glm::vec3 A, glm::vec3 B, float t) {
    glm::vec3 ret;
    for (uint i = 0; i < 3; i++)
        ret[i] = countLinCombination(A[i], B[i], t);
    return ret;
}

glm::vec4 GPU::countLinCombination(glm::vec4 A, glm::vec4 B, float t) {
    glm::vec4 ret;
    for (uint i = 0; i < 4; i++)
        ret[i] = countLinCombination(A[i], B[i], t);
    return ret;
}

Attribute GPU::countLinCombination(Attribute A, Attribute B, float t, AttributeType type) {
    Attribute a;

    switch (type) {
        case AttributeType::EMPTY:
            break;
        case AttributeType::FLOAT:
            a.v1 = countLinCombination(A.v1, B.v1, t);
            break;
        case AttributeType::VEC2:
            a.v2 = countLinCombination(A.v2, B.v2, t);
            break;
        case AttributeType::VEC3:
            a.v3 = countLinCombination(A.v3, B.v3, t);
            break;
        case AttributeType::VEC4:
            a.v4 = countLinCombination(A.v4, B.v4, t);
            break;
    }
    return a;
}

void GPU::perspectiveDivision(GPU::Assembly &ass) {
    for (uint8_t i = 0; i < 3; i++) {
        float w = ass.ov[i].gl_Position[3];
        for (uint8_t j = 0; j < 3; j++) {
            ass.ov[i].gl_Position[j] = ass.ov[i].gl_Position[j] / w;
        }
    }
}

void GPU::viewPortTransformation(Assembly &ass, float width, float height) {

    for (uint8_t i = 0; i < 3; i++) {
        ass.ov[i].gl_Position[0] = ((ass.ov[i].gl_Position[0] + 1.f) / 2) * (width - 1);
        ass.ov[i].gl_Position[1] = ((ass.ov[i].gl_Position[1] + 1.f) / 2) * (height - 1);
    }
}

std::vector<InFragment> GPU::rasterize(GPU::Assembly ass, AttributeType *v2s_types) {
    uint8_t xp = 0, yp = 1, zp = 2, hp = 3;

    glm::vec4 *A, *B, *C;
    A = &(ass.ov[0].gl_Position);
    B = &(ass.ov[1].gl_Position);
    C = &(ass.ov[2].gl_Position);

    Attribute *at_A, *at_B, *at_C;
    at_A = ass.ov[0].attributes;
    at_B = ass.ov[1].attributes;
    at_C = ass.ov[2].attributes;

    uint64_t left_down[2];
    uint64_t right_top[2];
    getConvexCover(*A, *B, *C, left_down, right_top);

    std::vector<glm::vec2> deltas;
    getDeltas(*A, *B, *C, deltas);

    glm::vec3 edges;
    edgeFunction(*A, *B, *C, deltas, left_down[xp], left_down[yp], edges);

    glm::vec3 lambdas;
    glm::vec3 homogenous((*A)[hp], (*B)[hp], (*C)[hp]);

    std::vector<InFragment> fragments;
    InFragment frag;

    float w5, h5;

    for (uint64_t h = left_down[yp]; h <= right_top[yp]; h++) {
        for (uint64_t w = left_down[xp]; w <= right_top[xp]; w++) {
            w5 = 0.5f + w;
            h5 = 0.5f + h;

            edgeFunction(*A, *B, *C, deltas, w5, h5, edges);

            if ((edges[0] >= 0 and edges[1] >= 0 and edges[2] >= 0)) {

                getBarycentricCoordinates(*A, *B, *C, glm::vec2(w5, h5), lambdas);

                frag.gl_FragCoord[xp] = w5;
                frag.gl_FragCoord[yp] = h5;

                frag.gl_FragCoord[zp] = perspectiveCorrection(lambdas, homogenous, (*A)[zp], (*B)[zp], (*C)[zp]);
                frag.gl_FragCoord[hp] = perspectiveCorrection(lambdas, homogenous, (*A)[hp], (*B)[hp], (*C)[hp]);

//                if (this->test_num == 22 and std::abs(30.f - w) <= 1.5 and std::abs(h - 20.f) <= 1.5) {
//                    printf("** lambdas ");
//                    for (uint8_t i = 0; i < 3; i++) {
//                        printf(" %f ", lambdas[i]);
//                    }
//                    printf("\n");
//                }

                for (uint8_t i = 0; i < maxAttributes; i++) {
                    switch (v2s_types[i]) {
                        case AttributeType::EMPTY:
                            break;
                        case AttributeType::FLOAT:
                            frag.attributes[i].v1 = perspectiveCorrection(lambdas, homogenous, at_A[i].v1,
                                                                          at_B[i].v1, at_C[i].v1);
                            break;
                        case AttributeType::VEC2:
                            frag.attributes[i].v2 = perspectiveCorrection(lambdas, homogenous, at_A[i].v2,
                                                                          at_B[i].v2, at_C[i].v2);
                            break;
                        case AttributeType::VEC3:
                            frag.attributes[i].v3 = perspectiveCorrection(lambdas, homogenous, at_A[i].v3,
                                                                          at_B[i].v3, at_C[i].v3);
                            break;
                        case AttributeType::VEC4:

                            frag.attributes[i].v4 = perspectiveCorrection(lambdas, homogenous, at_A[i].v4,
                                                                          at_B[i].v4, at_C[i].v4);
                        default:

                            break;
                    }
                }
                fragments.push_back(frag);
            }
        }
    }

    return fragments;
}


void GPU::getConvexCover(glm::vec4 A, glm::vec4 B, glm::vec4 C, uint64_t left_down[], uint64_t right_top[]) {
    float left_down_float[2];
    float right_top_float[2];

    for (uint8_t i = 0; i < 2; i++) {
        left_down_float[i] = std::min(std::min(A[i], B[i]), C[i]);
        right_top[i] = std::max(std::max(A[i], B[i]), C[i]);

        left_down_float[i] = std::max(left_down_float[i], 0.f);
        left_down_float[i] = std::max(left_down_float[i], 0.f);

    }

    right_top_float[0] = std::min(right_top_float[0], (float) getFramebufferWidth() - 1);
    right_top_float[1] = std::min(right_top_float[1], (float) getFramebufferHeight() - 1);

    for (uint8_t i = 0; i < 2; i++) {
        left_down[i] = floor(left_down_float[i] + 0.5);
        right_top_float[i] = floor(right_top_float[i] + 0.5);
    }
}

void GPU::getDeltas(glm::vec4 A, glm::vec4 B, glm::vec4 C, std::vector<glm::vec2> &deltas) {
    glm::vec2 delta;

    delta[0] = A[0] - B[0];
    delta[1] = A[1] - B[1];
    deltas.push_back(delta);

    delta[0] = B[0] - C[0];
    delta[1] = B[1] - C[1];
    deltas.push_back(delta);

    delta[0] = C[0] - A[0];
    delta[1] = C[1] - A[1];
    deltas.push_back(delta);
}

void GPU::edgeFunction(glm::vec4 A, glm::vec4 B, glm::vec4 C, std::vector<glm::vec2> &deltas, float x, float y,
                       glm::vec3 &edges) {

    uint8_t xp = 0, yp = 1;
    std::vector<glm::vec4> points;

    points.push_back(A);
    points.push_back(B);
    points.push_back(C);

    for (uint8_t i = 0; i < 3; i++) {
        edges[i] = (x - points[i][xp]) * deltas[i][yp] - (y - points[i][yp]) * deltas[i][xp];
    }

    if (edges[0] <= 0 and edges[1] <= 0 and edges[2] <= 0) {
        for (uint8_t i = 0; i < 3; i++) {
            edges[i] = edges[i] * -1;
            deltas[i][0] = deltas[i][0] * -1;
            deltas[i][1] = deltas[i][1] * -1;
        }
    }
}

void
GPU::getBarycentricCoordinates(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec2 Point, glm::vec3 &Coordinates) {
    Coordinates[0] = countCoordinates(A, B, C, Point);
    Coordinates[1] = countCoordinates(B, C, A, Point);
    Coordinates[2] = countCoordinates(C, A, B, Point);
}

float GPU::countCoordinates(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec2 Point) {
    uint8_t x = 0, y = 1;

    float res = ((B[y] - C[y]) * (Point[x] - C[x]) + (C[x] - B[x]) * (Point[y] - C[y])) /
                ((B[y] - C[y]) * (A[x] - C[x]) + (C[x] - B[x]) * (A[y] - C[y]));

    if (res < 0)
        return res = res * -1;

    return res;
}

float GPU::perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, float a1, float a2, float a3) {
    float jmenovatel = 0;
    float delitel = 0;

    glm::vec3 Attributes{a1, a2, a3};

    for (uint8_t i = 0; i < 3; i++) {
        jmenovatel += Attributes[i] * Coordinates[i] / h[i];
        delitel += Coordinates[i] / h[i];
    }
    return jmenovatel / delitel;
}

glm::vec2 GPU::perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, glm::vec2 a1, glm::vec2 a2, glm::vec2 a3) {
    glm::vec2 res;
    for (uint8_t i = 0; i < 2; i++) {
        res[i] = perspectiveCorrection(Coordinates, h, a1[i], a2[i], a3[i]);
    }
    return res;
}

glm::vec3 GPU::perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, glm::vec3 a1, glm::vec3 a2, glm::vec3 a3) {
    glm::vec3 res;
    for (uint8_t i = 0; i < 3; i++) {
        res[i] = perspectiveCorrection(Coordinates, h, a1[i], a2[i], a3[i]);
    }
    return res;
}

glm::vec4 GPU::perspectiveCorrection(glm::vec3 Coordinates, glm::vec3 h, glm::vec4 a1, glm::vec4 a2, glm::vec4 a3) {
    glm::vec4 res;
    for (uint8_t i = 0; i < 4; i++) {
        res[i] = perspectiveCorrection(Coordinates, h, a1[i], a2[i], a3[i]);
    }
    return res;
}


void GPU::putPixel(uint32_t x, uint32_t y, glm::vec4 color, float depth) {

    uint8_t *color_buffer = this->getFramebufferColor();
    float *depth_buffer = this->getFramebufferDepth();

    uint64_t position = getFramebufferWidth() * y + x;
    float buffer_depth;

    memcpy(&buffer_depth, depth_buffer + (position * DepthPixelS), DepthPixelS);

    // if (depth >= buffer_depth) return;

    uint8_t new_color[4];
    for (uint8_t i = 0; i < 4; i++) {
        new_color[i] = convertColor(color[i]);
    }

    memcpy(color_buffer + (position * ColorPixelS), new_color, ColorPixelS);
    memcpy(depth_buffer + (position * DepthPixelS), &depth, DepthPixelS);

}





/// @}
