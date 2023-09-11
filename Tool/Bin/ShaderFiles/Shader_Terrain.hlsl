#include "Shader_Tool_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_BrushTexture;

float			g_fBrushRadius;
unsigned int    g_iBrushPointCnt;
float3          g_vBrushCurrentPoint;
float3          g_vBrushPoint[256];

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
    vector vBrush = vector(0.0f, 0.0f, 0.0f, 0.0f)/*g_BrushTexture.Sample(LinearSampler, In.vTexUV)*/;

    /*for (int i = 0; i < g_iBrushPointCnt; i++)
    {
        if (g_vBrushPoint[i].x - g_fBrushRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPoint[i].x + g_fBrushRadius &&
            g_vBrushPoint[i].z - g_fBrushRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPoint[i].z + g_fBrushRadius)
        {
            float2		vTexUV;

            vTexUV.x = (In.vWorldPos.x - (g_vBrushPoint[i].x - g_fBrushRadius)) / (2.f * g_fBrushRadius);
            vTexUV.y = ((g_vBrushPoint[i].z - g_fBrushRadius) - In.vWorldPos.z) / (2.f * g_fBrushRadius);

            vBrush = g_BrushTexture.Sample(LinearSampler, vTexUV);
            vDiffuse += vBrush;
        }
    }   */

    /*if (g_vBrushCurrentPoint.x - g_fBrushRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushCurrentPoint.x + g_fBrushRadius &&
        g_vBrushCurrentPoint.z - g_fBrushRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushCurrentPoint.z + g_fBrushRadius)*/
    if(50.f <= In.vWorldPos.y)
    {
        vBrush = g_BrushTexture.Sample(LinearSampler, In.vTexUV);
    }

    Out.vColor = vDiffuse + vBrush;

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