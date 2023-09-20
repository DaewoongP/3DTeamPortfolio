#include "ImageFileDialog.h"

HRESULT CImageFileDialog::Initialize(ID3D11Device* _pDevice, const _char* _pTag)
{
    m_pDevice = _pDevice;
    Safe_AddRef(m_pDevice);
    m_strFileDialogTag = _pTag;
    _int iTemp1;
    _int iTemp2;

    if(false == LoadTextureFromFile("../../Resources/Default/Textures/Default0.jpg", &m_pDefaultTexture, &iTemp1, &iTemp2))
    {
        return E_FAIL;
    }

    return S_OK;
}

string CImageFileDialog::Get_FilePathName()
{
    return m_strFilePathName;
}

string CImageFileDialog::Get_CurretnPath()
{
    return m_strFilePath;
}

void CImageFileDialog::ChangeTexture(const _char* pFilePathName)
{
    _int iTemp1;
    _int iTemp2;
    Safe_Release(m_pTexture);
    LoadTextureFromFile(pFilePathName, &m_pTexture, &iTemp1, &iTemp2);
}

void CImageFileDialog::Tick()
{
    // 텍스처가 없으면 기본 텍스처 출력
    m_isOk = false;

    // 빈 텍스처면 기본 텍스처 출력
    if (nullptr == m_pTexture)
    {
        if (ImGui::ImageButton(m_pDefaultTexture, ImVec2(_float(m_iImageButtonWidth), _float(m_iImageButtonHeight))))
        {
            m_pInstance.OpenDialog(m_strFileDialogTag.data(), m_strFileDialogName.data(), m_strHeaderFilter.data(), m_strStartPath.data());
        }
    }
    else // 값이 있으면 새로운 이미지 출력
    {
        if (ImGui::ImageButton(m_pTexture, ImVec2(_float(m_iImageButtonWidth), _float(m_iImageButtonHeight))))
        {
            m_pInstance.OpenDialog(m_strFileDialogTag.data(), m_strFileDialogName.data(), m_strHeaderFilter.data(), m_strStartPath.data());
        }
    }

    // display
    if (m_pInstance.Display(m_strFileDialogTag.data()))
    {
        // action if OK
        if (m_pInstance.IsOk())
        {
            m_strFilePathName = m_pInstance.GetFilePathName();
            m_strFilePath = m_pInstance.GetCurrentPath();
            m_isOk = true;
            ChangeTexture(m_strFilePathName.data());
        }

        // close
        m_pInstance.Close();
    }

    // 열려있으면 버튼 아래에 사각형 이미지 출력.
    if (m_pInstance.IsOpened())
    {
        if (m_strPrevFilePathName != m_pInstance.GetFilePathName())
        {
            string strCurrentFilePathName = m_pInstance.GetFilePathName();
            ChangeTexture(strCurrentFilePathName.data());
            m_strPrevFilePathName = strCurrentFilePathName;
        }

        if (nullptr == m_pTexture)
        {
            ImGui::Image((void*)m_pDefaultTexture, ImVec2(_float(m_iImageWidth), _float(m_iImageHeight)));
        }
        else
        {
            ImGui::Image((void*)m_pTexture, ImVec2(_float(m_iImageWidth), _float(m_iImageHeight)));
        }
    }
}

_bool CImageFileDialog::IsOk()
{
    return m_isOk;
}

bool CImageFileDialog::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    m_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

CImageFileDialog* CImageFileDialog::Create(ID3D11Device* _pDevice, const _char* _pTag)
{
    CImageFileDialog* m_pInstance = New CImageFileDialog;

    if (FAILED(m_pInstance->Initialize(_pDevice, _pTag)))
    {
        MSG_BOX("Failed Create CImageFileDialog");
        Safe_Release(m_pInstance);
        return nullptr;
    }

    return m_pInstance;
}

void CImageFileDialog::Free(void)
{
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pTexture);
    Safe_Release(m_pDefaultTexture);
}
