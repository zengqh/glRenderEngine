attribute HVEC4  vPos;
uniform   HMAT4  matWVP;
varying   HFLOAT vDepth;

#ifdef HAS_SKELETON
attribute VEC4 vBoneIndices;
attribute VEC4 vBoneWeights;
uniform MAT4 matBones[MAX_BONES_NUM];
#endif

void main()
{
#ifdef HAS_SKELETON
	VEC4 vPos_ = VEC4(0.0, 0.0, 0.0, 1.0);
	
	for (INT i = 0; i < 4; ++i)
	{
		INT idx = INT(vBoneIndices[i] + 0.5);
		FLOAT wt = vBoneWeights[i];
		
		MAT4 matw = matBones[idx];
		vPos_.xyz += ((matw * vPos) * wt).xyz;
	}
#else
	VEC4 vPos_ = vPos;
#endif

	HVEC4 final_pos = matWVP * vPos_;
	vDepth = final_pos.z;
	gl_Position = final_pos;
}