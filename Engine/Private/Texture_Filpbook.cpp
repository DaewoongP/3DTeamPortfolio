#include "Texture_Filpbook.h"
#include "GameInstance.h"

CTexture_Flipbook::CTexture_Flipbook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTexture_Flipbook::CTexture_Flipbook(const CTexture_Flipbook& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
	, m_wstrPath(rhs.m_wstrPath)
	, m_iCurIndex(rhs.m_iCurIndex)
	, m_iWidthLength(rhs.m_iWidthLength)
	, m_iHeightLength(rhs.m_iHeightLength)
	, m_iMaxIndex(rhs.m_iMaxIndex)
{
}

HRESULT CTexture_Flipbook::Initialize_Prototype(_uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;
	m_wstrPath = pFilePath;
	m_iWidthLength = iWidthNum;
	m_iHeightLength = iHeightNum;
	m_iMaxIndex = m_iWidthLength * m_iHeightLength - 1;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 필요한 원본 텍스처가 없으면 원본 텍스처를 만드는 로직
	wstring ProtoTag;
	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_wstrPath.c_str()))))
			return E_FAIL;
	}

	if (true == m_isUseNormalTexture)
	{
		ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrNormalPath.c_str());
		if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
		{
			if (FAILED(pGameInstance->Add_Prototype(m_iLevel
				, ProtoTag.data()
				, CTexture::Create(m_pDevice, m_pContext, m_wstrNormalPath.c_str()))))
				return E_FAIL;
		}
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

	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Prototype_Component_VIBuffer_Rect")))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_VIBuffer_Rect")
			, CVIBuffer_Rect::Create(m_pDevice, m_pContext));
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTexture_Flipbook::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

void CTexture_Flipbook::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pTransform->LookAt(pGameInstance->Get_CamPosition()->xyz());
	Safe_Release(pGameInstance);
	++m_iCurIndex;
	if (m_iCurIndex > m_iMaxIndex)
		m_iCurIndex = 0;

	cout << m_iCurIndex << endl;
}

void CTexture_Flipbook::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG

#endif // _DEBUG
	}
}

HRESULT CTexture_Flipbook::Render()
{
#ifdef _DEBUG
	// Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Flipbook"), E_FAIL))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render(), E_FAIL))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture_Flipbook::Add_Components()
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

		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"),
			TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
			throw(TEXT("Com_Buffer"));
	}
	catch (const _char* pErrorMessage)
	{
		string strErrorMessage = "Failed to Cloned in CTexture_Flipbook";
		strErrorMessage += pErrorMessage;
		MSG_BOX(strErrorMessage.data());
	}

	return S_OK;
}

HRESULT CTexture_Flipbook::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Matrices
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()), E_FAIL)
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)), E_FAIL);

	// Textures
	FAILED_CHECK_RETURN(m_pTexture->Bind_ShaderResource(m_pShader, "g_AlphaTexture"), E_FAIL);
	if (true == m_isUseNormalTexture && nullptr != m_pNormalTexture)
	{
		FAILED_CHECK_RETURN(m_pNormalTexture->Bind_ShaderResource(m_pShader, "g_NormalTexture"), E_FAIL);
	}

	// Raw Values
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iCurIndex", &m_iCurIndex, sizeof m_iCurIndex), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iWidthLength", &m_iWidthLength, sizeof m_iWidthLength), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iHeightLength", &m_iHeightLength, sizeof m_iHeightLength), E_FAIL);

	_int iClipChannel = { 3 };
	if (m_strClipChannel == "Red") { iClipChannel = 0; }
	else if (m_strClipChannel == "Green") { iClipChannel = 1; }
	else if (m_strClipChannel == "Blue") { iClipChannel = 2; }
	else if (m_strClipChannel == "Alpha") { iClipChannel = 3; }
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iClipChannel", &iClipChannel, sizeof(_int)), E_FAIL);
		
	Safe_Release(pGameInstance);

	return S_OK;
}

CTexture_Flipbook* CTexture_Flipbook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum)
{
	CTexture_Flipbook* pInstance = New CTexture_Flipbook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel, pFilePath, iWidthNum, iHeightNum)))
	{
		MSG_BOX("Failed to Created CTexture_Flipbook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTexture_Flipbook::Clone(void* pArg)
{
	CTexture_Flipbook* pInstance = New CTexture_Flipbook(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTexture_Flipbook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture_Flipbook::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pNormalTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}