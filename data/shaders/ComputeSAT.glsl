
#version 440

#define NORMALIZATION_FACTOR 65535.0

#ifdef VERTICAL_SLICE

readonly uniform layout(binding=0, rgba32f) image2D sourceTexture;
uniform layout(binding=1, rgba32f) image2D targetTexture;
layout(location=0) uniform ivec2 textureSetup;

layout(local_size_x=128, local_size_y=1, local_size_z=1) in;

void main()
{
	ivec2 texel = ivec2(0);
	texel.x = int(gl_WorkGroupSize.x * gl_WorkGroupID.x + gl_LocalInvocationID.x);
	
	vec4 c = imageLoad(sourceTexture, texel);
	imageStore(targetTexture, texel, c);

	for( int i = 1; i < textureSetup.y; ++i )
	{
		texel.y += 1;
		c += imageLoad(sourceTexture, texel);
		imageStore(targetTexture, texel, c);
	} 
}

#endif

#ifdef HORIZONTAL_SLICE

uniform layout(binding=1, rgba32f) image2D targetTexture;
layout(location=0) uniform ivec2 textureSetup;

layout(local_size_x=1, local_size_y=128, local_size_z=1) in;

void main()
{
	ivec2 texel = ivec2(0);
	texel.y = int(gl_WorkGroupSize.y * gl_WorkGroupID.y + gl_LocalInvocationID.y);
	
	vec4 c = imageLoad(targetTexture, texel);

	for( int i = 1; i < textureSetup.x; ++i )
	{
		texel.x += 1;
		c += imageLoad(targetTexture, texel);
		imageStore(targetTexture, texel, c);
	} 
}

#endif

#ifdef RECTANGLE_BOUNDARIES

layout(std430, binding=0) buffer ScreenBuffer
{
	ivec4 m_color[];
};

readonly uniform layout(binding=1, r32f) image2D depthBuffer;
readonly uniform layout(binding=2, rgba32f) image2D sourceTexture;
uniform layout(location=1) vec2 cocParams;
uniform layout(location=0) ivec2 screenExtents;

layout(local_size_x=16, local_size_y=16, local_size_z=1) in;

void main()
{
	ivec2 texel = ivec2(gl_WorkGroupSize.xy * gl_WorkGroupID.xy + gl_LocalInvocationID.xy);

	if( texel.x > screenExtents.x - 1 )
		return;

	if( texel.y > screenExtents.y - 1 )
		return;

	vec4 rgba = imageLoad(sourceTexture, texel);

	float depth = imageLoad(depthBuffer, texel);
	
	int blurAmount = int(ceil(cocParams.x * depth + cocParams.y));
	
	int y = min( texel.y, screenExtents.y - 1 ) * screenExtents.x;
	int x = min(screenExtents.x - 1, texel.x );

	float blurArea = ( blurAmount * 2 + 1 ) * ( blurAmount * 2 + 1 );
	ivec4 addativeFactor = ivec4((rgba / blurArea).xyz * NORMALIZATION_FACTOR, 2);
	//ivec4 addativeFactor = ivec4(rgba * NORMALIZATION_FACTOR);

	//todo: compensate...
	if(texel.y + blurAmount > screenExtents.y - 1 )
		return;

	if(texel.y - blurAmount < 0 )
		return;

	if(texel.x + blurAmount > screenExtents.x - 1 )
		return;

	if(texel.x - blurAmount < 0 )
		return;

	int ymax = min( texel.y + blurAmount, screenExtents.y - 1 ) * screenExtents.x;
	int xmax = min( screenExtents.x - 1, texel.x + blurAmount);

	int ymin = max(0, texel.y - blurAmount) * screenExtents.x;
	int xmin = max(0, texel.x - blurAmount);

	atomicAdd(m_color[ymax + xmax].x, addativeFactor.x);
	atomicAdd(m_color[ymax + xmax].y, addativeFactor.y);
	atomicAdd(m_color[ymax + xmax].z, addativeFactor.z);
	atomicAdd(m_color[ymax + xmax].w, addativeFactor.w);

	atomicAdd(m_color[ymin + xmin].x, addativeFactor.x);
	atomicAdd(m_color[ymin + xmin].y, addativeFactor.y);
	atomicAdd(m_color[ymin + xmin].z, addativeFactor.z);
	atomicAdd(m_color[ymin + xmin].w, addativeFactor.w);

	addativeFactor = ivec4((rgba / blurArea).xyz * -NORMALIZATION_FACTOR, -2);

	atomicAdd(m_color[ymax + xmin].x, addativeFactor.x);
	atomicAdd(m_color[ymax + xmin].y, addativeFactor.y);
	atomicAdd(m_color[ymax + xmin].z, addativeFactor.z);
	atomicAdd(m_color[ymax + xmin].w, addativeFactor.w);

	atomicAdd(m_color[ymin + xmax].x, addativeFactor.x);
	atomicAdd(m_color[ymin + xmax].y, addativeFactor.y);
	atomicAdd(m_color[ymin + xmax].z, addativeFactor.z);
	atomicAdd(m_color[ymin + xmax].w, addativeFactor.w);
}

#endif

#ifdef RECTANGLE_SAT_HORIZ

restrict uniform layout(binding=1, rgba32f) image2D targetTexture;
layout(location=0) uniform ivec2 screenExtents;

layout(local_size_x=1, local_size_y=128, local_size_z=1) in;

void main()
{
	ivec2 texel = ivec2(0);
	texel.y = int(gl_WorkGroupSize.y * gl_WorkGroupID.y + gl_LocalInvocationID.y);

	if(texel.y > screenExtents.y - 1)
		return;

	vec4 c = imageLoad(targetTexture, texel);
	imageStore(targetTexture, texel, vec4(c.xyz / (c.w * 0.5), 1));

	for( int i = 1; i < screenExtents.x; ++i )
	{
		texel.x += 1;
		c += imageLoad(targetTexture, texel);
		imageStore(targetTexture, texel, vec4(c.xyz / (c.w * 0.5), 1));
	} 
}


#endif

#ifdef RECTANGLE_SAT_VERT

readonly layout(std430, binding=0) buffer ScreenBuffer
{
	ivec4 m_color[];
};

writeonly uniform layout(binding=1, rgba32f) image2D targetTexture;
layout(location=0) uniform ivec2 screenExtents;

layout(local_size_x=128, local_size_y=1, local_size_z=1) in;

void main()
{
	ivec2 texel = ivec2(0);
	texel.x = int(gl_WorkGroupSize.x * gl_WorkGroupID.x + gl_LocalInvocationID.x);

	if(texel.x > screenExtents.x - 1)
		return;

	vec4 c = vec4(
				m_color[texel.y * screenExtents.x + texel.x].xyz / vec3(NORMALIZATION_FACTOR), 
				m_color[texel.y * screenExtents.x + texel.x].w);
	imageStore(targetTexture, texel, c);

	for( int i = 1; i < screenExtents.y; ++i )
	{
		texel.y += 1;
		c += vec4(
				m_color[texel.y * screenExtents.x + texel.x].xyz / vec3(NORMALIZATION_FACTOR), 
				m_color[texel.y * screenExtents.x + texel.x].w);
		imageStore(targetTexture, texel, c);
	} 
}


#endif

