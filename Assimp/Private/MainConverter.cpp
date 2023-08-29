#include "..\Public\MainConverter.h"
#include "GameInstance.h"

HRESULT CMainConverter::Model_Convert(_uint iType, const _tchar* pFilePath)
{
	if (FAILED(ReadFileInDirectory(iType, pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainConverter::ReadFileInDirectory(_uint iType, const _tchar* pFilePath)
{
	// ���丮 ��θ� ��ȸ�� iterator
	fs::directory_iterator iter(fs::absolute(pFilePath));
	
	while (iter != fs::end(iter))
	{
		// ���� ���丮 ��θ� ����ִ� ���� (iterator�� ����)
		const fs::directory_entry& entry = *iter;

		// ���� entry ������ ���丮���� Ȯ�� �� ���丮�̸� ���
		if (fs::is_directory(entry.path()))
		{
			ReadFileInDirectory(iType, entry.path().c_str());
		}
		else
		{
			// fbx���� üũ
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".fbx")) ||
				!lstrcmp(entry.path().extension().c_str(), TEXT(".FBX")))
			{
				cout << entry.path() << endl;

				// �ش��ϴ� ������ ��θ� ���� �� �����Ϳ� �Ҵ�
				CModel_Converter::Convert(iType, entry.path().string().c_str());
			}
		}

		iter++;
	}

	return S_OK;
}

void CMainConverter::Convert(_uint iType, const _tchar* pFilePath)
{
	CMainConverter* pInstance = new CMainConverter();

	if (FAILED(pInstance->Model_Convert(iType, pFilePath)))
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
