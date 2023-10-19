#include "Enemy.h"
#include "GameInstance.h"

#include "Player.h"
#include "UI_Group_Enemy_HP.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4* pWorldMatric = reinterpret_cast<_float4x4*>(pArg);
		m_pTransform->Set_WorldMatrix(*pWorldMatric);
	}
	else
		m_pTransform->Set_Position(_float3(_float(rand() % 5) + 20.f, 2.f, 20.f - _float(rand() % 5)));

	BEGININSTANCE;
	if (nullptr == m_pPlayer)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	}
	ENDINSTANCE;

	if (nullptr == m_pPlayer)
		return E_FAIL;

	//미리 구조체 만들어주기~
	m_pTransform->Get_Position();
	{
		STICK_TICKDESC* pStickDesc = new STICK_TICKDESC();
		pStickDesc->pPosition = &m_vCurrentPosition;

		m_MagicTickDesc.emplace(BUFF_LEVIOSO, pStickDesc);
	}
	{
		STICK_4_TICKDESC* pStick4Desc = new STICK_4_TICKDESC();
		pStick4Desc->pHand[0] = &m_vCurrentPosition;
		pStick4Desc->pHand[1] = &m_vCurrentPosition;
		pStick4Desc->pFoot[0] = &m_vCurrentPosition;
		pStick4Desc->pFoot[1] = &m_vCurrentPosition;

		m_MagicTickDesc.emplace(BUFF_DESCENDO, pStick4Desc);
	}
	{
		STICK_TICKDESC* pStickDesc = new STICK_TICKDESC();
		pStickDesc->pPosition = &m_vCurrentPosition;

		m_MagicTickDesc.emplace(BUFF_LEVIOSO_TONGUE, pStickDesc);
	}

	return S_OK;
}

void CEnemy::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_vCurrentPosition = m_pTransform->Get_Position();

	m_pUI_HP->Tick(fTimeDelta);
}

void CEnemy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CEnemy::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrCollisionTag = CollisionEventDesc.pThisCollisionTag;

	if (wstring::npos != wstrCollisionTag.find(TEXT("MapObject")))
	{
		m_isOnGround = true;
	}
}

void CEnemy::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrCollisionTag = CollisionEventDesc.pThisCollisionTag;

	if (wstring::npos != wstrCollisionTag.find(TEXT("MapObject")))
	{
		m_isOnGround = false;
	}
}

HRESULT CEnemy::Render()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
				throw TEXT("Bind_Material Normal");

			if (FAILED(m_pShaderCom->Begin("AnimMesh")))
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CEnemy] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEnemy::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pShadowShaderCom->Begin("Shadow")))
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CEnemy] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEnemy::Make_AI()
{
	BEGININSTANCE;

	try
	{
		if (FAILED(m_pRootBehavior->Add_Type("pTransform", m_pTransform)))
			throw TEXT("Failed Add_Type pTransform");
		if (FAILED(m_pRootBehavior->Add_Type("pModel", m_pModelCom)))
			throw TEXT("Failed Add_Type pModel");
		if (FAILED(m_pRootBehavior->Add_Type("pHealth", m_pHealth)))
			throw TEXT("Failed Add_Type pHealth");
		if (FAILED(m_pRootBehavior->Add_Type("pRigidBody", m_pRigidBody)))
			throw TEXT("Failed Add_Type pRigidBody");

		if (FAILED(m_pRootBehavior->Add_Type("isSpawn", &m_isSpawn)))
			throw TEXT("Failed Add_Type isSpawn");
		if (FAILED(m_pRootBehavior->Add_Type("isParring", &m_isParring)))
			throw TEXT("Failed Add_Type isParring");
		if (FAILED(m_pRootBehavior->Add_Type("isOnGround", &m_isOnGround)))
			throw TEXT("Failed Add_Type isOnGround");
		if (FAILED(m_pRootBehavior->Add_Type("isHitCombo", &m_isHitCombo)))
			throw TEXT("Failed Add_Type isHitCombo");
		if (FAILED(m_pRootBehavior->Add_Type("isHitAttack", &m_isHitAttack)))
			throw TEXT("Failed Add_Type isHitAttack");
		if (FAILED(m_pRootBehavior->Add_Type("iCurrentSpell", &m_iCurrentSpell)))
			throw TEXT("Failed Add_Type iCurrentSpell");
		if (FAILED(m_pRootBehavior->Add_Type("isChangeAnimation", &m_isChangeAnimation)))
			throw TEXT("Failed Add_Type isChangeAnimation");

		if (FAILED(m_pRootBehavior->Add_Type("fTargetDistance", _float())))
			throw TEXT("Failed Add_Type fTargetDistance");
		if (FAILED(m_pRootBehavior->Add_Type("fAttackRange", 5.f)))
			throw TEXT("Failed Add_Type fAttackRange");
		if (FAILED(m_pRootBehavior->Add_Type("fTargetToDegree", _float())))
			throw TEXT("Failed Add_Type fTargetToDegree");
		if (FAILED(m_pRootBehavior->Add_Type("isTargetToLeft", _bool())))
			throw TEXT("Failed Add_Type isTargetToLeft");

		if (FAILED(m_pRootBehavior->Add_Type("cppTarget", &m_pTarget)))
			throw TEXT("Failed Add_Type cppTarget");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CEnemy] Failed Make_AI : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CEnemy::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* For.Com_RootBehavior */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RootBehavior"),
			TEXT("Com_RootBehavior"), reinterpret_cast<CComponent**>(&m_pRootBehavior))))
			throw TEXT("Com_RootBehavior");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CEnemy] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy::SetUp_ShaderResources()
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
		wstring wstrErrorMSG = TEXT("[CEnemy] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CEnemy::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShadowShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CEnemy] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CEnemy::Make_Hit_Combo(_Inout_ CSelector* pSelector)
{
	pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsHitCombo = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
				return false;

			return *pIsHitCombo;
		});

	return S_OK;
}

