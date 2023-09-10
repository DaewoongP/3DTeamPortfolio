#include "Trail.h"
#include "GameInstance.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	m_wstrPath = { TEXT("../../Resources/Effects/Textures/Default_Particle.png") };
}

CTrail::CTrail(const CTrail& rhs)
	: CGameObject(rhs)
	, m_wstrPath(rhs.m_wstrPath)
{
}

HRESULT CTrail::Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	// this->Load(_pDirectoryPath);
	// m_wstrPath = _pDirectoryPath;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 필요한 텍스처가 없으면 로드하는 로직.
	// 파일명을 통해 태그를 만든다.
	wstring ProtoTag;

	// 필요한 원본 버퍼가 없을 시에 컴포넌트 매니저에 원본 추가.
	ProtoTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrPath.c_str());
	if (nullptr == pGameInstance->Find_Prototype(_iLevel, ProtoTag.data()))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(_iLevel
			, ProtoTag.data()
			, CTexture::Create(m_pDevice, m_pContext, m_wstrPath.c_str()));
	}

	// 필요한 원본 텍스처가 없을 시에 컴포넌트 매니저에 원본 추가.
	if (nullptr == pGameInstance->Find_Prototype(_iLevel, TEXT("Prototype_Component_VIBuffer_Rect_Trail")))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(_iLevel
			, TEXT("Prototype_Component_VIBuffer_Rect_Trail")
			, CVIBuffer_Rect_Trail::Create(m_pDevice, m_pContext));
	}

	// 필요한 원본 셰이더 없을 시에 컴포넌트 매니저에 원본 추가.
	if (nullptr == pGameInstance->Find_Prototype(_iLevel, TEXT("Prototype_Component_Shader_Trail")))
	{
		// 없으면 원본을 추가한다.
		pGameInstance->Add_Prototype(_iLevel
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
	__super::Tick(fTimeDelta);

	if (nullptr != m_pBuffer)
		m_pBuffer->Tick();
}

void CTrail::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
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
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_Shader_DefaultEffect"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
			throw(TEXT("Com_Shader"));

		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(0, ToPrototypeTag(TEXT("Prototype_Component_Texture"), m_wstrPath.c_str()).c_str(),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
			throw(TEXT("Com_Texture"));

		/* Com_Buffer */
		CVIBuffer_Rect_Trail::TRAILDESC trailDesc;
		m_HighLocalMatrix.Translation(_float3(0.f, 1.f, 0.f));
		m_LowLocalMatrix.Translation(_float3(0.f, -1.f, 0.f));

		trailDesc.iTrailNum = 20;
		trailDesc.pHighLocalMatrix = &m_HighLocalMatrix;
		trailDesc.pLowLocalMatrix = &m_LowLocalMatrix;
		trailDesc.pPivotMatrix = &m_PivotMatrix;
		trailDesc.pWorldMatrix = m_pTransform->Get_WorldMatrixPtr();
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect_Trail"),
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

	//_float4x4 WorldMatrix = m_pTransform->Get_WorldMatrix();
	_float4x4 WorldMatrix = _float4x4();

	try
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			throw "g_WorldMatrix";

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw "g_ViewMatrix";

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw "g_ProjMatrix";

		if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture")))
			throw "g_Texture";
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

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture);
}
