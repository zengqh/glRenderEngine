// reference from: http://www.gamedev.net/topic/584256-atmospheric-scattering-and-dark-sky/
// reference from: http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
// sylphis3d engine, http://sylphis3d.com/
uniform VEC3 vInvWavelength;
uniform FLOAT fInnerRadius;
uniform FLOAT fKrESun;
uniform FLOAT fKmESun;
uniform FLOAT fKr4PI;
uniform FLOAT fKm4PI;
uniform FLOAT fScale;
uniform FLOAT fScaleDepth;
uniform FLOAT fScaleOverScaleDepth;

uniform VEC3 vCameraPos;
uniform VEC3 vLightPos;
uniform FLOAT fCameraHeight;

attribute VEC4 vPos;
uniform MAT4 matWVP;

varying VEC3 vMieColor;
varying VEC3 vRayleighColor;
varying VEC3 vCameraToVertexDir;
varying VEC3 vOutLightPos;

#define INTEGRAL_EQUATION_NUM_SAMPLES 2

//////////////////////////////////////////////////////////////////////////
FLOAT ExpScale(FLOAT fCos)
{
	FLOAT x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main()
{
	VEC3 vPositionEarthCenter = vPos.xyz;
	VEC4 in_vertex = vPos;
	in_vertex.y -= fInnerRadius;
	gl_Position = matWVP * in_vertex;
	
	VEC3 vRay = vPositionEarthCenter - vCameraPos;
	FLOAT fFar = length(vRay);
	vRay /= fFar;
	
	// Calculate the ray's starting position, then calculate its scattering offset
	VEC3 vStart = vCameraPos;
	FLOAT fHeight = length(vStart);
	FLOAT fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	FLOAT fStartAngle = dot(vRay, vStart) / fHeight;
	FLOAT fStartOffset = fDepth*ExpScale(fStartAngle);
	
	// Initialize the scattering loop variables
	FLOAT fSampleLength = fFar / FLOAT( INTEGRAL_EQUATION_NUM_SAMPLES );
	FLOAT fScaledLength = fSampleLength * fScale;
	VEC3 vSampleRay = vRay * fSampleLength;
	VEC3 vSamplePoint = vStart + vSampleRay * 0.5;
	
	// Now loop through the sample rays
	VEC3 vFrontColor = vec3(0.0, 0.0, 0.0);
	for (int i=0; i< INTEGRAL_EQUATION_NUM_SAMPLES; i++)
	{
		float fHeight = length(vSamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(vLightPos, vSamplePoint) / fHeight;
		float fCameraAngle = dot(vRay, vSamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(ExpScale(fLightAngle) - ExpScale(fCameraAngle)));
		VEC3 vAttenuate = exp(-fScatter * (vInvWavelength * fKr4PI + fKm4PI));
		vFrontColor += vAttenuate * (fDepth * fScaledLength);
		vSamplePoint += vSampleRay;
	}
	
	vMieColor = vFrontColor * fKmESun;
	vRayleighColor = vFrontColor * (vInvWavelength * fKrESun);
	vCameraToVertexDir = vCameraPos - vPositionEarthCenter;
	vOutLightPos = vLightPos;
}