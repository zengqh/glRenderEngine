uniform MSAMPLER2D SourceColor;
uniform VEC2 BlurSize;
uniform VEC2 UVScale;

varying VEC2 vTexOut;

const VEC2 TexelKernel[13] = vec2[13](
vec2( 6.0,  6.0),
vec2( 5.0,  5.0),
vec2( 4.0,  4.0),
vec2( 3.0,  3.0),
vec2( 2.0,  2.0),
vec2( 1.0,  1.0),
vec2( 0.0,  0.0),
vec2(-1.0, -1.0),
vec2(-2.0, -2.0),
vec2(-3.0, -3.0),
vec2(-4.0, -4.0),
vec2(-5.0, -5.0),
vec2(-6.0, -6.0)
);

const float BlurWeight[13] = float[13](
0.002216,
0.008764,
0.026995,
0.064759,
0.120985,
0.176033,
0.199471,
0.176033,
0.120985,
0.064759,
0.026995,
0.008764,
0.002216
);

void main()
{
	VEC4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 13; ++i)
	{
		color += texture2D(SourceColor, vTexOut + TexelKernel[i] * BlurSize * UVScale) * BlurWeight[i];
	}
	
	gl_FragColor = color;
}