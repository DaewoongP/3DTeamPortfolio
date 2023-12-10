#pragma once
#include "Composite.h"
#include "Texture.h"
#include "Client_Defines.h"


BEGIN(Client)
class CUI_Script;

class CScript final : public CComposite
{
private:
    explicit CScript(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CScript(const CScript& rhs);
    virtual ~CScript() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual void Late_Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_isRender(_bool isrender) {
        m_isRender = isrender;
    }
    _bool Is_Finished() {
        return m_isFinished;
    }

private:
    HRESULT Add_Prototype();
    HRESULT Add_Components();

public:
    HRESULT Add_Script(const _tchar* pTexturePath);

public:
    void    Reset_Script();
    _bool   Next_Script();

    static CScript* Find_Script(const _tchar* swzScriptTag, _umap<const _tchar*, CScript*>& pSript);

private:
    _tchar m_szScriptTag[MAX_PATH] = TEXT("");

private:
    _uint                   m_iCurrentScriptIndex = { 0 };
    vector<CTexture*>       m_Scripts;
    CUI_Script*             m_pUI_Script;
    _bool                   m_isRender = { false };
    _bool                   m_isFinished = { false };

    _bool                   m_isCardScript = { false };

public:
    static CScript* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END