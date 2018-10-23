attribute VEC4 vPos;

uniform MAT4 matWVP;
uniform MAT4 matWV;

varying VEC4 vTexOutProj;
varying VEC3 rayView;

void main()
{
	VEC4 vProjPos = matWVP * vPos;
	
	// 像素坐标往坐上偏移0.5像素，相当于uv往右下偏移0.5
    // u = (xclip + 1.0) * 0.5 + 0.5;
    // v = (yclip + 1.0) * 0.5 + 0.5;
    // 也就是：
    // u = (xproj / wproj + 1.0) * 0.5 + 0.5
    // v = (yproj / wproj + 1.0) * 0.5 + 0.5
    // 变成齐次：
    // u = (xproj + wproj) * 0.5 + 0.5 * wproj
    // v = (wproj + yproj) * 0.5 + 0.5 * wproj
    // w = wproj
    // 所以uvproj就是：
    // uvproj = float4(0.5 * (vec2(p.x+p.w, p.y + p.w) + p.w), p.zw)
	vTexOutProj = vec4(0.5 * vec2(vProjPos.x + vProjPos.w, vProjPos.y + vProjPos.w) + vProjPos.w, vProjPos.zw);
	
	rayView = (matWV * vPos).xyz;
	
	gl_Position = vProjPos;
}