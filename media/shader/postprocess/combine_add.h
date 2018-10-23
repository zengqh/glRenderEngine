uniform FLOAT EffectWeight;
uniform MSAMPLER2D SceneColor;
uniform MSAMPLER2D SourceColor;

varying VEC2 vTexOut;

VEC4 lerp(VEC4 a, VEC4 b, FLOAT s)
{
	return a + (b - a) * s;
}

void main()
{
	VEC4 sce_clr = texture2D(SceneColor, vTexOut);
	VEC4 src_clr = texture2D(SourceColor, vTexOut);
	
	gl_FragColor = lerp(sce_clr, sce_clr + src_clr, EffectWeight);
}