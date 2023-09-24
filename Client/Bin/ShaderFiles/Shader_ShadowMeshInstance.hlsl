#include "Shader_Client_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float		g_fCamFar;

struct VS_IN
{
	/* �׸��� ���� �������� */
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	/* �ν��Ͻ����� (�����ϳ��� �����ϱ����� ���) */
	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
	matrix			TransformMatrix;
	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	
	vector			vPosition;
	vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vProjPos : TEXCOORD0;
};

struct PS_OUT
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // �������� �佺���̽� z�� ������
    Out.vLightDepth.r = In.vProjPos.w / g_fCamFar;
    Out.vLightDepth.a = 1.f; // ����Ÿ�ٿ� ������ Ȯ���� ����.
	
	return Out;
}

technique11		DefaultTechnique
{
    pass Shadow
    {
		SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}