#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
	float3 vCurrentPos : POSITION;
	float3 vPrevPos : PREV_POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float3 vVelocity : VELOCITY; // 3D 움직임 벡터
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	// 클립 좌표계로 변환
	Out.vPosition = mul(float4(In.vCurrentPos, 1.0), matWVP);

	// 방향 계산
	Out.vVelocity = normalize(In.vCurrentPos-In.vPrevPos);

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vVelocity : VELOCITY; // 3D 움직임 벡터
};

struct PS_OUT
{
	float4 vColor : SV_TARGET;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	return Out;
}

technique11		DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}