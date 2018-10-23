/** attribute */
attribute VEC4 vPos;
attribute VEC3 vNormal;

#ifdef HAS_NORMALMAP
attribute VEC3 vTangent;
#endif

#ifdef HAS_UV1
attribute VEC2 vTex;
#endif

#ifdef HAS_DIFFUSE2MAP
attribute VEC2 vTex2;
#endif

/** varying */
varying VEC4 vPosWorld;
varying VEC3 vNormalWorld;

#ifdef HAS_NORMALMAP
varying VEC3 vTangentWorld;
#endif

#ifdef HAS_UV1
varying VEC2 vTexOut;
#endif

#ifdef HAS_DIFFUSE2MAP
varying VEC2 vTexOut2;
#endif

#ifdef HAS_DEFER_LIT
varying VEC4 vLitBufTex;
#endif

/** uniform */
uniform MAT4 matWorld;
uniform MAT3 matWorldIT;
uniform MAT4 matWVP;

#ifdef HAS_TEXANI
uniform MAT3 matUV;
#endif


#ifdef HAS_SHADOW
#ifdef HAS_PSSM
uniform MAT4 matShadowProj[4];
varying VEC4 vPosShadowProj[3];
#else
uniform MAT4 matShadowProj;
varying VEC4 vPosShadowProj;
#endif
#endif

#ifdef HAS_SKELETON
attribute VEC4 vBoneIndices;
attribute VEC4 vBoneWeights;
uniform MAT4 matBones[MAX_BONES_NUM];
#endif

/** main function */
void main()
{
#ifdef HAS_SKELETON
	VEC4 vPos_ = VEC4(0.0, 0.0, 0.0, 1.0);
	VEC3 vNormal_ = VEC3(0.0, 0.0, 0.0);
	
	for (INT i = 0; i < 4; ++i)
	{
		INT idx = INT(vBoneIndices[i] + 0.5);
		FLOAT wt = vBoneWeights[i];
		
		MAT4 matw = matBones[idx];
		MAT3 matn = MAT3(matw[0].xyz, matw[1].xyz, matw[2].xyz);
		
		vPos_.xyz += ((matw * vPos) * wt).xyz;
		vNormal_ += (matn * vNormal) * wt;
	}
#else
	VEC4 vPos_ = vPos;
	VEC3 vNormal_ = vNormal;
#endif

	vPosWorld = (matWorld * vPos_);
	vNormalWorld = (matWorldIT * vNormal_);
	
#ifdef HAS_NORMALMAP
	vTangentWorld = matWorldIT * vTangent;
#endif

#ifdef HAS_UV1
#ifdef HAS_TEXANI
	vTexOut = (matUV * VEC3(vTex.x, vTex.y, 1.0)).xy;
#else
	vTexOut = vTex;
#endif
#endif

	VEC4 vProjPos = matWVP * vPos_;

#ifdef HAS_SHADOW
	#ifdef HAS_PSSM
		vPosShadowProj[0] = matShadowProj[0] * vPosWorld;
		vPosShadowProj[1] = matShadowProj[1] * vPosWorld;
		vPosShadowProj[2] = matShadowProj[2] * vPosWorld;
		
		vec4 vPosSP3 = matShadowProj[3] * vPosWorld;
		vPosShadowProj[0].w = vPosSP3.x;
		vPosShadowProj[1].w = vPosSP3.y;
		vPosShadowProj[2].w = vPosSP3.z;
		
		vPosWorld.w = vProjPos.w;
	#else
		vPosShadowProj = matShadowProj * vPosWorld;
	#endif
#endif
	
	
#ifdef HAS_DEFER_LIT
	vLitBufTex = vec4(0.5 * vec2(vProjPos.x + vProjPos.w, vProjPos.y + vProjPos.w) + vProjPos.w, vProjPos.zw);
#endif
	
	gl_Position = vProjPos;
}