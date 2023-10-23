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
float g_fAmplitude = 1.f;
float g_fFrequency = 1.f;
unsigned int g_iNumOctaves = 1;
float g_fScrollSpeed = 0.f;
float g_fPersistence = 0.5f; // [0, 1] 0에 가까울 수록 부드럽다, 1에 가까울수록 거칠다.
//float fOctavesScale = 0.5f;
//float fOctavesMultiplier = 0.5f;
float2 g_vRemap = { -1.f, 1.f };
float3 g_vPositionAmount = { 1.f, 1.f, 1.f };
float3 g_vSizeAmount = { 0.f, 0.f, 0.f };
////////////////

// For. Emission
bool g_isEmission = { false };
float g_fEmissionFrequency = { 1.f };
float2 g_vEmissionRemap = { 1.f, 1.f };
float3 g_vEmissionColor = { 1.f, 1.f, 1.f };
int g_iEmissionChannel = { 0 };
texture2D g_EmissionTexture;
//////

struct VS_IN
{
	/* 그리기 위한 정점정보 */
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	/* 인스턴싱정보 (도형하나를 제어하기위한 행렬) */
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

/* 정점을 받고 변환하고 정점을 리턴한다. */
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

	// 노이즈모듈
	if (true == g_isNoiseActivated)
	{
		// Velocity의 w는 age값으로 사용중임.
		float fNoiseValue = FractalBrownianMotion(float3(In.vVelocity.x, In.vVelocity.y, In.vVelocity.w), 1, 0.5f, 1.f, 1.f);
		float fRemapValue = 0.f;

		// Remap
		float fNormalized = (fNoiseValue + 1.0f) * 0.5f; // [0, 1]
		fRemapValue = g_vRemap.x + fNormalized * (g_vRemap.y - g_vRemap.x);

		// 기존의 축들을 정규화
		float3 normalizedRight = normalize(In.vRight.xyz);
		float3 normalizedUp = normalize(In.vUp.xyz);
		float3 normalizedLook = normalize(In.vLook.xyz);

		// 원래의 스케일값 계산
		float3 originalScale = float3(length(In.vRight), length(In.vUp), length(In.vLook));

		// 노이즈 기반 스케일 값을 계산
		float3 vNoiseScale = float3(
			1.0f + fRemapValue * g_vSizeAmount.x,
			1.0f + fRemapValue * g_vSizeAmount.y,
			1.0f + fRemapValue * g_vSizeAmount.z);

		// 최종 스케일 값: 원래의 스케일 x 노이즈 스케일
		float3 finalScale = originalScale * vNoiseScale;

		// 스케일 값으로 각 축을 조정
		normalizedRight *= finalScale.x;
		normalizedUp *= finalScale.y;
		normalizedLook *= finalScale.z;

		float3 vNoiseTranslation = float3(
			fRemapValue * g_vPositionAmount.x,
			fRemapValue * g_vPositionAmount.y,
			fRemapValue * g_vPositionAmount.z);

		In.vTranslation += float4(vNoiseTranslation, 0.f);

		// 변환 행렬을 다시 구성
		TransformMatrix = matrix(
			float4(normalizedRight, 0.f),
			float4(normalizedUp, 0.f),
			float4(normalizedLook, 0.f),
			In.vTranslation);
	}

	vector			vPosition;
	vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	SplitUV(In.vTexUV, g_iWidthLength, g_iHeightLength, In.iCurrentIndex, Out.vTexUV);
	Out.vColor = In.vColor;

