#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fFlowPower;
float g_FrameTime;

texture2D g_FlowTexture1;
texture2D g_FlowTexture2;

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

    //vector FlowDir = g_FlowTexture1.Sample(LinearSampler, In.vTexUV) * 2.f - 1.f;
    //FlowDir *= g_fFlowPower;
    
    //float phase = frac(_Time[])
    
    
    

    float2 flowVector = g_FlowTexture1.Sample(LinearSampler, In.vTexUV).xy;
    float2 UVa = In.vTexUV + (float2(flowVector.x,  flowVector.y) - 0.3f) * g_fFlowPower * sin(g_FrameTime*0.5f+0.5f);
    float2 UVb = In.vTexUV + (float2(flowVector.x, flowVector.y) - 0.3f) * g_fFlowPower * cos(g_FrameTime*0.5f);
    float FlowLerp = abs(0.5f - (g_fFlowPower * sin(g_FrameTime * 0.5f)) / 0.5f);
    
    
    vector tex0 = g_FlowTexture2.Sample(LinearSampler, UVa);
    vector tex1 = g_FlowTexture2.Sample(LinearSampler,UVb);
    
    
    float3 finalColor = lerp(tex0, tex1, FlowLerp);
    
    Out.vColor = float4(finalColor, 1.f) * g_FlowTexture2.Sample(LinearSampler, In.vTexUV);
    Out.vColor.rgb *= Out.vColor.a;
    //Out.vColor.rgb = lerp(g_FlowTexture2.Sample(LinearSampler, UVa).rgb, g_FlowTexture2.Sample(LinearSampler, UVb).rgb,FlowLerp);
    
    return Out;
}

PS_OUT PS_MAIN_FLOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
  
    
    if (g_fFlowPower <= 0.001)
    {
        Out.vColor = 0.f;
        return Out;
    }
    Out.vColor = saturate(g_fFlowPower);
    
    return Out;
}



technique11 DefaultTechnique
{
   
    pass Flow
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

    pass Lightning
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_FLOW();
    }

  
}