#include "Shader_EngineHeader.hlsli"
#include "Shader_RenderFunc.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// PostProcessing
texture2D g_HDRTexture;
texture2D g_GlowTexture;


// HDR
texture2D g_DeferredTexture;
texture2D g_SkyTexture;
texture2D g_DOFTexture;
float g_fHDRPower;

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

    vector vHDR = g_HDRTexture.Sample(LinearSampler, In.vTexUV);
    vector vGlow = g_GlowTexture.Sample(LinearSampler, In.vTexUV);
    
    Out.vColor = vHDR + vGlow;

    return Out;
}

PS_OUT PS_MAIN_HDR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDeferredTexture = g_DeferredTexture.Sample(LinearSampler, In.vTexUV);
    vector vSkyTexture = g_SkyTexture.Sample(LinearSampler, In.vTexUV);
    vector vDOF = g_DOFTexture.Sample(LinearSampler, In.vTexUV);
    
    if (0.f == vDeferredTexture.a)
        vDeferredTexture = vSkyTexture;
    else
    {
        vDeferredTexture.rgb += ACESToneMapping(vDeferredTexture.rgb) * g_fHDRPower;
        vDOF.rgb += ACESToneMapping(vDOF.rgb) * g_fHDRPower;

        vDeferredTexture.rgb = (vDOF.rgb * vDOF.a) + (vDeferredTexture.rgb * (1.f - vDOF.a));
    }
    
    Out.vColor = vDeferredTexture;

    return Out;
}

technique11 DefaultTechnique
{
    pass HDR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_HDR();
    }
    pass PostProcessing
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