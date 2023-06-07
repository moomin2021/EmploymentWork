#include "Object3D.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	// 法線にワールド行列によるスケーリング・回転を適用
    float4 wnormal = normalize(mul(world, float4(normal, 0)));
    float4 wpos = mul(world, pos);
    
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svPos = mul(mul(viewproj, world), pos);
    output.worldPos = wpos;
    output.normal = wnormal.xyz;
    output.color = color;
    output.uv = uv;
    
    return output;
}