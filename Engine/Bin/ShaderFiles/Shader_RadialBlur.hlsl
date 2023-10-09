#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_TargetTexture;

float g_fBlurStart = 0.9f; // 확대 or 축소 // 화면비율과 연동해서 그리는 위치 결정해야함.
float g_fBlurWidth = 0.1f; // 블러하는 양
int g_iNumSample = 10;

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
    VS_OUT Out = (VS_OUT) 0;

    float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
    float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);
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
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vTex = g_TargetTexture.Sample(LinearSampler, In.vTexUV);

    float2 vBlurCenterUV = float2(0.5f, 0.5f);
    float2 uv = In.vTexUV;
    uv -= vBlurCenterUV;
    
    float fPrecompute = g_fBlurWidth * (1.0 / float(g_iNumSample - 1));
    
    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    
    // 카운트값을 코드로 제한해야 컴파일이 원활함.
    int iterationCount = min(g_iNumSample, 10);
    for (int i = 0; i < iterationCount; ++i)
    {
        float fScale = g_fBlurStart + (float(i) * fPrecompute);
        color += g_TargetTexture.Sample(LinearSampler, uv * fScale + vBlurCenterUV);
    }

    color /= float(g_iNumSample);
    color.a = 1.f;
    
    Out.vColor = color;
   
    return Out;
}

technique11 DefaultTechnique
{
    pass Radial
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}