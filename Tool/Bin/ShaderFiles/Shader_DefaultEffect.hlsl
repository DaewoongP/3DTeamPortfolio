#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_TransformationMatrix;
texture2D g_MaterialTexture;

int g_iClipChannel;
float g_fClipThreshold;

float4 g_vColor;
float4 g_vCamPos;

bool g_isFlutter = false;
float g_fTimeAcc;
float3 g_vStrength = { 1.f, 1.f, 1.f };
float g_fRimPower;

// For. Emission
bool g_isEmission = { false };
float g_fFrequency = { 1.f };
float2 g_vRemap = { 1.f, 1.f };
float3 g_vEmissionColor = { 1.f, 1.f, 1.f };
int g_iEmissionChannel = { 0 };
texture2D g_EmissionTexture;
//////

void Unity_PolarCoordinates_float(float2 UV, float2 Center, float RadialScale, float LengthScale, out float2 Out)
{
    float2 delta = UV - Center;
    float radius = length(delta) * 2 * RadialScale;
    float angle = atan2(delta.x, delta.y) * 1.0 / 6.28 * LengthScale;
    Out = float2(radius, angle);
}

void Unity_Twirl_float(float2 UV, float2 Center, float Strength, float2 Offset, out float2 Out)
{
    float2 delta = UV - Center;
    float angle = Strength * length(delta);
    float x = cos(angle) * delta.x - sin(angle) * delta.y;
    float y = sin(angle) * delta.x + cos(angle) * delta.y;
    Out = float2(x + Center.x + Offset.x, y + Center.y + Offset.y);
}

float2 unity_voronoi_noise_randomVector(float2 UV, float offset)
{
    float2x2 m = float2x2(15.27, 47.63, 99.41, 89.98);
    UV = frac(sin(mul(UV, m)) * 46839.32);
    return float2(sin(UV.y * +offset) * 0.5 + 0.5, cos(UV.x * offset) * 0.5 + 0.5);
}

void Unity_Voronoi_float(float2 UV, float AngleOffset, float CellDensity, out float Out, out float Cells)
{
    float2 g = floor(UV * CellDensity);
    float2 f = frac(UV * CellDensity);
    float t = 8.0;
    float3 res = float3(8.0, 0.0, 0.0);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 lattice = float2(x, y);
            float2 offset = unity_voronoi_noise_randomVector(lattice + g, AngleOffset);
            float d = distance(lattice + offset, f);
            if (d < res.x)
            {
                res = float3(d, offset.x, offset.y);
                Out = res.x;
                Cells = res.y;
            }
        }
    }
}


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

void MainLogic(VS_IN In, out VS_OUT Out)
{
    matrix matTW, matTWV, matTWVP;

    matTW = mul(g_TransformationMatrix, g_WorldMatrix);
    matTWV = mul(matTW, g_ViewMatrix);
    matTWVP = mul(matTWV, g_ProjMatrix);

    vector vWorldPos = mul(vector(In.vPosition, 1.f), matTW);
    //if (true == g_isFlutter)
    //{
    //    float fNoiseValue = PerlinNoise_3D(vWorldPos.x, vWorldPos.y, vWorldPos.z);
    //    vWorldPos.x += fNoiseValue * g_vStrength.x;
    //    vWorldPos.y += fNoiseValue * g_vStrength.y;
    //    vWorldPos.z += fNoiseValue * g_vStrength.z;
    //}

    Out.vPosition = mul(vWorldPos, mul(g_ViewMatrix, g_ProjMatrix));
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matTW));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = vWorldPos;

}

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    MainLogic(In, Out);

    return Out;
}

