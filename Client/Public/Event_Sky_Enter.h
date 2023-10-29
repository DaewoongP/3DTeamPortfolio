#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CEvent_Sky_Enter final : public CGameObject
{
	enum SKY_ENTER_SEQUENCE
	{
		SKY_ENTER_SEQUENCE_FADE_OUT,
		SKY_ENTER_SEQUENCE_PLAY_CUTSCENE,
		SKY_ENTER_SEQUENCE_FADE_IN,
		SKY_ENTER_SEQUENCE_END
	};

private:
	explicit CEvent_Sky_Enter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Sky_Enter(const CEvent_Sky_Enter& rhs);
	virtual ~CEvent_Sky_Enter() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	
private:
	CRenderer* m_pRenderer = { nullptr };

private:
	_bool m_isEnter = { false };
	SKY_ENTER_SEQUENCE m_eSky_Enter_Sequence = { SKY_ENTER_SEQUENCE_END };
	
	
private:
	void Check_Event_Play_CutScene();

private:
	HRESULT Add_Components();

public:
	static CEvent_Sky_Enter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Sky_Enter* Clone(void* pArg) override;
	virtual void Free() override;
};

END