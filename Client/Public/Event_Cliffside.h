#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CRenderer;

END

BEGIN(Client)
class CTrigger;
END

BEGIN(Client)

class CEvent_Cliffside final : public CGameObject
{
	enum CLIFFSIDE_SEQUENCE
	{
		CLIFFSIDE_SEQUENCE_FADE_OUT,
		CLIFFSIDE_SEQUENCE_PLAY_CUTSCENE,
		CLIFFSIDE_SEQUENCE_FADE_OUT_IN,
		CLIFFSIDE_SEQUENCE_END
	};

private:
	explicit CEvent_Cliffside(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Cliffside(const CEvent_Cliffside& rhs);
	virtual ~CEvent_Cliffside() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pCutScene_0_0 = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
private:
	_bool m_isEnter = { false };
	CLIFFSIDE_SEQUENCE m_eCliffside_Sequence = { CLIFFSIDE_SEQUENCE_END };
	void Check_Event_Play_CutScene_0_0();

private:
	HRESULT Add_Components();

public:
	static CEvent_Cliffside* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Cliffside* Clone(void* pArg) override;
	virtual void Free() override;
};

END