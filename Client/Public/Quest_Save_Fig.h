#pragma once
#include "Quest.h"

BEGIN(Client)

class CQuest_Save_Fig final : public CQuest
{
private:
	explicit CQuest_Save_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CQuest_Save_Fig() = default;

public:
	void Set_Saved() { m_isSaved = true; }
	virtual _bool Is_Finished() override;

public:
	HRESULT Initialize();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Clear_Quest();

private:
	_bool		m_isSaved = { false };

public:
	static CQuest_Save_Fig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END