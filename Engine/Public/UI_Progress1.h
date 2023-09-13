#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CUI_Progress final : public CComponent
{
public:
	enum GAUGE { MIN, MAX, CURRENT, GAUGE_END };

protected:
	explicit CUI_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Progress(const CUI_Progress& rhs);
	virtual ~CUI_Progress() = default;

public:
	_float	Get_Gauge_Percent()
	{
		return m_fCurrentGauge / m_fMaxGauge;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

public:
	void	Set_Gauge(_float fGauge, GAUGE eType);

public:
	void	Initailize_Gauge(_float fMin, _float fMax, _float fCurrent);

private:
	_float	m_fMinGauge = { 0.f };
	_float	m_fMaxGauge = { 0.f };
	_float	m_fCurrentGauge = { 0.f };

public:
	static CUI_Progress* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
