varying VEC3 vMieColor;
varying VEC3 vRayleighColor;
varying VEC3 vCameraToVertexDir;
varying VEC3 vOutLightPos;

uniform FLOAT g;
uniform FLOAT g2;

//////////////////////////////////////////////////////////////////////////
FLOAT GetRayleighPhase( FLOAT fCos2 )
{
	return 0.75 * (1.0 + fCos2);
}

FLOAT GetMiePhase( FLOAT fCos, FLOAT fCos2 )
{
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + g2) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
}

void main()
{
	FLOAT fCos = dot(vOutLightPos, vCameraToVertexDir) / length(vCameraToVertexDir);
	FLOAT fCos2 = fCos * fCos;
	FLOAT fRayleighPhase = GetRayleighPhase( fCos2);
	FLOAT fMiePhase = GetMiePhase( fCos, fCos2 );
	
	gl_FragColor = vec4(fRayleighPhase * vRayleighColor + fMiePhase * vMieColor, 1.0);
}