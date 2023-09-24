#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;

texture2D g_SSAOTexture;
texture2D g_BlurTexture;
texture2D g_PostProcessingTexture;
texture2D g_NoiseTexture;
texture2D g_AlphaTexture;
texture2D g_DoBlurTexture;
texture2D g_WhiteBloomTexture;
texture2D g_GlowTexture;
texture2D g_OriTexture;

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

//PS_OUT PS_MAIN_DISTORTION(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    // uv trans
//    float2 Trans = In.vTexUV + float2(0.001f, 0.001f); // 테스트 값으로 0.001f를 추가

//    // Noise 텍스처 샘플링
//    float4 Noise = g_vDistortionTexture.Sample(LinearSampler, Trans);

//    // 오리지날 씬의 UV를 흔들어 주기 위한 계산
//    float2 UV = In.vTexUV + Noise.xy * 0.35f; // 0.05 이 값 부분이 커질수록 UV 편차가 더욱 심해집니다.

//    // Orig 텍스처 샘플링
//    float4 Orig = g_PostProcessingTexture.Sample(LinearSampler, UV);

//    Out.vColor = Orig;

//    return Out;
//}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    
    
    vector noise1 = g_NoiseTexture.Sample(LinearSampler, In.texCoords1);
    vector noise2 = g_NoiseTexture.Sample(LinearSampler, In.texCoords2);
    vector noise3 = g_NoiseTexture.Sample(LinearSampler, In.texCoords3);
   // Out.vColor = noise1;
    //return Out;
    
    noise1.x = (noise1.x - 0.5f) * 2.0f;
    noise2.x = (noise2.x - 0.5f) * 2.0f;
    noise3.x = (noise3.x - 0.5f) * 2.0f;

    noise1.xy = noise1.xy * float2(0.1f, 0.2f);
    noise2.xy = noise2.xy * float2(0.1f, 0.3f);
    noise3.xy = noise3.xy * float2(0.1f, 0.1f);
    
    vector FinalNoise = noise1 + noise2 + noise3;
    float perturb = ((1.f - In.vTexUV.y) * 0.8f) + 0.5f;
    
    float2 newUV;
    newUV.xy = (FinalNoise.xy * perturb) + In.vTexUV.xy;
    
    vector vPost = g_PostProcessingTexture.Sample(DistortionSampler, newUV.xy);
    vector vAlpha = g_AlphaTexture.Sample(DistortionSampler, newUV.xy);
    
    Out.vColor = vPost;
    
    Out.vColor *= vAlpha;
    
    return Out;
}
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

    
    vector vBloom = g_PostProcessingTexture.Sample(BloomSampler, In.vTexUV); //vBloom은 하얀부분을뽑아낼 텍스쳐
   
    float BrightColor = 0.f;
    float Brigtness = dot(vBloom.rgb, float3(0.2126f, 0.7152f, 0.0722f));
   
    if (Brigtness > 0.99f)
    {
        BrightColor = vector(vBloom.rgb, 1.f);
        Out.vColor = BrightColor;
    }
    else
        discard;
    
    
    
    return Out;
}

PS_OUT PS_MAIN_BLOOM_AFTER(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vWhiteBloom = g_WhiteBloomTexture.Sample(BloomSampler, In.vTexUV); //하얀부분을뽑아낼 텍스쳐
    vector vBloomColor = g_DoBlurTexture.Sample(BloomSampler, In.vTexUV); //하얀부분을 블러처리
    vector vBloomOriTex = g_PostProcessingTexture.Sample(BloomSampler, In.vTexUV); //최초의 이미지
    
    vector vBloom = pow(pow(abs(vBloomColor), 2.2f) + pow(abs(vBloomOriTex), 2.2f), 1.f / 2.2f);
    
    float4 Value = vWhiteBloom;
    Value = pow(abs(Value), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);
    
    Value += vBloom;
    
    Out.vColor = pow(abs(Value), 1 / 2.2f);
    return Out;
    
}
PS_OUT PS_MAIN_RADIALBLUR(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    vector vRadialTex = g_PostProcessingTexture.Sample(PointSampler, In.vTexUV); 
    
    float2 CenterUV = (0.5f, 0.5f);
    float Raidus = 0.02f;
  
        float4 blurredColor = float4(0.0, 0.0, 0.0, 0.0); // 초기화
    
    // 라디얼 블러를 위한 루프
        for (int i = 0; i < 360; i++)
        {
            float angle = radians(i);
        float2 offset = float2(cos(angle), sin(angle)) * Raidus;
        
        // 현재 픽셀 주변의 텍스처 샘플 좌표 계산
        float2 sampleCoord = CenterUV + offset;
        
        // 텍스처에서 샘플
            float4 sampleColor = vRadialTex;
        
        // 샘플된 색상을 블러 결과에 누적
            blurredColor += sampleColor;
        }
    
    // 블러된 색상을 평균화
        blurredColor /= 360.0;
    
    Out.vColor = blurredColor;
    
        return Out;
    
    vector vPost = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    Out.vColor = dot(vPost.rgb, float3(0.98f, 0.89f, 0.89f));
    
}

PS_OUT PS_MAIN_GRAY(PS_IN_POSTEX In)
{
    PS_OUT Out = (PS_OUT) 0;
  
    vector vPost = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    Out.vColor = dot(vPost.rgb, float3(0.98f, 0.89f, 0.89f));
    
    return Out;
    
}

technique11 DefaultTechnique
{
   
    pass Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

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

    pass RadialBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_POSTEX();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
    }
    pass PS_MAIN_GRAY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_POSTEX();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_GRAY();
    }

}