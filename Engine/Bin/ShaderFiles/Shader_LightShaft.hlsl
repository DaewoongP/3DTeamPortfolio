#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_TargetTexture;
float4 g_vLightPosition;

#define SAMPLING 100

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
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vLightViewPos = mul(g_vLightPosition, g_ViewMatrix);
    vector vLightProjPos = mul(g_vLightPosition, g_ProjMatrix);
    vLightProjPos /= vLightProjPos.w;
    
    float2 screenSpaceLightPos = float2(vLightProjPos.x * 0.5f + 0.5f, vLightProjPos.y * -0.5f + 0.5f);
    float2 deltaTexCoord = In.vTexUV - screenSpaceLightPos;
    float sampleLength = 0.01;
    float4 color = float4(0.f, 0.f, 0.f, 0.f);

    float illuminationDecay = 0.1f;
    float4 shaftColor = float4(0.98, 0.98, 1.0, 1.0);
    float illumination = 0.1f;

    for (int i = 0; i < SAMPLING; ++i)
    {
        float2 texCoord = screenSpaceLightPos + deltaTexCoord * sampleLength * i;
        float4 texColor = g_TargetTexture.Sample(LinearSampler, texCoord);
        texColor *= illumination * illuminationDecay * shaftColor;
        color += texColor;
        illumination *= 0.99;
        illuminationDecay *= 0.95;
    }
    
    Out.vColor = color;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass LightShaft
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