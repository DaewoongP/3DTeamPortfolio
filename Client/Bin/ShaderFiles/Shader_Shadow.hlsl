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
    
    // camfar�� �������� �佺���̽��� z���̹Ƿ� ���ؼ� ����
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vObjectProjPos;    
    vObjectProjPos.x = In.vTexUV.x * 2.f - 1.f; // -1~1�� ����
    vObjectProjPos.y = In.vTexUV.y * -2.f + 1.f; // -1~1�� ���� (UV��ǥ�� ���������̽��� y�� �ݴ�)
    vObjectProjPos.z = vDepthDesc.x; // ������ w�����Ⱑ �� z���̹Ƿ� �״�� ����
    vObjectProjPos.w = 1.f;

    // z������ ����
    vObjectProjPos = vObjectProjPos * fViewZ;
    
    // ��ü ������ ������� ���Ͽ� �佺���̽��� ����.
    float4 vObjectViewPos = mul(vObjectProjPos, g_ProjMatrixInv);

    // ��ü�� ���� ������� ���Ͽ� ���彺���̽��� ����.
    float4 vObjectWorldPos = mul(vObjectViewPos, g_ViewMatrixInv);
    
    /////////////////// �� ���� ��ǥ ó�� ///////////////////////
    
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
    
    // ���� �ȼ��� ���������ǿ��� ���� ������ �����Ǿ��ִ� ���� ������� ���Ͽ�
    // ���� �佺���̽��� ����
    float4 vLightViewPos = mul(vWorldPos, LightViewMatrix);
    
    // �������� ����ǥ�� ������ǥ�� �����ϱ����� ���� ����� ����.
    float4 vLightProjPos = mul(vLightViewPos, LightProjMatrix);
    
    // w�����⸦ �����Ͽ� ���� ������ǥ�� ��ȯ
    vLightProjPos = vLightProjPos / vLightProjPos.w;
    
    // ���� UV��ǥ�� ���� x,y�����κ��� ���ؿ�.
    // ���̿� ������� ��ü�� ����� �������� ���� ���̿� ���ϱ� ����
    // �ش� UV��ǥ�� �˾ƾ��ϱ� ����.
    // ���������� UV��ǥ�� y�� �ݴ��̹Ƿ� -�� ���� ������.
    float2 vLightUV;
    vLightUV.x = vLightProjPos.x * 0.5f + 0.5f;
    vLightUV.y = vLightProjPos.y * -0.5f + 0.5f;

    // ��ü�� Render_Depth�Լ����� ������ �������� �ؽ�ó�� ������
    // ���� In�� �ȼ��� �������� ���� �ؽ�ó�� ��� �ִ��� �˾Ƴ���
    // �ش� �ؽ�ó�� �ȼ� ���� (������ LightProjPos.w / CamFar)�� ó���� ���� ������.
    // �������� �佺���̽� z���̶�� �����ȴ�.
    vector vLightDepth = LightDepthTexture.Sample(LinearSampler_Clamp, vLightUV);

    // �׷����� ���� In �ȼ��� "����" �佺���̽� ���� z����
    // UV��ǥ�� �����Ǿ��ִ� "����" �佺���̽����� �������� ���Ͽ�
    // UV��ǥ�� �����Ǿ��ִ� �������� "�������" �׸��ڷ� ó���Ѵ�.
    
    // ���� �������� �ø�ó��.
    // ���ϸ� �׸��ڰ� ������ ���� �� ����.
    // ���� ���������̽��� �ɸ��� ���� �����ǰ��� �׳� ���� �÷� �̾��ָ� �ȴ�.
    if (-1.f >= vLightProjPos.x ||
	    1.f <= vLightProjPos.x ||
	    -1.f >= vLightProjPos.y ||
	    1.f <= vLightProjPos.y ||
        0.f >= vLightProjPos.z ||
	    1.f <= vLightProjPos.z)
    {
        fRetValue = 1.f + fShadowPower;
    }
    // ���� �佺���̽� ������ z�� �ȼ��� ����Ʈ ���� (���� ����������� viewz��)
    // ���� �佺���̽� z���� UV��ǥ�� �佺���̽� z������ "Ŭ��� (�������)" �׸���.
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