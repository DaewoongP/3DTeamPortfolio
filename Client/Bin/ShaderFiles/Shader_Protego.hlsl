#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

// Matrix
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// Texture
texture2D g_Circle_LayerMask_Texture;
texture2D g_Noise04_Texture;
texture2D g_RibbonOffset_Texture;
texture2D g_Inky_Smoke_Texture;
texture2D g_Wisps_2_Texture;
texture2D g_Normal_Texture;
texture2D g_Collision_Texture;

// RawValue
vector g_vColor1 = vector(1.f, 1.f, 1.f, 1.f);
vector g_vColor2 = vector(1.f, 1.f, 1.f, 1.f);
vector g_vCamPos;
float g_fCamFar;
float g_fTime;
float g_fRimPower;
float g_fScale;
float g_fHitTimeAcc = 0.f;
float3 g_vSphereWorldPos;
float3 g_vCollisionPoint = float3(0.f, 0.f, 0.f);
float3 g_fProtegoCenterPos;
float PI = 3.14159265;
bool g_isHitEffect = false;
unsigned int g_iWidthLength;
unsigned int g_iHeightLength;
unsigned int g_iCurIndex;


struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float3 vWorldNormal : TEXCOORD2;
	float2 vFlipbookUV : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix).xyz;

	// Use only the 3x3 part of the world matrix to transform the normal
	float3x3 matWorld3x3 = (float3x3)g_WorldMatrix; // Assuming your HLSL version supports matrix3x3. If not, you can extract the top-left 3x3 manually.
	Out.vWorldNormal = mul(In.vNormal, matWorld3x3);

	Out.vFlipbookUV = In.vTexUV;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float3 vWorldNormal : TEXCOORD2;
	float2 vFlipbookUV : TEXCOORD3;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vCircleMask = (vector)0;
	vector vWisps2 = (vector)0;
	vector vNoise04 = (vector)0;

	float2 vOffsetWispUV = (float2)0;
	float2 vOffsetNoiseUV1 = (float2)0;
	float2 vOffsetNoiseUV2 = (float2)0;
	float2 vOffsetNoiseUV = (float2)0;

	float3 vViewDir = (float3)0;

	float fAlpha = 0.f;
	float fFresnel = 0.f;
	float fDistance = 0.f;

	if (true == g_isHitEffect)
	{
		// 사각형의 방향과 크기 정의
		float3 vRelativePos = In.vWorldPos - g_vCollisionPoint; // 충돌 지점으로부터의 상대적인 위치
		float3 vDirection = normalize(g_vCollisionPoint - g_vSphereWorldPos);
		float2 fSize = float2(g_fScale * 0.2f, g_fScale * 0.2f);
		float3 vUp = float3(0.f, 1.f, 0.f);
		float3 vDirectionX, vDirectionZ;
		vDirectionX = normalize(cross(vUp, vDirection));  // 정규화 추가
		vDirectionZ = normalize(cross(vDirection, vDirectionX));  // 크로스 프로덕트 순서 변경 및 정규화 추가
		float fDistanceOnX = dot(vRelativePos, vDirectionX);
		float fDistanceOnZ = dot(vRelativePos, vDirectionZ);

		if (abs(fDistanceOnX) < fSize.x && abs(fDistanceOnZ) < fSize.y
			&& distance(In.vWorldPos, g_vCollisionPoint) <= 1.5f)
		{
			In.vFlipbookUV.x = 0.5 + (fDistanceOnX / (2.0 * fSize.x));
			In.vFlipbookUV.y = 0.5 + (fDistanceOnZ / (2.0 * fSize.y));
			SplitUV(In.vFlipbookUV, g_iWidthLength, g_iHeightLength, g_iCurIndex, In.vFlipbookUV);

			Out.vColor = g_Collision_Texture.Sample(LinearSampler, In.vFlipbookUV);
			Out.vColor.a = Out.vColor.r;
			Out.vColor.rgb = g_vColor1.rgb;
			return Out;
		}
	}

	// 노이즈 생성
	TilingAndOffset_float(In.vTexUV, 1.f, g_fTime * 0.15f, vOffsetNoiseUV1); // 우상으로 이동
	TilingAndOffset_float(In.vTexUV, 1.f, -g_fTime * 0.15f, vOffsetNoiseUV2); // 좌상으로 이동
	vOffsetNoiseUV = vOffsetNoiseUV1 * vOffsetNoiseUV2; // 노이즈 섞기
	vNoise04 = g_Noise04_Texture.Sample(LinearSampler, vOffsetNoiseUV); // 노이즈로 샘플링

	// 색상 섞기.
	Out.vColor = lerp(g_vColor1, g_vColor2, vNoise04.r);

	// 외곽선 따기
	vViewDir = normalize(g_vCamPos.xyz - In.vWorldPos);
	FresnelEffect_float(In.vWorldNormal, vViewDir, g_fRimPower, fFresnel);
	Out.vColor.a = fFresnel;

	if (Out.vColor.a <= 0.1f)
		discard;

	// 일렁임 효과
	TilingAndOffset_float(In.vTexUV, 1.f, vOffsetNoiseUV, vOffsetWispUV);
	vWisps2 = g_Wisps_2_Texture.Sample(PointSampler, vOffsetWispUV);
	Out.vColor.a *= vWisps2.r;

	return Out;
}

technique11 DefaultTechnique
{
	pass Protego
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Alpha, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}