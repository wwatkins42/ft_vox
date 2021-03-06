#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in ivec2 aAo;
layout (location = 2) in int aId;
layout (location = 3) in int aVisibleFaces;
layout (location = 4) in int aLight;

out mat4 mvp;
out vec3 gFragPos;
flat out ivec2 gAo;
flat out int gId;
flat out int gVisibleFaces;
flat out int gLight;

uniform mat4 _model;
uniform mat4 _mvp;

void main() {
    gl_Position = _mvp * vec4(aPos, 1.0);
    mvp = _mvp;
    gAo = aAo;
    gId = aId;
    gVisibleFaces = aVisibleFaces;
    gLight = aLight;
    gFragPos = vec3(_model * vec4(aPos, 1.0));
}