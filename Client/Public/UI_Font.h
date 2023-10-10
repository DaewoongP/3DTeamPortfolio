#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CUI_Font final : public CGameObject
{

public:
	struct FONTDESC
	{
		_tchar			m_pText[MAX_PATH] = TEXT("");
		_float2			m_vPos = { 650.f, 360.f };
		XMVECTOR		m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
		_float			m_fRotation = { 0.f };
		_float2			m_vOrigin = { 0.f, 0.f };
		_float2 		m_vScale = { 1.f, 1.f };
	};

protected:
	explicit CUI_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Font(const CUI_Font& rhs);
	virtual ~CUI_Font() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFontFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_vPos(_float2 vPos) { m_vPos = vPos; }
	void Set_vScale(_float2 vScale) { m_vScale = vScale; }
	void Set_vColor(_float4 vColor) { m_vColor = vColor; }
	void Set_Text(wstring wText);
	void Set_Origin(_float2 vOrigin) { m_vOrigin = vOrigin; }

	void Left_Align();



protected:
	_tchar			m_pText[MAX_PATH] = TEXT("");
	_float2			m_vPos = { 650.f, 360.f };
	_float4			m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	//XMVECTOR		m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float			m_fRotation = { 0.f };
	_float2			m_vOrigin = { 0.f, 0.f };
	_float2 		m_vScale = { 1.f, 1.f };


private:
	HRESULT		Add_Component();
	_bool		Is_In_Rect();

private:
	SpriteBatch* m_pBatch = { nullptr };
	SpriteFont* m_pFont = { nullptr };

private:
	CRenderer*		m_pRendererCom = { nullptr };

public:
	static CUI_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END