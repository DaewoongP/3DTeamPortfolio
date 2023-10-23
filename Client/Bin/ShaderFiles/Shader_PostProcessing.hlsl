#include "Shader_EngineHeader.hlsli"
#include "Shader_RenderFunc.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
// PostProcessing
texture2D g_HDRTexture;
texture2D g_GlowTexture;
texture2D g_SSAOTexture;
texture2D g_DepthTexture;
texture2D g_LightShaftTexture;
texture2D g_RainTexture;
bool g_isSSAO;
bool g_isCircleFog;
bool g_isFloorFog;
bool g_isNight;
float g_fCamFar;
float4 g_vCamPos;

// fog
float3 g_vCircleFogCenter;
float4 g_vFogColor;
float g_fCircleFogRadius;
float g_fTime;

// HDR
texture2D g_DeferredTexture;
texture2D g_SkyTexture;
texture2D g_DOFTexture;
float g_fHDRPower;


float fract(float f)
{
    return f - floor(f);
}

float3 fract(float3 f)
{
    return f - floor(f);
}

float hash11(float p);
float smoothNoise13(in float3 x);
float FractionalBrownianMotion(float3 p);

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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vHDR = g_HDRTexture.Sample(LinearSampler, In.vTexUV);
    vector vGlow = g_GlowTexture.Sample(LinearSampler, In.vTexUV);
    vector vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV);
    vector vLightShaft = g_LightShaftTexture.Sample(LinearSampler, In.vTexUV);
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vPosition;
    
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    if (true == g_isNight)
    {
        vLightShaft *= 0.f;
    }
    
    if (true == g_isSSAO)
        Out.vColor = vHDR * vSSAO + vGlow + vLightShaft;
    else
        Out.vColor = vHDR + vGlow + vLightShaft;

    if (true == g_isCircleFog)
    {
        float fFogPower = 0.f;
    
        
        float fDistance = distance(g_vCircleFogCenter, vPosition.xyz);
        if (g_fCircleFogRadius > fDistance)
        {
            fFogPower = 0.f;
        }
        else
        {
            fFogPower = saturate((g_fCircleFogRadius - fDistance) / -20.f);
        }
        
        Out.vColor = lerp(Out.vColor, g_vFogColor, fFogPower);
    }
    
    // floor fog
    if (g_isFloorFog)
    {
        float2 uv = In.vTexUV * float2(1280.f, 720.f) / 720.f;
        float3 vFbmInput = float3(uv.x - g_fTime * 0.1f, vPosition.y * 2.f, 0.0);
        if (g_vCamPos.y > vPosition.y)
        {
            vFbmInput = float3(uv.x - g_fTime * 0.1f, uv.y * 2.f, 0.0);
        }
        float3 vFogColor = float3(0.7, 0.7, 0.9);
        float ft = saturate((g_vCamPos.y - vPosition.y) / 3.f);
        Out.vColor.rgb += In.vTexUV.y * vFogColor * FractionalBrownianMotion(vFbmInput) * ft;
    
        vFbmInput = float3(uv.x - g_fTime * 0.1f, uv.y * 2.f, 0.0);
        Out.vColor.rgb += In.vTexUV.y * vFogColor * FractionalBrownianMotion(vFbmInput) * 0.3f;       
    }
    
    //y°ª ¾È°³
    //if (vPosition.y >= 10.f)
    //    fFogPower = 0.f;
    //else
    //    fFogPower = saturate((vPosition.y - 10.f) / -10.f);

    Out.vColor += g_RainTexture.Sample(LinearSampler, In.vTexUV);
    saturate(Out.vColor);
    
    return Out;
}

PS_OUT PS_MAIN_HDR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDeferredTexture = g_DeferredTexture.Sample(LinearSampler, In.vTexUV);
    vector vSkyTexture = g_SkyTexture.Sample(LinearSampler, In.vTexUV);
    vector vDOF = g_DOFTexture.Sample(LinearSampler, In.vTexUV);
    
    if (0.f == vDeferredTexture.a)
        vDeferredTexture = vSkyTexture;
    else
    {
        vDeferredTexture.rgb += ACESToneMapping(vDeferredTexture.rgb) * g_fHDRPower;
        vDOF.rgb += ACESToneMapping(vDOF.rgb) * g_fHDRPower;

        vDeferredTexture.rgb = (vDOF.rgb * vDOF.a) + (vDeferredTexture.rgb * (1.f - vDOF.a));
    }
    
    Out.vColor = vDeferredTexture;

    return Out;
}

technique11 DefaultTechnique
{
    pass PostProcessing
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
    pass HDR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_HDR();
    }
}

float hash11(float p)
{
    float3 p3 = fract(float3(p, p, p) * 0.1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float smoothNoise13(in float3 x)
{
    float3 p = floor(x);
    float3 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y * 57.0 + 113.0 * p.z;

    return lerp(
        		lerp(
                    lerp(hash11(n + 0.0), hash11(n + 1.0), f.x),
					lerp(hash11(n + 57.0), hash11(n + 58.0), f.x),
                    f.y),
				lerp(
                    lerp(hash11(n + 113.0), hash11(n + 114.0), f.x),
					lerp(hash11(n + 170.0), hash11(n + 171.0), f.x),
                    f.y),
        		f.z);
}

float FractionalBrownianMotion(float3 p)
{
    float3x3 m = float3x3(0.00, 1.60, 1.20, -1.60, 0.72, -0.96, -1.20, -0.96, 1.28);
    
    float f = 0.5000 * smoothNoise13(p);
    p = mul(m, p) * 1.2;
    f += 0.2500 * smoothNoise13(p);
    p = mul(m, p) * 1.3;
    f += 0.1666 * smoothNoise13(p);
    p = mul(m, p) * 1.4;
    f += 0.0834 * smoothNoise13(p);
    return f;
}