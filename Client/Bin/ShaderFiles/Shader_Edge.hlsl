#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DepthTexture;

float2 g_vViewPort;

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
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

    float2 uv = In.vTexUV;
    float fWidth = 1.f;

    int i = 0;
    // right
    if (0.1f < g_DepthTexture.Sample(LinearSampler, uv + float2(fWidth / g_vViewPort.x, 0.f)).r)
    {
        //Out.vColor = float4(1.f, 0.f, 1.f, 1.f);
        ++i;
    }
    // left
    if (0.1f < g_DepthTexture.Sample(LinearSampler, uv + float2(-fWidth / g_vViewPort.x, 0.f)).r)
    {
        //Out.vColor = float4(1.f, 0.f, 1.f, 1.f);
        ++i;
    }
    // down
    if (0.1f < g_DepthTexture.Sample(LinearSampler, uv + float2(0.f, fWidth / g_vViewPort.y)).r)
    {
        //Out.vColor = float4(1.f, 0.f, 1.f, 1.f);
        ++i;
    }
    // up 
    if (0.1f < g_DepthTexture.Sample(LinearSampler, uv + float2(0.f, -fWidth / g_vViewPort.y)).r)
    {
        ++i;
    }
    
    if (i > 0 && i < 4)
        Out.vColor = float4(1.f, 0.f, 1.f, 1.f);
    else
        Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Edge
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