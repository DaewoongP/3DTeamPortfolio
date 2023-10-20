#include "Card_Fig.h"
#include "GameInstance.h"

#include "MagicSlot.h"
#include "MagicBall.h"
#include "Weapon_Fig_Wand.h"

CCard_Fig::CCard_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCard_Fig::CCard_Fig(const CCard_Fig& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCard_Fig::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCard_Fig::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CARDFIGINITDESC* pInitDesc = reinterpret_cast<CARDFIGINITDESC*>(pArg);
		m_pParentWorldMatrix = pInitDesc->pParentWorldMatrix;
		m_OffsetMatrix = XMMatrixTranslation(1.f, 0.f, -2.f);
	}

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_pModelCom->Change_Animation(TEXT("Stand_Listen"));

	m_AttackTags.push_back(TEXT("Attack_Cast_Crucio"));
	m_AttackTags.push_back(TEXT("Attack_Cast_Bombarda"));
	m_AttackTags.push_back(TEXT("Attack_Cast_Finisher"));

	return S_OK;
}

void CCard_Fig::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (false == m_isSpawn)
	{
		BEGININSTANCE;
		//_float3 vCameraPosition = pGameInstance->Get_CamPosition()->xyz();
		//_float3 vCameraLook = *pGameInstance->Get_CamLook();
		_float4x4 CameraWorldMatrix = *m_pParentWorldMatrix;//XMMatrixTranslation(vCameraPosition.x, vCameraPosition.y, vCameraPosition.z);
		_float4x4 WorldMatrix = m_OffsetMatrix * CameraWorldMatrix;
		m_pTransform->Set_WorldMatrix(WorldMatrix);
		//m_pTransform->Set_Look(vCameraLook);
		ENDINSTANCE;
	}
	else
	{
		Jump_Up(fTimeDelta);
		Action(fTimeDelta);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CCard_Fig::Late_Tick(_float fTimeDelta)
{
	if (false == m_isSpawn)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CCard_Fig::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");

			if (0 == i)
			{
				if (FAILED(m_pShaderCom->Begin("HairMesh")))
					throw TEXT("Shader Begin HairMesh");
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
					throw TEXT("Bind_Material Normal");

				if (FAILED(m_pShaderCom->Begin("AnimMesh")))
					throw TEXT("Shader Begin AnimMesh");
			}

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CCard_Fig::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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
			wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

void CCard_Fig::Spawn_Fig(const CGameObject* pTarget)
{
	if (nullptr == pTarget)
		return;

	m_pTarget = pTarget;
	m_isSpawn = true;
}

HRESULT CCard_Fig::Make_Magics()
{
	//Skill Magic CRUCIO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_CURSE;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = CRUCIO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic BOMBARDA
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = BOMBARDA;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic FINISHER
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = FINISHER;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, CRUCIO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, BOMBARDA);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(2, FINISHER);

	return S_OK;
}

HRESULT CCard_Fig::Make_Notifies()
{
	function<void()> Func = [&] { (*this).Cast_Crucio(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Crucio"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Cast_Bombarda(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Bombarda"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Cast_Finisher(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Finisher"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCard_Fig::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Professor_Fig"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.85f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.25f, 0.6f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");
		RigidBodyDesc.eThisCollsion = COL_NPC;
		RigidBodyDesc.eCollisionFlag = COL_STATIC;

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Fig_Wand::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Weapon_Fig_Wand"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");

		/* For.MagicSlot */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
			TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
		{
			MSG_BOX("Failed CTest_Player Add_Component : (Com_MagicSlot)");
			return E_FAIL;
		}
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCard_Fig::SetUp_ShaderResources()
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

		_float3 vHairColor = { 0.7f, 0.7f, 0.7f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
			throw TEXT("Failed Bind_RawValue : g_vHairColor");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CCard_Fig::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CCard_Fig::Action(const _float& fTimeDelta)
{
	if (true == m_isAction1)
	{
		m_pModelCom->Change_Animation(TEXT("Jump_Down_Run"));
		m_pRigidBody->Set_Gravity(false);
		m_isAction1 = false;
	}

	if (m_pModelCom->Is_Finish_Animation())
	{
		if (false == m_isAction2)
		{
			m_pModelCom->Change_Animation(TEXT("Soft"));
			m_pRigidBody->Set_Gravity(true);
			m_isAction2 = true;
		}
		else
		{
			if (false == m_isAction3)
			{
				m_pModelCom->Change_Animation(m_AttackTags[m_iCurrentTagIndex++]);
				m_iCurrentTagIndex = (m_iCurrentTagIndex == m_AttackTags.size()) ? 0 : m_iCurrentTagIndex;
				m_isAction3 = true;
			}
			else
			{
				if (false == m_isAction4)
				{
					m_pRigidBody->Set_Gravity(false);
					m_pModelCom->Change_Animation(TEXT("Spawn_Out"));
					m_isAction4 = true;
				}
				else
				{
					m_pRigidBody->Set_Gravity(true);
					m_isAction1 = true;
					m_isAction2 = false;
					m_isAction3 = false;
					m_isAction4 = false;
					m_isSpawn = false;
				}
			}
		}
	}
}

void CCard_Fig::Jump_Up(const _float& fTimeDelta)
{
	if (false == m_isAction4)
	{
		m_fTimeAcc = 0.f;
		return;
	}

	m_fTimeAcc += fTimeDelta;
	_float fWaitTime = 0.4f;

	if (m_fTimeAcc < fWaitTime)
		return;

	_float3 vForce = _float3();
	_float fUpTimeAcc = m_fTimeAcc - fWaitTime;
	_float fUpTime = 1.f;
	vForce = m_pTransform->Get_Up() * 300.f;

	vForce = vForce - vForce * (fUpTimeAcc / (fUpTime / 2.f));

	if (fUpTimeAcc < fUpTime)
	{
		m_pRigidBody->Add_Force(vForce);
	}
}

void CCard_Fig::Cast_Crucio()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CCard_Fig::Cast_Bombarda()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(1, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CCard_Fig::Cast_Finisher()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(2, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CCard_Fig::Shot_Magic()
{
	m_CastingMagic->Do_MagicBallState_To_Next();
}

CCard_Fig* CCard_Fig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCard_Fig* pInstance = New CCard_Fig(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCard_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCard_Fig::Clone(void* pArg)
{
	CCard_Fig* pInstance = New CCard_Fig(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCard_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCard_Fig::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pMagicSlot);
	Safe_Release(m_pWeapon);
	Safe_Release(m_pRigidBody);
}
