#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float PI = acos(-1.0);
float g_fTime;
float2 g_vViewPort;

float fract(float x)
{
    return x - floor(x);
}

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

float rnd(float t)
{
    return fract(sin(t * 745.523) * 7894.552);
}

float rain(float3 p)
{
    p.y -= g_fTime * 4.0;
    p.xy *= 60.0;
  
    p.y += rnd(floor(p.x)) * 80.0;
  
    return clamp(1.0 - length(float2(cos(p.x * PI), sin(p.y * 0.1) - 1.7)), 0.0, 1.0);
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexUV;
    uv -= 0.5;
    uv.y *= -1.f;
    float fSpeed = 1.f;
    uv /= float2(g_vViewPort.y / g_vViewPort.x, fSpeed);

    float3 col = float3(0, 0, 0);
    float fr = rain(float3(-uv, 5));
    col = float3(fr, fr, fr);
    col += rain(float3(-uv * 2.3, 5)) * 0.5;
    col += rain(float3(-uv * 4.7, 5)) * 0.25;

    col = pow(col, float3(0.4545, 0.4545, 0.4545));

    Out.vColor = float4(col, 1);
    
    if (0.f == Out.vColor.a)
        discard;

    return Out;
}

technique11 DefaultTechnique
{
    pass Rain
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
}