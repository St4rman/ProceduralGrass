#version 330 core

uniform sampler2D diffuseTex;
uniform int		  useTexture;

in Vertex{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;
void main(void) {
	fragColour = IN.colour;
	fragColour = texture(diffuseTex, IN.texCoord);
	// fragColour = vec4(1.0,0.0,0.0,1.0);
	
}