#include "Trail.h"
#include "GameInstance.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CTrail::CTrail(const CTrail& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_wstrPath(rhs.m_wstrPath)
	, m_iTrailNum(rhs.m_iTrailNum)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_HighLocalMatrix(rhs.m_HighLocalMatrix)
	, m_LowLocalMatrix(rhs.m_LowLocalMatrix)
	, m_isEnable(rhs.m_isEnable)
	, m_vHeadColor(rhs.m_vHeadColor)
	, m_vTailColor(rhs.m_vTailColor)
	, m_fWidth(rhs.m_fWidth)
	, m_fTailDuration(rhs.m_fTailDuration)
{

}

void CTrail::Stright_Move(_float3 vTargerPosition, _float3 vStartPosition, _float fLerpAcc)
{
	_float3 movedPos = XMVectorLerp(vStartPosition, vTargerPosition, fLerpAcc);
	m_pTransform->Set_Position(movedPos);
}

void CTrail::Spin_Move(_float3 vTargerPosition, _float3 vStartPosition, _float fLerpAcc)
{
	_float3 movedPos = XMVectorLerp(vStartPosition, vTargerPosition, fLerpAcc);
	_float4x4 transMatirx = XMMatrixTranslation(movedPos.x, movedPos.y, movedPos.z);
	_float3 axis = XMVector3Normalize(vTargerPosition - vStartPosition);
	_float3 tempAxis = _float3(0, 1, 0);
	_float3	normal = XMVector3Cross(axis, tempAxis);
	_float4x4 offsetMatirx = XMMatrixTranslation(normal.x, normal.y, normal.z);
	_float4x4 rotationMatrix = XMMatrixRotationAxis(axis, fLerpAcc * 100);
	_float4x4 CombineMatrix = offsetMatirx * rotationMatrix * transMatirx;
	m_pTransform->Set_Position(_float3(CombineMatrix.m[3][0], CombineMatrix.m[3][1], CombineMatrix.m[3][2]));
}

void CTrail::Spline_Move(_float3 vSpline01, _float3 vTargerPosition, _float3 vStartPosition,  _float3 vSpline02,_float fLerpAcc)
{
	_float3 movedPos = XMVectorCatmullRom(vSpline01, vStartPosition, vTargerPosition, vSpline02, fLerpAcc);
	m_pTransform->Set_Position(movedPos);
}

HRESULT CTrail::Save(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	WriteFile(hFile, m_strPass.data(), sizeof(char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, m_wstrPath.data(), sizeof(wchar_t) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isEnable, sizeof(m_isEnable), &dwByte, nullptr);
	WriteFile(hFile, &m_iTrailNum, sizeof(m_iTrailNum), &dwByte, nullptr);
	WriteFile(hFile, &m_iTrailNum, sizeof(m_iTrailNum), &dwByte, nullptr);
	WriteFile(hFile, &m_PivotMatrix, sizeof(m_PivotMatrix), &dwByte, nullptr);
	WriteFile(hFile, &m_HighLocalMatrix, sizeof(m_HighLocalMatrix), &dwByte, nullptr);
	WriteFile(hFile, &m_LowLocalMatrix, sizeof(m_LowLocalMatrix), &dwByte, nullptr);
	WriteFile(hFile, &m_vHeadColor, sizeof(m_vHeadColor), &dwByte, nullptr);
	WriteFile(hFile, &m_vTailColor, sizeof(m_vTailColor), &dwByte, nullptr);
	WriteFile(hFile, &m_fWidth, sizeof(m_fWidth), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTrail::Load(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	char szBuffer[MAX_PATH];
	wchar_t wszBuffer[MAX_PATH];
	_ulong dwByte = 0;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	ReadFile(hFile, szBuffer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
	m_strPass = pGameInstance->Make_Char(szBuffer);
	
	ReadFile(hFile, wszBuffer, sizeof(wchar_t) * MAX_PATH, &dwByte, nullptr);
	m_wstrPath = pGameInstance->Make_WChar(wszBuffer);

	ReadFile(hFile, &m_isEnable, sizeof(m_isEnable), &dwByte, nullptr);
	ReadFile(hFile, &m_iTrailNum, sizeof(m_iTrailNum), &dwByte, nullptr);
	ReadFile(hFile, &m_iTrailNum, sizeof(m_iTrailNum), &dwByte, nullptr);
	ReadFile(hFile, &m_PivotMatrix, sizeof(m_PivotMatrix), &dwByte, nullptr);
	ReadFile(hFile, &m_HighLocalMatrix, sizeof(m_HighLocalMatrix), &dwByte, nullptr);
	ReadFile(hFile, &m_LowLocalMatrix, sizeof(m_LowLocalMatrix), &dwByte, nullptr);
	ReadFile(hFile, &m_vHeadColor, sizeof(m_vHeadColor), &dwByte, nullptr);
	ReadFile(hFile, &m_vTailColor, sizeof(m_vTailColor), &dwByte, nullptr);
	ReadFile(hFile, &m_fWidth, sizeof(m_fWidth), &dwByte, nullptr);

	Safe_Release(pGameInstance);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTrail::Initialize_Prototype(const _tchar* _pFilePath, _uint _iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	 this->Load(_pFilePath);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_iLevel = _iLevel;
	// 필요한 텍스처가 없으면 로드하는 로직.
	// 파일명을 통해 태그를 만든다.
	wstring ProtoTag;

	// 필요한 원본 버퍼가 없을 시에 컴포넌트 매니저에 원본 추가.
	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_wstrPath.c_str()));
	}

	// 필요한 텍스처가 없을 시에 컴포넌트 매니저에 원본 추가.
	ProtoTag = TEXT("Prototype_Component_Texture_CustomLinearGradient");
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Gradients/CustomLinearGradient.png")));
	}

	// 필요한 원본 텍스처가 없을 시에 컴포넌트 매니저에 원본 추가.
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Trail")))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_VIBuffer_Rect_Trail")
			, CVIBuffer_Rect_Trail::Create(m_pDevice, m_pContext));
	}

	// 필요한 원본 셰이더가 없을 시에 컴포넌트 매니저에 원본 추가.
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_Trail")))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_Shader_Trail")
			, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"),
				VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements));
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

