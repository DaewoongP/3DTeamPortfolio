#include "Shader_Tool_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_BrushTexture[4];

unsigned int    g_iBrushTextureIndex;
float3          g_vBrushCurrentPos;
float			g_fBrushCurrentRange;

unsigned int    g_iBrushPosCnt;
float3          g_vBrushPos[512];
float           g_fBrushRange[512];
int             g_iBrushIndex[512];

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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = normalize(vWorldNormal);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vWorldPos.x < 1.f && In.vWorldPos.z == 0.f)
        discard;
    if (In.vWorldPos.x == 0.f && In.vWorldPos.z < 1.f)
        discard;
    
    Out.vColor = float4(0.f, 1.f, 1.f, 1.f);

    return Out;
}

PS_OUT PS_BRUSH(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);
    vector vBrush = vector(0.0f, 0.0f, 0.0f, 0.0f);

    // 불러온 정보로 지형을 그림
    for (int i = 0; i < g_iBrushPosCnt; i++)
    {
        if (g_vBrushPos[i].x - g_fBrushRange[i] < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos[i].x + g_fBrushRange[i] &&
            g_vBrushPos[i].z - g_fBrushRange[i] < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos[i].z + g_fBrushRange[i])
        {
            float2		vTexUV;

            vTexUV.x = (In.vWorldPos.x - (g_vBrushPos[i].x - g_fBrushRange[i])) / (2.f * g_fBrushRange[i]);
            vTexUV.y = ((g_vBrushPos[i].z - g_fBrushRange[i]) - In.vWorldPos.z) / (2.f * g_fBrushRange[i]);

            if (0 == g_iBrushIndex[i])
                vBrush = g_BrushTexture[0].Sample(LinearSampler, vTexUV);
            else if (1 == g_iBrushIndex[i])
                vBrush = g_BrushTexture[1].Sample(LinearSampler, vTexUV);
            else if (2 == g_iBrushIndex[i])
                vBrush = g_BrushTexture[2].Sample(LinearSampler, vTexUV);
            else if (3 == g_iBrushIndex[i])
                vBrush = g_BrushTexture[3].Sample(LinearSampler, vTexUV);

            vDiffuse = vBrush;
        }
    }

    // 현재 브러쉬 위치
    if (g_vBrushCurrentPos.x - g_fBrushCurrentRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushCurrentPos.x + g_fBrushCurrentRange &&
        g_vBrushCurrentPos.z - g_fBrushCurrentRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushCurrentPos.z + g_fBrushCurrentRange)
    {
        float2		vTexUV;

        vTexUV.x = (In.vWorldPos.x - (g_vBrushCurrentPos.x - g_fBrushCurrentRange)) / (2.f * g_fBrushCurrentRange);
        vTexUV.y = ((g_vBrushCurrentPos.z - g_fBrushCurrentRange) - In.vWorldPos.z) / (2.f * g_fBrushCurrentRange);

        if (0 == g_iBrushTextureIndex)
            vBrush = g_BrushTexture[0].Sample(LinearSampler, vTexUV);
        else if (1 == g_iBrushTextureIndex)
            vBrush = g_BrushTexture[1].Sample(LinearSampler, vTexUV);
        else if (2 == g_iBrushTextureIndex)
            vBrush = g_BrushTexture[2].Sample(LinearSampler, vTexUV);
        else if (3 == g_iBrushTextureIndex)
            vBrush = g_BrushTexture[3].Sample(LinearSampler, vTexUV);

        vDiffuse = vBrush;
    }

    Out.vColor = vDiffuse;

    return Out;
}

technique11 DefaultTechnique
{
    pass Terrain
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

    pass Terrain_Brush
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_BRUSH();
    }
}