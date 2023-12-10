#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_DebugTexture;

texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_ShadowTexture;
texture2D g_SpecularTexture;
texture2D g_BloomTexture;
texture2D g_EmissiveTexture;

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

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DebugTexture.Sample(LinearSampler, In.vTexUV);

    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    if (vDiffuse.a == 0.f)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    vSpecular *= 0.2f;
    vector vShadow = g_ShadowTexture.Sample(LinearSampler, In.vTexUV);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    vEmissive.a = 0.f;
    vector vBloom = g_BloomTexture.Sample(LinearSampler, In.vTexUV);
    
    if (vShadow.r < 0.8f)
    {
        vShade *= vShadow.r;
    }
    
    Out.vColor = vDiffuse * vShade + vSpecular + vEmissive + vBloom;
    
    return Out;
}

float fbm(float2 n)
{
    float total = 0.0, amplitude = 1.0;
    for (int i = 0; i < 5; i++)
    {
        total += noise(n) * amplitude;
        n += n;
        amplitude *= 0.4;
    }
    return total;
}

float lightShafts(float2 st)
{
    float angle = -0.2;
    float2 _st = st;
    st = float2(st.x * cos(angle) - st.y * sin(angle),
              st.x * sin(angle) + st.y * cos(angle));
    float val = fbm(float2(st.x * 2. + 200., st.y / 4.));
    val += fbm(float2(st.x * 2. + 200., st.y / 4.));
    val = val / 3.;
    float mask = pow(clamp(1.0 - abs(_st.y - 0.15), 0., 1.) * 0.49 + 0.5, 2.0);
    mask *= clamp(1.0 - abs(_st.x + 0.2), 0., 1.) * 0.49 + 0.5;
    return pow(val * mask, 2.0);
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }
}