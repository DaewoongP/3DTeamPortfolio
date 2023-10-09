#pragma once

#include "UI_Back.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CUI_Button;
class CVIBuffer_Rect;
class CCoolTime;
END

BEGIN(Client)

END

BEGIN(Client)
class CUI_Dynamic_Back : public CUI_Back
{
private:
	enum FADE_STATE { FADE_IN, FADE_OUT, LOOP, FADE_STATE_END };

private:
	explicit CUI_Dynamic_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Dynamic_Back(const CUI_Dynamic_Back& rhs);
	virtual ~CUI_Dynamic_Back() = default;

private:
	void Set_Duration(_float fDuration);

public:
	HRESULT Set_Texture(_uint iIndex, CTexture* pTexture);

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play_FadeIn();
	void PushBackNull();

private:
	void OnFadeInTick(_float fTimeDelta);
	void OnLoopTick(_float fTimeDelta);
	void OnFadeOutTick(_float fTimeDelta);

private:
	virtual HRESULT SetUp_ShaderResources() override;

private:
	CCoolTime* m_pDuration = { nullptr };

private:
	FADE_STATE m_eState = { FADE_STATE_END };
	_float m_fAlphaRatio = { 1.f };

public:
	static CUI_Dynamic_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END
