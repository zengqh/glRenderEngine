uniform FLOAT Luminance;
uniform FLOAT MiddleGray;
uniform FLOAT WhiteCutoff;
uniform FLOAT EffectWeight;
uniform MSAMPLER2D SourceColor;

varying VEC2 vTexOut;

void main()
{	
	VEC4 color_out = texture2D(SourceColor, vTexOut);
	
	color_out.rgb *= MiddleGray / (Luminance + 0.001);
	color_out.rgb *= (1.0 + (color_out.rgb / (WhiteCutoff * WhiteCutoff)));
	color_out.rgb -= 5.0;
	
	color_out.rgb = max(color_out.rgb, 0.0);
	color_out.rgb /= (10.0 + color_out.rgb);
	gl_FragColor = vec4(color_out.rgb * 6, 1.0);
}