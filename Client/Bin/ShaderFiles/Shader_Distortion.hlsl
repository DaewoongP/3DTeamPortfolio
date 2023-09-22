#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;

texture2D g_AlphaTexture;
texture2D g_OriTexture;
texture2D g_NoiseTexture;

float g_FrameTime;
float3 g_ScrollSpeed;
float3 g_Scales;

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
//tex값자체흔들기 
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

//노이즈텍스쳐가져와서흔들기

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

    noise1.xy = noise1.xy * float2(0.1f, 0.2f);// float2값들을 글로벌로 받아와서 던져주는걸로 표현가능
    noise2.xy = noise2.xy * float2(0.1f, 0.3f);
    noise3.xy = noise3.xy * float2(0.1f, 0.1f);
    
    vector FinalNoise = noise1 + noise2 + noise3;
    float perturb = ((1.f - In.vTexUV.y) * 0.8f) + 0.5f;
    
    float2 newUV;
    newUV.xy = (FinalNoise.xy * perturb) + In.vTexUV.xy;
    
    vector vOrigin = g_OriTexture.Sample(DistortionSampler, newUV.xy);
    vector vAlpha = g_AlphaTexture.Sample(DistortionSampler, newUV.xy);
    
    Out.vColor = vOrigin;
    
  //  Out.vColor *= vAlpha;//알파텍스쳐합성
    
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



}