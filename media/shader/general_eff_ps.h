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
// Time: 2013/09/07
// File: general_eff_ps.h
//


//////////////////////////////////////////////////////////////////////////
// vars
varying HVEC4 vPosWorld;
varying MVEC3 vNormalWorld;

#ifdef HAS_DEFER_LIT
varying VEC4 vLitBufTex;
#endif

#ifdef HAS_NORMALMAP
varying MVEC3 vTangentWorld;
#endif

#ifdef HAS_UV1
varying MVEC2 vTexOut;
#endif

#ifdef HAS_DIFFUSE2MAP
varying MVEC2 vTexOut2;
#endif

uniform MFLOAT mtr_alpha;

#ifdef HAS_LIGHTING
uniform MVEC3 mtr_diffuse;

#ifdef HAS_SPECULAR
uniform MVEC4 mtr_spec;
#endif

uniform MVEC3 global_amb;
uniform MSAMPLER2D LitBufMap;

#ifdef HAS_DIR_LITS
uniform MVEC3 dir_light_dir;
uniform MVEC3 dir_light_diff;
#endif

#ifdef HAS_SHADOW
#ifdef HAS_PSSM
varying VEC4 vPosShadowProj[3];
#else
varying VEC4 vPosShadowProj;
#endif
#endif

#ifdef HAS_SHADOW
uniform VEC3 shadowInfo;
uniform VEC2 shadowFade;
uniform VEC3 splitZView;
uniform MSAMPLER2D mapShadow;
uniform MSAMPLER2D mapRandom;
uniform vec4 poissonDisk[MAX_POISSON_DISK];
#endif

#endif		// end of HAS_LIGHTING

#ifdef HAS_AO
uniform MSAMPLER2D mapAO;
#endif

#ifdef HAS_EMISSIVE
uniform MVEC3 mtr_emis;
#endif

#ifdef HAS_COLORMAP
uniform MVEC4 texClr;
#endif

#ifdef HAS_DIFFUSEMAP
uniform MSAMPLER2D texDiff;
#endif

#ifdef HAS_DIFFUSE2MAP
uniform MSAMPLER2D texDiff2;
#endif

#ifdef HAS_NORMALMAP
uniform MSAMPLER2D texNormal;
#endif

#ifdef HAS_SEPCULARMAP
uniform MSAMPLER2D texSpecular;
#endif

#ifdef HAS_EMISSIVEMAP
uniform MSAMPLER2D texEmissive;
#endif

#ifdef HAS_FOG
uniform HVEC2 fogParam;
uniform MVEC3 fogColor;
#endif

#ifdef HAS_ALPHA_TEST
uniform HFLOAT alpha_test_ref;
#endif

uniform HVEC3 camPos;

//////////////////////////////////////////////////////////////////////////
// functions
VEC2 mad(VEC2 v0, VEC2 v1, VEC2 v2)
{
	return v0 * v1 + v2;
}

