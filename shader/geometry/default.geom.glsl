#version 400 core
layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

in mat4 mvp[];
in vec3 gFragPos[];
flat in int gId[];
flat in int gVisibleFaces[];
flat in ivec2 gAo[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float Ao;
flat out int Id;

uniform vec3 viewPos;

// const float[4] aoCurve = float[4]( 1.0, 0.7, 0.6, 0.15 ); // best soft
const float[4] aoCurve = float[4]( 1.0, 0.65, 0.6, .15 ); // best hard
// const float[4] aoCurve = float[4]( 1.0, 0.66, 0.33, 0. ); // linear
// const float[4] aoCurve = float[4]( 1.0, 0.4, 0.2, 0. ); // accentuated debug
/* 3 +---+ 2
     | / |
   1 +---+ 0 */
void    AddQuad(vec4 center, vec4 dy, vec4 dx, int ao, bool flip_uv) {
    Ao = aoCurve[(ao&0xC0)>>6];
    TexCoords = (flip_uv ? vec2(1, 0) : vec2(0, 1) );
    gl_Position = center + ( dx - dy);
    EmitVertex();
    Ao = aoCurve[(ao&0x30)>>4];
    TexCoords = vec2(1, 1);
    gl_Position = center + (-dx - dy);
    EmitVertex();
    Ao = aoCurve[(ao&0x0C)>>2];
    TexCoords = vec2(0, 0);
    gl_Position = center + ( dx + dy);
    EmitVertex();
    Ao = aoCurve[(ao&0x03)>>0];
    TexCoords = (flip_uv ? vec2(0, 1) : vec2(1, 0) );
    gl_Position = center + (-dx + dy);
    EmitVertex();
    EndPrimitive();
}
/* 1 +---+ 3
     | \ |
   0 +---+ 2 */
void    AddQuadFlipped(vec4 center, vec4 dy, vec4 dx, int ao, bool flip_uv) {
    Ao = aoCurve[(ao&0x30)>>4];
    TexCoords = vec2(1, 1);
    gl_Position = center + (-dx - dy);
    EmitVertex();
    Ao = aoCurve[(ao&0x03)>>0];
    TexCoords = (flip_uv ? vec2(0, 1) : vec2(1, 0) );
    gl_Position = center + (-dx + dy);
    EmitVertex();
    Ao = aoCurve[(ao&0xC0)>>6];
    TexCoords = (flip_uv ? vec2(1, 0) : vec2(0, 1) );
    gl_Position = center + ( dx - dy);
    EmitVertex();
    Ao = aoCurve[(ao&0x0C)>>2];
    TexCoords = vec2(0, 0);
    gl_Position = center + ( dx + dy);
    EmitVertex();
    EndPrimitive();
}

void    main() {
    vec4 center = gl_in[0].gl_Position;
    
    vec4 dx = mvp[0][0] / 2.0;// * 0.75;
    vec4 dy = mvp[0][1] / 2.0;// * 0.75;
    vec4 dz = mvp[0][2] / 2.0;// * 0.75;

    Id = gId[0];
    FragPos = gFragPos[0];


    // Normal = vec3( 1.0, 0.0, 0.0);
    // if ( (gVisibleFaces[0] & 0x20) != 0 && dot(Normal, (FragPos + dx.xyz) - viewPos) < 0) /* right */
    //     AddQuad(center + dx, dy, dz);
    // Normal = vec3(-1.0, 0.0, 0.0);
    // if ( (gVisibleFaces[0] & 0x10) != 0 && dot(Normal, (FragPos - dx.xyz) - viewPos) < 0) /* left */
    //     AddQuad2(center - dx, dz, dy);
    // Normal = vec3( 0.0, 1.0, 0.0);
    // if ( (gVisibleFaces[0] & 0x02) != 0 && dot(Normal, (FragPos + dy.xyz) - viewPos) < 0) /* top */
    //     AddQuad(center + dy, dz, dx);
    // Normal = vec3( 0.0,-1.0, 0.0);
    // if ( (gVisibleFaces[0] & 0x01) != 0 && dot(Normal, (FragPos - dy.xyz) - viewPos) < 0) /* bottom */
    //     AddQuad(center - dy, dx, dz);
    // Normal = vec3( 0.0, 0.0, 1.0);
    // if ( (gVisibleFaces[0] & 0x08) != 0 && dot(Normal, (FragPos + dz.xyz) - viewPos) < 0) /* front */
    //     AddQuad2(center + dz, dx, dy);
    // Normal = vec3( 0.0, 0.0,-1.0);
    // if ( (gVisibleFaces[0] & 0x04) != 0 && dot(Normal, (FragPos - dz.xyz) - viewPos) < 0) /* back */
    //     AddQuad(center - dz, dy, dx);

    /* (a00 + a11 > a01 + a10) -> Flip quad

       a00 +----+ a10
           |    |
       a01 +----+ a11

    */

    /* fixes visual issue */
    Normal = vec3( 1.0, 0.0, 0.0);
    if (dot(Normal, (FragPos + dx.xyz) - viewPos) < 0) {
        if ( (gVisibleFaces[0] & 0x20) != 0) { /* right */
            int ao = (gAo[0][0] & 0xFF000000) >> 24;
            if ( pow((ao&0x03)>>0, 2) + pow((ao&0xC0)>>6, 2) < pow((ao&0x0C)>>2, 2) + pow((ao&0x30)>>4, 2) )
                AddQuadFlipped(center + dx, dy, dz, ao, false);
            else
                AddQuad(center + dx, dy, dz, ao, false);
        }
    }
    else {
        Normal = vec3(-1.0, 0.0, 0.0);
        if ( (gVisibleFaces[0] & 0x10) != 0) { /* left */
            int ao = (gAo[0][0] & 0x00FF0000) >> 16;
            if ( pow((ao&0x30)>>4, 2) + pow((ao&0x0C)>>2, 2) > pow((ao&0xC0)>>6, 2) + pow((ao&0x03)>>0, 2) )
                AddQuadFlipped(center - dx, dz, dy, ao, true);
            else
                AddQuad(center - dx, dz, dy, ao, true);
        }
    }
    Normal = vec3( 0.0, 1.0, 0.0);
    if (dot(Normal, (FragPos + dy.xyz) - viewPos) < 0) {
        if ( (gVisibleFaces[0] & 0x02) != 0) { /* top */
            int ao = (gAo[0][1] & 0x0000FF00) >> 8;
            if ( pow((ao&0x30)>>4, 2) + pow((ao&0x0C)>>2, 2) > pow((ao&0x03)>>0, 2) + pow((ao&0xC0)>>6, 2) )
                AddQuadFlipped(center + dy, dz, dx, ao, false);
            else
                AddQuad(center + dy, dz, dx, ao, false);
        }
    }
    else {
        Normal = vec3( 0.0,-1.0, 0.0);
        if ( (gVisibleFaces[0] & 0x01) != 0) { /* bottom */
            int ao = (gAo[0][1] & 0x000000FF);
            if ( pow((ao&0x30)>>4, 2) + pow((ao&0x0C)>>2, 2) > pow((ao&0xC0)>>6, 2) + pow((ao&0x03)>>0, 2) )
                AddQuadFlipped(center - dy, dx, dz, ao, false);
            else
                AddQuad(center - dy, dx, dz, ao, false);
        }
    }
    Normal = vec3( 0.0, 0.0, 1.0);
    if (dot(Normal, (FragPos + dz.xyz) - viewPos) < 0) {
        if ( (gVisibleFaces[0] & 0x08) != 0) { /* front */
            int ao = (gAo[0][0] & 0x0000FF00) >> 8;
            if ( pow((ao&0xC0)>>6, 2) + pow((ao&0x03)>>0, 2) < pow((ao&0x0C)>>2, 2) + pow((ao&0x30)>>4, 2) )
                AddQuadFlipped(center + dz, dx, dy, ao, true);
            else
                AddQuad(center + dz, dx, dy, ao, true);
        }
    }
    else {
        Normal = vec3( 0.0, 0.0,-1.0);
        if ( (gVisibleFaces[0] & 0x04) != 0) { /* back */
            int ao = (gAo[0][0] & 0x000000FF);
            if ( pow((ao&0x0C)>>2, 2) + pow((ao&0x30)>>4, 2) > pow((ao&0x03)>>0, 2) + pow((ao&0xC0)>>6, 2) )
                AddQuadFlipped(center - dz, dy, dx, ao, false);
            else
                AddQuad(center - dz, dy, dx, ao, false);
        }
    }

}
