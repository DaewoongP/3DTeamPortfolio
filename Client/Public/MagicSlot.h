#pragma once
#include "Component.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Client)

// ��� ������ ������ ��ü�鿡�� ������ ���������� �ο��ϴ°� �ʹ� �Ʊ����.
// ���� ��Ÿ/���� ���� �⺻ ������ �־��ְ�
// �������� ���ϴ´�� Ŀ���͸���¡ �ؼ� ����ִ°ɷ� ������.
// add_magic_skill�� �̿��� desc�� �Բ� ������ �߰��Ұ�.
class CMagicBallPool;

class CMagicSlot final : public CComponent
{
private:
	explicit CMagicSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicSlot(const CMagicSlot& rhs);
	virtual ~CMagicSlot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	//Add New Skill
	HRESULT Add_Magics(CMagic::MAGICDESC SkillDesc);
	//Add Function to Skill
	HRESULT Add_Function_To_Magic(function<void()> func , SPELL eSpellType);

	//Add Magic to Slot
	HRESULT Add_Magic_To_Skill_Slot(_uint iSlotIndex , SPELL eSpellType);
	HRESULT Add_Magic_To_Basic_Slot(_uint iSlotIndex , SPELL eSpellType);

public:
	//Skill Action
	CMagicBall* Action_Magic_Skill(_uint iIndex, CTransform* pTarget,_float4x4 TargetOffsetMatrix, const _float4x4* pWeaponMatrix, _float4x4 WeaponOffsetMatrix, COLLISIONFLAG eCollisionFlag);
	//Basic Skill Action
	CMagicBall* Action_Magic_Basic(_uint iIndex, CTransform* pTarget, _float4x4 TargetOffsetMatrix, const _float4x4* pWeaponMatrix, _float4x4 WeaponOffsetMatrix, COLLISIONFLAG eCollisionFlag);

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