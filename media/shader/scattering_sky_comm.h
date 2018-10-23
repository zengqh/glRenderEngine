//////////////////////////////////////////////////////////////////////////
FLOAT ExpScale(FLOAT fCos)
{
	FLOAT x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

FLOAT GetRayleighPhase( FLOAT fCos2 )
{
	return 0.75 * (1.0 + fCos2);
}

FLOAT GetMiePhase( FLOAT fCos, FLOAT fCos2 )
{
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
}

