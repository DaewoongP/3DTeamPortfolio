#include "Magic.h"
#include "MagicBallPool.h"
#include "GameInstance.h"

CMagic::CMagic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

// 재생성될때 값 가지고 올 필요 없음.
// 클론할때 데이터 입력해줄거임.
CMagic::CMagic(const CMagic& rhs)
	: CComposite(rhs)
{
}

HRESULT CMagic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMagic::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	MAGICDESC* InitDesc = static_cast<MAGICDESC*>(pArg);
	m_eMagicGroup = InitDesc->eMagicGroup;
	m_eMagicType = InitDesc->eMagicType;
	m_eBuffType = InitDesc->eBuffType;
	m_fInitCoolTime = InitDesc->fCoolTime;
	m_iDamage = InitDesc->iDamage;
	m_fCastDistance = InitDesc->fCastDistance;
	m_fBallDistance = InitDesc->fBallDistance;
	m_eMagicTag = InitDesc->eMagicTag;
	m_fLifeTime = InitDesc->fLifeTime;

	return S_OK;
}

void CMagic::Tick(_float fTimeDelta)
{
	if (m_fCurrentCoolTime > 0)
	{
		m_fCurrentCoolTime -= fTimeDelta;
	}

	__super::Tick(fTimeDelta);
}

void CMagic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMagic::ResetMagicDesc(MAGICDESC SkillDesc)
{
	m_eMagicGroup = SkillDesc.eMagicGroup;
	m_eMagicType = SkillDesc.eMagicType;
	m_eBuffType = SkillDesc.eBuffType;
	m_fInitCoolTime = SkillDesc.fCoolTime;
	m_iDamage = SkillDesc.iDamage;
	m_fCastDistance = SkillDesc.fCastDistance;
	m_fBallDistance = SkillDesc.fBallDistance;
	m_eMagicTag = SkillDesc.eMagicTag;
	m_fLifeTime = SkillDesc.fLifeTime;

	return S_OK;
}

CMagicBall* CMagic::Magic_Cast(CTransform* pTarget, _float4x4 targetOffsetMatrix, const _float4x4* pWeaponMatrix, _float4x4 WeaponOffsetMatrix, COLLISIONFLAG eCollisionFlag)
{
	if (m_fCurrentCoolTime > 0)
		return nullptr;

	//마법을 생성 합니다.
	CMagicBall::MAGICBALLINITDESC ballInit;
	ballInit.eBuffType = m_eBuffType;
	ballInit.eMagicGroup = m_eMagicGroup;
	ballInit.eMagicTag = m_eMagicTag;
	ballInit.eMagicType = m_eMagicType;
	ballInit.eCollisionFlag = eCollisionFlag;
	ballInit.iDamage = m_iDamage;
	ballInit.fDistance = m_fBallDistance;
	ballInit.pTarget = pTarget;
	ballInit.TargetOffsetMatrix = targetOffsetMatrix;
	ballInit.fLifeTime = m_fLifeTime;
	ballInit.pWeaponMatrix = pWeaponMatrix;
	ballInit.WeaponOffsetMatrix = WeaponOffsetMatrix;

	BEGININSTANCE;

	//타입별 생성을 위한 태그지정임.
	_tchar objTag[MAX_PATH] = TEXT("GameObject_MagicBall_");
	swprintf_s(objTag, TEXT("%s%s"), objTag, Generate_HashtagW().c_str());

	_char msgBoxText[MAX_PATH] = "Failed Add_GameObject : GameObject_";
	_char objName[MAX_PATH] = "";
	WCharToChar(m_szTagArray[m_eMagicTag], objName);
	sprintf_s(msgBoxText, "%s%s", msgBoxText, objName);

	CMagicBallPool* pMagicBallPool = CMagicBallPool::GetInstance();
	Safe_AddRef(pMagicBallPool);
	
	if (FAILED(pGameInstance->Add_Component(pMagicBallPool->Get_Magic(ballInit), LEVEL_STATIC, TEXT("Layer_Magic"), objTag)))
	{
		MSG_BOX(msgBoxText);
		ENDINSTANCE;
		Safe_Release(pMagicBallPool);
		return nullptr;
	}
	Safe_Release(pMagicBallPool);

	CMagicBall* pMagicBall = dynamic_cast<CMagicBall*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Magic"), objTag));
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);

	ENDINSTANCE;

	for (_uint i = 0; i < m_ActionVec.size(); i++)
	{
		m_ActionVec[i]();
	}

	m_fCurrentCoolTime = m_fInitCoolTime;

	return pMagicBall;
}

HRESULT CMagic::Add_ActionFunc(function<void()> func)
{
	m_ActionVec.push_back(func);
	return S_OK;
}

CMagic* CMagic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagic* pInstance = New CMagic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMagic* CMagic::Clone(void* pArg)
{
	CMagic* pInstance = New CMagic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMagic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagic::Free()
{
	__super::Free();
}
