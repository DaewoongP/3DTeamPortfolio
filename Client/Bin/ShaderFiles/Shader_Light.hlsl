#include "Shader_EngineHeader.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D g_NormalTexture;
texture2D g_DepthTexture;

vector g_vCamPosition;
float g_fCamFar;

vector g_vLightPos;
float g_fLightRange;

vector g_vLightDir;
float g_fSpotPower;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

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
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    Out.vShade.a = 1.f;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vPosition;

    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

    vPosition = vPosition * fViewZ;
    vector vLightDir = vPosition - g_vLightPos;

    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vPosition - g_vCamPosition;

    // 방향성 광원 스펙큘러는 좀 과해서 처리안함.
    //Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 10.f);
    Out.vSpecular.a = 0.f;
    
    return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vPosition;

	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;
    float fDistance = length(vLightDir);

    // 빛의 감쇄량을 0~1로 정규화 시켜 처리하기 위함.
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
	
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    Out.vShade.a = 1.f;

    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 20.f) * fAtt;
    Out.vSpecular.a = 0.f;
    
    return Out;
}

PS_OUT PS_MAIN_SPOTLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vPosition;

	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;

    float fDistance = length(vLightDir);

    // spotlight factor
    float fSpot = pow(max(dot(normalize(vLightDir), g_vLightDir), 0.0f), g_fSpotPower);

	// 감쇄 효과
    // 실제 거리에 따른 공식처리 -> fSpot / fDistance * fDistance // -> f
    float fAtt = fSpot / fDistance * fDistance;
	
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    Out.vShade.a = 1.f;

    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f , normalize(vLook)), 0.0f), 10.f) * fAtt;

    return Out;
}

technique11 DefaultTechnique
{
    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        // 빛을 모두 더해서 적용하기 위한 BS
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        // 점광원은 여러개로 처리될 수 있기 때문에 알파블렌드를 활용하여 픽셀의 색상값을 계속 더해주어
        // 하나의 렌더타겟에 모든값이 저장될 수 있도록 다중 광원을 처리해야한다.
        // 따라서 가지고있는 픽셀의 색상값을 그대로 가져와서 ADD 처리 해야함.
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Light_Spotlight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SPOTLIGHT();
    }
}