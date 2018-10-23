// x = near blur depth (!= y); y = focal plane depth; z = far blur depth (!= y))
// w = blurriness cutoff constant for objects behind the focal plane											
uniform VEC4 DOFParams;

uniform MSAMPLER2D RGBTexture;
uniform MSAMPLER2D DepthTexture;

uniform FLOAT zFar;

varying VEC2 vTexOut;

void main()
{
	gl_FragColor.rgb = texture2D(RGBTexture, vTexOut).rgb;
	
	FLOAT normalDepth = texture2D(DepthTexture, vTexOut).r;
	
	FLOAT depth = zFar * normalDepth;
	
	FLOAT f;
	
	// Scale depth value between near blur distance and focal distance to [-1, 0] range
	if (depth < DOFParams.y)
	{
		f = (depth - DOFParams.y) / (DOFParams.y - DOFParams.x);
	}
	// Scale depth value between focal distance and far blur distance to [0, 1] range
	else
	{
		f = (depth - DOFParams.y) / (DOFParams.z - DOFParams.y);
	}
	
	f = clamp(f, 0.0, DOFParams.w);
	
	FLOAT a = clamp(f * 0.5 + 0.5, 0.0, 1.0);
	
	gl_FragColor.a = a;
}