HRESULT CEnemy::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsHitCombo = { nullptr };
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
				return false;
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (BUFF_NONE == *pICurrentSpell || true == *pIsHitCombo)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CEnemy::Make_Fly_Descendo(_Inout_ CSequence* pSequence)
{
	pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pCurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pCurrentSpell)))
				return false;

			return BUFF_DESCENDO & *pCurrentSpell;
		});
	pSequence->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsHitCombo = { nullptr };
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
				return false;
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (true == isFlying(*pICurrentSpell))
				Remove_Fly_Spells();
			if (BUFF_DESCENDO & *pICurrentSpell)
				*pICurrentSpell ^= BUFF_DESCENDO;

			*pIsHitCombo = false;

			return true;
		});

	return S_OK;
}

void CEnemy::Set_Current_Target()
{
	if (false == m_isSpawn)
		return;

	_float3 vPosition = m_pTransform->Get_Position();

	for (auto& Pair : m_RangeInEnemies)
	{
		if (nullptr == m_pTarget)
			m_pTarget = Pair.second;
		else
		{
			_float3 vSrcTargetPosition = m_pTarget->Get_Transform()->Get_Position();
			_float3 vDstTargetPosition = Pair.second->Get_Transform()->Get_Position();

			_float vSrcDistance = _float3::Distance(vPosition, vSrcTargetPosition);
			_float vDstDistance = _float3::Distance(vPosition, vDstTargetPosition);

			m_pTarget = (vSrcDistance < vDstDistance) ? m_pTarget : Pair.second;
		}
	}

	m_isRangeInEnemy = (nullptr == m_pTarget) ? false : true;

	if (false == m_isRangeInEnemy)
	{
		m_pTarget = m_pPlayer;
	}
}

void CEnemy::Tick_Spells()
{
	for (auto iter = m_CurrentTickSpells.begin(); iter != m_CurrentTickSpells.end(); )
	{
		if (iter->first & m_iCurrentSpell)
		{
			auto Desciter = m_MagicTickDesc.find(iter->first);
			if (Desciter != m_MagicTickDesc.end())
			{
				iter->second(Desciter->second);
			}

			++iter;
		}
		else
			iter = m_CurrentTickSpells.erase(iter);
	}
}

HRESULT CEnemy::Remove_GameObject(const wstring& wstrObjectTag)
{
	auto iter = find_if(m_RangeInEnemies.begin(), m_RangeInEnemies.end(), [&](auto& Pair)->_bool
		{
			if (wstring::npos != Pair.first.find(wstrObjectTag))
				return true;

			return false;
		});

	if (iter == m_RangeInEnemies.end())
		return E_FAIL;

	m_RangeInEnemies.erase(iter);

	return S_OK;
}

