
const vec2 vec[4] = vec2[4](
	vec2( 1.0,  0.0),
	vec2(-1.0,  0.0),
	vec2( 0.0,  1.0),
	vec2( 0.0, -1.0)
);

uniform FLOAT SampleRadius;
uniform FLOAT Intensity;
uniform FLOAT Scale;
uniform FLOAT Bias;
uniform INT RandomSize;
uniform MSAMPLER2D RandomNormalsMap;
uniform VIEC2 Resolution;
uniform MSAMPLER2D NormalMap;
uniform MSAMPLER2D DepthMap;
uniform FLOAT zFar;
uniform VEC2 InvFocalLen;

varying VEC2 vTexOut;

VEC3 decodeNormalVector(VEC2 normal)
{
	VEC2 fenc = normal * 4.0 - 2.0;
	float f = dot(fenc, fenc);
	float g = sqrt(1.0 - f / 4.0);
	
	VEC3 n;
	n.xy = fenc * g;
	n.z = 1.0 - f / 2.0;
	return n;
}

VEC3 uv_to_eye(VEC2 uv, float eye_z, VEC2 invFocalLen)
{
	uv = uv * 2.0 - 1.0;
	return vec3(uv * invFocalLen * eye_z, eye_z);
}

VEC4 getNormalViewShininess(VEC2 uv)
{
	VEC4 objNormViewShin = texture2D(NormalMap, uv);
	return vec4(decodeNormalVector(objNormViewShin.rg), objNormViewShin.b * 255.0);
}

FLOAT getEyeZ(VEC2 uv)
{
	FLOAT normal_z = texture2D(DepthMap, uv).x;
	return normal_z * zFar;
}

VEC3 fetch_eye_pos(VEC2 uv, VEC2 invFocalLen)
{
	FLOAT eye_z = getEyeZ(uv);
	return uv_to_eye(uv, eye_z, invFocalLen);
} 

HFLOAT saturate(HFLOAT val)
{
	return clamp(val, 0.0, 1.0);
}

VEC2 getRandom(VEC2 uv)
{
	VEC2 ruv = Resolution * uv / RandomSize;
	return normalize(texture2D(RandomNormalsMap, ruv.xy).xy * 2.0 - 1.0);
}

FLOAT doAmbientOcclusion(VEC2 tcoord, VEC2 uv, VEC3 p, VEC3 cnorm)
{
	VEC3 diff = fetch_eye_pos(tcoord + uv, InvFocalLen) - p;
	VEC3 v = normalize(diff);
	float d = length(diff)*Scale;
	return max(0.0, dot(cnorm, v) - Bias)*(1.0/(1.0 + d))*Intensity;
}

void main()
{
	VEC3 vPosView = fetch_eye_pos(vTexOut, InvFocalLen);
	
	VEC4 objNormViewShin = getNormalViewShininess(vTexOut);
	VEC3 vN = objNormViewShin.xyz;
	
	vN.z = -vN.z;
	
	VEC2 rand = getRandom(vTexOut);
	
	FLOAT ao = 0.0;
	FLOAT rad = SampleRadius / vPosView.z;
	INT iterations = 4;
	for (INT i = 0; i < iterations; ++i)
	{
		VEC2 coord1 = reflect(vec[i], rand) * rad;
		VEC2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);
		
		ao += doAmbientOcclusion(vTexOut, coord1*0.25, vPosView, vN);
		ao += doAmbientOcclusion(vTexOut, coord2*0.5,  vPosView, vN);
		ao += doAmbientOcclusion(vTexOut, coord1*0.75, vPosView, vN);
		ao += doAmbientOcclusion(vTexOut, coord2,      vPosView, vN);
	}
	
	ao /= FLOAT(iterations) * 4.0;
	ao = 1.0 - saturate(ao);
	gl_FragColor = vec4(ao, ao, ao, ao);
}