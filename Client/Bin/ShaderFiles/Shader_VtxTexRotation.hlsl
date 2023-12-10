//#include "Shader_Client_Defines.hlsli"
//
//Texture2D g_Texture;
//float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//
//// For Progress
//float g_fPercent;
//
//// For MiniMap
//float2 g_vMiniMapSize;
//float3 g_vPlayerPos;
//
//SamplerState g_Sampler
//{
//	AddressU = CLAMP;
//	AddressV = CLAMP;
//};
//
//struct VS_IN
//{
//	float3 vPosition : POSITION;
//	float2 vTexUV : TEXCOORD0;
//};
//
//struct VS_OUT
//{
//	float4 vPosition : SV_POSITION;
//	float2 vTexUV : TEXCOORD0;
//};
//
//VS_OUT VS_MAIN(VS_IN In)
//{
//	VS_OUT Out = (VS_OUT)0;
//
//	float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
//	float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);
//
//	Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);
//	Out.vTexUV = In.vTexUV;
//
//	return Out;
//}
//
//struct PS_IN
//{
//	float4 vPosition : SV_POSITION;
//	float2 vTexUV : TEXCOORD0;
//};
//
//float4 PS_MAIN(PS_IN In) : SV_TARGET0
//{
//	float4 vColor = (float4) 0;
//
//	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
//
//	return vColor;
//}
//
//float4 PS_MAIN_UI(PS_IN In) : SV_TARGET0
//{
//	//// 텍스처에서 현재 픽셀 좌표의 색상을 읽습니다.
//	//float4 texColor = g_Texture.Sample(g_Sampler, texCoord);
//
//	//// 픽셀 색상이 파란색(예: B값이 최대)인 경우 왼쪽으로 회전, 빨간색인 경우 오른쪽으로 회전합니다.
//	//float rotationAngle = 0.0; // 회전 각도 초기화
//
//	//if (texColor.b == 1.0)
//	//{
//	//	// 파란색 픽셀은 Z 축을 기준으로 왼쪽으로 회전합니다.
//	//	rotationAngle = -3.14159265 / 4.0; // 왼쪽으로 45도 회전 (-π/4 radians)
//	//}
//	//else if (texColor.r == 1.0)
//	//{
//	//	// 빨간색 픽셀은 Z 축을 기준으로 오른쪽으로 회전합니다.
//	//	rotationAngle = 3.14159265 / 4.0; // 오른쪽으로 45도 회전 (π/4 radians)
//	//}
//
//	//// 현재 픽셀 좌표를 중심점을 기준으로 회전합니다.
//	//float2 center = float2(0.5, 0.5);
//	//float2 rotatedCoord = float2(
//	//	(texCoord.x - center.x) * cos(rotationAngle) - (texCoord.y - center.y) * sin(rotationAngle) + center.x,
//	//	(texCoord.x - center.x) * sin(rotationAngle) + (texCoord.y - center.y) * cos(rotationAngle) + center.y
//	//);
//
//	//// 회전된 좌표에서 텍스처의 색상을 읽습니다.
//	//float4 rotatedColor = g_Texture.Sample(g_Sampler, rotatedCoord);
//
//	//return rotatedColor;
//}
//
//
//technique11 DefaultTechnique
//{
//	pass BackGround
//	{
//		SetRasterizerState(RS_Default);
//		SetDepthStencilState(DSS_Default, 0);
//		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
//
//		VertexShader = compile vs_5_0 VS_MAIN();
//		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
//		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
//		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
//		PixelShader = compile ps_5_0 PS_MAIN();
//	}
//
//	pass UI
//	{
//		SetRasterizerState(RS_Default);
//		SetDepthStencilState(DSS_Default, 0);
//		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
//
//		VertexShader = compile vs_5_0 VS_MAIN();
//		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
//		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
//		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
//		PixelShader = compile ps_5_0 PS_MAIN_UI();
//	}
//}