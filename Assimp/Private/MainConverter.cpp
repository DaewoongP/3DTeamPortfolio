#include "..\Public\MainConverter.h"

HRESULT CMainConverter::Model_Convert(const _tchar* pFilePath)
{

	return S_OK;
}

void CMainConverter::Convert(const _tchar* pFilePath)
{
	CMainConverter* pInstance = new CMainConverter();

	if (FAILED(pInstance->Model_Convert(pFilePath)))
	{
		MSG_BOX("Failed Convert Models");
	}
	else
	{
		MSG_BOX("Success Convert Models!");
	}

	Safe_Release(pInstance);
}

void CMainConverter::Free()
{
}