_bool CEnemy::IsEnemy(const wstring& wstrObjectTag)
{
	if (wstring::npos != wstrObjectTag.find(TEXT("Player")))
		return true;

	if (wstring::npos != wstrObjectTag.find(TEXT("Fig")))
		return true;

	return false;
} 

_bool CEnemy::IsDebuff(const _uint& iType)
{
	if (BUFF_LEVIOSO & iType)
		return true;
	if (BUFF_LEVIOSO_TONGUE & iType)
		return true;
	if (BUFF_DESCENDO & iType)
		return true;

	return false;
}

_bool CEnemy::isFlying(const _uint& iType)
{
	if (BUFF_ACCIO & iType)
		return true;
	if (BUFF_LEVIOSO & iType)
		return true;
	if (BUFF_FLIPENDO & iType)
		return true;

	return false;
}

_bool CEnemy::isCombo(const _uint& iType)
{
	_bool ReturnData = { false };

	/* 내 현재 상태에 디버프가 없는 경우 */
	if (false == IsDebuff(m_iCurrentSpell))
		return false;

	if (BUFF_DESCENDO & m_iCurrentSpell)
	{
		if (iType & BUFF_ATTACK_LIGHT || iType & BUFF_ATTACK_HEAVY)
			return false;

		if (true == isFlying(iType))
		{
			m_iCurrentSpell ^= BUFF_DESCENDO;
			return false;
		}
	}

	if (BUFF_LEVIOSO_TONGUE & m_iCurrentSpell)
	{
		if(iType & BUFF_ATTACK_LIGHT || iType & BUFF_ATTACK_HEAVY)
		return false;

		if (iType & BUFF_DESCENDO)
		{
			m_iCurrentSpell ^= BUFF_LEVIOSO_TONGUE;
			return false;
		}
	}
		

	if (iType & BUFF_ATTACK_LIGHT)
		ReturnData = m_isHitCombo = true;

	else if (iType & BUFF_ATTACK_HEAVY)
		ReturnData = m_isHitCombo = true;

	else if (true == IsDebuff(iType))
		ReturnData = m_isHitCombo = true;

	return ReturnData;
}

_bool CEnemy::isControlSpell(const _uint& iType)
{
	if (BUFF_LEVIOSO & iType)
		return true;

	return false;
}

_bool CEnemy::isPowerSpell(const _uint& iType)
{
	if (BUFF_ACCIO & iType)
		return true;

	if (BUFF_DESCENDO & iType)
		return true;

	if (BUFF_FLIPENDO & iType)
		return true;

	return false;
}

_bool CEnemy::isDamageSpell(const _uint& iType)
{
	if (BUFF_CONFRINGO & iType)
		return true;

	if (BUFF_NCENDIO & iType)
		return true;

	return false;
}

void CEnemy::Remove_Fly_Spells()
{
	if (BUFF_ACCIO & m_iCurrentSpell)
		m_iCurrentSpell ^= BUFF_ACCIO;

	if (BUFF_LEVIOSO & m_iCurrentSpell)
		m_iCurrentSpell ^= BUFF_LEVIOSO;

	if (BUFF_FLIPENDO & m_iCurrentSpell)
		m_iCurrentSpell ^= BUFF_FLIPENDO;

	if (BUFF_LEVIOSO_TONGUE & m_iCurrentSpell)
		m_iCurrentSpell ^= BUFF_LEVIOSO_TONGUE;
}

void CEnemy::On_Gravity()
{
	if (true == m_isDead)
		return;

	if (nullptr != m_pRigidBody)
	{
#ifdef _DEBUG
		cout << "On Gravity" << endl;
#endif // _DEBUG
		m_pRigidBody->Set_Gravity(true);
	}
}

void CEnemy::Off_Gravity()
{
	if (true == m_isDead)
		return;

	if (nullptr != m_pRigidBody)
	{
#ifdef _DEBUG
		cout << "Off Gravity" << endl;
#endif // _DEBUG
		m_pRigidBody->Set_Gravity(false);
	}
}

void CEnemy::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRootBehavior);
		Safe_Release(m_pUI_HP);
		Safe_Release(m_pHealth);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pShadowShaderCom);

		for (auto iter = m_MagicTickDesc.begin(); iter != m_MagicTickDesc.end(); ++iter)
		{
			if(iter->second!=nullptr)
				Safe_Delete(iter->second);
		}
		
	}
}
