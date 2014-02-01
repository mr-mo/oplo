#version 440

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 vertexIn;
layout(location = 1) in vec4 scaleAndTranslation;
layout(location = 2) in vec4 colorIn;
layout(location = 3) in vec4 rotationIn;

layout(location = 0) uniform mat4 modelviewProjection;

out vec4 colorFrag;

 vec3 rotate( vec4 q, vec3 v )
 { 
	return v + 2.0 * cross( cross(v, q.xyz ) + q.w * v, q.xyz );
}

void main()
{
	vec3 v = rotate(rotationIn, ( vertexIn * scaleAndTranslation.x ) ) + scaleAndTranslation.yzw; //todo: rotation
	colorFrag = colorIn;
	gl_Position = modelviewProjection * vec4(v, 1);
}

#endif

#ifdef FRAGMENT_SHADER

layout(location = 0) out vec4 color;

in vec4 colorFrag;

void main()
{
	color = colorFrag;
}


#endif
