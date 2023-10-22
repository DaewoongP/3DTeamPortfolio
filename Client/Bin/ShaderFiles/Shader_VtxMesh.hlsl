#include "Shader_Client_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_DissolveTexture;

float g_fCamFar, g_fDissolveAmount;
float2 g_vOffset;

float4 g_vEmissive;

//sky
float2 g_vMoonPos;
int g_iFrame;
float g_fTime;

float IsIn_Range(float fMin, float fMax, float fValue)
{
    return (fMin <= fValue) && (fMax >= fValue);
}

float fract(float f)
{
    return f - floor(f);
}

float3 fract(float3 f)
{
    return f - floor(f);
}


float hash12(float2 p)
{
    float3 p3 = fract(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float NoisyStarField(in float2 vSamplePos, float fThreshhold)
{
    float StarVal = hash12(vSamplePos);
    if (StarVal >= fThreshhold)
        StarVal = pow((StarVal - fThreshhold) / (1.0 - fThreshhold), 6.0);
    else
        StarVal = 0.0;
    return StarVal;
}

float StableStarField(in float2 vSamplePos, float fThreshhold)
{
    // Linear interpolation between four samples.
    // Note: This approach has some visual artifacts.
    // There must be a better way to "anti alias" the star field.
    float fractX = fract(vSamplePos.x);
    float fractY = fract(vSamplePos.y);
    float2 floorSample = floor(vSamplePos);
    float v1 = NoisyStarField(floorSample, fThreshhold);
    float v2 = NoisyStarField(floorSample + float2(0.0, 1.0), fThreshhold);
    float v3 = NoisyStarField(floorSample + float2(1.0, 0.0), fThreshhold);
    float v4 = NoisyStarField(floorSample + float2(1.0, 1.0), fThreshhold);

    float StarVal = v1 * (1.0 - fractX) * (1.0 - fractY)
        			+ v2 * (1.0 - fractX) * fractY
        			+ v3 * fractX * (1.0 - fractY)
        			+ v4 * fractX * fractY;
    return StarVal;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vMRO : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = g_vEmissive;
    
    return Out;
}

PS_OUT PS_MAIN_SKY_BLACK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = float4(0.f, 0.f, 0.f, 1.f);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_SKY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;

    vDiffuse *= 0.15f;

    float2 vNormalizedUv = In.vTexUV;
    
	// moon
    float2 vUvMoonDiff = In.vTexUV - g_vMoonPos;
    float fMoonDot = max(0.0, 1.0 - dot(vUvMoonDiff, vUvMoonDiff));
    Out.vDiffuse.rgb += float3(0.6, 0.6, 0.6) * pow(fMoonDot, 350.0) + vDiffuse.rgb;
	
	// moon haze
    Out.vDiffuse.rgb += float3(0.48, 0.54, 0.6) * pow(fMoonDot, 300.0);
    Out.vDiffuse.a = 1.f;
    
    //[0.9, 0.9999]
    float StarFieldThreshhold = 0.985;
    
    // Stars with a slow spin.
    float fSpinRate = 0.0001;
    float2 vInputPos = (2.0 * In.vTexUV * float2(1280.f, 720.f) / 720.f) - float2(1.0, 1.0);
    float fSampleAngle = fSpinRate * float(g_iFrame) + atan2(vInputPos.y, vInputPos.x);
    float2 vSamplePos = (0.5 * length(vInputPos) * float2(cos(fSampleAngle), sin(fSampleAngle)) + float2(0.5, 0.5)) * 720.f;
    float StarVal = StableStarField(vSamplePos, StarFieldThreshhold);
    Out.vDiffuse.rgb += float3(StarVal, StarVal, StarVal);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + g_vOffset);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);

    if (vDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, g_fCamFar, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float DissolveDesc = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

    clip(DissolveDesc - g_fDissolveAmount);

    float diff = DissolveDesc - g_fDissolveAmount;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    float fStepValue1 = IsIn_Range(0.03f, 0.05f, diff);
    float fStepValue2 = IsIn_Range(0.05f, 0.065f, diff);
    float fStepValue3 = IsIn_Range(0.065f, 0.08f, diff);

    Out.vDiffuse = IsIn_Range(0.f, 0.03f, diff) * vector(1.f, 0.95f, 0.9f, 1.f) +
                   fStepValue1 * vector(1.f, 0.9f, 0.4f, 1.f) +
                   fStepValue2 * vector(0.9f, 0.1f, 0.f, 1.f) +
                   fStepValue3 * vector(0.3f, 0.0f, 0.f, 1.f) +
                   IsIn_Range(0.08f, 1.f, diff) * g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;

    //PS_OUT Out = (PS_OUT) 0;
    //
    //vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    //vector maskColor = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    //
    //float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    //float fProgress = saturate(1.f - g_fDissolveAmount);
    //float fDissolveFactor = (maskColor.r - fProgress) / g_fThickness;
    //
    //if (maskColor.r > fProgress + g_fThickness)
    //{
    //    discard;
    //}
    //else if (maskColor.r > fProgress)
    //{
    //    vDiffuse = lerp(vDiffuse, float4(1.0, 0.1, 0.1, 1.0), fDissolveFactor);
    //}
    //
    //vNormal = mul(vNormal, WorldMatrix);
    //
    //Out.vDiffuse = vDiffuse;
    //Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    //return Out;
}

technique11 DefaultTechnique
{
    pass Mesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Sky
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SKY();
    }

    pass VaultSky
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SKY_BLACK();
    }

    pass Mesh_No_Cull
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

    pass Effect_Mesh
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    pass Mesh_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }
}
