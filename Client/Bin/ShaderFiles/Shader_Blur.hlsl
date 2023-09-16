#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;

float3 g_Ran[13] =
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
    float3(0.2024539f, -0.7101201f, 0.29143293f)
};

float BlurWeights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};

float total = 11.4776f;

sampler BlurSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
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
    
    float dx = 1.0f / (1280.f / 4.f);
    
    float2 UV = 0;
    
    for (int i = -11; i < 11; ++i)
    {

        UV = In.vTexUV + float2(dx * i, 0.f);
        vector SSAO = g_Texture.Sample(BlurSampler, UV);
        
        Out.vColor += BlurWeights[11 + i] * SSAO;
    }

    Out.vColor /= total;
    
    return Out;
}

PS_OUT PS_MAIN_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float dy = 1.0f / (720.f / 2.f);
    
    float2 UV = 0;
   
    for (int i = -11; i < 11; ++i)
    {
        UV = In.vTexUV + float2(0, dy * i);
        vector SSAO = g_Texture.Sample(BlurSampler, UV);
        Out.vColor += BlurWeights[11 + i] * SSAO;
    }
    Out.vColor /= total;

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