#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_LightViewMatrix1, g_LightProjMatrix1;

uint g_iNumLights;

float g_fCamFar;

texture2D g_DepthTexture;
texture2D g_LightDepthTexture;
texture2D g_LightDepthTexture1;

float Create_ShadowPower(float4 vWorldPos, matrix LightViewMatrix, matrix LightProjMatrix, texture2D LightDepthTexture, float fShadowPower);

// DownSample
texture2D g_ShadowTexture;
float g_fWinSizeX, g_fWinSizeY;

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

PS_OUT PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler_Clamp, In.vTexUV);
    
    // camfar로 나누어진 뷰스페이스의 z값이므로 곱해서 복구
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vObjectProjPos;    
    vObjectProjPos.x = In.vTexUV.x * 2.f - 1.f; // -1~1로 변경
    vObjectProjPos.y = In.vTexUV.y * -2.f + 1.f; // -1~1로 변경 (UV좌표는 투영스페이스와 y가 반대)
    vObjectProjPos.z = vDepthDesc.x; // 투영에 w나누기가 된 z값이므로 그대로 대입
    vObjectProjPos.w = 1.f;

    // z나누기 복원
    vObjectProjPos = vObjectProjPos * fViewZ;
    
    // 객체 투영의 역행렬을 곱하여 뷰스페이스로 변경.
    float4 vObjectViewPos = mul(vObjectProjPos, g_ProjMatrixInv);

    // 객체의 뷰의 역행렬을 곱하여 월드스페이스로 변경.
    float4 vObjectWorldPos = mul(vObjectViewPos, g_ViewMatrixInv);
    
    /////////////////// 빛 기준 좌표 처리 ///////////////////////
    
    uint iNumLights = min(g_iNumLights, 2);
    
    float fShadowPower = 1.f;
    
    if (0 < iNumLights)
        fShadowPower *= Create_ShadowPower(vObjectWorldPos, g_LightViewMatrix, g_LightProjMatrix, g_LightDepthTexture, 0.5f);
    if (1 < iNumLights)
        fShadowPower *= Create_ShadowPower(vObjectWorldPos, g_LightViewMatrix1, g_LightProjMatrix1, g_LightDepthTexture1, 0.3f);

    Out.vColor = float4(fShadowPower, fShadowPower, fShadowPower, 1.f);
    saturate(Out.vColor);
    
    return Out;
}

PS_OUT PS_MAIN_DOWNSAMPLING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexelSize = 1.f / float2(g_fWinSizeX, g_fWinSizeY);
    float fSampling = 0.5f;
    // 2x2
    Out.vColor += g_ShadowTexture.Sample(LinearSampler_Clamp, In.vTexUV + float2(-fSampling, -fSampling) * vTexelSize);
    Out.vColor += g_ShadowTexture.Sample(LinearSampler_Clamp, In.vTexUV + float2(fSampling, -fSampling) * vTexelSize);
    Out.vColor += g_ShadowTexture.Sample(LinearSampler_Clamp, In.vTexUV + float2(-fSampling, fSampling) * vTexelSize);
    Out.vColor += g_ShadowTexture.Sample(LinearSampler_Clamp, In.vTexUV + float2(fSampling, fSampling) * vTexelSize);

    Out.vColor /= 4.f;

    return Out;
}

technique11 DefaultTechnique
{
    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
    pass DownSampling
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DOWNSAMPLING();
    }

}

float Create_ShadowPower(float4 vWorldPos, matrix LightViewMatrix, matrix LightProjMatrix, texture2D LightDepthTexture, float fShadowPower)
{
    float fRetValue = 1.f;
    
    // 지금 픽셀의 월드포지션에서 현재 빛으로 설정되어있는 값의 뷰행렬을 곱하여
    // 빛의 뷰스페이스로 변경
    float4 vLightViewPos = mul(vWorldPos, LightViewMatrix);
    
    // 빛기준의 뷰좌표를 투영좌표로 변경하기위해 투영 행렬을 곱함.
    float4 vLightProjPos = mul(vLightViewPos, LightProjMatrix);
    
    // w나누기를 실행하여 실제 투영좌표로 변환
    vLightProjPos = vLightProjPos / vLightProjPos.w;
    
    // 빛의 UV좌표를 투영 x,y값으로부터 구해옴.
    // 깊이와 상관없이 객체를 뷰행렬 기준으로 찍은 깊이와 비교하기 위해
    // 해당 UV좌표를 알아야하기 때문.
    // 마찬가지로 UV좌표는 y가 반대이므로 -를 같이 곱해줌.
    float2 vLightUV;
    vLightUV.x = vLightProjPos.x * 0.5f + 0.5f;
    vLightUV.y = vLightProjPos.y * -0.5f + 0.5f;

    // 객체의 Render_Depth함수에서 저장한 빛기준의 텍스처를 가져와
    // 현재 In의 픽셀의 포지션이 빛의 텍스처에 어디에 있는지 알아내고
    // 해당 텍스처의 픽셀 값인 (빛기준 LightProjPos.w / CamFar)를 처리한 값을 가져옴.
    // 빛기준의 뷰스페이스 z값이라고 봐도된다.
    vector vLightDepth = LightDepthTexture.Sample(LinearSampler_Clamp, vLightUV);

    // 그럼이제 현재 In 픽셀의 "빛의" 뷰스페이스 상의 z값과
    // UV좌표에 설정되어있는 "빛의" 뷰스페이스상의 뎁스값을 비교하여
    // UV좌표에 설정되어있는 뎁스보다 "깊을경우" 그림자로 처리한다.
    
    // 빛의 투영값을 컬링처리.
    // 안하면 그림자가 여러번 보일 수 있음.
    // 빛의 투영스페이스에 걸리지 않은 포지션값은 그냥 원래 컬러 뽑아주면 된다.
    if (-1.f >= vLightProjPos.x ||
	    1.f <= vLightProjPos.x ||
	    -1.f >= vLightProjPos.y ||
	    1.f <= vLightProjPos.y ||
        0.f >= vLightProjPos.z ||
	    1.f <= vLightProjPos.z)
    {
        fRetValue = 1.f + fShadowPower;
    }
    // 빛의 뷰스페이스 포지션 z와 픽셀의 라이트 뎁스 (실제 월드공간상의 viewz값)
    // 빛의 뷰스페이스 z값이 UV좌표의 뷰스페이스 z값보다 "클경우 (깊을경우)" 그림자.
    else if (vLightViewPos.z - 0.22f > vLightDepth.x * g_fCamFar)
    {
        fRetValue = fShadowPower;
    }
    else
    {
        fRetValue = 1.f + fShadowPower;
    }
    
    return fRetValue;
}