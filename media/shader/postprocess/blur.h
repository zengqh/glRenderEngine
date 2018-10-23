uniform VEC2 TexelKernel[13];
uniform FLOAT BlurWeight[13];
uniform MSAMPLER2D SourceColor;
uniform FLOAT BlurSize;

varying VEC2 vTexOut;

void main()
{
	VEC4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 13; ++i)
	{
		color += texture2D(SourceColor, vTexOut + TexelKernel[i] * BlurSize) * BlurWeight[i];
	}
	
	gl_FragColor = color;
}