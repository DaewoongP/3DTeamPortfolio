#pragma once
#include "Assimp_Defines.h"
#include "Base.h"

BEGIN(Assimp)

class CMainConverter final : public CBase
{
private:
	explicit CMainConverter() = default;
	virtual ~CMainConverter() = default;

public:
	HRESULT Model_Convert(const _tchar* pFilePath);

private:
	// 디렉토리를 순회하면서 모델파일을 바이너리화 하며 타입 설정.
	// 1. eType : 애니메이션모델, 스태틱모델 타입 설정
	// 2. DirectoryPath : 순회할 최상위폴더 설정.
	//HRESULT ReadFileInDirectory(CModelConverter::TYPE eType, const _tchar* pDirectoryPath);

public:
	static void Convert(const _tchar* pFilePath);
	virtual void Free() override;
};

END