VS_OUT VS_MAIN2(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    MainLogic(In, Out);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    TilingAndOffset_float(In.vTexUV, g_vTililing, g_vOffset, In.vTexUV);

    vector vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);
    //vector		vClipTexture = g_ClipTexture.Sample(LinearSampler, In.vTexUV);

    if (0 == g_iClipChannel)
        Out.vColor.a = vDiffuse.r;
    else if (1 == g_iClipChannel)
        Out.vColor.a = vDiffuse.g;
    else if (2 == g_iClipChannel)
        Out.vColor.a = vDiffuse.b;
    else if (3 == g_iClipChannel)
        Out.vColor.a = vDiffuse.a;

    if (Out.vColor.a < g_fClipThreshold)
    {
        discard;
    }

    Out.vColor = vDiffuse;
    Out.vColor *= g_vColor;

    if (g_isEmission)
    {
        float fEmissionValue = 0.f, fRemapValue = 0.f;
        vector vEmission = g_EmissionTexture.Sample(LinearSampler, In.vTexUV);
        float fSineTime = sin(g_fFrequency * g_fTimeAcc);
        if (0 == g_iClipChannel)
            fEmissionValue = vEmission.r;
        else if (1 == g_iClipChannel)
            fEmissionValue = vEmission.g;
        else if (2 == g_iClipChannel)
            fEmissionValue = vEmission.b;
        else if (3 == g_iClipChannel)
            fEmissionValue = vEmission.a;
        // out1 + (val - in1) * (out2 - out1) / (in2 - in1);

        Remap_float(fSineTime, float2(-1.f, 1.f), g_vRemap, fRemapValue);

        Out.vColor += fRemapValue * fEmissionValue * float4(g_vEmissionColor, 0.f);
    }
    return Out;
}

PS_OUT PS_MAIN_CLOISTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    TilingAndOffset_float(In.vTexUV, g_vTililing, g_vOffset, In.vTexUV);

    vector vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);

    if (vDiffuse.a < 0.5f)
        discard;

    Out.vColor = vDiffuse;
    Out.vColor *= g_vColor;
    if (g_isEmission)
    {
        float fEmissionValue = 0.f, fRemapValue = 0.f;
        vector vEmission = g_EmissionTexture.Sample(LinearSampler, In.vTexUV);
        float fSineTime = sin(g_fFrequency * g_fTimeAcc);
        if (0 == g_iClipChannel)
            fEmissionValue = vEmission.r;
        else if (1 == g_iClipChannel)
            fEmissionValue = vEmission.g;
        else if (2 == g_iClipChannel)
            fEmissionValue = vEmission.b;
        else if (3 == g_iClipChannel)
            fEmissionValue = vEmission.a;
        // out1 + (val - in1) * (out2 - out1) / (in2 - in1);

        Remap_float(fSineTime, float2(-1.f, 1.f), g_vRemap, fRemapValue);

        Out.vColor += fRemapValue * fEmissionValue * float4(g_vEmissionColor, 0.f);
    }
    return Out;
}
/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_NONBLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    TilingAndOffset_float(In.vTexUV, g_vTililing, g_vOffset, In.vTexUV);

    vector vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);

    Out.vColor = vDiffuse;
    Out.vColor *= g_vColor;
    if (g_isEmission)
    {
        float fEmissionValue = 0.f, fRemapValue = 0.f;
        vector vEmission = g_EmissionTexture.Sample(LinearSampler, In.vTexUV);
        float fSineTime = sin(g_fFrequency * g_fTimeAcc);
        if (0 == g_iClipChannel)
            fEmissionValue = vEmission.r;
        else if (1 == g_iClipChannel)
            fEmissionValue = vEmission.g;
        else if (2 == g_iClipChannel)
            fEmissionValue = vEmission.b;
        else if (3 == g_iClipChannel)
            fEmissionValue = vEmission.a;
        // out1 + (val - in1) * (out2 - out1) / (in2 - in1);

        Remap_float(fSineTime, float2(-1.f, 1.f), g_vRemap, fRemapValue);

        Out.vColor += fRemapValue * fEmissionValue * float4(g_vEmissionColor, 0.f);
    }
    return Out;
}

