#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;
texture2D		g_ClipTexture;
float			g_fClipThreshold = 0.33f;
int				g_iClipChannel;

vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);

/// For.TextureSheetAnimation //
unsigned int g_iWidthLength = 1;
unsigned int g_iHeightLength = 1;
bool g_isUseNormalTexture = false;
texture2D g_NormalTexture;
////////////////////////////////

struct VS_IN
{
	/* �׸��� ���� �������� */
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	/* �ν��Ͻ����� (�����ϳ��� �����ϱ����� ���) */
	float4			vRight : TEXCOORD1;
	float4			vUp : TEXCOORD2;
	float4			vLook : TEXCOORD3;
	float4			vTranslation : TEXCOORD4;
	float4			vColor : COLOR0;
	unsigned int	iCurrentIndex : TEXCOORD5;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vColor : COLOR0;
};

/* ������ �ް� ��ȯ�ϰ� ������ �����Ѵ�. */
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
	Out.vTexUV = In.vTexUV;
	Out.vColor = In.vColor;

	return Out;
}

VS_OUT VS_TS_MAIN(VS_IN In)
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
	SplitUV(In.vTexUV, g_iWidthLength, g_iHeightLength, In.iCurrentIndex, Out.vTexUV);
	Out.vColor = In.vColor;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vColor : COLOR0;
};

struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

/* �ȼ��� �ް� �ȼ��� ���� �����Ͽ� �����Ѵ�. */
PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	vector vClipTexture = g_ClipTexture.Sample(LinearSampler, In.vTexUV);

	if (0 == g_iClipChannel)
		Out.vColor.a = vClipTexture.r;
	else if (1 == g_iClipChannel)
		Out.vColor.a = vClipTexture.g;
	else if (2 == g_iClipChannel)
		Out.vColor.a = vClipTexture.b;
	else if (3 == g_iClipChannel)
		Out.vColor.a = vClipTexture.a;

	Out.vColor *= (In.vColor);

	if (Out.vColor.a < g_fClipThreshold)
		discard;

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
		GeometryShader = NULL;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass TextureSheetAnimation
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}