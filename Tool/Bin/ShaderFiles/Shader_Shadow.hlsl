#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

matrix g_vLightViewMatrix;
matrix g_vLightProjMatrix;

float g_fCamFar;

texture2D g_DepthTexture;
texture2D g_vLightDepthTexture;

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

PS_OUT PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vPosition;
    
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

    vPosition = mul(vPosition, g_ViewMatrixInv);

    vPosition = mul(vPosition, g_vLightViewMatrix);

    vector vLightProjPos = mul(vPosition, g_vLightProjMatrix);
    // xyz : -1~1, -1~1, 0~1
    vLightProjPos = vLightProjPos / vLightProjPos.w;
    
    float2 vLightUV;
    vLightUV.x = vLightProjPos.x * 0.5f + 0.5f;
    vLightUV.y = vLightProjPos.y * -0.5f + 0.5f;

    vector vLightDepth = g_vLightDepthTexture.Sample(LinearSampler, vLightUV);

    if (-1.f > vLightProjPos.x ||
		1.f < vLightProjPos.x ||
		-1.f > vLightProjPos.y ||
		1.f < vLightProjPos.y ||
        0.f > vLightProjPos.z ||
		1.f < vLightProjPos.z)
    {
        Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    }
    else if (vPosition.z - 0.1f < vLightDepth.x * g_fCamFar)
    {
        Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    }
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

}