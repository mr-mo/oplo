#version 440

#ifdef VERTEX_SHADER

layout(location = 0) in vec2 vertexIn;

out vec2 texCoord;

void main()
{
	texCoord = vertexIn * 0.5 + 0.5;
	gl_Position = vec4( vertexIn, 0, 1 );
}

#endif

#ifdef FRAGMENT_SHADER

//Todo: views
layout(binding = 0) uniform sampler2DArray framebufferData;

layout(location = 0) out vec3 color;
layout(location = 1) uniform vec3 lightDirection = vec(0, 1, 0);

void main()
{
	vec3 colorIn = texelFetch(framebufferData, ivec3(gl_FragCoord.xy, 0), 0).xyz;
	vec3 normalIn = texelFetch(framebufferData, ivec3(gl_FragCoord.xy, 1), 0).xyz;

	color = colorIn * max(0, dot(normalIn, lightDirection)) + colorIn * vec3(0.2); 
}


#endif

#ifdef BLUR_TO_SCREEN

layout(binding = 0) uniform sampler2DArray framebufferData;
layout(binding = 2) uniform sampler2D SAT;
layout(location = 0) out vec3 color;
layout(location = 0) uniform float blurRadius = 4.5;

in vec2 texCoord;

vec4 sampleSAT(vec2 xy, vec2 ab)
{
	vec4 r = texture(SAT, xy + ab); // S[x+a, y+b]
	r = r - texture(SAT, xy + vec2(ab.x, 0) ); // S[x+a, y]
	r = r - texture(SAT, xy + vec2(0, ab.y) ); // S[x, y+b]
	r = r + texture(SAT, xy); // S[x, y]
	return r / (4.5 * 4.5);
}

void main()
{
	color = sampleSAT(texCoord, vec2(blurRadius / textureSize(SAT, 0))).xyz;
}

#endif

#ifdef COPY_TO_SCREEN

//Todo: views
layout(binding = 0) uniform sampler2DArray framebufferData;

layout(location = 0) out vec3 color;

void main()
{
	vec3 colorIn = texelFetch(framebufferData, ivec3(gl_FragCoord.xy, 2), 0).xyz;
	color = vec3(1); //colorIn;
}


#endif

