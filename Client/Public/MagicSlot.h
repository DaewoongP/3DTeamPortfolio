#pragma once
#include "Component.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Client)

// 모든 마법을 가지는 객체들에게 가득찬 매직슬롯을 부여하는건 너무 아까웠음.
// 따라서 평타/쉴드 같은 기본 마법만 넣어주고
// 나머지는 원하는대로 커스터마이징 해서 집어넣는걸로 결정함.
// add_magic_skill를 이용해 desc와 함께 마법을 추가할것.
class CMagicBallPool;

class CMagicSlot final : public CComponent
{
private:
	explicit CMagicSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicSlot(const CMagicSlot& rhs);
	virtual ~CMagicSlot() = default;
public:


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	_bool IsCoolOn_Skill(_uint iSlot) { return (m_MagicSlots[iSlot]->IsCoolOn()); }
	_bool IsCoolOn_Essential(_uint iSlot) { return (m_MagicEssentialSlots[iSlot]->IsCoolOn()); }
	
	//겟셋 비추
	_float Get_CoolTime(_uint iSlot) { return (m_MagicSlots[iSlot]->Get_CoolTime()); }
	void Set_CoolTime(_uint iSlot, _float value) { m_MagicSlots[iSlot]->Set_CoolTime(value); }
	_float Get_CoolSpeedTime(_uint iSlot) { return (m_MagicSlots[iSlot]->Get_CoolSpeedTime()); }
	void Set_CoolSpeedTime(_uint iSlot, _float value) { m_MagicSlots[iSlot]->Set_CoolSpeedTime(value); }
	_float Get_CoolMultipleTimer(_uint iSlot) { return (m_MagicSlots[iSlot]->Get_CoolMultipleTimer()); }
	void Set_CoolMultipleTimer(_uint iSlot, _float value) { m_MagicSlots[iSlot]->Set_CoolMultipleTimer(value); }

	//스킬 n초간 n만큼 가속.
	void Set_SkillCoolMultiple(_float fTime, _float fValue)
	{
		for (CMagic* pMagic : m_MagicEssentialSlots)
		{
			if(pMagic!=nullptr)
				pMagic->Set_SkillCoolMultiple(fTime, fValue);
		}
		for (CMagic* pMagic : m_MagicSlots)
		{
			if (pMagic != nullptr)
				pMagic->Set_SkillCoolMultiple(fTime, fValue);
		}
	}

public:
	//Add New Skill
	HRESULT Add_Magics(CMagic::MAGICDESC SkillDesc);

	//Add Magic to Slot
	HRESULT Add_Magic_To_Skill_Slot(_uint iSlotIndex , SPELL eSpellType);
	HRESULT Add_Magic_To_Basic_Slot(_uint iSlotIndex , SPELL eSpellType);

	CMagicBall* Action_Magic_Skill(_uint iIndex, const CGameObject* pTarget, const CGameObject* pWeaponMatrix, COLLISIONFLAG eCollisionFlag, _bool isPowerUp=false);
	CMagicBall* Action_Magic_Basic(_uint iIndex, const CGameObject* pTarget, const CGameObject* pWeaponMatrix, COLLISIONFLAG eCollisionFlag, _bool isPowerUp=false);

private:

private:
	//4 Slot To Skill
	array<class CMagic*, 4>						m_MagicSlots = { nullptr };
	//8 Slot To Basic Skill
	array<class CMagic*, 8>						m_MagicEssentialSlots = { nullptr };

private:
	//SPELL_END Slot To All Skill
	array<class CMagic*, SPELL::SPELL_END>		m_Magics = { nullptr };

public:
	static CMagicSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END