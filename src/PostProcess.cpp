#include "PostProcess.hpp"
#include "glm/ext.hpp"

PostProcess::PostProcess( void ) {
    this->createRenderQuad();
    this->setup(GL_STATIC_DRAW);
}

PostProcess::~PostProcess( void ) {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

void    PostProcess::createRenderQuad( void ) {
    /* create quad */
    std::vector<float> v = {{
        -1.0,-1.0, 0.0,  0.0, 1.0, // top-left
         1.0,-1.0, 0.0,  1.0, 1.0, // top-right
         1.0, 1.0, 0.0,  1.0, 0.0, // bottom-right
        -1.0, 1.0, 0.0,  0.0, 0.0  // bottom-left
    }};
    for (size_t i = 5; i < v.size()+1; i += 5) {
        tQuadVertex vertex;
        vertex.Position = glm::vec3(v[i-5], v[i-4], v[i-3]);
        vertex.TexCoords = glm::vec2(v[i-2], v[i-1]);
        this->vertices.push_back(vertex);
    }
    this->indices = {{ 0, 1, 2,  2, 3, 0 }};
}

void    PostProcess::render( Shader shader, GLuint tex ) {
    /* bind textures */
    glActiveTexture(GL_TEXTURE0);
    shader.setIntUniformValue("l_tex", 0);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* render */
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void    PostProcess::setup( int mode ) {
    // gen buffers and vertex arrays
	glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ebo);
    // bind vertex array object, basically this is an object to allow us to not redo all of this process each time
    // we want to draw an object to screen, all the states we set are stored in the VAO
	glBindVertexArray(this->vao);
    // copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(tQuadVertex), this->vertices.data(), mode);
    // copy our indices array in a buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), mode);
    // set the vertex attribute pointers:
    // position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(tQuadVertex), static_cast<GLvoid*>(0));
    // texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(tQuadVertex), reinterpret_cast<GLvoid*>(offsetof(tQuadVertex, TexCoords)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
