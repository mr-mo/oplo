#version 440

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 vertexIn;
layout(location = 1) in vec3 normalIn;

layout(location = 2) in vec4 scaleAndTranslation;
layout(location = 3) in vec4 colorIn;
layout(location = 4) in vec4 rotationIn;

layout(location = 0) uniform mat4 modelviewProjection;

out vec4 colorFrag;
out vec3 normalOut;

 vec3 rotate( vec4 q, vec3 v )
 { 
	return v + 2.0 * cross( cross(v, q.xyz ) + q.w * v, q.xyz );
}

void main()
{
	vec3 v = rotate(rotationIn, ( vertexIn * scaleAndTranslation.x ) ) + scaleAndTranslation.yzw; //todo: rotation
	normalOut = rotate(rotationIn, normalIn);
	colorFrag = colorIn;
	gl_Position = modelviewProjection * vec4(v, 1);
}

#endif

#ifdef FRAGMENT_SHADER

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
layout(location = 1) uniform vec3 lightDirection = vec(0, 1, 0);

in vec4 colorFrag;
in vec3 normalOut;

void main()
{
	color = colorFrag.xyz; // * max(0, dot(normalOut, lightDirection)) + colorFrag * vec4(0.2); //mix(colorFrag * max(0, dot(normalOut, lightDirection)) + colorFrag * vec4(0.2), vec4(normalOut * 0.5 + 0.5, colorFrag.a), 0.2);
	normal = normalOut;
}


#endif
