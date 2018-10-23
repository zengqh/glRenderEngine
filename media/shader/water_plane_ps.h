//***************************************************tex
uniform MSAMPLER2D normal_map_tex;
uniform MSAMPLER2D refract_tex;
uniform MSAMPLER2D reflect_tex;
uniform MSAMPLER2D fade_tex;

//***************************************************param
uniform VEC3 water_color;
uniform VEC4 water_info;
uniform VEC3 light_dir;
uniform VEC3 light_color;
uniform VEC3 fog_color;

uniform FLOAT specular_power;
uniform VEC2 fresnel_info;

//***************************************************input
varying VEC4 out_hpos;
varying VEC4 out_hpos_reflect;
varying VEC4 out_tex0;
varying VEC2 out_fade_tex;
varying VEC3 out_view_dir;

/////////////////////////////////////////////////////////////
void main()
{
//-----------------------------计算屏幕坐标
	VEC2 texcoord = (out_hpos.xy/out_hpos.w) * 0.5 + vec2(0.5, 0.5);
	
//-----------------------------计算法线和扰动
	VEC3 normal = texture2D(normal_map_tex, out_tex0.xy).rgb + texture2D(normal_map_tex, out_tex0.zw).rgb;
	normal = normal * 2.0 - vec3(2.0, 2.0, 2.0);
	normal = normalize(normal).xzy;
	VEC3 disort = 0.05 * normal;
	
//-----------------------------计算折射
	VEC2 refract_texcoord = texcoord;// + disort.xz * water_info.y;
	FLOAT fade = water_info.w;
	
	VEC4 refract_color = texture2D(refract_tex, refract_texcoord);
	VEC4 color = mix(vec4(water_color, 1.0), refract_color, fade);
	
//-----------------------------计算反射所需信息
	VEC3 view_dir;
	view_dir = normalize(out_view_dir);
	FLOAT facing = 1.0 - dot(view_dir, normal*vec3(0.1, 1.0, 0.1));
	FLOAT fresnel = clamp(pow(facing, fresnel_info.x) * fresnel_info.y, 0.0, 1.0);
	
//-----------------------------计算反射
	VEC2 reflect_texcoord = (out_hpos_reflect.xy / out_hpos_reflect.w) * 0.5 + vec2(0.5, 0.5) + disort.xz * water_info.z;
	VEC4 reflect_color = texture2D(reflect_tex, reflect_texcoord);
	color = mix(color, reflect_color, fresnel);
	
//-----------------------------计算反光
	VEC3 half_dir = normalize(light_dir + view_dir);
	VEC3 specular = pow(max(dot(half_dir, normal), 0), specular_power) * light_color;
	color.xyz += specular * fresnel;
	
	gl_FragColor = color;
}