void CTrail::Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;
	__super::Tick(fTimeDelta);
	if (nullptr != m_pBuffer)
		m_pBuffer->Tick();

}

void CTrail::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLOOM, this);
	}
		
}

HRESULT CTrail::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_strPass.c_str())))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail::Add_Components()
{
	try
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw(TEXT("Com_Renderer"));

		/* Com_Shader */
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_Trail"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
			throw(TEXT("Com_Shader"));

		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrPath.c_str()).c_str(),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
			throw(TEXT("Com_Texture"));

		/* Com_GradientTexture */
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Texture_CustomLinearGradient"),
			TEXT("Com_GradientTexture"), reinterpret_cast<CComponent**>(&m_pGradientTexture))))
			throw(TEXT("Com_GradientTexture"));

		/* Com_Buffer */
		CVIBuffer_Rect_Trail::TRAILDESC trailDesc;
		m_HighLocalMatrix.Translation(_float3(0.f, m_fWidth * 0.5f, 0.f));
		m_LowLocalMatrix.Translation(_float3(0.f, -m_fWidth * 0.5f, 0.f));

		trailDesc.iTrailNum = m_iTrailNum;
		trailDesc.pHighLocalMatrix = &m_HighLocalMatrix;
		trailDesc.pLowLocalMatrix = &m_LowLocalMatrix;
		trailDesc.pPivotMatrix = &m_PivotMatrix;
		trailDesc.pWorldMatrix = m_pTransform->Get_WorldMatrixPtr();
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Trail"),
			TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &trailDesc)))
			throw(TEXT("Com_Buffer"));
	}
	catch (const _tchar* ErrorMessage)
	{
		wstring wstrErrorMSG = TEXT("Failed SetupResources : ");
		wstrErrorMSG += ErrorMessage;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrail::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix = _float4x4();
	try
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", (m_LocalSpace==nullptr)? &WorldMatrix : m_LocalSpace)))
			throw "g_WorldMatrix";

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw "g_ViewMatrix";

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw "g_ProjMatrix";

		if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_AlphaTexture")))
			throw "g_AlphaTexture";

		if (FAILED(m_pGradientTexture->Bind_ShaderResource(m_pShader, "g_GradientTexture")))
			throw "g_GradientTexture";

		if (FAILED(m_pShader->Bind_RawValue("g_vHeadColor", &m_vHeadColor, sizeof m_vHeadColor)))
			throw "g_vHeadColor";

		if (FAILED(m_pShader->Bind_RawValue("g_vTailColor", &m_vTailColor, sizeof m_vTailColor)))
			throw "g_vTailColor";
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed SetupResources : ");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* _pDirectoryPath, _uint _iLevel)
{
	CTrail* pInstance = New CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath, _iLevel)))
	{
		MSG_BOX("Failed to Created CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = New CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pShader);
		Safe_Release(m_pBuffer);
		Safe_Release(m_pTexture);
		Safe_Release(m_pGradientTexture);
	}
}