PS_OUT PS_DRAGON_PROTEGO(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float3 vViewDir;
    TilingAndOffset_float(In.vTexUV, g_vTililing, g_vOffset, In.vTexUV);

    vector vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);
    //vector		vClipTexture = g_ClipTexture.Sample(LinearSampler, In.vTexUV);

    if (Out.vColor.a < g_fClipThreshold)
    {
        discard;
    }

    Out.vColor.rgb = vDiffuse.rgb;
    Out.vColor *= g_vColor;

    if (g_isEmission)
    {
        float fEmissionValue = 0.f, fRemapValue = 0.f;
        vector vEmission = g_EmissionTexture.Sample(LinearSampler, In.vTexUV);
        float fSineTime = sin(g_fFrequency * g_fTimeAcc);
        if (0 == g_iClipChannel)
            fEmissionValue = vEmission.r;
        else if (1 == g_iClipChannel)
            fEmissionValue = vEmission.g;
        else if (2 == g_iClipChannel)
            fEmissionValue = vEmission.b;
        else if (3 == g_iClipChannel)
            fEmissionValue = vEmission.a;
        // out1 + (val - in1) * (out2 - out1) / (in2 - in1);

        Remap_float(fSineTime, float2(-1.f, 1.f), g_vRemap, fRemapValue);

        Out.vColor += fRemapValue * fEmissionValue * float4(g_vEmissionColor, 0.f);
    }
    return Out;
}


PS_OUT PS_PORTAL(PS_IN In)
{

    float2 OutValue;
    Unity_PolarCoordinates_float(In.vTexUV, float2(0.5f, 0.5f), 4.f, 3.f, OutValue);

    PS_OUT Out = (PS_OUT)0;

    TilingAndOffset_float(OutValue, g_vTililing, g_vOffset, OutValue);

    vector vDiffuse = g_MaterialTexture.Sample(LinearSampler, OutValue);
    //vector		vClipTexture = g_ClipTexture.Sample(LinearSampler, In.vTexUV);

    if (0 == g_iClipChannel)
        Out.vColor.a = vDiffuse.r;
    else if (1 == g_iClipChannel)
        Out.vColor.a = vDiffuse.g;
    else if (2 == g_iClipChannel)
        Out.vColor.a = vDiffuse.b;
    else if (3 == g_iClipChannel)
        Out.vColor.a = vDiffuse.a;

    if (Out.vColor.a < g_fClipThreshold)
    {
        discard;
    }

    Out.vColor = vDiffuse;
    Out.vColor *= g_vColor;

    if (g_isEmission)
    {
        float fEmissionValue = 0.f, fRemapValue = 0.f;
        vector vEmission = g_EmissionTexture.Sample(LinearSampler, OutValue);
        float fSineTime = sin(g_fFrequency * g_fTimeAcc);
        if (0 == g_iClipChannel)
            fEmissionValue = vEmission.r;
        else if (1 == g_iClipChannel)
            fEmissionValue = vEmission.g;
        else if (2 == g_iClipChannel)
            fEmissionValue = vEmission.b;
        else if (3 == g_iClipChannel)
            fEmissionValue = vEmission.a;
        // out1 + (val - in1) * (out2 - out1) / (in2 - in1);

        Remap_float(fSineTime, float2(-1.f, 1.f), g_vRemap, fRemapValue);

        Out.vColor += fRemapValue * fEmissionValue * float4(g_vEmissionColor, 0.f);
    }
    return Out;



}

PS_OUT PS_PORTAL2(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 OutValue;
    float NewTimeValue;
    NewTimeValue = g_fTimeAcc * 0.5f;
    Unity_Twirl_float(In.vTexUV, float2(0.5f, 0.5f), 7.35f, NewTimeValue, OutValue);
    float VoronoiValue;
    float cellvalue;
    Unity_Voronoi_float(OutValue, 2.f, 2.89f, VoronoiValue, cellvalue);
    float PowerVoronoi;
    PowerVoronoi = VoronoiValue * VoronoiValue;

    vector vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);
    vector NewColor;
    NewColor = g_vColor * vDiffuse;

    Out.vColor = NewColor * PowerVoronoi;

    return Out;


}

technique11 DefaultTechnique
{
    pass Default_AlphaBlend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass CloisterMap
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_CLOISTER();
    }

    pass Default_NonBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_NONBLEND();
    }

    pass ImpulseSphere
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DragonProtego
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_DRAGON_PROTEGO();
    }
    pass Portal
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_PORTAL();
    }
    pass Portal2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_M AIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_PORTAL2();
    }



}