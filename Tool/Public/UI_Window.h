#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CUI_Window final : public CImWindow
{
public:
	struct UI_Tree
	{
		wstring		m_wstrName;
		_bool		m_isFolder;

		vector <UI_Tree*> children;
	};

private:
	explicit CUI_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector <const _tchar*> m_FilePath = { nullptr };

public:
	void	Open_Dialog();
	void	Open_File_Path_Tree(UI_Tree* pTree); // 트리 노드 생성.
	HRESULT Read_File_In_Directory_Tree(_Inout_ UI_Tree& Root, const _tchar* pFilePath, const _tchar* pExt); // 폴더 경로 트리 알고리즘 통해 탐색.


private:
	vector <wstring>	m_FilePaths;
	UI_Tree				m_TreeDesc;
	list <UI_Tree*>		m_Trees;

public:
	static CUI_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END