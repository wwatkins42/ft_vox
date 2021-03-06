#include "utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::vec4    hex2vec( int64_t hex ) {
    return glm::vec4(
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >>  8) & 0xFF) / 255.0f,
        ((hex      ) & 0xFF) / 255.0f,
        1.0f
    );
}

glm::vec2    mousePosToClipSpace( const glm::dvec2& pos, int winWidth, int winHeight ) {
    glm::vec2    mouse = glm::vec2({(float)pos[0] / winWidth, (float)pos[1] / winHeight}) * 2.0f - 1.0f;
    mouse.y = -mouse.y;
    return (mouse);
}

void    createCube( std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices ) {
    vertices = {{
        -0.5, -0.5,  0.5,   0.0, 1.0, // front top-left
         0.5, -0.5,  0.5,   1.0, 1.0, // front top-right
         0.5,  0.5,  0.5,   1.0, 0.0, // front bottom-right
        -0.5,  0.5,  0.5,   0.0, 0.0, // front bottom-left
        -0.5, -0.5, -0.5,   0.0, 1.0,
         0.5, -0.5, -0.5,   1.0, 1.0,
         0.5,  0.5, -0.5,   1.0, 0.0,
        -0.5,  0.5, -0.5,   0.0, 0.0,
    }};
    indices = {{
        0, 1, 2,  2, 3, 0,
        1, 5, 6,  6, 2, 1,
        7, 6, 5,  5, 4, 7,
        4, 0, 3,  3, 7, 4,
        4, 5, 1,  1, 0, 4,
        3, 2, 6,  6, 7, 3,
    }};
}

GLuint  loadTexture( const char* filename, GLuint textureMode ) {
    std::cout << "> texture: " << filename << std::endl;
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (data) {
        GLenum format;
        switch (channels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        };
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMode);
        stbi_image_free(data);
    }
    else {
        stbi_image_free(data);
        throw Exception::ModelError("TextureLoader", filename);
    }
    return (textureID);
}

GLuint  loadTextureSrgb( const char* filename, GLuint textureMode ) {
    std::cout << "> texture: " << filename << std::endl;
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char*  data = stbi_load(filename, &width, &height, &channels, 0);

    if (data) {
        GLenum internal_format;
        GLenum format;
        switch (channels) {
            case 1: internal_format = GL_RED; break;
            case 3: internal_format = GL_SRGB; break;
            case 4: internal_format = GL_SRGB_ALPHA; break;
        };
        switch (channels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        };
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMode);
        stbi_image_free(data);
    }
    else {
        stbi_image_free(data);
        throw Exception::ModelError("TextureLoader", filename);
    }
    return (textureID);
}

GLuint  loadTextureMipmapSrgb( const std::vector<std::string>& paths ) {
    // std::cout << "> texture: " << filename << std::endl;
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    for (int i = 0; i < paths.size(); i++) {
        int width, height, channels;
        unsigned char*  data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            GLenum internal_format;
            GLenum format;
            switch (channels) {
                case 1: internal_format = GL_RED; break;
                case 3: internal_format = GL_SRGB; break;
                case 4: internal_format = GL_SRGB_ALPHA; break;
            };
            switch (channels) {
                case 1: format = GL_RED; break;
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
            };
            glTexImage2D(GL_TEXTURE_2D, i, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            stbi_image_free(data);
            throw Exception::ModelError("TextureLoader", paths[i].c_str());
        }
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, paths.size()-1);
    glBindTexture(GL_TEXTURE_2D, 0);
    return (textureID);
}

GLuint  loadCubemap( const std::vector<std::string>& paths ) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, channels;
    for (size_t i = 0; i < paths.size(); ++i) {
        std::cout << "> texture: " << paths[i].c_str() << std::endl;
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            GLenum format;
            switch (channels) {
                case 1: format = GL_RED; break;
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
            };
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            stbi_image_free(data);
            throw Exception::ModelError("CubemapLoader", paths[i]);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return (textureID);
}

GLuint  loadCubemapSrgb( const std::vector<std::string>& paths ) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, channels;
    for (size_t i = 0; i < paths.size(); ++i) {
        std::cout << "> texture: " << paths[i].c_str() << std::endl;
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            GLenum format;
            GLenum internal_format;
            switch (channels) {
                case 1: internal_format = GL_RED; break;
                case 3: internal_format = GL_SRGB; break;
                case 4: internal_format = GL_SRGB_ALPHA; break;
            };
            switch (channels) {
                case 1: format = GL_RED; break;
                case 3: format = GL_RGB; break;
                case 4: format = GL_RGBA; break;
            };
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            stbi_image_free(data);
            throw Exception::ModelError("CubemapLoader", paths[i]);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return (textureID);
}