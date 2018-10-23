attribute VEC4 vPos;
attribute VEC2 vTex;
attribute VEC2 vTex2;

uniform MAT4 matWVP;
uniform VEC3 eye_pos;

uniform VEC4 water_offset;
uniform VEC4 water_rot;
uniform VEC4 water_rot2;

uniform MAT4 reflect_wvp;

varying VEC4 out_hpos;
varying VEC4 out_hpos_reflect;
varying VEC4 out_tex0;
varying VEC2 out_fade_tex;
varying VEC3 out_view_dir;

void main()
{
	out_hpos = matWVP * vPos;
	gl_Position = out_hpos;
	
//-----------------------------计算滚动纹理坐标
	VEC2 tex;
	tex.x = dot(water_rot.xy, vTex);
	tex.y = dot(water_rot.zw, vTex);
	out_tex0.xy = tex.xy + water_offset.xy;
	
	tex.x = dot(water_rot2.xy, vTex);
	tex.y = dot(water_rot2.zw, vTex);
	out_tex0.zw = tex.xy + water_offset.zw;

//-----------------------------计算观察向量
	out_view_dir = eye_pos - vPos.xyz;
	
//-----------------------------计算反射齐次坐标
	out_hpos_reflect = reflect_wvp * vPos;
}