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
	// 디렉토리 경로를 순회할 iterator
	fs::directory_iterator iter(fs::absolute(pFilePath));
	
	while (iter != fs::end(iter))
	{
		// 실제 디렉토리 경로를 담고있는 변수 (iterator의 원본)
		const fs::directory_entry& entry = *iter;

		// 현재 entry 변수가 디렉토리인지 확인 후 디렉토리이면 재귀
		if (fs::is_directory(entry.path()))
		{
			ReadFileInDirectory(iType, entry.path().c_str());
		}
		else
		{
			// fbx파일 체크
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".fbx")) ||
				!lstrcmp(entry.path().extension().c_str(), TEXT(".FBX")))
			{
				cout << entry.path() << endl;

				// 해당하는 파일의 경로를 실제 모델 컨버터에 할당
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
