#version 430 core

layout(local_size_x = 20, local_size_y =20, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(location = 0) uniform float t;

uniform vec3 mapSize;
uniform vec2 density;
uniform vec3 grassDims;

uniform vec3 offset;

uniform float windSpeed;
uniform vec2 windDir;

uniform vec3 camPos;

layout(binding = 2, std430) buffer ssbo1 {
	vec3 positions[160000];
	vec4 grassDimensions;
};

layout(binding = 3, std430) buffer heightBuffer{
    float yPos[160000];
};

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, s, -s, c);
	return m * v;
}

vec3 voroNoise(vec2 st, float angleOffset){
	st += t * windSpeed * normalize(windDir);
    st *= 15.0;
	
    vec3 color = vec3(0.0, 0.0, 0.0);

    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 1.; 
    vec2 m_point;   

    for (int j=-1; j<=1; j++ ) {
        for (int i=-1; i<=1; i++ ) {
            vec2 neighbor = vec2(float(i),float(j));
            vec2 point = random2(i_st + neighbor);
            vec2 diff = neighbor + point - f_st;
            float dist = length(diff);

            if( dist < m_dist ) {
                m_dist = dist;
                m_point = point;
            }
        }
    }

    color += m_dist;
	color = vec3(smoothstep(0.001,0.9, m_dist));
    return color;
}

//conditional, both scales need to be equal. 
int getArrayFromUV(vec2 uv){
	return int( density.x *gl_WorkGroupSize.x* uv.x) +   int(uv.y);
}



void populatePosition(vec2 uv){

	vec3 tempWorldPos;
	tempWorldPos.x =  uv.x *  float(mapSize.x / grassDims.x)/float( density.x * gl_WorkGroupSize.x );
	tempWorldPos.z =  uv.y *  float(mapSize.z / grassDims.z)/float( density.y * gl_WorkGroupSize.y );
  
	tempWorldPos.xz += random2(uv) *mapSize.xz/float(density.x * gl_WorkGroupSize.x );
	positions[getArrayFromUV(uv)] = tempWorldPos + offset;	
}


void MakeNoise(vec2 uv){

	vec4 col = vec4(1,1,0,1);
    col.x =  float(uv.x)/(gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    col.y =  float(uv.x)/(gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    
    vec2 st = uv ;
    vec3 vNoise = voroNoise(uv/1000, 0.05* t);
    imageStore(imgOutput, ivec2(st), vec4(vNoise,1.0));
}

void main(){
	
 	ivec2 uv = ivec2(gl_GlobalInvocationID.xy);
    grassDimensions = vec4(grassDims, 0.0);
	populatePosition(uv);
    MakeNoise(uv);
}