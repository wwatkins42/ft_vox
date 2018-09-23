#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <thread>

#include "Exception.hpp"
#include "Env.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Light.hpp"

typedef struct  sDepthMap {
    unsigned int    id;
    unsigned int    fbo;
    size_t          width;
    size_t          height;
}               tDepthMap;

typedef std::unordered_map<std::string, Shader*> tShaderMap;
typedef std::chrono::duration<double,std::milli> tMilliseconds;
typedef std::chrono::steady_clock::time_point tTimePoint;

class Renderer {

public:
    Renderer( Env* env );
    ~Renderer( void );

    void	loop( void );
    void    updateShadowDepthMap( void );
    void    renderLights( void );
    void    renderMeshes( void );
    void    renderSkybox( void );
    void    renderPostFxaa( void );

private:
    Env*            env;
    Camera          camera;
    tShaderMap      shader;
    tDepthMap       depthMap;       /* custom depth-map */
    tDepthMap       framebuffer;
    glm::mat4       lightSpaceMat;
    float           framerate;
    bool            fxaa;

    tTimePoint      lastTime;

    void    initDepthMap( void );
    void    initFramebuffer( void );

};
