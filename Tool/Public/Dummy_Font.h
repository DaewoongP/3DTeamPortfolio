#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CDummy_Font final : public CGameObject
{
protected:
	explicit CDummy_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_Font(const CDummy_Font& rhs);
	virtual ~CDummy_Font() = default;

public:
	_float2 Get_vPos() { return m_vPos; }
	_float2	Get_vScale() { return m_vScale; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFontFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_vPos(_float2 vPos) { m_vPos = vPos; }
	void Set_vScale(_float2 vScale) { m_vScale = vScale;}

protected:
	_tchar			m_pText[MAX_PATH] = TEXT("");
	_float2			m_vPos = { 650.f, 360.f };
	XMVECTOR		m_vColor = Colors::White;
	_float			m_fRotation = { 0.f };
	_float2			m_vOrigin = { 0.f, 0.f };
	_float2 		m_vScale = { 1.f, 1.f };

private:
	SpriteBatch*		m_pBatch = { nullptr };
	SpriteFont*			m_pFont = { nullptr };

private:
	_bool			m_isClone = { false };
	CRenderer*		m_pRendererCom = { nullptr };


public:
	static CDummy_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END