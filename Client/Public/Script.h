#pragma once
#include "Composite.h"
#include "Texture.h"
#include "Client_Defines.h"


BEGIN(Client)
class CUI_Script;

class CMyScript final : public CComposite
{
protected:
    explicit CMyScript(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CMyScript(const CMyScript& rhs);
    virtual ~CMyScript() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

private:
    HRESULT Add_Prototype();
    HRESULT Add_Components();

public:
    HRESULT Add_Script(const _tchar* pTexturePath);

public:
    _bool   Next_Script();
    HRESULT Render_Script(_float fTimeDelta);

private:
    _tchar m_szScriptTag[MAX_PATH] = TEXT("");

private:
    _uint                   m_iCurrentScriptIndex = { 0 };
    vector<CTexture*>       m_Scripts;
    CUI_Script*             m_pUI_Script;

public:
    static CMyScript* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END