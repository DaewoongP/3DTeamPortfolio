#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CQuest abstract : public CGameObject
{
public:
	QUESTSTATE Get_State() { return m_eCurState; }
	void Set_State(QUESTSTATE eState) {
		m_eCurState = eState; 
	}

	virtual _bool Is_Finished() PURE;

protected:
	explicit CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

public:
	virtual void	Clear_Quest() PURE;

protected:
	QUESTSTATE		m_eCurState = { QUESTSTATE_QUESTIONMARK };

public:
	virtual CGameObject* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END