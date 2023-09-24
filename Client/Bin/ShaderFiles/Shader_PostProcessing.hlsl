#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_PostProcessingTexture;
texture2D g_BloomTexture;
texture2D g_GlowTexture;
texture2D g_DOFTexture;

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

   vector vPost = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    vector Bloom = g_BloomTexture.Sample(LinearSampler, In.vTexUV);
    vector DOF = g_DOFTexture.Sample(LinearSampler, In.vTexUV);
   
    //if(DOF.x>0.f)
    //{
    //    Out.vColor.xyz = (vPost.xyz * DOF.xyz) + Bloom;
    //}
    //else
    //{
    //    Out.vColor = vPost+Bloom;
    //}
     Out.vColor = vPost+Bloom ;
    return Out;
}

technique11 DefaultTechnique
{
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