#ifdef HAS_SHADOW
	#ifdef HAS_PSSM
		void calcSplitUVRange( float zView, vec4 posSPA, vec4 posSPB, vec4 posSPC, out vec4 uvRange, out vec3 uvz )
		{
			if ( zView < splitZView.x ) // < split[1]
			{
				uvRange = vec4( 0.0, 0.0, 0.5, 0.5 );
				uvz     = posSPA.xyz;
			}
			else if ( zView < splitZView.y ) // < split[2]
			{
				uvRange = vec4( 0.5, 0.0, 1.0, 0.5 );
				uvz     = posSPB.xyz;
			}
			else if ( zView < splitZView.z )  // < split[3]
			{
				uvRange = vec4( 0.0, 0.5, 0.5, 1.0 );
				uvz     = posSPC.xyz;
			}
			else
			{
				uvRange = vec4( 0.5, 0.5, 1.0, 1.0 );
				uvz     = vec3( posSPA.w, posSPB.w, posSPC.w );
			}
		}
		
		float getShadowDepth( vec4 uvRange, vec2 uv )
		{
			//return any(lessThan(vec4(uv, uvRange.zw), vec4(uvRange.xy, uv))) ? 1.0 :
				return texture2D(mapShadow, clamp(uv, uvRange.xy, uvRange.zw)).r;
		}
	#else
		float getShadowDepth(VEC2 uv)
		{
			return texture2D(mapShadow, uv).r;
		}
	#endif
	
	#ifdef HAS_PSSM
		float getShadowIllum( vec3 uvz, vec4 uvRange )
	#else
		FLOAT getShadowIllum(VEC3 uvz)
	#endif
		{
			FLOAT shadowIllum = 0.0;
			
			vec2 rotations = texture2D(mapRandom, uvz.xy * shadowInfo.x).xy * 2.0 - 1.0; // shadowInfo.x == shadow map size
			mat2 rotMat    = mat2(vec2(rotations.x, -rotations.y), vec2(rotations.y, rotations.x));
			
			for (int i = 0; i < SHADOW_TAPGROUPS; ++i)
			{
				vec4 shadowResult;
				
				#ifdef HAS_PSSM
				shadowResult.x = getShadowDepth( uvRange, mad(rotMat * poissonDisk[i*2].xy, shadowInfo.yy, uvz.xy) );
				shadowResult.y = getShadowDepth( uvRange, mad(rotMat * poissonDisk[i*2].zw, shadowInfo.yy, uvz.xy) );
				shadowResult.z = getShadowDepth( uvRange, mad(rotMat * poissonDisk[i*2+1].xy, shadowInfo.yy, uvz.xy) );
				shadowResult.w = getShadowDepth( uvRange, mad(rotMat * poissonDisk[i*2+1].zw, shadowInfo.yy, uvz.xy) );
				#else
				shadowResult.x = getShadowDepth( mad(rotMat * poissonDisk[i*2].xy, shadowInfo.yy, uvz.xy) );
				shadowResult.y = getShadowDepth( mad(rotMat * poissonDisk[i*2].zw, shadowInfo.yy, uvz.xy) );
				shadowResult.z = getShadowDepth( mad(rotMat * poissonDisk[i*2+1].xy, shadowInfo.yy, uvz.xy) );
				shadowResult.w = getShadowDepth( mad(rotMat * poissonDisk[i*2+1].zw, shadowInfo.yy, uvz.xy) );
				#endif
				
				shadowIllum += dot( 
                (lessThan(uvz.zzzz, shadowResult) ? vec4(0.0, 0.0, 0.0, 0.0) : vec4(1.0, 1.0, 1.0, 1.0)), 
                shadowInfo.zzzz // shadowInfo.z == shadow weight
				);
			}
			
			return shadowIllum;
		}
#endif

