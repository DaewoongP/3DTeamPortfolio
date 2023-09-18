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
	, m_iMaxIndex(rhs.m_iMaxIndex)
	, m_iWidthLength(rhs.m_iWidthLength)
	, m_iHeightLength(rhs.m_iHeightLength)
	, m_isIncrease(rhs.m_isIncrease)
	, m_strClipChannel(rhs.m_strClipChannel)
	, m_isMixColor(rhs.m_isMixColor)
	, m_vColor(rhs.m_vColor)
	, m_isEnable(rhs.m_isEnable)
	, m_strLoopOption(rhs.m_strLoopOption)
	, m_isUseNormalTexture(rhs.m_isUseNormalTexture)
	, m_wstrNormalPath(rhs.m_wstrNormalPath)
	, m_isDeleteY(rhs.m_isDeleteY)
	, m_fUpdateInterval(rhs.m_fUpdateInterval)
	, m_isUseFlipbookMaterialTexture(rhs.m_isUseFlipbookMaterialTexture)
	, m_wstrFlipbookMaterialPath(rhs.m_wstrFlipbookMaterialPath)
{
}

HRESULT CTexture_Flipbook::Initialize_Prototype(_uint iLevel, const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;
	if (nullptr != pFilePath)
		this->Load(pFilePath);

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

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrNormalPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_wstrNormalPath.c_str()))))
			return E_FAIL;
	}

	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrFlipbookMaterialPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, ProtoTag.data()))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_wstrFlipbookMaterialPath.c_str()))))
			return E_FAIL;
	}

	// 필요한 원본 셰이더가 없을 시에 컴포넌트 매니저에 원본 추가.
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_Component_Shader_Flipbook")))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(m_iLevel
			, TEXT("Prototype_Component_Shader_Flipbook")
			, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Flipbook.hlsl"),
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
	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pTransform->LookAt(pGameInstance->Get_CamPosition()->xyz(), m_isDeleteY);
	Safe_Release(pGameInstance);

	if ("Once" == m_strLoopOption)
	{
		Tick_Once(fTimeDelta);
	}
	else if ("Loop" == m_strLoopOption)
	{
		Tick_Loop(fTimeDelta);
	}
	else if ("Ping-Pong" == m_strLoopOption)
	{
		Tick_PingPong(fTimeDelta);
	}
}

void CTexture_Flipbook::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;
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

	if (FAILED(m_pShader->Begin("Flipbook")))
		return E_FAIL;

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void CTexture_Flipbook::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

HRESULT CTexture_Flipbook::Bind_DiffuseTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pDiffuseTexture->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CTexture_Flipbook::Bind_NormalTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pNormalTexture->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CTexture_Flipbook::Bind_FilpbookMaterialTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pFlipbookMaterialTexture->Bind_ShaderResource(pShader, pConstantName);
}

void CTexture_Flipbook::Play(_float3 vPos)
{
	Set_Position(vPos);
	m_iCurIndex = 0;
	m_isEnable = true;
	m_fTimeAcc = 0.f;
}

void CTexture_Flipbook::Play(_float3 vPos, LOOP_OPTION eLoopOption)
{
	Play(vPos);
	switch (eLoopOption)
	{
	case Engine::CTexture_Flipbook::ONCE:
		m_strLoopOption = "Once";
		break;
	case Engine::CTexture_Flipbook::LOOP:
		m_strLoopOption = "Loop";
		break;
	case Engine::CTexture_Flipbook::PING_PONG:
		m_strLoopOption = "Ping-Pong";
		break;
	}
}

void CTexture_Flipbook::Stop()
{
	m_isEnable = false;
}

