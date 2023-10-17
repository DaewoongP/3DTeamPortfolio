#include "Shader_Client_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fTime;

#define TAU 6.28318530718
#define MAX_ITER 5

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

    float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
    float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);
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
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fTime = g_fTime * 0.5f + 23.f;

    float2 vNewUV = fmod(In.vTexUV * TAU * 10.f, TAU) - 250.f;

    float2 vValue = vNewUV;
    float fPower = 1.f;
    float inten = 0.005f;
    
    for (int i = 0; i < MAX_ITER; ++i)
    {
        float t = fTime * (1.f - (3.5f / float(i + 1)));
        vValue = vNewUV + float2(cos(t - vValue.x) + sin(t + vValue.y), sin(t - vValue.y) + cos(t + vValue.x));
        fPower += 1.f / length(float2(vNewUV.x / (sin(vValue.x + t) / inten), vNewUV.y / (cos(vValue.y + t) / inten)));
    }
    
    fPower /= float(MAX_ITER);
    fPower = 1.17f - pow(fPower, 1.4f);
    float3 vColor = pow(abs(fPower), 6.f);
    Out.vDiffuse.rgb = saturate(vColor + float3(0.f, 0.35f, 0.5f));
    Out.vDiffuse.a = 1.f;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Water
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}