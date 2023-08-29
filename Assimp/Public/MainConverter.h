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
	// ���丮�� ��ȸ�ϸ鼭 �������� ���̳ʸ�ȭ �ϸ� Ÿ�� ����.
	// 1. eType : �ִϸ��̼Ǹ�, ����ƽ�� Ÿ�� ����
	// 2. DirectoryPath : ��ȸ�� �ֻ������� ����.
	//HRESULT ReadFileInDirectory(CModelConverter::TYPE eType, const _tchar* pDirectoryPath);

public:
	static void Convert(const _tchar* pFilePath);
	virtual void Free() override;
};

END