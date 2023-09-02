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
		wstring			m_wstrName; // ���ϰ��
		_bool			m_isFolder;
		_bool			m_isShow; // Show ��ư ��ȣ�ۿ�.
		

		// Create ��ư �������� AddPortotype �� �ѹ��� �ϵ���.
		_bool			m_isAddPrototype;


		// �̹��� �̸����� ����ϱ� ���� ������
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
	HRESULT Read_File_In_Directory_Tree(_Inout_ UI_Tree& Root, const _tchar* pFilePath, const _tchar* pExt); // ���� ��� Ʈ�� �˰��� ���� Ž��.

public:
	void	Open_File_Path_Tree(UI_Tree* pTree); // Ʈ�� ��� ����.
	void	Create_UI(UI_Tree* pTree); // Open_File_Path_Tree �Լ� �� Create ��ư ��ȣ�ۿ�.
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
	// �Ʒ��� �����Ҵ��� ��ü�� ��Ƶΰ� �����ϴ� ����Ʈ.
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
	class CDummy_UI* m_pDummy_UI = { nullptr }; // ���� ���콺�� ���� �༮ ����
	POINT			 m_MousePos = {}; // ���� ���콺 ��ġ


public:
	static CUI_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END