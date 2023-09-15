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
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
vector g_vCamPos;
float g_fCamFar;
float g_fTime;

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

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;

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

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_vColor;
    
    vector vCircleMask = (vector)0;
    vector vWisps2 = (vector)0;
    vector vNoise04 = (vector)0;
    float2 vOffsetWispUV = (float2)0;
    float2 vOffsetNoiseUV = (float2)0;
    vCircleMask = g_Circle_LayerMask_Texture.Sample(LinearSampler, In.vTexUV);

    TilingAndOffset_float(In.vTexUV, 1.f, float2(0.f, g_fTime), vOffsetNoiseUV);
    TilingAndOffset_float(In.vTexUV, 1.f, vOffsetNoiseUV, vOffsetWispUV);

    vNoise04 = g_Noise04_Texture.Sample(LinearSampler, vOffsetNoiseUV);
    vWisps2 = g_Wisps_2_Texture.Sample(PointSampler, vOffsetWispUV);

    Out.vColor.a = vCircleMask.r;

    if (vCircleMask.r == 0.f && vCircleMask.g == 0.f && vCircleMask.b == 0.f)
        discard;

    // 가운데 구멍뚫기
    if (Out.vColor.a >= 0.5f)
        discard;

    Out.vColor.a = 1.f - Out.vColor.a;

    // 외곽 일렁거림
    Out.vColor.a *= vWisps2.r;

    return Out;
}

technique11 DefaultTechnique
{
    pass Protego
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}