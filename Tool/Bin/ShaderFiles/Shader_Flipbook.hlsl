#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

int g_iCurIndex;
int g_iWidthLength;
int g_iHeightLength;
int g_iClipChannel;

bool g_isUseNormalTexture = false;
bool g_isMixColor = false;

vector g_vColor;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	// ex)
	// g_iCurIndex = 6, 
	// g_iWidthLength = 4, g_iHeightLenght = 5

	// ���� ���
	int iCurWidth = g_iCurIndex % g_iWidthLength;		// 6 % 4 = 2
	int iCurHeight = int(g_iCurIndex / g_iWidthLength); // 6 / 4 = int(1)
	float fUnitWidth = 1.f / g_iWidthLength;			// 1.f / 4 = 0.25f
	float fUnitHeight = 1.f / g_iHeightLength;			// 1.f / 5 = 0.2f;
	float2 vStartUV;

	// ��������Ʈ�� ���� UV ��ǥ ���
	vStartUV.x = iCurWidth * fUnitWidth;	// 2 * 0.25f = 0.5f
	vStartUV.y = iCurHeight * fUnitHeight;	// 1 * 0.2f = 0.2f

	// �ش� ��������Ʈ �������� UV ���
	Out.vTexUV.x = In.vTexUV.x * fUnitWidth + vStartUV.x;	// x * 0.25f + 0.5f
	Out.vTexUV.y = In.vTexUV.y * fUnitHeight + vStartUV.y;	// y * 0.2f + 0.2f

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* �ȼ��� �ް� �ȼ��� ���� �����Ͽ� �����Ѵ�. */
PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuseTexture = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.rgb = vDiffuseTexture.rgb;
	if (true == g_isMixColor)
	{
		Out.vColor *= g_vColor;
	}

	if(0 == g_iClipChannel) Out.vColor.a = vDiffuseTexture.r;
	else if(1 == g_iClipChannel) Out.vColor.a = vDiffuseTexture.g;
	else if(2 == g_iClipChannel) Out.vColor.a = vDiffuseTexture.b;
	else if(3 == g_iClipChannel) Out.vColor.a = vDiffuseTexture.a;
	
	return Out;
}

struct PS_POST_BLEND_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
};

/* �ȼ��� �ް� �ȼ��� ���� �����Ͽ� �����Ѵ�. */
PS_POST_BLEND_OUT	PS_POST_BLEND(PS_IN In)
{
	PS_POST_BLEND_OUT		Out = (PS_POST_BLEND_OUT)0;

	vector		vDiffuseTexture = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector      vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
	vector      vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vDiffuse = vDiffuseTexture;
	if (true == g_isMixColor)
	{
		Out.vDiffuse *= g_vColor;
	}

	if (0 == g_iClipChannel) Out.vDiffuse.a = vDiffuseTexture.r;
	else if (1 == g_iClipChannel) Out.vDiffuse.a = vDiffuseTexture.g;
	else if (2 == g_iClipChannel) Out.vDiffuse.a = vDiffuseTexture.b;
	else if (3 == g_iClipChannel) Out.vDiffuse.a = vDiffuseTexture.a;

	Out.vNormal = vNormal;

	return Out;
}
technique11		DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass PostBlend
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_POST_BLEND();
	}
}