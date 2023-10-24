#include "..\Public\MagicSlot.h"
#include "GameInstance.h"
#include "MagicBallPool.h"

CMagicSlot::CMagicSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CMagicSlot::CMagicSlot(const CMagicSlot& rhs)
	: CComposite(rhs)
{
}

HRESULT CMagicSlot::Initialize_Prototype()
{
	return S_OK;
}

//Add Basic Magic 
HRESULT CMagicSlot::Initialize(void* pArg)
{
	//Basic Magic Shield
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PROTEGO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.fLifeTime = 1.3f;
		Add_Magics(magicInitDesc);
	}
	//Basic Magic BasicCast
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_ATTACK_LIGHT;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = BASICCAST;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 0.3f;
		Add_Magics(magicInitDesc);
	}

	Add_Magic_To_Basic_Slot(0, BASICCAST);
	Add_Magic_To_Basic_Slot(1, PROTEGO);
	try /* Check Add_Components */
	{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Magic_Sound_Manager"),
		TEXT("Com_Magic_Sound_Manager"), reinterpret_cast<CComponent**>(&m_pMagic_SoundMgr))))
		throw TEXT("Com_Magic_Sound_Manager");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CMagicSlot::Tick(_float fTimeDelta)
{
	for (auto magic : m_MagicSlots)
	{
		if (magic == nullptr)
			continue;

		magic->Tick(fTimeDelta);
	}

	for (auto magic : m_MagicEssentialSlots)
	{
		if (magic == nullptr)
			continue;

		magic->Tick(fTimeDelta);
	}
}

HRESULT CMagicSlot::Add_Magics(CMagic::MAGICDESC SkillDesc)
{
	//if 'SkillDesc.eMagicTag' Index Magic is Already exist, Reset MagicDesc
	if (m_Magics[SkillDesc.eMagicTag] != nullptr)
	{
		if (FAILED(m_Magics[SkillDesc.eMagicTag]->ResetMagicDesc(SkillDesc))) 
		{
			MSG_BOX("Failed to ResetMagicDesc");
		}
	}
	else
	{
		BEGININSTANCE;

		m_Magics[SkillDesc.eMagicTag] = (dynamic_cast<CMagic*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Magic"), &SkillDesc)));
		if(m_Magics[SkillDesc.eMagicTag] == nullptr)
			__debugbreak();

		ENDINSTANCE;
	}

	return S_OK;
}

HRESULT CMagicSlot::Add_Magic_To_Skill_Slot(_uint iSlotIndex, SPELL eSpellType)
{
	if(m_Magics[eSpellType] == nullptr)
		MSG_BOX("Failed to Add Skill to Slot Because Skill is Empty");

	//if Skill Slot is not nullptr, Release Slot Magic
	if (m_MagicSlots[iSlotIndex] != nullptr)
		Safe_Release(m_MagicSlots[iSlotIndex]);

	m_MagicSlots[iSlotIndex] = m_Magics[eSpellType];
	Safe_AddRef(m_MagicSlots[iSlotIndex]);
	return S_OK;
}

HRESULT CMagicSlot::Add_Magic_To_Basic_Slot(_uint iSlotIndex, SPELL eSpellType)
{
	if (m_Magics[eSpellType] == nullptr)
		MSG_BOX("Failed to Add Skill to Slot Because Skill is Empty");

	//if Skill Slot is not nullptr, Release Slot Magic
	if (m_MagicEssentialSlots[iSlotIndex] != nullptr)
		Safe_Release(m_MagicEssentialSlots[iSlotIndex]);

	m_MagicEssentialSlots[iSlotIndex] = m_Magics[eSpellType];
	Safe_AddRef(m_MagicEssentialSlots[iSlotIndex]);

	return S_OK;
}

CMagicBall* CMagicSlot::Action_Magic_Skill(_uint iIndex, const CGameObject* pTarget, const CGameObject* pWeaponMatrix, COLLISIONFLAG eCollisionFlag, COLLISIONFLAG eThisCollisionFlag, _bool isPowerUp)
{

	//if Size over = MSG / if Index Slot is nullptr, nothing
	if (iIndex < m_MagicSlots.size())
	{
		if (m_MagicSlots[iIndex] == nullptr)
		{
			return nullptr;
		}
		
		m_pMagic_SoundMgr->Spell_Magic(m_eOwnerType,m_MagicSlots[iIndex]->Get_SpellType(), m_fVolum);
		return m_MagicSlots[iIndex]->Magic_Cast(pTarget, pWeaponMatrix, eCollisionFlag, eThisCollisionFlag, isPowerUp);
	}
	else if (m_MagicSlots[iIndex] == nullptr)
	{

	}
	else
		MSG_BOX("Skill Cast Index is out of Size");
	return nullptr;
}

CMagicBall* CMagicSlot::Action_Magic_Basic(_uint iIndex, const CGameObject* pTarget, const CGameObject* pWeaponMatrix, COLLISIONFLAG eCollisionFlag, COLLISIONFLAG eThisCollisionFlag, _bool isPowerUp)
{
	//if Size over = MSG / if Index Slot is nullptr, nothing
	if (iIndex < m_MagicEssentialSlots.size())
	{
		if (m_MagicEssentialSlots[iIndex] == nullptr)
		{
			return nullptr;
		}
		m_pMagic_SoundMgr->Spell_Magic(m_eOwnerType,m_MagicEssentialSlots[iIndex]->Get_SpellType(), m_fVolum);
		return m_MagicEssentialSlots[iIndex]->Magic_Cast(pTarget, pWeaponMatrix, eCollisionFlag, eThisCollisionFlag, isPowerUp);
	}
	else if (m_MagicEssentialSlots[iIndex] == nullptr)
	{

	}
	else
		MSG_BOX("Basic Cast Index is out of Size");
	return nullptr;
}

CMagicSlot* CMagicSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagicSlot* pInstance = New CMagicSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagicSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMagicSlot::Clone(void* pArg)
{
	CMagicSlot* pInstance = New CMagicSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMagic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagicSlot::Free()
{
	if (m_isCloned)
	{
		Safe_Release(m_pMagic_SoundMgr);
	}

	for (auto magic : m_Magics)
	{
		Safe_Release(magic);
	}

	for (auto magic : m_MagicSlots)
	{
		Safe_Release(magic);
	}

	for (auto magic : m_MagicEssentialSlots)
	{
		Safe_Release(magic);
	}

	__super::Free();
}
