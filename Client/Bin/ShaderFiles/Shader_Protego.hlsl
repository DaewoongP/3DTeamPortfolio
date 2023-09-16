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

// RawValue
vector g_vColor1 = vector(1.f, 1.f, 1.f, 1.f);
vector g_vColor2 = vector(1.f, 1.f, 1.f, 1.f);
vector g_vCamPos;
float g_fCamFar;
float g_fTime;
float g_fRimPower;
float3 g_vSphereWorldPos;

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
};

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vWorldNormal = mul(vector(In.vNormal, 1.f), g_WorldMatrix);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
    float3 vWorldNormal : TEXCOORD2;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
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
    // test
    //fDistance = g_vCamPos - In.vWorldPos;
    //vViewDir = normalize(fDistance);
    //FresnelEffect_float(In.vWorldNormal, vViewDir, g_fRimPower, fAlpha);
    //Out.vColor = g_vColor1;
    //Out.vColor.a = fAlpha;
    //return Out;
    
    // 노이즈 생성
    TilingAndOffset_float(In.vTexUV, 1.f, g_fTime * 0.15f, vOffsetNoiseUV1);
    TilingAndOffset_float(In.vTexUV, 1.f, -g_fTime * 0.15f, vOffsetNoiseUV2);
    vOffsetNoiseUV = vOffsetNoiseUV1 * vOffsetNoiseUV2;
    vNoise04 = g_Noise04_Texture.Sample(LinearSampler, vOffsetNoiseUV);

    // 색상 섞기.
    Out.vColor = lerp(g_vColor1, g_vColor2, vNoise04.r);

    // 외곽선 따기
    vViewDir = normalize(g_vCamPos - In.vWorldPos);
    FresnelEffect_float(In.vWorldNormal, vViewDir, g_fRimPower, fFresnel);
    Out.vColor.a = fFresnel;

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
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}