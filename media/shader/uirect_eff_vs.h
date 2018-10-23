attribute VEC4 vPos;
attribute VEC3 vNormal;
attribute VEC2 vTex;

uniform float isSwapV;

varying VEC2 vTexOut;

void main()
{
	
	if (isSwapV > 0.5)
	{
		VEC2 vTexTemp = vTex;
		vTexTemp.y = 1.0 - vTexTemp.y;
		vTexOut = vTexTemp;
	}
	else
	{
		vTexOut = vTex;
	}
	

	gl_Position = vPos;
}
