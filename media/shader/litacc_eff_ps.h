varying VEC4 vTexOutProj;
varying VEC3 rayView;

uniform MSAMPLER2D normalGB;
uniform MSAMPLER2D depthViewGB;
uniform FLOAT zFar;
uniform VEC2 InvFocalLen;

// point light
uniform HVEC4 point_light_pos;
uniform MVEC3 point_light_diff;

// spot light
uniform HVEC4 spot_light_pos;
uniform MVEC4 spot_light_direction;
uniform MVEC4 spot_light_diff;

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
	return vec3(uv * invFocalLen * eye_z, -eye_z);
}

VEC3 getPosView(VEC4 uvProj, VEC3 rayView)
{
	// 因为vPosView / rayView = vPosViewZ / rayViewZ
    // 得到vPowView = rayView * (vPosViewZ / rayViewZ)

	FLOAT depthView = -texture2DProj(depthViewGB, uvProj).r;
	return rayView * ((depthView * zFar) / rayView.z);
}

VEC4 getNormalViewShininess(VEC4 uvProj)
{
	VEC4 objNormViewShin = texture2DProj(normalGB, uvProj);
	return vec4(decodeNormalVector(objNormViewShin.rg), objNormViewShin.b * 255.0);
}

HFLOAT saturate(HFLOAT val)
{
	return clamp(val, 0.0, 1.0);
}

void main()
{
	VEC4 final_clr = vec4(0.0, 0.0, 0.0, 0.0);
	VEC3 objPosView = getPosView(vTexOutProj, rayView);
	
	VEC4 objNormViewShin = getNormalViewShininess(vTexOutProj);
	VEC3 vN = objNormViewShin.xyz;
	
	VEC3 totalLitDiff = vec3(0.0, 0.0, 0.0);
    VEC3 totalLitSpec = vec3(0.0, 0.0, 0.0);
	
	#ifdef HAS_POINT_LITS
		VEC4 posAtt = point_light_pos;
		VEC3 lDiff  = point_light_diff;
		
		VEC3 vL = posAtt.xyz - objPosView;
	    FLOAT  vL_len = length(vL);
	    vL = vL / vL_len;
		
		FLOAT att = cos(clamp(vL_len * posAtt.w, 0.0, M_PI)) * 0.5 + 0.5;
        lDiff *= att;
		
		totalLitDiff = max(0.0, dot(vN, vL)) * lDiff;
		
		if (objNormViewShin.a > 0.001)
		{
			// 因为campos = 0，vEye = 0-objPosView，vH = vL + vEye
            VEC3 vH = normalize(vL - objPosView);
            totalLitSpec = pow(max(0.0, dot(vN, vH)), objNormViewShin.a) * lDiff;
		}
	#endif
	
	#ifdef HAS_SPOT_LITS
		float4 posAtt      = spot_light_pos;
        float4 spotDirCosI = spot_light_direction;
        float4 lDiffCosIO  = spot_light_diff;
		
		VEC3 vL = posAtt.xyz - objPosView;
	    float  vL_len = length(vL);
	    vL = vL / vL_len;

	    float att = cos(clamp(vL_len * posAtt.w, 0.0, M_PI)) * 0.5 + 0.5;
        att *= square( saturate( (dot(spotDirCosI.xyz, vL) - spotDirCosI.w) * lDiffCosIO.w ) );
        lDiffCosIO.rgb *= att;

        totalLitDiff = max(0.0, dot(vN, vL)) * lDiffCosIO.rgb;

        if ( objNormViewShin.a > 0.001 )
        {
            float3 vH = normalize(vL - objPosView);
            totalLitSpec = pow(max(0.0, dot(vN, vH)), objNormViewShin.a) * lDiffCosIO.rgb;
        }
	#endif
	
	final_clr.rgb = totalLitDiff;
	final_clr.a = toLumin(totalLitSpec);
	
	gl_FragColor = final_clr;
}