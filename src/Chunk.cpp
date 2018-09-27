#include "Chunk.hpp"
#include "glm/ext.hpp"

Chunk::Chunk( const glm::vec3& position, const glm::ivec3& size, const uint8_t* texture, const uint margin ) : position(position), size(size), margin(margin), meshed(false), outOfRange(false) {
    this->createModelTransform(position);
    // this->texture = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * size.x * size.y * size.z));
    // memcpy(this->texture, texture, size.x * size.y * size.z);
    this->texture = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * (size.x+margin) * (size.y+margin) * (size.z+margin)));
    memcpy(this->texture, texture, (size.x+margin) * (size.y+margin) * (size.z+margin));

    /* read the extra information of the texture (6 faces neighbours of chunk) */
    // this->textureExtras = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * size.x * 6 * size.z)); // pass the 6 constant as arg
    // memcpy(this->textureExtras, (texture + size.x * size.y * size.z), size.x * 6 * size.z); // pass the 6 constant as arg
}

Chunk::~Chunk( void ) {
    free(this->texture);
    this->texture = NULL;
    glDeleteBuffers(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
}

uint8_t Chunk::getVisibleFaces( int i ) {
    uint8_t faces = 0x0;
    faces |=  (this->texture[i + 1                                ] == 0) << 5; // right
    faces |=  (this->texture[i - 1                                ] == 0) << 4; // left
    faces |=  (this->texture[i + (size.x+margin)                  ] == 0) << 3; // front
    faces |=  (this->texture[i - (size.x+margin)                  ] == 0) << 2; // back
    faces |=  (this->texture[i + (size.x+margin) * (size.z+margin)] == 0) << 1; // up
    faces |=  (this->texture[i - (size.x+margin) * (size.z+margin)] == 0) << 0; // down
    return faces;
}

bool    Chunk::isVoxelCulled( int i ) {
    uint8_t b = 0x1;
    b &= (this->texture[i + 1                                ] != 0); // right
    b &= (this->texture[i - 1                                ] != 0); // left
    b &= (this->texture[i + (size.x+margin)                  ] != 0); // front
    b &= (this->texture[i - (size.x+margin)                  ] != 0); // back
    b &= (this->texture[i + (size.x+margin) * (size.z+margin)] != 0); // top
    b &= (this->texture[i - (size.x+margin) * (size.z+margin)] != 0); // bottom
    return b;
}

/*    0 __________ 1
      / |        / |
     /  |       /  |
  3 /__________/ 2 |
    |   |_ _ _ |_ _|
    |  /4      |  / 5
    | /        | /
    |/_________|/
  7              6 
    we have 6 faces, each fce have 4 vertices, each vertex have 4 different ao levels (3 levels + 1 clear)
    so we have 16 configurations for ao per face (16 = 2^4 = 4bits), so we can use an int using 6 * 4bits
*/

glm::ivec2  Chunk::getVerticesAoValue( int i, uint8_t visibleFaces ) {
    glm::ivec2  ao = glm::ivec2(0, 0);

    const int x_step = 1;
    const int y_step = (size.x+margin) * (size.z+margin);
    const int z_step = (size.x+margin);
    /* offsets in texture */
    const int tnw = -x_step + -z_step + y_step;
    const int tn  =           -z_step + y_step;
    const int tne =  x_step + -z_step + y_step;
    const int te  =  x_step +           y_step;
    const int tse =  x_step +  z_step + y_step;
    const int ts  =            z_step + y_step;
    const int tsw = -x_step +  z_step + y_step;
    const int tw  = -x_step +           y_step;

    const int mnw = -x_step + -z_step;
    const int mne =  x_step + -z_step;
    const int mse =  x_step +  z_step;
    const int msw = -x_step +  z_step;

    const int bnw = -x_step + -z_step + -y_step;
    const int bn  =           -z_step + -y_step;
    const int bne =  x_step + -z_step + -y_step;
    const int be  =  x_step +           -y_step;
    const int bse =  x_step +  z_step + -y_step;
    const int bs  =            z_step + -y_step;
    const int bsw = -x_step +  z_step + -y_step;
    const int bw  = -x_step +           -y_step;
    
    /* we have to test 20 voxels */
    int p[20];
    /* 8 top */
    p[0]  = (int)(texture[i + tnw] != 0); // top:0
    p[1]  = (int)(texture[i + tn ] != 0); // top:1
    p[2]  = (int)(texture[i + tne] != 0); // top:2
    p[3]  = (int)(texture[i + te ] != 0); // top:3
    p[4]  = (int)(texture[i + tse] != 0); // top:4
    p[5]  = (int)(texture[i + ts ] != 0); // top:5
    p[6]  = (int)(texture[i + tsw] != 0); // top:6
    p[7]  = (int)(texture[i + tw ] != 0); // top:7
    /* 4 middle */
    p[8]  = (int)(texture[i + mnw] != 0); // middle:0
    p[9]  = (int)(texture[i + mne] != 0); // middle:1
    p[10] = (int)(texture[i + mse] != 0); // middle:2
    p[11] = (int)(texture[i + msw] != 0); // middle:3
    /* 8 bottom */
    p[12] = (int)(texture[i + bnw] != 0); // bottom:0
    p[13] = (int)(texture[i + bn ] != 0); // bottom:1
    p[14] = (int)(texture[i + bne] != 0); // bottom:2
    p[15] = (int)(texture[i + be ] != 0); // bottom:3
    p[16] = (int)(texture[i + bse] != 0); // bottom:4
    p[17] = (int)(texture[i + bs ] != 0); // bottom:5
    p[18] = (int)(texture[i + bsw] != 0); // bottom:6
    p[19] = (int)(texture[i + bw]  != 0); // bottom:7
    /*      top                  middle                 bottom
    +-----+-----+-----+    +-----+/-/-/+-----+    +-----+-----+-----+
    |  0  |  1  |  2  |    |  0  |/////|  1  |    |  0  |  1  |  2  |
    +-----+/-/-/+-----+    +/-/-/+/-/-/+/-/-/+    +-----+/-/-/+-----+
    |  7  |/////|  3  |    |/////|/////|/////|    |  7  |/////|  3  |
    +-----+/-/-/+-----+    +/-/-/+/-/-/+/-/-/+    +-----+/-/-/+-----+
    |  6  |  5  |  4  |    |  3  |/////|  2  |    |  6  |  5  |  4  |
    +-----+-----+-----+    +-----+/-/-/+-----+    +-----+-----+-----+
    */
    if (visibleFaces & 0x20) { // [1, 0, 2, 3]
        ao.x |= (int)( std::min(p[10]*.75f + p[4] *.5f + p[3] *.75f, 1.5f) * 2) << 26; // right:0
        ao.x |= (int)( std::min(p[3] *.75f + p[2] *.5f + p[9] *.75f, 1.5f) * 2) << 24; // right:1
        ao.x |= (int)( std::min(p[9] *.75f + p[14]*.5f + p[15]*.75f, 1.5f) * 2) << 28; // right:2
        ao.x |= (int)( std::min(p[15]*.75f + p[16]*.5f + p[10]*.75f, 1.5f) * 2) << 30; // right:3
    }
    if (visibleFaces & 0x10) { // [2, 1, 3, 0]
        ao.x |= (int)( std::min(p[8] *.75f + p[0] *.5f + p[7] *.75f, 1.5f) * 2) << 22; // left:0
        ao.x |= (int)( std::min(p[7] *.75f + p[6] *.5f + p[11]*.75f, 1.5f) * 2) << 18; // left:1
        ao.x |= (int)( std::min(p[11]*.75f + p[18]*.5f + p[19]*.75f, 1.5f) * 2) << 16; // left:2
        ao.x |= (int)( std::min(p[19]*.75f + p[12]*.5f + p[8] *.75f, 1.5f) * 2) << 20; // left:3
    }
    if (visibleFaces & 0x08) { // [2, 1, 3, 0]
        ao.x |= (int)( std::min(p[11]*.75f + p[6] *.5f + p[5] *.75f, 1.5f) * 2) << 14; // front:0
        ao.x |= (int)( std::min(p[5] *.75f + p[4] *.5f + p[10]*.75f, 1.5f) * 2) << 10; // front:1
        ao.x |= (int)( std::min(p[10]*.75f + p[16]*.5f + p[17]*.75f, 1.5f) * 2) <<  8; // front:2
        ao.x |= (int)( std::min(p[17]*.75f + p[18]*.5f + p[11]*.75f, 1.5f) * 2) << 12; // front:3
    }
    if (visibleFaces & 0x04) { // [1, 0, 2, 3]
        ao.x |= (int)( std::min(p[9] *.75f + p[2] *.5f + p[1] *.75f, 1.5f) * 2) <<  2; // back:0
        ao.x |= (int)( std::min(p[1] *.75f + p[0] *.5f + p[8] *.75f, 1.5f) * 2) <<  0; // back:1
        ao.x |= (int)( std::min(p[8] *.75f + p[12]*.5f + p[13]*.75f, 1.5f) * 2) <<  4; // back:2
        ao.x |= (int)( std::min(p[13]*.75f + p[14]*.5f + p[9] *.75f, 1.5f) * 2) <<  6; // back:3
    }
    if (visibleFaces & 0x02) { // [3, 2, 0, 1]
        ao.y  |= (int)( std::min(p[7] *.75f + p[0] *.5f + p[1] *.75f, 1.5f) * 2) << 12; // top:0
        ao.y  |= (int)( std::min(p[1] *.75f + p[2] *.5f + p[3] *.75f, 1.5f) * 2) << 14; // top:1
        ao.y  |= (int)( std::min(p[3] *.75f + p[4] *.5f + p[5] *.75f, 1.5f) * 2) << 10; // top:2
        ao.y  |= (int)( std::min(p[5] *.75f + p[6] *.5f + p[7] *.75f, 1.5f) * 2) <<  8; // top:3
    }
    if (visibleFaces & 0x01) { // [1, 2, 0, 3]
        ao.y  |= (int)( std::min(p[19]*.75f + p[12]*.5f + p[13]*.75f, 1.5f) * 2) <<  4; // bottom:0
        ao.y  |= (int)( std::min(p[13]*.75f + p[14]*.5f + p[15]*.75f, 1.5f) * 2) <<  0; // bottom:1
        ao.y  |= (int)( std::min(p[15]*.75f + p[16]*.5f + p[17]*.75f, 1.5f) * 2) <<  2; // bottom:2
        ao.y  |= (int)( std::min(p[17]*.75f + p[18]*.5f + p[19]*.75f, 1.5f) * 2) <<  6; // bottom:3
    }
    return ao;
}

void    Chunk::buildMesh( void ) {
    this->meshed = true;
    this->voxels.reserve(this->size.x * this->size.y * this->size.z);
    for (int y = 0; y < this->size.y; ++y)
        for (int z = 0; z < this->size.z; ++z)
            for (int x = 0; x < this->size.x; ++x) {
                int i = (x+margin/2) + (z+margin/2) * (this->size.x+margin) + (y+margin/2) * (this->size.x+margin) * (this->size.z+margin);
                if (this->texture[i] != 0) { /* if voxel is not air */
                    if (!isVoxelCulled(i)) {
                        uint8_t visibleFaces = getVisibleFaces(i);
                        uint8_t b = (uint8_t)(this->texture[i] - 1);
                        /* change dirt to grass on top */
                        if (texture[i + (size.x+margin) * (size.z+margin)] == 0)
                            b = 1;
                        glm::ivec2 ao = getVerticesAoValue(i, visibleFaces);
                        this->voxels.push_back( (tPoint){ glm::vec3(x, y, z), b, visibleFaces, ao } );
                    }
                }
            }
    this->setup(GL_STATIC_DRAW);
}

void    Chunk::render( Shader shader, Camera& camera, GLuint textureAtlas, uint renderDistance ) {
    // if (glm::distance(this->position * glm::vec3(1,0,1), camera.getPosition() * glm::vec3(1,0,1)) > renderDistance * 1.5) {
    //     outOfRange = true;
    //     return;
    // }
    glm::vec3 size = this->size;
    if (camera.aabInFustrum(-(this->position + size / 2), size) && this->voxels.size() > 0 && this->texture) {
        /* set transform matrix */
        shader.setMat4UniformValue("_mvp", camera.getViewProjectionMatrix() * this->transform);
        shader.setMat4UniformValue("_model", this->transform);
        /* texture atlas */
        glActiveTexture(GL_TEXTURE0);
        shader.setIntUniformValue("atlas", 0);
        glBindTexture(GL_TEXTURE_2D, textureAtlas);
        /* render */
        glBindVertexArray(this->vao);
        glDrawArrays(GL_POINTS, 0, this->voxels.size());
        glBindVertexArray(0);
    }
}

void    Chunk::setup( int mode ) {
	glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->voxels.size() * sizeof(tPoint), this->voxels.data(), mode);

    /* position attribute */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(tPoint), static_cast<GLvoid*>(0));
    /* id attribute */
    glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(tPoint), reinterpret_cast<GLvoid*>(offsetof(tPoint, id)));
    /* occluded faces attribute */
    glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(tPoint), reinterpret_cast<GLvoid*>(offsetof(tPoint, visibleFaces)));
    /* ao attribute */
    glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 2, GL_INT, sizeof(tPoint), reinterpret_cast<GLvoid*>(offsetof(tPoint, ao)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void    Chunk::createModelTransform( const glm::vec3& position ) {
    this->transform = glm::mat4();
    this->transform = glm::translate(this->transform, position);
}