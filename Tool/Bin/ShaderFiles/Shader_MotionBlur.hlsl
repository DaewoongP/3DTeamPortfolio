#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_PreCamViewMatrix, g_PreCamProjMatrix;
texture2D g_TargetTexture;
texture2D g_DepthTexture;

float g_fCamFar;

uint g_iSampleCnt = 16;
float g_fMotionBlurStrength = 1.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
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

PS_OUT PS_MAIN_MOTIONBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
		   
    float fViewZ = vDepthDesc.y * g_fCamFar;
		    
    vector vPixelWorldPos, vPixelPos;

    vPixelWorldPos.x = In.vTexUV.x * 2.f - 1.f;
    vPixelWorldPos.y = In.vTexUV.y * -2.f + 1.f;
    vPixelWorldPos.z = vDepthDesc.x;
    vPixelWorldPos.w = 1.0f;

    vPixelPos = vPixelWorldPos;
	    
    vPixelWorldPos *= fViewZ;

    vPixelWorldPos = mul(vPixelWorldPos, g_ProjMatrixInv);

    vPixelWorldPos = mul(vPixelWorldPos, g_ViewMatrixInv);
	
    matrix matVP = mul(g_PreCamViewMatrix, g_PreCamProjMatrix);

    vector vPrePixelPos = mul(vPixelWorldPos, matVP);
    vPrePixelPos /= vPrePixelPos.w;

    float2 vPixelVelocity = ((vPixelPos - vPrePixelPos) * 0.5f).xy;
    float2 texCoord = In.vTexUV;

    vector vColor = vector(0.f, 0.f, 0.f, 0.f);

    for (int i = -10; i < 10; ++i)
    {
        texCoord += vPixelVelocity * (0.005f + g_fMotionBlurStrength) * i;
        float4 currentColor = g_TargetTexture.Sample(LinearSampler_Clamp, texCoord);
        vColor += currentColor;
    }

    Out.vColor = vColor / 20.f;
	
    return Out;
}

technique11 DefaultTechnique
{
    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
    }
}