	return Out;
}

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
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
	//// r는 색상 피킹용도
	//Out.vColor *= (vGradientColor);
	//// g는 세기용도
	//Out.vColor *= 1 / (1 - vNormalColor.g);
	//// b는 모름

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
	//VS_NoiseModule(In, Out);



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

	if (true == g_isNoiseActivated)
	{
		// Velocity의 w는 age값으로 사용중임.
		float fNoiseValueX = FractalBrownianMotion(float3(In.vTranslation.x, In.vTranslation.y, In.vVelocity.w), g_iNumOctaves, g_fPersistence, g_fFrequency, g_fAmplitude);
		float fNoiseValueY = FractalBrownianMotion(float3(In.vTranslation.y, In.vTranslation.z, In.vVelocity.w), g_iNumOctaves, g_fPersistence, g_fFrequency, g_fAmplitude);
		float fNoiseValueZ = FractalBrownianMotion(float3(In.vTranslation.z, In.vTranslation.x, In.vVelocity.w), g_iNumOctaves, g_fPersistence, g_fFrequency, g_fAmplitude);

		// Remap
		float fRemapValueX = 0.f;
		float fRemapValueY = 0.f;
		float fRemapValueZ = 0.f;
		float fNormalizedX = (fNoiseValueX + 1.0f) * 0.5f; // [0, 1]
		float fNormalizedY = (fNoiseValueY + 1.0f) * 0.5f; // [0, 1]
		float fNormalizedZ = (fNoiseValueZ + 1.0f) * 0.5f; // [0, 1]
		fRemapValueX = g_vRemap.x + fNormalizedX * (g_vRemap.y - g_vRemap.x);
		fRemapValueY = g_vRemap.x + fNormalizedY * (g_vRemap.y - g_vRemap.x);
		fRemapValueZ = g_vRemap.x + fNormalizedZ * (g_vRemap.y - g_vRemap.x);
		//fRemapValueX = fNoiseValueX;
		//fRemapValueY = fNoiseValueY;
		//fRemapValueZ = fNoiseValueZ;
		// 기존의 축들을 정규화
		float3 normalizedRight = normalize(In.vRight.xyz);
		float3 normalizedUp = normalize(In.vUp.xyz);
		float3 normalizedLook = normalize(In.vLook.xyz);

		// 원래의 스케일값 계산
		float3 originalScale = float3(length(In.vRight), length(In.vUp), length(In.vLook));

		// 노이즈 기반 스케일 값을 계산
		float3 vNoiseScale = float3(
			1.0f + fRemapValueX * g_vSizeAmount.x,
			1.0f + fRemapValueX * g_vSizeAmount.y,
			1.0f + fRemapValueX * g_vSizeAmount.z);

		// 최종 스케일 값: 원래의 스케일 x 노이즈 스케일
		float3 finalScale = originalScale * vNoiseScale;

		// 스케일 값으로 각 축을 조정
		normalizedRight *= finalScale.x;
		normalizedUp *= finalScale.y;
		normalizedLook *= finalScale.z;

		float3 vNoiseTranslation = float3(
			fRemapValueX * g_vPositionAmount.x,
			fRemapValueY * g_vPositionAmount.y,
			fRemapValueZ * g_vPositionAmount.z);

		In.vTranslation += float4(vNoiseTranslation, 0.f);

		// 변환 행렬을 다시 구성
		TransformMatrix = matrix(
			float4(normalizedRight, 0.f),
			float4(normalizedUp, 0.f),
			float4(normalizedLook, 0.f),
			In.vTranslation);
	}

	vector			vPosition;
	vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	matrix		matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	Out.vPosition = mul(vPosition, matWVP);

	Out.vColor = In.vColor;
	Out.vVelocity = In.vVelocity;

	// 텍스처 시트
	if (true == g_isTextureSheetAnimationActivated)
	{
		SplitUV(In.vTexUV, g_iWidthLength, g_iHeightLength, In.iCurrentIndex, Out.vTexUV);
	}
	else
	{
		Out.vTexUV = In.vTexUV;
	}

	// UV 애니메이션
	TilingAndOffset_float(Out.vTexUV, g_vTililing, g_vOffset, Out.vTexUV);
}

void VS_NoiseModule(VS_IN In, inout VS_OUT Out)
{
	if (false == g_isNoiseActivated)
		return;

	// 노이즈 스크롤 적용
	//float3 vNoiseInput = In.vPosition + float3(0, g_fNoiseTimeAcc * g_fScrollSpeed, 0);
	//float fNoiseValue = PerlinNoise_3D(vNoiseInput.x, vNoiseInput.y, vNoiseInput.z
	//	, g_fFrequency, g_fPersistence, fOctavesScale, fOctavesMultiplier, g_iOctaves);

	// 노이즈 강도 적용
	//float3 vDisplacedPosition = In.vPosition + fNoiseValue * g_fStrength;
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
	// r는 색상 피킹용도
	Out.vColor *= (vGradientColor);
	// g는 세기용도
	Out.vColor *= 1 / (1 - vNormalColor.g);
	// b는 모름

	if (Out.vColor.a < g_fClipThreshold)
	{
		discard;
	}

	if (g_isEmission) 
	{
		float fEmissionValue = 0.f, fRemapValue = 0.f;
		vector vEmission = g_EmissionTexture.Sample(LinearSampler, In.vTexUV);
		float fSineTime = sin(g_fEmissionFrequency * In.vVelocity.w);
		
		if (0 == g_iEmissionChannel)
			fEmissionValue = vEmission.r;
		else if (1 == g_iEmissionChannel)
			fEmissionValue = vEmission.g;
		else if (2 == g_iEmissionChannel)
			fEmissionValue = vEmission.b;
		else if (3 == g_iEmissionChannel)
			fEmissionValue = vEmission.a;
		// out1 + (val - in1) * (out2 - out1) / (in2 - in1);

		Remap_float(fSineTime, float2(-1.f, 1.f), g_vEmissionRemap, fRemapValue);
		Out.vColor += fRemapValue * fEmissionValue * float4(g_vEmissionColor, 0.f);
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


