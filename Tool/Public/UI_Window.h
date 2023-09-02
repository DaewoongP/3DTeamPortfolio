#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CUI_Window final : public CImWindow
{
public:
	struct UI_Tree
	{
		UI_Tree()
		{
			m_isShow = { false };
			m_SRV = { nullptr };
			m_iWidth = { 0 };
			m_iHeight = { 0 };
			m_isAddPrototype = { false };
		}
		wstring			m_wstrName; // 파일경로
		_bool			m_isFolder;
		_bool			m_isShow; // Show 버튼 상호작용.
		

		// Create 버튼 눌렀을때 AddPortotype 은 한번만 하도록.
		_bool			m_isAddPrototype;


		// 이미지 미리보기 출력하기 위한 변수들
		ID3D11ShaderResourceView*	m_SRV;
		_int						m_iWidth;
		_int						m_iHeight;

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
	HRESULT Read_File_In_Directory_Tree(_Inout_ UI_Tree& Root, const _tchar* pFilePath, const _tchar* pExt); // 폴더 경로 트리 알고리즘 통해 탐색.

public:
	void	Open_File_Path_Tree(UI_Tree* pTree); // 트리 노드 생성.
	void	Create_UI(UI_Tree* pTree); // Open_File_Path_Tree 함수 내 Create 버튼 상호작용.
	void	Show_Object_List();

public:
	void	Interaction_UI();
	void	Move_UI();

public:
	_bool Load_ImTexture(const _char* pFilePath, ID3D11ShaderResourceView** out_srv, _int* out_width, _int* out_height);


private:
	vector <wstring>					m_FilePaths;
	
	// for Tree
	UI_Tree								m_TreeDesc;
	// 아래는 동적할당한 객체들 모아두고 삭제하는 리스트.
	list <UI_Tree*>						m_Trees;
	list <ID3D11ShaderResourceView*>	m_SRVs;

private:
	_uint				m_iInitIndex = { 0 };
	_uint				m_iTreeIndex = { 0 };


private:
	_bool m_isSelected = { false };
	_bool m_isListMouseInteraction = { false };

private:
	_bool				m_isShowImageList;

private:
	class CDummy_UI* m_pDummy_UI = { nullptr }; // 현재 마우스로 잡은 녀석 보관
	POINT			 m_MousePos = {}; // 이전 마우스 위치


public:
	static CUI_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END