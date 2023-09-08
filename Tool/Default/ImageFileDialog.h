#pragma once
#include "CustomWidget.h"
#include "GameInstance.h"

BEGIN(Tool)
class CImageFileDialog : public CCustomWidget
{
private:
    explicit CImageFileDialog() = default;
    virtual ~CImageFileDialog() = default;

public:
    HRESULT Initialize(ID3D11Device* _pDevice, const _char* _pTag);

    string Get_FilePathName();
    string Get_CurretnPath();

public:
    void Tick();
    _bool IsOk();

private:
    bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);


public: // 이미지 버튼 관련 변수
    _int m_iImageButtonWidth = { 16 };
    _int m_iImageButtonHeight = { 16 };

public: // 이미지 관련 변수
    _int m_iImageWidth = { 64 };
    _int m_iImageHeight = { 64 };

public: // 파일 다이얼로그 관련 변수
    string m_strFileDialogName = { "Image FileDialog" };
    string m_strStartPath = { "../../Resources/" };
    string m_strHeaderFilter = { ".png,.dds,.jpg" };
    _bool m_isOk = { false };

private:
    string m_strFileDialogTag;
    string m_strPrevFilePathName;
    string m_strFilePathName;
    string m_strFilePath;
    
private:
    ID3D11Device* m_pDevice = { nullptr };
    ImGuiFileDialog m_pInstance;

private: // 이미지 관련 변수
    ID3D11ShaderResourceView* m_pDefaultTexture = { nullptr };
    ID3D11ShaderResourceView* m_pTexture = { nullptr };

public:
    static CImageFileDialog* Create(ID3D11Device* _pDevice, const _char* _pTag);
    virtual void Free(void) override;
};
END
