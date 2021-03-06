#version 400 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 chunkPosition;
uniform vec3 chunkSize;
uniform int margin;
uniform sampler2D noiseSampler;

#define PI 3.14159265359

float   random(vec2 p) {
    return fract(sin(mod(dot(p, vec2(12.9898,78.233)), 3.14))*43758.5453);
}

vec2    random2(vec2 p) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

float   noise( vec3 x ) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
    vec2 rg = texture(noiseSampler, (uv + 0.5) / 256.0, 0.0).rg;
    return mix(rg.y, rg.x, f.z);
}

float   fbm2d(in vec2 st, in float amplitude, in float frequency, in int octaves, in float lacunarity, in float gain) {
    float value = 0.0;
    st *= frequency;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise(vec3(st, 1.0));
        st *= lacunarity;
        amplitude *= gain;
    }
    return value;
}

float   fbm3d(in vec3 st, in float amplitude, in float frequency, in int octaves, in float lacunarity, in float gain) {
    float value = 0.0;
    st *= frequency;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise(st);
        st *= lacunarity;
        amplitude *= gain;
    }
    return value;
}

vec2    voronoi2d( vec2 x ) {
    vec2 n = floor(x);
    vec2 f = fract(x);
	vec3 m = vec3(8.0);
    for (int j=-1; j<=1; j++)
    for (int i=-1; i<=1; i++) {
        vec2 g = vec2(i, j);
        vec2 o = random2(n + g);
        vec2 r = g - f + o;
        float d = dot(r, r);
        if (d < m.x)
            m = vec3(d, o);
    }
    return vec2(sqrt(m.x), m.y+m.z);
}

#define AIR 0.
#define DIRT 1/255.
#define GRASS 2/255.
#define STONE 3/255.
#define BEDROCK 4/255.
#define COAL 5/255.
#define IRON 6/255.
#define GOLD 7/255.
#define LAPIS 8/255.
#define REDSTONE 9/255.
#define DIAMOND 10/255.
#define GRAVEL 11/255.
#define SAND 12/255.
#define OAK_WOOD 13/255.
#define OAK_LEAVES 14/255.
#define WATER 15/255.

float   map(vec3 p) {
    /* TODO : implement biomes with voronoi cells */
    float res;
    /* ceiling level */
    if (p.y > 255)
        return 0;
    /* bedrock level */
    if (p.y == 0 || fbm3d(p, 1.0, 20.0, 2, 1.5, 0.5) > p.y/3.)
        return BEDROCK;
    /* terrain */
    int g0 = int(fbm3d(p, 0.4, 0.0075, 6, 1.7, 0.5) > p.y / 340.); /*  low-frequency landscape */
    int g1 = int(fbm3d(p, 0.5, 0.0215, 4, 1.4, 0.5) > p.y / 340.); /* high-frequency landscape */
    /* caves */
    int g2 = int( (1-abs( fbm3d(vec3(p.x-5, p.y*1.1   , p.z + 21.), 0.45, 0.067, 5, 1.3, 0.49) * 2.-1.)) * 
                  (1-abs( fbm3d(vec3(p.z  , p.y*1.1+4., p.x - 42.), 0.45, 0.046, 5, 0.9, 0.49) * 2.-1.)) < 0.91);
    int g13 = int(fbm3d(p, 0.44, 0.04, 6, 2.0, 0.3) < 0.5);
    /* resource distribution */
    int g3 = int(fbm3d(p+340., 0.35, 0.20, 3, 1.5, 0.37) < 0.1 && p.y < 130);/* coal */
    int g4 = int(fbm3d(p-100., 0.45, 0.30, 3, 1.8, 0.30) < 0.1 && p.y < 64); /* iron */
    int g10= int(fbm3d(p+100., 0.45, 0.35, 3, 1.2, 0.33) < 0.1 && p.y < 32); /* gold */
    int g11= int(fbm3d(p-230., 0.55, 0.30, 3, 1.2, 0.33) < 0.1 && p.y < 32); /* lapis */
    int g12= int(fbm3d(p-160., 0.45, 0.35, 3, 0.2, 0.30) < 0.1 && p.y < 16); /* redstone */
    int g5 = int(fbm3d(p+100., 0.45, 0.20, 3, 1.5, 0.38) < 0.1 && p.y < 16); /* diamond */
    /* stone (we use the same values for fbm as landscape but with a vertical offset) */
    int g7 = int(fbm3d(p+vec3(0,16,0), 0.40, 0.0075, 5, 1.7, 0.5) > p.y / 340.); /*  low-frequency landscape */
       g7 &= int(fbm3d(p+vec3(0,16,0), 0.55, 0.0215, 3, 1.4, 0.5) > p.y / 340.); /* high-frequency landscape */
       g7 &= int(fbm3d(p+vec3(0, 5,0), 0.40, 0.0075, 5, 1.7, 0.5) > p.y / 340.); /*  low-frequency landscape */
       g7 &= int(fbm3d(p+vec3(0, 5,0), 0.55, 0.0215, 3, 1.4, 0.5) > p.y / 340.); /* high-frequency landscape */
    /* pockets of dirt and gravel in undergrounds */
    int g8 = int(abs(fbm3d(p+40, 0.32, 0.11, 3, 1.0, 0.5)*2.-1.) < 0.05 + (1.0-p.y/96.)*0.05);
    int g9 = int(fbm3d(p-70, 0.45, 0.14, 3, 1.0, 0.2) < 0.05 + (1.0-p.y/200.)*0.05);
    /* trees */
    // int g14= int(fbm2d(p.xz, 0.25, 0.01, 4, 2.7, 0.2) < 0.5);
    //    g14&= int(fbm2d(p.xz, 0.47, 0.25, 4, 2.5, 0.1) < 0.5);
    /* water source */
    int g15= int(p.y == 85) * (g2 & g13);

    res = float(g0 & g1 & g2 & g13) * DIRT;

    res = (res == DIRT  &&  g7 == 1 ? STONE : res );
    res = (res == STONE &&  g5 == 1 ? DIAMOND : res );
    res = (res == STONE && g12 == 1 ? REDSTONE : res );
    res = (res == STONE && g11 == 1 ? LAPIS : res );
    res = (res == STONE && g10 == 1 ? GOLD : res );
    res = (res == STONE &&  g4 == 1 ? IRON : res );
    res = (res == STONE &&  g3 == 1 ? COAL : res );
    res = (res == STONE &&  g8 == 1 ? DIRT : res );
    res = (res == STONE &&  g9 == 1 ? GRAVEL : res );
    // res = (res == DIRT && g14 == 0 ? OAK_WOOD : res);
    res = (res == AIR && g15 == 1 ? WATER : res );

    return res;
}

/* 3d volume texture */
void    main() {
    vec2 uv = vec2(TexCoords.x, (1.0 - TexCoords.y));

    vec2 c_uv = floor(uv * chunkSize.xy);
    float z = mod(floor(uv.y * chunkSize.y * chunkSize.z), chunkSize.z);
    vec3 pos = vec3(c_uv, z);

    ivec3 border = ivec3(chunkSize-1);
    int low = margin/2-1;
    if ((low <= pos.x && pos.x < border.x) && (low <= pos.y && pos.y < border.y) && (low <= pos.z && pos.z < border.z)) { /* ignore outer margins */
        vec3 worldPos = (chunkPosition + pos - float(margin)*0.5);
        FragColor.r = sqrt(map(worldPos)); /* values from [0..255] (0..1) are in normalized fixed-point representation, a simple sqrt() fixes that. */
    }
    else /* border value (255) */
        FragColor.r = 1.0;
}
