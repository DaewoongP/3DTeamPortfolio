#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CUI_DamageFont final : public CGameObject
{

public:
	struct DAMAGEFONTDESC
	{
		_tchar			m_pText[MAX_PATH] = TEXT("");
		_float3			m_vWorldPos = { 0.f, 0.f, 0.f };
		_float2			m_vPos = { 650.f, 360.f };
		_float			m_fWorldY = { 0.f };
		XMVECTOR		m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
		_float			m_fRotation = { 0.f };
		_float2			m_vOrigin = { 0.f, 0.f };
		_float2			m_vMinScale = { 1.f, 1.f };
		_float2 		m_vMaxScale = { 1.f, 1.f };

		_float			m_fLifeTime = { 0.f };
		_float			m_fMoveSpeed = { 0.f };
		_float			m_fAlphaSpeed = { 0.f };
	};

private:
	explicit CUI_DamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_DamageFont(const CUI_DamageFont& rhs);
	virtual ~CUI_DamageFont() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFontFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool Get_isLife() { return m_isLife; }

	void Set_vPos(_float2 vPos) { m_vPos = vPos; }
	void Set_vScale(_float2 vScale) { m_vMaxScale = vScale; }
	void Set_vColor(_float4 vColor) { m_vColor = vColor; }
	void Set_Text(wstring wText);
	void Set_Origin(_float2 vOrigin) { m_vOrigin = vOrigin; }

	void Left_Align();

private:
	_tchar			m_pText[MAX_PATH] = TEXT("");
	_float2			m_vPos = { 650.f, 360.f };
	_float3			m_vWorldPos;
	_float			m_fWorldY = { 0.f };

	_float4			m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float			m_fRotation = { 0.f };
	_float2			m_vOrigin = { 0.f, 0.f };
	
	_float2			m_vMinScale = { 1.f, 1.f };
	_float2 		m_vMaxScale = { 1.f, 1.f };
	_float2			m_vScale = { 0.f, 0.f };

private:
	_float			m_fCurrentTime = { 0.f };
	_float			m_fLifeTime = { 0.f };
	_float			m_fMoveSpeed = { 0.f };
	_float			m_fAlphaSpeed = { 0.f };

	_bool			m_isLife = { true };

private:
	HRESULT			Add_Component();
	void			Play_DamageFont(_float fTimeDelta);

private:
	_float2	Caculate_Projection(_float3 vPos);


private:
	SpriteBatch*	m_pBatch = { nullptr };
	SpriteFont*		m_pFont = { nullptr };

private:
	CRenderer*		m_pRendererCom = { nullptr };

public:
	static CUI_DamageFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END