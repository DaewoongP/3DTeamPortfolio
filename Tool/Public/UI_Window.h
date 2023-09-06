#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CUI_Window final : public CImWindow
{
public:
	enum FONT { GOTHIC, FONT_END };

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

private:
	void	Open_Dialog();
	HRESULT Read_File_In_Directory_Tree(_Inout_ UI_Tree& Root, const _tchar* pFilePath, const _tchar* pExt); // ���� ��� Ʈ�� �˰��� ���� Ž��.

public: // Tree & Image
	void	Open_File_Path_Tree(UI_Tree* pTree); // Ʈ�� ��� ����.
	void	Create_UI(UI_Tree* pTree); // Open_File_Path_Tree �Լ� �� Create ��ư ��ȣ�ۿ�.

public: // Object List
	void	Object_List();
	void	Open_Object_List();
	void	Select_Obejct(CGameObject* pGameObject);
	void	Object_List_Button();
	void	Add_Group(CGameObject* pGameObject);

public: // UI Gruop
	void	Input_Text();
	void	Create_UI_Gruop(string _pGroupName);
	void	UI_Gruop_Combo();
	void	UI_Group_Tree();

public:
	void	Create_Font();

public: // Function
	void	Interaction_UI();
	void	Move_UI();
	void	Correction_Pick();
	void	Save_UI();
	void	Load_UI();
	void	Capture();

public:
	_bool Load_ImTexture(const _char* pFilePath, ID3D11ShaderResourceView** out_srv, _int* out_width, _int* out_height);
	HRESULT	Initialize_Font();

private:
	vector <wstring>					m_FilePaths;
	
	// For Tree
	UI_Tree								m_TreeDesc;
	// �Ʒ��� �����Ҵ��� ��ü�� ��Ƶΰ� �����ϴ� ����Ʈ.
	list <UI_Tree*>						m_Trees;
	list <ID3D11ShaderResourceView*>	m_SRVs;

private:
	_uint				m_iInitIndex = { 0 };
	_uint				m_iTreeIndex = { 0 };


private: // Tree & Image ����
	_bool				m_isShowImageList;

private: // Object List ����
	_bool	m_isObjectSelected = { false };
	_int	m_iObjectListIndex = { 0 };
	_bool	m_isOpenList= { false };
	_bool	m_isAddGruopSelected = { false };
	_int	m_AddGroupIndex = { 0 };
	_char	m_szObjectListInputText[256] = "";
	_bool	m_isParent = { false };
	_int	m_ParentChildComboIndex = { 0 }; // ���õ� �׸��� �ε���

	_bool m_isListMouseInteraction = { false };

private:
	class CLayer* m_pUILayer = { nullptr };
	vector <class CGameObject*> m_pUIVector = { nullptr };

	class CLayer* m_pUIGroupLayer = { nullptr };
	vector <class CGameObject*> m_pGroupVector = { nullptr };
	class CDummy_UI_Group* m_pDummy_UI_Group = { nullptr }; // ���� ���콺�� ���� �༮ ����

	class CLayer* m_pFontLayer = { nullptr };



private:
	class CDummy_UI* m_pDummy_UI = { nullptr }; // ���� ���콺�� ���� �༮ ����
	POINT			 m_MousePos = {}; // ���� ���콺 ��ġ

private:
	_char				m_szInputText[MAX_PATH] = "";
	_bool				m_isEnterGroupName = { false };
	_int				m_GroupComboIndex = { 0 }; // ���õ� �׸��� �ε���
	//_int				m_isPreGroupComboIndex = { 0 };

	_char				m_szCaptureText[MAX_PATH] = "";
	_bool				m_isCapture = { false };

	_tchar				m_wszFontText[MAX_PATH] = TEXT("");
	_char				m_szFontText[MAX_PATH] = "";
	_bool				m_isEnterTextName = { false };
	_int				m_FontComboIndex = { 0 }; // ���õ� �׸��� �ε���

private:
	FONT				m_eFont = { FONT_END };

public:
	static CUI_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END