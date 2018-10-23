attribute VEC4 vPos;
attribute VEC2 vTex;

varying VEC2 vTexOut;

uniform MAT4 matWVP;

void main()
{
	vTexOut = vTex;
	gl_Position = matWVP * vPos;
}