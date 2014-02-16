
#version 440

readonly uniform layout(binding=0, rgba32f) image2D sourceTexture;
uniform layout(binding=1, rgba32f) image2D targetTexture;
layout(location=0) uniform ivec2 textureSetup;

#ifdef VERTICAL_SLICE

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
