//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/08/30
// File: gbuffer_eff_vs.h
//

uniform MAT4 matWV;
uniform MAT4 matWVP;
uniform HFLOAT zFar;

attribute VEC4 vPos;
attribute VEC3 vNormal;

#ifdef HAS_SKELETON
attribute VEC4 vBoneIndices;
attribute VEC4 vBoneWeights;
uniform MAT4 matBones[MAX_BONES_NUM];
#endif

varying VEC4 vNormalDepth;

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

	VEC4 vFinalPos = matWVP * vPos_;
	vNormalDepth.xyz = MAT3(matWV) * vNormal_;
	vNormalDepth.w = vFinalPos.w / zFar;
	
	gl_Position = vFinalPos;
}