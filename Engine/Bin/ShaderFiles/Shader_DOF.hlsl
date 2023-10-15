#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

int g_iTest;


float4  g_vCamPosition;
float   g_fCamFar;

float   g_fFocusDistance;
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
    
    if (g_fFocusDistance - g_fFocusRange > fViewZ)
    {
        Out.vColor = vTarget;
        Out.vColor.a = saturate(1.f - (fViewZ / (g_fFocusDistance - g_fFocusRange)));
    }
    else if (g_fFocusDistance + g_fFocusRange < fViewZ)
    {
        float fMaxAtt = 30.f;
        float fMaxDistance = g_fFocusDistance + g_fFocusRange + fMaxAtt;
        Out.vColor = vTarget;
        // f + R ~ 100.f(특정 Z) 까지를 0~1로 정규화
        // ViewZ - (F+R) / (100.f - (f+R)) // 100.f 는 CamFar가 정석인거같은데, "사실상 보이는 위치" 로 설정하는게 맞는듯.
        Out.vColor.a = saturate((fViewZ - (g_fFocusDistance + g_fFocusRange)) / (fMaxDistance - (g_fFocusDistance + g_fFocusRange)));
    }
    else
        Out.vColor = float4(0.f, 0.f, 0.f, 0.f);

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