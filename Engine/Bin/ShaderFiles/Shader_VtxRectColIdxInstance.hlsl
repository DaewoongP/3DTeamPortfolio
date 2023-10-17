#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;
texture2D		g_ClipTexture;
texture2D		g_GradientTexture;
texture2D		g_NoiseTexture;
float			g_fTime;
float			g_fClipThreshold = 0.33f;
float			g_fCamFar;
int				g_iClipChannel;

vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);

/// For.TextureSheetAnimation //
bool g_isTextureSheetAnimationActivated = false;
bool g_isUseNormalTexture = false;
bool g_isUseGradientTexture = false;
unsigned int g_iWidthLength = 1;
unsigned int g_iHeightLength = 1;
texture2D g_NormalTexture;
////////////////////////////////

// For. Noise //
bool g_isNoiseActivated = false;
unsigned int g_iOctaves = 1;
float g_fPersistence;
float g_fFrequency = 0.5f;
float g_fStrength = 1.f;
float g_fScrollSpeed = 0.f;
float g_fNoiseTimeAcc = 0.f;
float fOctavesScale = 0.5f;
float fOctavesMultiplier = 0.5f;
float fPositionAmount = 0.0f;
float fRotationAmount = 0.0f;
float fSizeAmount = 0.0f;
////////////////

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
	float4			vColor : TEXCOORD5;
	float4			vVelocity : TEXCOORD6;
	unsigned int	iCurrentIndex : TEXCOORD7;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vColor : COLOR0;
	float4		vVelocity : VELOCITY;
};

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vColor : COLOR0;
	float4		vVelocity : VELOCITY;
};

struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

struct PS_NONBLEND_OUT
{
	float4	vColor : SV_TARGET0;
	float4	vNormal : SV_TARGET1;
};

void VS_Module(VS_IN In, inout VS_OUT Out);
void PS_Module(PS_IN In, inout PS_OUT Out);

void VS_MainModule(VS_IN In, inout VS_OUT Out);
void VS_TextureSheetAnimationModule(VS_IN In, inout VS_OUT Out);
void VS_NoiseModule(VS_IN In, inout VS_OUT Out);

void PS_MainModule(PS_IN In, inout PS_OUT Out);
void PS_TextureSheetAnimationModule(PS_IN In, inout PS_OUT Out);
void PS_NoiseModule(PS_IN In, inout PS_OUT Out);

/* ������ �ް� ��ȯ�ϰ� ������ �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	VS_Module(In, Out);
	
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

/* �ȼ��� �ް� �ȼ��� ���� �����Ͽ� �����Ѵ�. */
PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	PS_Module(In, Out);

	return Out;
}
PS_OUT PS_MOTION_BLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	//PS_Module(In, Out);
	Out.vColor = In.vVelocity;
	//Remap_float4(normalize(In.vVelocity), float2(-1.f, 1.f), float2(0.f, 1.f), Out.vColor);
	return Out;
}


PS_NONBLEND_OUT PS_NONBLEND(PS_IN In)
{
	PS_NONBLEND_OUT Out = (PS_NONBLEND_OUT)0;

	vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);
	vector vClipTexture = g_ClipTexture.Sample(LinearSampler, In.vTexUV);

	if (0 == g_iClipChannel)
		Out.vColor.a = vClipTexture.r;
	else if (1 == g_iClipChannel)
		Out.vColor.a = vClipTexture.g;
	else if (2 == g_iClipChannel)
		Out.vColor.a = vClipTexture.b;
	else if (3 == g_iClipChannel)
		Out.vColor.a = vClipTexture.a;

	if (Out.vColor.a < g_fClipThreshold)
	{
		discard;
	}

	Out.vColor = vDiffuse;
	Out.vColor.rgb *= In.vColor.rgb;
	if (true == g_isUseNormalTexture)
	{
		vector vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
		Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	}
	
	return Out;
}

