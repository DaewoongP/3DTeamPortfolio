#include "Shader_Client_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_BoneMatrices[512];
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;
texture2D g_DissolveTexture;
texture2D g_DistortionTexture;

texture2D g_DiffuseTexture_Cat;

float4 g_vHairColor = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vColor;
float g_fCamFar;
int g_iCatNum;

float g_fDissolveAmount;
float g_fTimeAcc;

float4 g_vEmissive;
float3 g_vEmissiveStrength;
float g_fInvinRatio;

float g_fEmissivePower;

float IsIn_Range(float fMin, float fMax, float fValue)
{
    return (fMin <= fValue) && (fMax >= fValue);
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4  vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    // SV_POSITION으로 나가기전의 포지션 값 -> 투영스페이스상의 포지션값 대입.
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
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

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

PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    
    if (vDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vDiffuse * g_vColor;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Out;
}

// DAO 텍스처 넣어줘야됨
PS_OUT PS_MAIN_HAIR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = g_vHairColor;
	
    // UNORM 4개 타입에 값을 넣으므로 여기서 0~1로 보정처리하고 나중에 받을때 -1~1로 보정처리를 다시한다.
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    // SV_POSITION으로 설정되지 않았던 투영포지션 값이므로 w나누기를 수행한 z값 (투영스페이스) 값을 r, 
    // 다시 이후 셰이더에서 w를 곱해주기 위해 b에 값을 다시 대입해줌.
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

    Out.vDiffuse = vDiffuse * 0.1f + vEmissive * 0.25f;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

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

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DistortionTexture.Sample(LinearSampler, In.vTexUV * g_fTimeAcc);
    vDiffuse.a = 1.f;
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.

    Out.vDiffuse = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_CAT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vDiffuse1 = g_DiffuseTexture_Cat.Sample(LinearSampler, In.vTexUV);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    if (vDiffuse.a < 0.1f)
        discard;
        
    if (0 != g_iCatNum)
        Out.vDiffuse = vDiffuse1;
    else
        Out.vDiffuse = vDiffuse;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_TROLL(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    if (vDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = vEmissive;

    return Out;
}

PS_OUT PS_MAIN_DRAGON_INVIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector vEmissive = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    if (vDiffuse.a < 0.1f)
        discard;

    float4 vDark = (float4)0;
    Out.vDiffuse = lerp(vDark, vDiffuse, g_fInvinRatio);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = float4(vEmissive.rgb * g_vEmissiveStrength, 0.f);

    return Out;
}

PS_OUT PS_MAIN_PENSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    // 텍스처의 노말값은 -1~1로 출력을 못하기때문에 0~1로 정규화되어 있다. 따라서 강제적으로 변환해줘야함.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vEmissive.rgba = vEmissive.rgra * g_fEmissivePower;
    
    vNormal = mul(vNormal, WorldMatrix);
    if (vDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = vEmissive;

    return Out;
}

technique11 DefaultTechnique
{
	pass AnimMesh
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
    pass AnimMeshColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
    }

    pass HairMesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_HAIR();
    }
	pass AnimMeshNonCull
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }
    pass AnimMesh_E
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }
    pass AnimMesh_Dissolve
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
    pass AnimMesh_Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }
    pass AnimMesh_Cat
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_CAT();
    }
    pass AnimMesh_Owl
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
    pass AnimMesh_Troll
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_TROLL();
    }

    pass AnimMesh_DragonInvin
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DRAGON_INVIN();
    }
    
    pass Pensive
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_PENSIVE();
    }
}