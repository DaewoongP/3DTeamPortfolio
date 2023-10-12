#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

int g_iTest;


float4  g_vCamPosition;
float   g_fCamFar;

float   g_fFocusDis;
float   g_fFocusRange;

texture2D g_TargetTexture;
texture2D g_DepthTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vPosition.xy = sign(Out.vPosition.xy);

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
    
    vector vDepth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    vector vTarget = g_TargetTexture.Sample(LinearSampler, In.vTexUV);
    
    float fViewZ = vDepth.y * g_fCamFar;
    
    if (g_fFocusDis - g_fFocusRange > fViewZ)
    {
        Out.vColor = vTarget;
        Out.vColor.a = saturate(1.f - (fViewZ / (g_fFocusDis - g_fFocusRange)) * 1.5f);
    }
    else if (g_fFocusDis + g_fFocusRange < fViewZ)
    {
        Out.vColor = vTarget * 0.7f;
        Out.vColor.a = saturate((fViewZ - g_fFocusDis + g_fFocusRange) / g_fCamFar * 25);
    }
    else
        discard;

    return Out;
}

technique11 DefaultTechnique
{
    pass DOF
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}