HRESULT CTexture_Flipbook::Save(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	_ulong dwByte = 0;

	WriteFile(hFile, &m_iLevel, sizeof(m_iLevel), &dwByte, nullptr);
	WriteFile(hFile, m_wstrPath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_iMaxIndex, sizeof(m_iMaxIndex), &dwByte, nullptr);
	WriteFile(hFile, &m_iWidthLength, sizeof(m_iWidthLength), &dwByte, nullptr);
	WriteFile(hFile, &m_iHeightLength, sizeof(m_iHeightLength), &dwByte, nullptr);
	WriteFile(hFile, m_strClipChannel.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isMixColor, sizeof(m_isMixColor), &dwByte, nullptr);
	WriteFile(hFile, &m_vColor, sizeof(m_vColor), &dwByte, nullptr);
	WriteFile(hFile, &m_isEnable, sizeof(m_isEnable), &dwByte, nullptr);
	WriteFile(hFile, &m_isIncrease, sizeof(m_isIncrease), &dwByte, nullptr);
	WriteFile(hFile, m_strLoopOption.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isUseNormalTexture, sizeof(m_isUseNormalTexture), &dwByte, nullptr);
	WriteFile(hFile, m_wstrNormalPath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isDeleteY, sizeof(m_isDeleteY), &dwByte, nullptr);
	WriteFile(hFile, &m_fUpdateInterval, sizeof(m_fUpdateInterval), &dwByte, nullptr);
	WriteFile(hFile, &m_isUseFlipbookMaterialTexture, sizeof(m_fUpdateInterval), &dwByte, nullptr);
	WriteFile(hFile, m_wstrFlipbookMaterialPath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTexture_Flipbook::Load(const _tchar* pFilePath)
{

	HANDLE hFile = CreateFile(pFilePath
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}
	
	_ulong dwByte = 0;

	_tchar wszBuffer[MAX_PATH];
	_char szBuffer[MAX_PATH];

	ReadFile(hFile, &m_iLevel, sizeof(m_iLevel), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	m_wstrPath = wszBuffer;
	ReadFile(hFile, &m_iMaxIndex, sizeof(m_iMaxIndex), &dwByte, nullptr);
	ReadFile(hFile, &m_iWidthLength, sizeof(m_iWidthLength), &dwByte, nullptr);
	ReadFile(hFile, &m_iHeightLength, sizeof(m_iHeightLength), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_strClipChannel = szBuffer;
	ReadFile(hFile, &m_isMixColor, sizeof(m_isMixColor), &dwByte, nullptr);
	ReadFile(hFile, &m_vColor, sizeof(m_vColor), &dwByte, nullptr);
	ReadFile(hFile, &m_isEnable, sizeof(m_isEnable), &dwByte, nullptr);
	ReadFile(hFile, &m_isIncrease, sizeof(m_isIncrease), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	m_strLoopOption = szBuffer;
	ReadFile(hFile, &m_isUseNormalTexture, sizeof(m_isUseNormalTexture), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	m_wstrNormalPath = wszBuffer;
	ReadFile(hFile, &m_isDeleteY, sizeof(m_isDeleteY), &dwByte, nullptr);
	ReadFile(hFile, &m_fUpdateInterval, sizeof(m_fUpdateInterval), &dwByte, nullptr);
	ReadFile(hFile, &m_isUseFlipbookMaterialTexture, sizeof(m_isUseFlipbookMaterialTexture), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	m_wstrFlipbookMaterialPath = wszBuffer;

	CloseHandle(hFile);

	return S_OK;
}

void CTexture_Flipbook::Tick_Once(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_fUpdateInterval)
		m_fTimeAcc = 0.f;
	else
		return;

	++m_iCurIndex;
	if (m_iCurIndex > m_iMaxIndex)
		m_isEnable = false;
}

void CTexture_Flipbook::Tick_Loop(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_fUpdateInterval)
		m_fTimeAcc = 0.f;
	else
		return;

	++m_iCurIndex;
	if (m_iCurIndex > m_iMaxIndex)
		m_iCurIndex = 0;
}

void CTexture_Flipbook::Tick_PingPong(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_fUpdateInterval)
		m_fTimeAcc = 0.f;
	else
		return;

	if (true == m_isIncrease)
		++m_iCurIndex;
	else
		--m_iCurIndex;

	if (m_iCurIndex > m_iMaxIndex || 0 > m_iCurIndex)
	{
		// Toggle
		m_isIncrease = (true == m_isIncrease) ? false : true;
	}
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
		if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_Component_Shader_Flipbook"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
			throw(TEXT("Com_Shader"));

		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrPath.c_str()).c_str(),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDiffuseTexture))))
			throw(TEXT("Com_Texture"));

		/* Com_NormalTexture */
		if (FAILED(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrNormalPath.c_str()).c_str(),
			TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pNormalTexture))))
			throw(TEXT("Com_NormalTexture"));

		/* Com_FlipbookMaterialTexture */
		if (FAILED(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrFlipbookMaterialPath.c_str()).c_str(),
			TEXT("Com_FlipbookMaterialTexture"), reinterpret_cast<CComponent**>(&m_pFlipbookMaterialTexture))))
			throw(TEXT("Com_FlipbookMaterialTexture"));

		/* Com_Buffer */
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
	if (nullptr != m_pDiffuseTexture)
		FAILED_CHECK_RETURN(m_pDiffuseTexture->Bind_ShaderResource(m_pShader, "g_DiffuseTexture"), E_FAIL);
	if (true == m_isUseNormalTexture)
		FAILED_CHECK_RETURN(m_pNormalTexture->Bind_ShaderResource(m_pShader, "g_NormalTexture"), E_FAIL);
	if (true == m_isUseFlipbookMaterialTexture)
		FAILED_CHECK_RETURN(m_pFlipbookMaterialTexture->Bind_ShaderResource(m_pShader, "g_FlipbookMaterialTexture"), E_FAIL);
	// Raw Values
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iCurIndex", &m_iCurIndex, sizeof m_iCurIndex), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iWidthLength", &m_iWidthLength, sizeof m_iWidthLength), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iHeightLength", &m_iHeightLength, sizeof m_iHeightLength), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_isUseNormalTexture", &m_isUseNormalTexture, sizeof(m_isUseNormalTexture)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_isUseFlipbookMaterialTexture", &m_isUseNormalTexture, sizeof(m_isUseNormalTexture)), E_FAIL);

	_int iClipChannel = { 3 };
	if (m_strClipChannel == "Red") { iClipChannel = 0; }
	else if (m_strClipChannel == "Green") { iClipChannel = 1; }
	else if (m_strClipChannel == "Blue") { iClipChannel = 2; }
	else if (m_strClipChannel == "Alpha") { iClipChannel = 3; }
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_iClipChannel", &iClipChannel, sizeof(iClipChannel)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_isMixColor", &m_isMixColor, sizeof(m_isMixColor)), E_FAIL);
	if (true == m_isMixColor)
		FAILED_CHECK_RETURN(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor)), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

CTexture_Flipbook* CTexture_Flipbook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath)
{
	CTexture_Flipbook* pInstance = New CTexture_Flipbook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel, pFilePath)))
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
	Safe_Release(m_pDiffuseTexture);
	Safe_Release(m_pNormalTexture);
	Safe_Release(m_pFlipbookMaterialTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
}