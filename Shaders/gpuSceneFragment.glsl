#version 430 core

uniform sampler2D diffuseTex;
uniform sampler2D windTex;

uniform int		  useTexture;
uniform float 	  time;


uniform vec4 	lightColor;
uniform vec3 	lightPos;
uniform float 	lightRadius;

uniform vec2 		windDir;

float contrast   = 0.99;
float saturation = 1.5;
float brightness = 0.1;
float gamma      = 2.2;

vec4 AoColor 		= vec4(0.02, 0.0, 0.22, 1.0);
vec4 topGreen 		= vec4(0.2039, 0.5373, 0.1647, 1.0);
vec4 bottomGreen 	= vec4(0.0, 0.3608, 0.2275, 1.0);


layout (binding  = 2, std430) readonly buffer ssbo1 { 
	vec3 positions[160000];
	vec4 grassDimensions;
};

layout(binding = 3, std430) buffer heightBuffer{
    float yPos[160000];
};


in Vertex{
	vec2 texCoord;
	vec3 cameraReg;
	float dist;
	vec3 nWorldPos;
	vec3 normal;
} IN;

out vec4 fragColour;

mat4 satMatrix( float saturation){
	vec3 luminance = vec3( 0.3086, 0.6094, 0.0820 );
    
    float oneMinusSat = 1.0 - saturation;
    
    vec3 red = vec3( luminance.x * oneMinusSat );
    red+= vec3( saturation, 0, 0 );
    
    vec3 green = vec3( luminance.y * oneMinusSat );
    green += vec3( 0, saturation, 0 );
    
    vec3 blue = vec3( luminance.z * oneMinusSat );
    blue += vec3( 0, 0, saturation );
    
    return mat4( red,     0,
                 green,   0,
                 blue,    0,
                 0, 0, 0, 1 );
}

mat4 contrastMatrix(float contrast){
	float t = ( 1.0 - contrast ) / 2.0;
    
    return mat4( contrast, 0, 0, 0,
                 0, contrast, 0, 0,
                 0, 0, contrast, 0,
                 t, t, t, 1 );
}

mat4 brightnessMatrix( float _brightness )
{
    return mat4( 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 _brightness, _brightness, _brightness, 1 );
}

vec4 addWind( vec2 uv ){

	uv = uv/5.0f;
	float windTime = time * 0.02 * 10.0f;
	uv  += windTime * vec2(-windDir.x, windDir.y);
	return (texture2D(windTex, uv));
	
}

float heightBlend(float yPosition, float heightBlendFactor){

	//heigthmap y scale is 3.0
	float tempY = (yPosition - 1.9f)/ 3.0f;
	return smoothstep(0.0f, heightBlendFactor, tempY );
}

//////////////////// FLAT COLORIZATION !!! ///////////////////////////////
vec4 colorize(vec2 uv, vec3 objectPos , float inRegion, vec3 camDist){
	
	vec4 bladeCol 	 = mix(topGreen, bottomGreen, uv.y);
	vec4 windValue   =  texture2D(diffuseTex, objectPos.xz);
	windValue 		*= vec4(1.0, 1.0, 1.0, 1.0);
	vec4 aoCol 		 = mix(vec4(1.0f), AoColor, uv.y);
	vec4 tip 		 = 0.5 * clamp(1.0 -  (windValue * 2.0), 0.0, 1.0);
	tip 			 = mix(vec4(0.0), tip, uv.y - 0.5);

	vec4 finCol;
	finCol = bladeCol;
	finCol += vec4(tip.rgb * 0.5, 1.0);
	finCol *= aoCol;

	finCol +=vec4(0.1137, 0.7176, 0.1922, 1.0) * heightBlend(objectPos.y , 10.0);

	//Enable to see wind on top of grass 
	// vec4 wc = addWind(objectPos.xz);
	// wc = clamp (wc, 0.0, 1.0);
	// vec3 wct = mix(vec3(0.0), wc.xyz, heightBlend(objectPos.y , 8.0));
	// finCol.xyz = mix(finCol.xyz, wct, uv.y);

	if(inRegion > 1){
		// Fade out in progress from camera depending on the distance.
	
		// vec2 camuv = normalize(camDist.xz);
		// // camuv *= 1.0 - camuv.yx;
		// float vig = camuv.x * camuv.y * 10.0;
		// finCol =vec4(camuv, 0.0, 1.0);
	}

	finCol.a = 1.0;
	return finCol;
}


void main(void) {

	vec2 uv 		= IN.texCoord;
	vec3 objectPos	= IN.nWorldPos;

	if(useTexture == 0){

		fragColour 		= contrastMatrix(contrast) * satMatrix(saturation) * colorize(uv, objectPos, IN.dist, IN.cameraReg);
		fragColour.rgb  = pow(fragColour.rgb, vec3(1.0/gamma));

	}else {
		fragColour = texture2D(diffuseTex, uv);
	}

} 