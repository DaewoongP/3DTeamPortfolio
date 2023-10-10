#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_TargetTexture;

vector g_vCamPosition;

float g_fBlurStart = 1.f; // 확대 or 축소 // 화면비율과 연동해서 그리는 위치 결정해야함.
float g_fBlurWidth = 0.1f; // 블러하는 양
int g_iNumSample = 10;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
	
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix TransformMatrix;
    TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	
    vector vPosition;
    vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * length(In.vRight), In.vPSize.y * length(In.vUp));
	
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    vector vLook = g_vCamPosition - In[0].vPosition;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexUV = float2(0.f, 0.f);

    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexUV = float2(1.f, 0.f);

    Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexUV = float2(1.f, 1.f);

    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexUV = float2(0.f, 1.f);

    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
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
    
    float2 vBlurCenterUV = float2(0.5f, 0.5f);
    float2 uv = In.vTexUV;
    uv -= vBlurCenterUV;
    
    float fPrecompute = g_fBlurWidth * (1.0 / float(g_iNumSample - 1));
    
    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    
    // 카운트값을 코드로 제한해야 컴파일이 원활함.
    int iterationCount = min(g_iNumSample, 10);
    for (int i = 0; i < iterationCount; ++i)
    {
        float fScale = g_fBlurStart + (float(i) * fPrecompute);
        color += g_TargetTexture.Sample(LinearSampler, uv * fScale + vBlurCenterUV);
    }

    color /= float(g_iNumSample);
    color.a = 1.f;
    
    Out.vColor = color;
   
    return Out;
}

technique11 DefaultTechnique
{
    pass Radial
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}