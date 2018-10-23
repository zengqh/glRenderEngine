varying MVEC2 vTexOut;

uniform MSAMPLER2D texDiff;

void main()
{
	gl_FragColor = texture2D(texDiff, vTexOut);
}