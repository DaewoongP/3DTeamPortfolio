#include "Weapon_Player_Wand.h"
#include "GameInstance.h"
#include "Player.h"

CWeapon_Player_Wand::CWeapon_Player_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Player_Wand::CWeapon_Player_Wand(const CWeapon_Player_Wand& rhs)
	: CParts(rhs)
{
}

HRESULT CWeapon_Player_Wand::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWeapon_Player_Wand::Initialize(void* pArg)
{
	CWEAPON_PLAYER_WAND_DESC* pCweapon_Player_Wand_Desc = static_cast<CWEAPON_PLAYER_WAND_DESC*>(pArg);
	
	if (FAILED(__super::Initialize(&pCweapon_Player_Wand_Desc->ParentMatrixDesc)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;


	m_pisLightOn = pCweapon_Player_Wand_Desc->pisLightOn;

	//�Ž����� ������ ���� �� ������ ã�� ����.(������ �� ������ ���� ������.)
	vector<class CMesh*>* meshVec = m_pModelCom->Get_MeshesVec();

	_float dist = 0;
	_float sub = 0;
	// ��� �Ž����� ��ȸ�ϸ鼭
	for (auto mesh : (*meshVec))
	{
		// ��� �������� ��ȸ�ϸ鼭
		vector<_float3>* pointVec = mesh->Get_VerticesPositionVec();
		for (auto point : (*pointVec))
		{
			// �ϵ��� �������� ���� �� ���� ã�´�.
			sub = point.Length();
			if (sub > dist)
			{
				dist = sub;
				m_OffsetMatrix = _float4x4(XMMatrixTranslation(point.x, point.y, point.z)) * m_pModelCom->Get_PivotFloat4x4();
			}
		}
	}

	BEGININSTANCE;

	CLight::LIGHTDESC LightInfo;
	ZEROMEM(&LightInfo);
	LightInfo.eType = CLight::TYPE_POINT;
	LightInfo.vPos = _float4(_float4(m_OffsetMatrix.Translation().x,
		m_OffsetMatrix.Translation().y,
		m_OffsetMatrix.Translation().z, 1.f));

	LightInfo.fRange = 5.f;
	LightInfo.fSpotPower = 2.f;
	LightInfo.vAmbient = BLACKDEFAULT;
	LightInfo.vSpecular = BLACKDEFAULT;
	LightInfo.vDiffuse = BLACKDEFAULT;

	if (FAILED(pGameInstance->Add_Light(LightInfo, &m_pLight, true, 1)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}
	
	pGameInstance->Set_IsShadowRender(1, false);

	ENDINSTANCE;

 	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CWeapon_Player_Wand::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Do_Lumos(fTimeDelta);
}

void CWeapon_Player_Wand::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWeapon_Player_Wand::Render()
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Failed Bind_Material : g_DiffuseTexture");

			if (FAILED(m_pShaderCom->Begin("Mesh")))
				throw TEXT("Failed Begin : Mesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Failed Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CWeapon_Player_Wand] Failed Render : \n");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
			__debugbreak();

			return E_FAIL;
		}
	}

	return S_OK;
}

