#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CUI_Progress;
END

BEGIN(Client)

class CUI_HP final : public CUI
{
public:
	enum HPTYPE
	{
		PLAYER,
		MONSTER,
		HPTYPE_END
	};

private:
	explicit CUI_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_HP(const CUI_HP& rhs);
	virtual ~CUI_HP() = default;

public:
	void	Set_HP(_float fGauge);
	void	Set_HP(_float fGauge, CUI_Progress::GAUGE eType);
	void	Set_HPtype(HPTYPE eType) { m_eHPType = eType; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CUI_Progress*		m_pProgressCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	HRESULT	Initialize_Gauge(_float fMin, _float fMax, _float fCurrent);

private:
	HPTYPE				m_eHPType;
	_float				m_fPercent = { 0.f };

public:
	static CUI_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
