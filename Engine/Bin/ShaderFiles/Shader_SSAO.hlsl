#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float g_fCamFar;

float gOcclusionRadius = 0.5f;
float gOcclusionFadeStart = 0.2f;
float gOcclusionFadeEnd = 2.0f;
float gSurfaceEpsilon = 0.05f;

texture2D g_PostProcessingTexture;
//SSAO
float g_fRadius = 0.001f;
float g_fFalloff = 0.000002f;
float g_fStrength = 0.0007f;
float g_fTotStrength = 1.38f;
float g_fInSamples = 1.f / 16.f;


texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_SSAOTexture;
texture2D g_BlurTexture;
texture2D g_NoiseTexture;

//ray�� ������鼭 ���������Ǵ��ϱ����� rand���� �ʿ��� ���̴�������ҵ�

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

float3 RandNormal(float2 vTexUV)
{
    float x = frac(sin(dot(vTexUV, float2(15.8989f, 76.132f) * 1.f)) * 46336.2364f);
    float y = frac(sin(dot(vTexUV, float2(11.5234f, 62.128f) * 2.f)) * 24636.2412f);
    float z = frac(sin(dot(vTexUV, float2(13.2726f, 63.723f) * 3.f)) * 56372.3263f);
    
    return normalize(float3(x, y, z));
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    
    vNormalDesc = normalize(vNormalDesc * 2.f - 1.f);
    // ���� z��
    float fViewZ = vDepthDesc.y * g_fCamFar;

    float3 vRay;
    float3 vReflect;
    float2 vRandomUV;
    float fOccNorm;
    
    int iColor = 0;
    for (int i = 0; i < 29; i++)
    {
        vRay = normalize(reflect(RandNormal(In.vTexUV), g_Ran[i]));
        vReflect = normalize(reflect(normalize(vRay), normalize(vNormalDesc.rgb))) * g_fRadius;
        vReflect.x *= -1.f;
        vRandomUV = In.vTexUV + vReflect.xy;
        fOccNorm = g_DepthTexture.Sample(LinearSampler, vRandomUV).y * g_fCamFar;
        if (fOccNorm <= fViewZ + 0.1f)
            ++iColor;
    }
    
    float4 vAmbient = abs((iColor / 29.f) - 1);
   
    Out.vColor = 1.f - vAmbient;
   
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
    pass SSAO
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
