#pragma once
#include "CustomWidget.h"
#include "GameInstance.h"

BEGIN(Tool)
class CImageFileDialog : public CCustomWidget
{
private:
    explicit CImageFileDialog() = default;
    virtual ~CImageFileDialog() = default;

private:
    HRESULT Initialize(ID3D11Device* _pDevice, const _char* _pTag);

    string Get_FilePathName();
    string Get_CurretnPath();

public:
    void Tick();

private:
    bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

private:
    ID3D11Device* m_pDevice = { nullptr };

public: // 파일 다이얼로그 관련 변수
    string m_strTag;
    string m_strFileDialogTag; // 
    string m_strFileDialogName = { "Image FileDialog" };
    string m_strStartPath = { "../../Resources/" };
    string m_strHeaderFilter = {".png,.dds,.jpg"};

private:
    string m_strFilePathName;
    string m_strFilePath;

public: // 이미지 버튼 관련 변수
    _int m_iImageButtonWidth = { 16 };
    _int m_iImageButtonHeight = { 16 };

private: // 이미지 관련 변수
    ID3D11ShaderResourceView* m_pDefaultTexture = { nullptr };
    ID3D11ShaderResourceView* m_pTexture = { nullptr };
    _int m_iDefaultImageWidth;
    _int m_iDefaultImageHeight;
    ImVec2  m_ImageSize;
    _int m_iImageWidth = { 64 };
    _int m_iImageHeight = { 64 };
    _bool m_isShow = { false };

public:
    static CImageFileDialog* Create(ID3D11Device* _pDevice, const _char* _pTag);
    virtual void Free(void) override;
};
END