void CWeapon_Player_Wand::Do_Lumos(_float fTimeDelta)
{
	DelayTime += fTimeDelta;
	BEGININSTANCE;
	/*if (false == *m_pisLightOn)
	{
		AccTime = 0.f;
		DelayTime = 0.0f;
	}
	else if (true == *m_pisLightOn)
	{
		AccTime = 0.f;
		DelayTime = 0.0f;
	}*/
	if (m_isPreLight != *m_pisLightOn)
	{
		AccTime = 0.f;
		DelayTime = 0.0f;
	}

	m_isPreLight = *m_pisLightOn;

	if (*m_pisLightOn && DelayTime >= 0.1f)
	{
		CLight::LIGHTDESC LightInfo;
		ZEROMEM(&LightInfo);

		if (nullptr == m_pOwner ||
			nullptr == m_pLight)
		{
			ENDINSTANCE;
			return;
		}
		CTransform* pOwnerTrans = static_cast<CGameObject*>(m_pOwner)->Get_Transform();

		_float3 vPos = pOwnerTrans->Get_Position();
		_float3 vLook = XMVector3Normalize(pOwnerTrans->Get_Look()) * 2.f;
		LightInfo.eType = CLight::TYPE_POINT;
		LightInfo.vPos = (vPos + vLook + _float3(0.f, 2.5f, 0.f)).TransCoord();
		LightInfo.vLookAt = vPos.TransCoord();
		LightInfo.fRange = 17.f;
		LightInfo.fSpotPower = 2.f;
		if (AccTime < 1.f)
			AccTime += fTimeDelta * 3.f;

		m_LightIntensity = XMVectorLerp(BLACKDEFAULT, WHITEDEFAULT, AccTime);
		LightInfo.vAmbient = m_LightIntensity;
		LightInfo.vSpecular = m_LightIntensity;
		LightInfo.vDiffuse = m_LightIntensity;
		pGameInstance->Set_IsShadowRender(1, true);
		pGameInstance->Update_ShadowMatrix(1, LightInfo);
		m_pLight->Set_LightDesc(LightInfo);

		if (LEVEL_VAULT == pGameInstance->Get_CurrentLevelIndex())
		{
			LightInfo.vPos = (vPos + vLook + _float3(0.f, 2.5f, 0.f)).TransCoord();
			LightInfo.vLookAt = (vPos + vLook * 2.f).TransCoord();
			pGameInstance->Set_IsShadowRender(0, true);
			pGameInstance->Update_ShadowMatrix(0, LightInfo);
			if (nullptr == m_pVaultLight)
			{
				pGameInstance->Add_Light(LightInfo, &m_pVaultLight, true, 0);
			}
			else
				m_pVaultLight->Set_LightDesc(LightInfo);
		}
	}
	else if (false == *m_pisLightOn && DelayTime >= 0.1f)
	{
		CLight::LIGHTDESC LightInfo;
		ZEROMEM(&LightInfo);

		if (nullptr == m_pOwner ||
			nullptr == m_pLight)
		{
			ENDINSTANCE;
			return;
		}

		CTransform* pOwnerTrans = static_cast<CGameObject*>(m_pOwner)->Get_Transform();

		_float3 vPos = pOwnerTrans->Get_Position();
		_float3 vLook = XMVector3Normalize(pOwnerTrans->Get_Look()) * 2.f;
		LightInfo.eType = CLight::TYPE_POINT;
		LightInfo.vPos = (vPos + vLook + _float3(0.f, 2.5f, 0.f)).TransCoord();
		LightInfo.vLookAt = vPos.TransCoord();
		LightInfo.fRange = 17.f;
		LightInfo.fSpotPower = 0.f;
		if (AccTime < 1.f)
			AccTime += fTimeDelta * 3.f;

		m_LightIntensity = XMVectorLerp(WHITEDEFAULT, BLACKDEFAULT, AccTime);

		LightInfo.vAmbient = m_LightIntensity;
		LightInfo.vSpecular = m_LightIntensity;
		LightInfo.vDiffuse = m_LightIntensity;
		pGameInstance->Set_IsShadowRender(1, false);
		pGameInstance->Update_ShadowMatrix(1, LightInfo);
		m_pLight->Set_LightDesc(LightInfo);

		if (LEVEL_VAULT == pGameInstance->Get_CurrentLevelIndex())
		{
			LightInfo.vPos = (vPos + vLook + _float3(0.f, 2.5f, 0.f)).TransCoord();
			LightInfo.vLookAt = (vPos + vLook * 2.f).TransCoord();
			pGameInstance->Set_IsShadowRender(0, false);
			pGameInstance->Update_ShadowMatrix(0, LightInfo);
			if (nullptr == m_pVaultLight)
			{
				pGameInstance->Add_Light(LightInfo, &m_pVaultLight, false, 0);
			}
			else
				m_pVaultLight->Set_LightDesc(LightInfo);
		}
	}

	ENDINSTANCE;
}

HRESULT CWeapon_Player_Wand::Add_Components(void* pArg)
{
	try /* Check Add_Components */
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Player_Wand"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom, this)))
			throw TEXT("Failed Add_Component : Com_Model");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
			(CComponent**)&m_pRendererCom, this)))
			throw TEXT("Failed Add_Component : Com_Renderer");

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader_Mesh"),
			(CComponent**)&m_pShaderCom, this)))
			throw TEXT("Failed Add_Component : Com_Shader_Mesh");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWeapon_Player_Wand] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Player_Wand::Set_Shader_Resources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWeapon_Player_Wand] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CWeapon_Player_Wand* CWeapon_Player_Wand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Player_Wand* pInstance = New CWeapon_Player_Wand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Player_Wand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CWeapon_Player_Wand* CWeapon_Player_Wand::Clone(void* pArg)
{
	CWeapon_Player_Wand* pInstance = New CWeapon_Player_Wand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Player_Wand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Player_Wand::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
		Safe_Release(m_pLight);
		Safe_Release(m_pVaultLight);
	}
}
