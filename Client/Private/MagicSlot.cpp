#include "..\Public\MagicSlot.h"
#include "GameInstance.h"

CMagicSlot::CMagicSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CMagicSlot::CMagicSlot(const CMagicSlot& rhs)
	: CComponent(rhs)
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
		magicInitDesc.eBuffType = CMagic::BUFF_SHILED;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PROTEGO;
		magicInitDesc.fCoolTime = 1.f;
		magicInitDesc.fDamage = 0.f;
		magicInitDesc.fCastDistance = 1000;
		magicInitDesc.fBallDistance = 30;
		magicInitDesc.fLifeTime = 5.f;
		Add_Magics(magicInitDesc);
	}
	//Basic Magic BasicCast
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = CMagic::BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = BASICCAST;
		magicInitDesc.fCoolTime = 0.f;
		magicInitDesc.fDamage = 10.f;
		magicInitDesc.fCastDistance = 1000;
		magicInitDesc.fBallDistance = 30;
		magicInitDesc.fLifeTime = 0.8f;
		Add_Magics(magicInitDesc);
	}

	Add_Magic_To_Basic_Slot(0,BASICCAST);
	Add_Magic_To_Basic_Slot(1,PROTEGO);

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
		ENDINSTANCE;
	}
	return S_OK;
}

HRESULT CMagicSlot::Add_Function_To_Magic(function<void()> func, SPELL eSpellType)
{
	m_Magics[eSpellType]->Add_ActionFunc(func);
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

void CMagicSlot::Action_Magic_Skill(_uint iIndex, CTransform* pTarget, _float4x4 TargetOffsetMatrix, const _float4x4* pWeaponMatrix, _float4x4 WeaponOffsetMatrix)
{
	//if Size over = MSG / if Index Slot is nullptr, nothing
	if (iIndex < m_MagicSlots.size())
	{
		if (m_MagicSlots[iIndex] == nullptr)
		{

		}
		m_MagicSlots[iIndex]->Magic_Cast(pTarget, TargetOffsetMatrix, pWeaponMatrix, WeaponOffsetMatrix);
	}
	else if (m_MagicSlots[iIndex] == nullptr)
	{

	}
	else
		MSG_BOX("Skill Cast Index is out of Size");
}

void CMagicSlot::Action_Magic_Basic(_uint iIndex, CTransform* pTarget, _float4x4 TargetOffsetMatrix,const _float4x4* pWeaponMatrix, _float4x4 WeaponOffsetMatrix)
{
	//if Size over = MSG / if Index Slot is nullptr, nothing
	if (iIndex < m_MagicEssentialSlots.size())
	{
		if (m_MagicEssentialSlots[iIndex] == nullptr)
		{

		}
		m_MagicEssentialSlots[iIndex]->Magic_Cast(pTarget, TargetOffsetMatrix, pWeaponMatrix, WeaponOffsetMatrix);
	}
	else if (m_MagicEssentialSlots[iIndex] == nullptr)
	{

	}
	else
		MSG_BOX("Basic Cast Index is out of Size");
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
