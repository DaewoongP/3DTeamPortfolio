#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;
texture2D g_Texture;

//timer
float g_fTimeRatio;
float g_PI = acos(-1);

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

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

    if (0.1f > Out.vColor.a)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_TIMER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //단위원으로 변경하기 위해 값 변경
    float2 uv = float2((In.vTexUV.x - 0.5f) * 2.f, (In.vTexUV.y - 0.5f) * -2.f);

    // 현재 픽셀의 각도
    float fAngle = 0.f;
    
    float fTime = g_PI * 2.f * g_fTimeRatio;
    
    float4 vCurrentColor = float4(1.f, 1.f, 1.f, 1.f);
    
    // 원의 방정식
    float fCircle = pow(uv.x, 2.f) + pow(uv.y, 2.f);
    if (fCircle <= 1.f &&
        fCircle > 0.2f)
    {
        // 1사분면
        if (fTime < g_PI * 0.5f)
        {
            if (uv.x > 0.f &&
                uv.y > 0.f)
            {
                fAngle = atan(uv.x / uv.y);
                
                if (fTime > fAngle)
                {
                    discard;
                }
            }
        }
        // 2사분면
        else if (fTime < g_PI)
        {
            // 1사분면 삭제
            if (uv.x > 0.f &&
                uv.y > 0.f)
                discard;
            
            if (uv.x > 0.f &&
                uv.y < 0.f)
            {
                fAngle = atan(uv.y / uv.x * -1.f);
                
                if (fTime - g_PI * 0.5f > fAngle)
                {
                    discard;
                }
            }
        }
        else if (fTime < g_PI * 1.5f)
        { 
            // 1사분면 삭제
            if (uv.x > 0.f &&
                uv.y > 0.f)
                discard;
            // 2사분면 삭제
            if (uv.x > 0.f &&
                uv.y < 0.f)
                discard;
                
            if (uv.x < 0.f &&
                uv.y < 0.f)
            {
                fAngle = atan(uv.x / uv.y);
                
                if (fTime - g_PI > fAngle)
                {
                    discard;
                }
            }
        }
        else if (fTime < g_PI * 2.f)
        {
            // 1사분면 삭제
            if (uv.x > 0.f &&
                uv.y > 0.f)
                discard;
            // 2사분면 삭제
            if (uv.x > 0.f &&
                uv.y < 0.f)
                discard;
            // 3사분면 삭제
            if (uv.x < 0.f &&
                uv.y < 0.f)
                discard;
                
            if (uv.x < 0.f &&
                uv.y > 0.f)
            {
                fAngle = atan(uv.y / uv.x * -1.f);
                
                if (fTime - g_PI * 1.5f > fAngle)
                {
                    discard;
                }
            }
        }
    }
    else
        discard;
    
    vCurrentColor.gb *= 1.f - g_fTimeRatio;
    Out.vColor = vCurrentColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Point
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Point_Timer
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_TIMER();
    }
}