//////////////////////////////////////////////////////////////////////////
// main
void main()
{
#ifdef HAS_COLORMAP
	MVEC4 finalClr = texClr;
#else
	MVEC4 finalClr = vec4(1.0, 1.0, 1.0, 1.0);
#endif

#ifdef HAS_DIFFUSEMAP
	MVEC4 texDiffClr = texture2D(texDiff, vTexOut);
	finalClr.a *= texDiffClr.a;
#else
	MVEC4 texDiffClr = vec4(1.0, 1.0, 1.0, 1.0);
#endif

	finalClr.a *= mtr_alpha;
	
#ifdef HAS_ALPHA_TEST
	if (finalClr.a < alpha_test_ref)
	{
		discard;
	}
#endif

#ifdef HAS_LIGHTING
    MVEC3 totLitClrDiff = vec3(0.0, 0.0, 0.0);
    MVEC3 totLitClrSpec = vec3(0.0, 0.0, 0.0);
	
	#ifdef HAS_NORMALMAP
		MMAT3 tangentSpace;
		tangentSpace[0] = normalize(vTangentWorld);
		tangentSpace[1] = normalize(cross(vNormalWorld, vTangentWorld));
		tangentSpace[2] = normalize(vNormalWorld);
		
		MVEC3 vNTan = texture2D(texNormal, vTexOut).xyz;
		vNTan = normalize((vNTan - 0.5) * 2.0);
		MVEC3 vN = tangentSpace * vNTan;
	#else
		MVEC3 vN = normalize(vNormalWorld);
	#endif
	
	MVEC3 vE = normalize(camPos - vPosWorld.xyz);
	
	// point lits and spot lits have been deferred
	#ifdef HAS_DIR_LITS
		#ifdef HAS_SHADOW
			#ifdef HAS_PSSM
				vec4 uvRange;
                vec3 uvz;
				
				calcSplitUVRange( vPosWorld.w, vPosShadowProj[0], vPosShadowProj[1], vPosShadowProj[2], uvRange, uvz );
                float lightIllum = getShadowIllum(uvz, uvRange);
			#else
				FLOAT lightIllum = getShadowIllum(vPosShadowProj.xyz);
			#endif
			
			HFLOAT vEye_len = length(camPos - vPosWorld.xyz);
			
			lightIllum = 1.0 - lightIllum * (1.0 - smoothstep(shadowFade.x, shadowFade.y, vEye_len));
			
		#else
			FLOAT lightIllum = 1.0;
		#endif
		/** directional light */
		MVEC3 vL = dir_light_dir;
		MFLOAT df = max(0.0, dot(vN, vL));
		
		totLitClrDiff += df * lightIllum * dir_light_diff;
		
		#ifdef HAS_SPECULAR
			MVEC3 vH = normalize(vL + vE);
			MFLOAT sf = max(0.0, dot(vN, vH));
			
			sf = pow(sf, mtr_spec.a);
			
			totLitClrSpec += sf * dir_light_diff;
		#endif
	#endif
	
	// deferred lit buffer
	VEC4 diffspec = vec4(0, 0, 0, 1);
	#ifdef HAS_DEFER_LIT
		diffspec = texture2DProj(LitBufMap, vLitBufTex);
		totLitClrDiff += diffspec.rgb;
	#endif
	
	#ifdef HAS_SPECULAR
        totLitClrSpec += diffspec.rgb * (diffspec.a / (toLumin(diffspec.rgb) + 0.02));
    #endif
	
	totLitClrDiff = global_amb + totLitClrDiff * mtr_diffuse;
	
	#ifdef HAS_SPECULAR
		totLitClrSpec = totLitClrSpec * mtr_spec.rgb;
	#endif
	
	#ifdef HAS_SPECULARMAP
		totLitClrSpec *= texture2D(texSpecular, vTexOut).rgb;
	#endif

	// no lighting
#else
	MVEC3 totLitClrDiff = vec3(1.0, 1.0, 1.0);
    MVEC3 totLitClrSpec = vec3(0.0, 0.0, 0.0);
#endif

	// all the light clr
	MVEC3 totReflectClr = totLitClrDiff * texDiffClr.rgb + totLitClrSpec;
	
	#ifdef HAS_DEFER_LIT
		#ifdef HAS_AO
			FLOAT ao = texture2DProj(mapAO, vLitBufTex).a;
			totReflectClr.rgb *= ao;
		#endif
	#endif
	
	#ifdef HAS_EMISSIVE
		#ifdef HAS_EMISSIVE_MAP
			totReflectClr += mtr_emis * texture2D(texEmissive, vTexOut).rgb;
		#else
			totReflectClr += mtr_emis;
		#endif
	#endif
	
	// combine color & lit & diff tex
	finalClr.rgb *= totReflectClr;
	
	// light map
	#ifdef HAS_DIFFUSE2MAP
		finalClr.rgb *= texture2D(texDiff2, vTexOut2).rgb;
	#endif
	
	#ifdef HAS_FOG
		HFLOAT camDist = length(camPos - vPosWorld.xyz);
		HFLOAT fogP = clamp((fogParam.y - camDist) / fogParam.x,0.0, 1.0);
		finalClr.rgb = mix(fogColor, finalClr.rgb, fogP);
	#endif
	
	gl_FragColor = finalClr;
}




