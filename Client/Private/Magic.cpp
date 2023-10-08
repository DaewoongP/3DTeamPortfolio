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
	memcpy(&m_MagicDesc, InitDesc,sizeof(MAGICDESC));
	m_fCurrentCoolTime = m_MagicDesc.fInitCoolTime;
	return S_OK;
}

void CMagic::Tick(_float fTimeDelta)
{
	if (m_fCoolMultipleTimer > 0)
	{
		m_fCoolMultipleTimer -= fTimeDelta;
	}
	else 
	{
		m_fCoolSpeed = 1.0f;
	}

	if (m_fCurrentCoolTime > 0)
	{
		m_fCurrentCoolTime -= fTimeDelta * m_fCoolSpeed;
		if(m_fCurrentCoolTime <= 0.0f)
	{
		m_fCurrentCoolTime = 0.0f;
	}
	}
	
	__super::Tick(fTimeDelta);
	
	
}

void CMagic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMagic::ResetMagicDesc(MAGICDESC SkillDesc)
{
	memcpy(&m_MagicDesc, &SkillDesc, sizeof(MAGICDESC));
	m_fCurrentCoolTime = m_MagicDesc.fInitCoolTime;
	return S_OK;
}

CMagicBall* CMagic::Magic_Cast(const CGameObject* pTarget, const CGameObject* pWeapon, COLLISIONFLAG eCollisionFlag,_bool PowerUp)
{
	if (m_fCurrentCoolTime > 0)
		return nullptr;

	//마법을 생성 합니다.
	CMagicBall::MAGICBALLINITDESC ballInit;
	ballInit.eBuffType = m_MagicDesc.eBuffType;
	ballInit.eMagicGroup = m_MagicDesc.eMagicGroup;
	ballInit.eMagicTag = m_MagicDesc.eMagicTag;
	ballInit.eMagicType = m_MagicDesc.eMagicType;
	ballInit.eCollisionFlag = eCollisionFlag;
	ballInit.iDamage = m_MagicDesc.iDamage;
	ballInit.fLifeTime = m_MagicDesc.fLifeTime;
	ballInit.pTarget = pTarget;
	ballInit.pWeapon = pWeapon;
	
	if (PowerUp)
		ballInit.iDamage *= 2;

	BEGININSTANCE;

	//타입별 생성을 위한 태그지정임.
	_tchar objTag[MAX_PATH] = TEXT("GameObject_MagicBall_");
	swprintf_s(objTag, TEXT("%s%s"), objTag, Generate_HashtagW().c_str());

	_char msgBoxText[MAX_PATH] = "Failed Add_GameObject : GameObject_";
	_char objName[MAX_PATH] = "";
	WCharToChar(m_szTagArray[m_MagicDesc.eMagicTag], objName);
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

	m_fCurrentCoolTime = m_MagicDesc.fInitCoolTime;

	return pMagicBall;
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
