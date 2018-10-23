uniform MSAMPLER2D texDiff;
varying VEC2 vTexOut;

void main()
{
	gl_FragColor = texture2D(texDiff, vTexOut);
}