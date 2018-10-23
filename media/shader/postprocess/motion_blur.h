uniform MSAMPLER2D ColorTexture;
uniform MSAMPLER2D DepthTexture;
uniform MAT4 ClipSpaceToWorldSpace;
uniform MAT4 PreviousWorldSpaceToClipSpace;
uniform INT NumberOfSamples;
uniform FLOAT VelocityFactor;
uniform FLOAT zNear;
uniform FLOAT zFar;

varying VEC2 vTexOut;

// nonlinear z to linear z
// http://web.archive.org/web/20130416194336/http://olivers.posterous.com/linear-depth-in-glsl-for-real
// the right formula is the first comment.
// z_e = 2*zFar*zNear / (-zFar - zNear + (zFar - zNear)*z_n);

void main()
{
	FLOAT norm_z = texture2D(DepthTexture, vTexOut).r;
	FLOAT eye_z = -norm_z * zFar;
	
	// get non linear z from view space z(linear)
	FLOAT non_linear_z = (2.0 * zNear * zFar / eye_z + zNear + zFar) / (zFar - zNear);
	
	// clip space to world space
	VEC4 H = vec4(vTexOut * 2.0 - 1.0, non_linear_z, 1.0);
	VEC4 D = ClipSpaceToWorldSpace * H;
	VEC4 worldPos = D / D.w;
	
	VEC4 currentPos = H;
	
	VEC4 previousPos = PreviousWorldSpaceToClipSpace * worldPos;
	previousPos /= previousPos.w;
	
	VEC2 velocity = (currentPos.xy - previousPos.xy) * VelocityFactor;
	
	VEC4 color = vec4(1.0, 0.0, 0.0, 1.0);
	for (INT i = 0; i < NumberOfSamples; i++)
	{
		// Get offset in range [-0.5, 0.5]
		FLOAT scale = float(i) / float(NumberOfSamples) - 0.5;
		VEC2 offset = scale * velocity;
		color += texture2D(ColorTexture, vTexOut + offset);
	}
	
	gl_FragColor = color / float(NumberOfSamples);
}