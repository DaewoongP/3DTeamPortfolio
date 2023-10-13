#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_TargetTexture;

float g_fWinSizeX, g_fWinSizeY;

uint g_iSampleCnt;

uint g_iWeights = 19; // 개수 통일
float g_fBlurWeights[19] =
{
    0.0561f, 0.1353f, 0.278f,
    0.3221f, 0.4868f, 0.5881f, 
    0.6661f, 0.7261f, 0.9231f,
    0.999f, 
    0.9231f, 0.7261f, 0.6661f,
    0.5881f, 0.4868f, 0.3221f,
    0.278f, 0.1353f, 0.0561f, 
};

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

PS_OUT PS_MAIN_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fDeltaX = 1.0f / g_fWinSizeX * g_iSampleCnt;
    float2 vNewUV = float2(0, 0);
    float fTotal = 0.f;
    
    int iValue = (g_iWeights - 1) / 2;

    for (int i = -iValue; i < iValue; ++i)
    {
        vNewUV = In.vTexUV + float2(fDeltaX * i, 0.f);
        vector vTargetColor = g_TargetTexture.Sample(LinearSampler, vNewUV);
        if (0.f != vTargetColor.a)
            Out.vColor += g_fBlurWeights[iValue + i] * vTargetColor;
        fTotal += g_fBlurWeights[iValue + i];
    }

    Out.vColor /= fTotal;
    
    return Out;
}

PS_OUT PS_MAIN_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fDeltaY = 1.f / g_fWinSizeY * g_iSampleCnt;
    float2 vNewUV = float2(0, 0);
    float fTotal = 0.f;
   
    int iValue = (g_iWeights - 1) / 2;

    for (int i = -iValue; i < iValue; ++i)
    {
        vNewUV = In.vTexUV + float2(0, fDeltaY * i);
        vector vTargetColor = g_TargetTexture.Sample(LinearSampler, vNewUV);
        if (0.f != vTargetColor.a)
            Out.vColor += g_fBlurWeights[iValue + i] * vTargetColor;
        fTotal += g_fBlurWeights[iValue + i];
    }

    Out.vColor /= fTotal;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLURX();
    }
    pass BlurY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLURY();
    }
}