PS_OUT PS_SMOKE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	//float4 vNormalColor = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	//float2 vGradientUV = float2(vNormalColor.r, 0);
	//float4 vGradientColor = g_GradientTexture.Sample(LinearSampler, vGradientUV);
	//Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	//
	//float4 vNoiseValue = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
	//float2 vOffsetNoise;



	//TilingAndOffset_float(In.vTexUV, 1.f, g_fTime * 0.15f, vOffsetNoise);
	//vector vClipTexture = g_ClipTexture.Sample(LinearSampler, vOffsetNoise);

	//if (0 == g_iClipChannel)
	//	Out.vColor.a = vClipTexture.r;
	//else if (1 == g_iClipChannel)
	//	Out.vColor.a = vClipTexture.g;
	//else if (2 == g_iClipChannel)
	//	Out.vColor.a = vClipTexture.b;
	//else if (3 == g_iClipChannel)
	//	Out.vColor.a = vClipTexture.a;


	//Out.vColor.a *= vWisps2.r;

	//Out.vColor *= (In.vColor);
	//// r�� ���� ��ŷ�뵵
	//Out.vColor *= (vGradientColor);
	//// g�� ����뵵
	//Out.vColor *= 1 / (1 - vNormalColor.g);
	//// b�� ��

	//if (Out.vColor.a < g_fClipThreshold)
	//{
	//	discard;
	//}

	return Out;
}

technique11		DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Alpha, 0);
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
		SetDepthStencilState(DSS_Alpha, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass NonBlend
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_NONBLEND();
	}
	pass MotionBlur
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Alpha, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MOTION_BLUR();
	}
    pass Default_Depth_Disable
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

	pass Smoke
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Depth_Disable, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_SMOKE();
	}
}







void VS_Module(VS_IN In, inout VS_OUT Out)
{
	VS_MainModule(In, Out);
	VS_TextureSheetAnimationModule(In, Out);
	//VS_NoiseModule(In, Out);

	TilingAndOffset_float(Out.vTexUV, g_vTililing, g_vOffset, Out.vTexUV);

}
void PS_Module(PS_IN In, inout PS_OUT Out)
{
	PS_MainModule(In, Out);
	//PS_TextureSheetAnimationModule(In, Out);
	//PS_NoiseModule(In, Out);
}

void VS_MainModule(VS_IN In, inout VS_OUT Out)
{
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
	Out.vVelocity = In.vVelocity;
}
void VS_TextureSheetAnimationModule(VS_IN In, inout VS_OUT Out)
{
	if (false == g_isTextureSheetAnimationActivated)
		return;

	SplitUV(In.vTexUV, g_iWidthLength, g_iHeightLength, In.iCurrentIndex, Out.vTexUV);
}
void VS_NoiseModule(VS_IN In, inout VS_OUT Out)
{
	if (false == g_isNoiseActivated)
		return;

	// ������ ��ũ�� ����
	float3 vNoiseInput = In.vPosition + float3(0, g_fNoiseTimeAcc * g_fScrollSpeed, 0);
	float fNoiseValue = PerlinNoise_3D(vNoiseInput.x, vNoiseInput.y, vNoiseInput.z
		, g_fFrequency, g_fPersistence, fOctavesScale, fOctavesMultiplier, g_iOctaves);

	// ������ ���� ����
	float3 vDisplacedPosition = In.vPosition + fNoiseValue * g_fStrength;
}

void PS_MainModule(PS_IN In, inout PS_OUT Out)
{
	float4 vNormalColor = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float2 vGradientUV = float2(vNormalColor.r, 0);
	float4 vGradientColor = g_GradientTexture.Sample(LinearSampler, vGradientUV);

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
	// r�� ���� ��ŷ�뵵
	Out.vColor *= (vGradientColor);
	// g�� ����뵵
	Out.vColor *= 1 / (1 - vNormalColor.g);
	// b�� ��

	if (Out.vColor.a < g_fClipThreshold)
	{
		discard;
	}
}
void PS_TextureSheetAnimationModule(PS_IN In, inout PS_OUT Out)
{
	if (false == g_isTextureSheetAnimationActivated)
		return;
}
void PS_NoiseModule(PS_IN In, inout PS_OUT Out)
{
	if (false == g_isNoiseActivated)
		return;
}


