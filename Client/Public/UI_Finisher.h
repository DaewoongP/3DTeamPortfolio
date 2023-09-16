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

class CUI_Finisher final : public CUI
{
private:
	explicit CUI_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Finisher(const CUI_Finisher& rhs);
	virtual ~CUI_Finisher() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Gauge(_float fGauge, CUI_Progress::GAUGE eType);

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CUI_Progress*		m_pProgressCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

#ifdef _DEBUG
	HRESULT Debug_UI(_float fTimeDelta);
#endif // _DEBUG

public:
	static CUI_Finisher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
