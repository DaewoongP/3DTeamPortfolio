#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;


texture2D g_DoBlurTexture;
texture2D g_WhiteBloomTexture;
texture2D g_GlowTexture;
texture2D g_OriTexture;
texture2D g_PostProcessingTexture;
texture2D g_SpecularTexture;

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

struct VS_OUT_POSTEX
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};


struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    
    //Out.texCoords1 = In.vTexUV;
    Out.texCoords1 = (In.vTexUV * g_Scales.x);
    Out.texCoords1.y = Out.texCoords1.y + (g_FrameTime * g_ScrollSpeed.x);
    
    Out.texCoords2 = (In.vTexUV * g_Scales.y);
    Out.texCoords2.y = Out.texCoords2.y + (g_FrameTime * g_ScrollSpeed.y);
    
    Out.texCoords3 = (In.vTexUV * g_Scales.z);
    Out.texCoords3.y = Out.texCoords3.y + (g_FrameTime * g_ScrollSpeed.z);
    
    
    
    return Out;
}
VS_OUT_POSTEX VS_MAIN_POSTEX(VS_IN In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX) 0;

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
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};
struct PS_IN_POSTEX
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};



PS_OUT PS_MAIN_BLURX(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    float dx = 1.0f / (1280.f / 4.f);
    
    float2 UV = 0;
    
    for (int i = -11; i < 11; ++i)
    {

        UV = In.vTexUV + float2(dx * i, 0.f);
        vector SSAO = g_DoBlurTexture.Sample(BlurSampler, UV);
        
        Out.vColor += BlurWeights[11 + i] * SSAO;
    }
    Out.vColor /= total;
    return Out;
}
PS_OUT PS_MAIN_BLURY(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
    
 
    float dy = 1.0f / (720.f / 2.f);
    
    float2 UV = 0;
   
    for (int i = -11; i < 11; ++i)
    {
        UV = In.vTexUV + float2(0, dy * i);
        vector SSAO = g_DoBlurTexture.Sample(BlurSampler, UV);
        Out.vColor += BlurWeights[11 + i] * SSAO;
    }
    Out.vColor /= total;
    
    
    
    return Out;
}


PS_OUT PS_MAIN_BLOOM(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;

    
    vector vBloom = g_OriTexture.Sample(BloomSampler, In.vTexUV); //vBloom은 하얀부분을뽑아낼 텍스쳐
    vector vSpecular = g_SpecularTexture.Sample(BloomSampler, In.vTexUV);
    vBloom = vBloom * vSpecular;
    
    float Brigtness = dot(vBloom.rgb, float3(0.2126f, 0.7152f, 0.0722f));
   
    if (Brigtness > 0.99f)
    {
        Out.vColor = vector(vBloom.rgb, 1.f) * 2;
    }
    else
        discard;

    if(Out.vColor.a <= 0.1f)
        discard;

    return Out;
}

PS_OUT PS_MAIN_BLOOM_AFTER(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vWhiteBloom = g_WhiteBloomTexture.Sample(BloomSampler, In.vTexUV); //하얀부분을뽑아낼 텍스쳐
    vector vBloomColor = g_DoBlurTexture.Sample(BloomSampler, In.vTexUV); //하얀부분을 블러처리
    vector vBloomOriTex = g_OriTexture.Sample(BloomSampler, In.vTexUV); //최초의 이미지
    
    vector vBloom = pow(pow(abs(vBloomColor), 2.2f) + pow(abs(vBloomOriTex), 2.2f), 1.f / 2.2f);
    
    float4 Value = vWhiteBloom;
    Value = pow(abs(Value), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);
    
    Value += vBloom;
    
    Out.vColor = pow(abs(Value), 1 / 2.2f);
    return Out;
    
}

PS_OUT PS_MAIN_GLOW(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vTextureColor = g_OriTexture.Sample(LinearSampler, In.vTexUV);
    vector vPixelColor = g_GlowTexture.Sample(LinearSampler, In.vTexUV);
    
    
   // Out.vColor = saturate(vTextureColor)
    return Out;
    
}
technique11 DefaultTechnique
{
  

    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_POSTEX();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }
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

    pass FinBloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_POSTEX();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_AFTER();
    }
    pass Glow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_POSTEX();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW();
    }


}