#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out float Near;
out float Far;

uniform mat4 model;
uniform float near;
uniform float far;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Near = near;
    Far = far;

    mat4 modelView = view * model;
    modelView[0].xyz = vec3(model[0][0], 0, 0);
    modelView[1].xyz = vec3(0, model[1][1], 0);
    modelView[2].xyz = vec3(0, 0, model[2][2]);
    gl_Position = projection * modelView * vec4(aPos, 1.0);
}
