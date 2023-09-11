#pragma once
#include "UI_Group.h"
#include "Client_Defines.h"

#include "UI_Progress1.h"
BEGIN(Client)

class CUI_Group_Finisher final : public CUI_Group
{
private:
	enum FINISHER
	{
		FRONT,
		BACK,
		FINISHER_END
	};

private:
	explicit CUI_Group_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Finisher(const CUI_Group_Finisher& rhs);
	virtual ~CUI_Group_Finisher() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_bool	Set_Gauge(_float fMin, _float fMax, _float fCurrent, CUI_Progress::GAUGE eType = CUI_Progress::CURRENT);

private:
	HRESULT Add_ProtoType();

private:
	vector<_tchar*> m_ProtoTypeTags;

private:
	// 0 = Front, 1 = Back
	vector<class CUI_Back*>		m_pBacks;
	vector<class CUI_Finisher*>	m_pFinishers;

public:
	static CUI_Group_Finisher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END