varying VEC2 vTexOut;

uniform MSAMPLER2D BlurTexture;
uniform MSAMPLER2D Texture;
uniform VEC2 pixelSizeHigh;
uniform VEC2 pixelSizeLow;

#define NUM_TAPS 8

const VEC2 poisson[NUM_TAPS] = vec2[NUM_TAPS](
	vec2( 0.7, -0.2),
	vec2( 0.1, -0.9),
	vec2( 0.3,  0.8),
	vec2( 0.8,  0.3),
	vec2(-0.7,  0.4),
	vec2(-0.4,  0.6),
	vec2(-0.8, -0.5),
	vec2(-0.4,  0.6)
);

const VEC2 maxCoC = vec2(5.0, 10.0);

const FLOAT radiusScale = 0.4;

void main()
{
	VEC4 color = texture2D(Texture, vTexOut);
	
	FLOAT centerDepth = color.a;
	
	// Convert depth into blur radius in pixels
	FLOAT discRadius = abs(color.a * maxCoC.y - maxCoC.x);
	
	// Compute radius on low-res image
	FLOAT discRadiusLow = discRadius * radiusScale;
	
	color = vec4(0.0, 0.0, 0.0, 0.0);
	
	for (INT i = 0; i < NUM_TAPS; ++i)
	{
		// Compute tap texture coordinates
		VEC2 coordLow = vTexOut + pixelSizeLow * poisson[i] * discRadiusLow;
		VEC2 coordHigh = vTexOut + pixelSizeHigh * poisson[i] * discRadius;
		
		// Fetch low- and high-res tap
		VEC4 tapLow = texture2D(BlurTexture, coordLow);
		VEC4 tapHigh = texture2D(Texture, coordHigh);
		
		// Mix low- and high-res taps based on tap blurriness
		FLOAT tapBlur = abs(tapHigh.a * 2.0 - 1.0);
		VEC4 tap = mix(tapHigh, tapLow, tapBlur);
		
		// 'smart' blur ignores taps that are closer than the center tap and in focus
		tap.a = (tap.a >= centerDepth) ? 1.0 : abs(tap.a * 2.0 - 1.0);
		
		color.rgb += tap.rgb * tap.a;
		color.a += tap.a;
	}
	
	gl_FragColor = color / color.a;
}