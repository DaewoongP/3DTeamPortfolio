#include "Shader_EngineHeader.hlsli"
#include "Shader_RenderFunc.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_TargetTexture;
// ���� �κ� �����ϴ� Luma �������� ���
float3 g_vLuminancekey = float3(0.2126f, 0.7152f, 0.0722f);
float g_fGamma = 2.2f;

// bloom
texture2D g_WhiteTexture;
texture2D g_WhiteBlurTexture;

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

PS_OUT PS_MAIN_WHITE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vTargetColor = g_TargetTexture.Sample(PointSampler_Clamp, In.vTexUV);
    if (0.f == vTargetColor.a)
        discard;

    Out.vColor = float4(vTargetColor.rgb, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vHDR = g_TargetTexture.Sample(PointSampler_Clamp, In.vTexUV);
    vector vWhite = g_WhiteTexture.Sample(PointSampler_Clamp, In.vTexUV);
    vector vWhiteBlured = g_WhiteBlurTexture.Sample(PointSampler_Clamp, In.vTexUV);
    
    vector vBloom = pow(pow(abs(vWhiteBlured), g_fGamma) + pow(abs(vWhite), g_fGamma), 1.f / g_fGamma);

    vHDR = pow(abs(vHDR), g_fGamma);
    vBloom = pow(abs(vBloom), g_fGamma);
    
    vHDR += vBloom;
    
    Out.vColor = pow(abs(vHDR), 1 / g_fGamma);

    return Out;
}

technique11 DefaultTechnique
{
    pass WhiteSpace
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_WHITE();
    }
    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }
}