uniform MSAMPLER2D Texture;
uniform VEC2 invRTTSize;
varying VEC2 vTexOut;

// Neighbor offset table
const vec2 Offsets[16] = vec2[16](
vec2( 1.5, -1.5),
vec2( 1.5, -0.5),
vec2( 1.5,  0.5),
vec2( 1.5,  1.5),
vec2( 0.5, -1.5),
vec2( 0.5, -0.5),
vec2( 0.5,  0.5),
vec2( 0.5,  1.5),
vec2(-0.5, -1.5),
vec2(-0.5, -0.5),
vec2(-0.5,  0.5),
vec2(-0.5,  1.5),
vec2(-1.5, -1.5),
vec2(-1.5, -0.5),
vec2(-1.5,  0.5),
vec2(-1.5,  1.5)
);

void main()
{
	VEC4 color = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < 16; i++)
	{
		color += texture2D(Texture, vTexOut + Offsets[i] * invRTTSize);
	}
	
	gl_FragColor = color / 16.0;
}