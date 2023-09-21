#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;
float g_fGlowPower;


texture2D g_SSAOTexture;
texture2D g_BlurTexture;
texture2D g_PostProcessingTexture;
texture2D g_vDistortionTexture;
texture2D g_AlphaTexture;
texture2D g_DoBlurTexture;
texture2D g_WhiteBloomTexture;
texture2D g_GlowTexture;
texture2D g_OriTexture;
texture2D g_NoiseTexture;


vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

float g_FrameTime;
float3 g_ScrollSpeed;
float3 g_Scales;

float BlurWeights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};
float total = 11.4776f;

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

    vector GlowColor = g_GlowTexture.Sample(LinearSampler, In.vTexUV);
    vector vAlpha = g_AlphaTexture.Sample(DistortionSampler, In.vTexUV);

    
    if ((GlowColor.r == 0.f) && (GlowColor.g == 0.f) && (GlowColor.b == 0.f))
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    }
    else
    {
        Out.vColor = GlowColor;
    }
    
    return Out;
}

PS_OUT PS_MAIN_GLOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector GlowColor = g_GlowTexture.Sample(LinearSampler, In.vTexUV);
     
  
    vector vAlpha = g_AlphaTexture.Sample(DistortionSampler, In.vTexUV);
    
    if(g_fGlowPower<=0.001)
    {
        Out.vColor = 0.f;
        return Out;
    }
        Out.vColor = saturate(GlowColor + (vAlpha * g_fGlowPower));
    
        return Out;
 }

PS_OUT PS_MAIN_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float dx = 1.0f / 1280.f;
    float dy = 1.f / (720.f / 2.f);
    int Count = 0;
    float2 UV = 0;
  //  for (int j = -5; j < 5;++j)
    {
        for (int i = -5; i < 5; ++i)
        {
            UV = In.vTexUV + float2(dx * i, 0.f);
            vector SSAO = g_DoBlurTexture.Sample(BlurSampler, UV);
        
            Out.vColor += BlurWeights[5 + i] * SSAO;
            Count += 1;
        }
    }
   //if(Count>0)
        Out.vColor /= total;
    //else
    //    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    
        return Out;
    }


technique11 DefaultTechnique
{
   
    pass Glow
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
        PixelShader = compile ps_5_0 PS_MAIN_GLOW();
    }
    pass Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }
  
}