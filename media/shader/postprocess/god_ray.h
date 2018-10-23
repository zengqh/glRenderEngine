uniform INT	NumberOfSamples;
uniform FLOAT Density;
uniform FLOAT Weight;
uniform FLOAT Decay;
uniform VEC2 LightPosition;
uniform VEC4 Color;
uniform MSAMPLER2D Map;

varying VEC2 vTexOut;

void main()
{
	VEC3 color = texture2D(Map, vTexOut);
	
	// Calculate vector
	VEC2 texUV = vTexOut;
	VEC2 deltaTexUV = texUV - LightPosition;
	
	deltaTexUV *= 1.0 / NumberOfSamples * Density;
	
	FLOAT illuminationDecay = 1.0;
	
	for (INT i = 0; i < NumberOfSamples; i++)
	{
		texUV -= deltaTexUV;
		
		VEC3 sample = texture2D(Map, texUV).rgb;
		
		sample *= illuminationDecay * Weight;
		
		color += sample;
		
		illuminationDecay *= Decay;
	}
	
	VEC3 resultingColor = color * Color;
	gl_FragColor = vec4(resultingColor, 1.0);
}