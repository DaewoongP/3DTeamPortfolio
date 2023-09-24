#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float4 g_vCamPosition;
float    g_fFocusDis;
float    g_fFocusRange;
float    gMaxBlur;
float    g_fCamFar;

float gOcclusionRadius = 0.5f;
float gOcclusionFadeStart = 0.2f;
float gOcclusionFadeEnd = 2.0f;
float gSurfaceEpsilon = 0.05f;

texture2D g_PostProcessingTexture;
float g_fRadius = 0.001f;
float4 vDofParams;

float2 vMaxCoC = float2(5.f, 10.f);
float radiusScale = 0.4f;



texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_SSAOTexture;
texture2D g_BlurTexture;
texture2D g_NoiseTexture;




float3 g_Ran[29] =
{
    float3(0.2024537f, 0.841204f, -0.9060241f),
	float3(-0.221324f, 0.324325f, -0.8234234f),
	float3(0.8724724f, 0.8547973f, -0.43252611f),
	float3(0.2698734f, 0.5684943f, -0.12515022f),
	float3(0.26482924f, 0.236820f, 0.72384287f),
	float3(0.20348342f, 0.234832f, 0.23682923f),
	float3(-0.0012315f, 0.8234823f, 0.23483244f),
	float3(-0.2342863f, 0.234982f, -0.00001524f),
	float3(-0.3426888f, 0.780742f, -0.8349823f),
	float3(-0.5234832f, 0.8291234f, 0.23941929f),
	float3(0.90889192f, 0.8123121f, -0.12812992f),
	float3(0.4520239f, 0.1201011f, -0.82943914f),
    float3(0.2024539f, -0.7101201f, 0.29143293f),
    float3(0.2362823f, -0.9797897f, -0.29143293f),
    float3(0.2024539f, 0.7101201f, 0.1234346f),
    float3(0.5161231f, 0.646465f, 0.31231654f),
    float3(0.2024539f, 0.1231343f, -0.29143293f),
    float3(-0.5494946f, 0.2222347f, 0.1234987f),
    float3(0.37897872f, 0.7101201f, 0.3164949f),
    float3(0.7895136f, -0.9797312f, 0.1553549f),
    float3(0.1234657f, -0.7418931f, 0.13124329f),
    float3(-0.2176539f, 0.3197985f, -0.35278933f),
    float3(0.3317687f, 0.1319749f, 0.973527893f),
    float3(0.9317687f, -0.659756f, 0.293523293f),
    float3(-0.0176315f, 0.8297823f, 0.235283244f),
    float3(-0.3176888f, 0.789742f, -0.83529823f),
    float3(0.2017637f, 0.841974f, -0.90352241f),
    float3(0.2017639f, 0.7109701f, 0.12352346f),
    float3(0.7817636f, -0.9797312f, 0.13523549f),
};

float BlurWeights[31] =
{
    0.0011, 0.0123, 0.0234, 0.0561, 0.0864, 0.1353, 0.2312, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231, 0.9986, 0.9999,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278,0.2312, 0.1353, 0.0864, 0.0561, 0234, 0.0123, 0.0011
};
float total = 11.9827f;


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



float computeDepthBlur(float depth /*in view space*/)
{
    float f;
    if (depth < vDofParams.y)
    { // scale depth value between near blur distance and focal distance to ]-1,0]range
        f = (depth - vDofParams.y) / (vDofParams.y - vDofParams.x);
    }
    else
    {
//scale depth value betweenfocal dsiatnce and far blur distance to [0,1]range
        f = (depth - vDofParams.y) / (vDofParams.z - vDofParams.y);
//clamp the far blur to a maximum blurriness 
        f = clamp(f, 0, vDofParams.w);
    }
    return f * 0.5f + 0.f;
}

//PS_OUT PS_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
//    vector vNormalDesc = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
   
//    float2 NewUv = In.vTexUV;
   
//    float FocusDistance = 5.f;
//   // length(g_vCamPosition - In.vPosition);
   
//    float DOF = saturate(abs(FocusDistance - g_fFocusDis) / g_fFocusRange);
   
//    float blurAmount = lerp(0, gMaxBlur, DOF);
   
//        for (int i = -3; i <= 3; i++)
//    {
//        for (int j = -3; j <= 3; j++)
//        {
//            float2 offset = float2(i, j) *2.f; // 블러 강도 조절
//            float2 offsetTexCoord = NewUv + offset * blurAmount;
//            Out.vColor += g_PostProcessingTexture.Sample(LinearSampler, offsetTexCoord);
//        }
//    }
   
//    Out.vColor /= 49.f;
//    vector vDepth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    
//    float fViewZ = vDepth.y * g_fCamFar;
  
//    return Out;
//}
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //float discRadius, discRadiusLow, centerDepth;
    
    //vector vOutSet = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    //centerDepth = vOutSet.a;
    //discRadius = abs(vOutSet.a * vMaxCoC.y - vMaxCoC.x);
    //discRadiusLow = discRadius * radiusScale;
    //float fViewZ = vOutSet.y * g_fCamFar;

    //for (int i = 0; i < 8;++i)
    //{
    //    float2 coordlow = In.vTexUV + (1 / 1280.f * i * 0.01 * discRadiusLow);
    //    float2 coordHigh = In.vTexUV + (1 / 720.f * i * 0.01 * discRadius);
       
    //    float4 tapLow = g_PostProcessingTexture.Sample(LinearSampler, coordlow);
    //    float4 tapHigh = g_PostProcessingTexture.Sample(LinearSampler, coordHigh);
        
    //    float tapblur = abs(tapHigh.a * 2.f - 1.f);
    //    float4 tap = lerp(tapHigh, tapLow, tapblur);
        
    //    tap.a = (tap.a >= centerDepth) ? 1.f : abs(tap.aa * 2.f - 1.f);
    //    Out.vColor.rgb += tap.rgb * tap.a;
    //    Out.vColor.a += tap.a;
        
    //}
    vector vPost = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    Out.vColor = dot(vPost.rgb, float3(1.f, 1.f, 1.f));
    
    if(Out.vColor.a<1.f)
        Out.vColor *= 0.1f;
    
    return Out;
}

PS_OUT PS_MAIN_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    float dx = 1.0f / 1280.f;
    float dy = 1.f / (720.f / 2.f);
    float2 UV = 0;
    float Color = g_SSAOTexture.Sample(BlurSampler, In.vTexUV).x;
    
    {
        for (int i = -15; i < 15; ++i)
        {

            UV = In.vTexUV + float2(dx * i,0);
            vector SSAO = g_SSAOTexture.Sample(BlurSampler, UV);
        
            Out.vColor += BlurWeights[15 + i] * SSAO;
        }
    }
        Out.vColor /= total;
     
    return Out;
 }

    PS_OUT PS_MAIN_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
 
    float dy = 1.0f / (720.f / 2.f);
    
    float2 UV = 0;
   
    for (int i = -15; i < 15; ++i)
    {
        UV = In.vTexUV + float2(0,dy * i);
        vector SSAO = g_SSAOTexture.Sample(BlurSampler, UV);
        Out.vColor += BlurWeights[15 + i] * SSAO;
    }
    Out.vColor /= total;
    
    
    
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
