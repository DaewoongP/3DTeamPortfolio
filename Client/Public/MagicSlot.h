#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Client)

class CMagicSlot final : public CGameObject
{
private:
	explicit CMagicSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicSlot(const CMagicSlot& rhs);
	virtual ~CMagicSlot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	HRESULT Add_Magic(_uint iSlotNum, SPELL eMagicTag);

private:
	array<class CMagic*, 4>						m_MagicSlots = { nullptr };

private:
	array<class CMagic*, SPELL::SPELL_END>		m_Magics;

private:
	HRESULT Add_Components();

public:
	static CMagicSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END