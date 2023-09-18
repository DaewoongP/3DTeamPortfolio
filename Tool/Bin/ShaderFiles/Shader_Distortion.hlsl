#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;

texture2D g_SSAOTexture;
texture2D g_BlurTexture;
texture2D g_PostProcessingTexture;
texture2D g_vDistortionTexture;
texture2D g_vAlphaTexture;
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

matrix MyMatrixLookAtLH(float4 vEye, float4 vAt)
{
    matrix ViewMatrix = matrix(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f);
    
    vector vLook = float4(normalize(vAt.xyz - vEye.xyz), 0.f);
    vector vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f);
    vector vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f);
    
    ViewMatrix = matrix(vRight, vUp, vLook, float4(0.f, 0.f, 0.f, 1.f));
    matrix TransposeViewMatrix = transpose(ViewMatrix);
    
    vector vPosition = float4(
    -1.f * dot(vEye, vRight),
    -1.f * dot(vEye, vUp),
    -1.f * dot(vEye, vLook),
    1.f);
    
    TransposeViewMatrix._41 = vPosition.x;
    TransposeViewMatrix._42 = vPosition.y;
    TransposeViewMatrix._43 = vPosition.z;
    TransposeViewMatrix._44 = vPosition.w;

    return TransposeViewMatrix;
}

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
    
    vector vPost = g_PostProcessingTexture.Sample(DistortionSampler, newUV.xy);
    vector vAlpha = g_vAlphaTexture.Sample(DistortionSampler, newUV.xy);
    
    Out.vColor = vPost;
    
    Out.vColor *= vAlpha;//알파텍스쳐합성
    
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