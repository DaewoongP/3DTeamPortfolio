#pragma once
#include "ImWindow.h"
#include "Dummy_UI.h"

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
		wstring						m_wstrName; // 파일경로
		_bool						m_isFolder;
		_bool						m_isShow; // Show 버튼 상호작용.
		
		// Create 버튼 눌렀을때 AddPortotype 은 한번만 하도록.
		_bool						m_isAddPrototype;


		// 이미지 미리보기 출력하기 위한 변수들
		ID3D11ShaderResourceView*	m_SRV;
		_int						m_iWidth;
		_int						m_iHeight;

		vector <UI_Tree*>			children;
	};

public:
	struct UI_DATA
	{
		UI_DATA()
		{
			isSave = { false };

			vCombinedXY = { 0.f, 0.f };
			m_fX = { 0.f };
			m_fY = { 0.f };
			m_fZ = { 0.f };

			m_fSizeX = { 0.f };
			m_fSizeY = { 0.f };

			ZeroMemory(m_wszTextureName, MAX_PATH);

			m_eType = { CDummy_UI::UI_ID::UI_ID_END };
			m_isParent = { false };

			m_isAlpha = { false };
			m_vColor = { 1.f, 1.f, 1.f, 1.f };
			ZeroMemory(m_wszAlphaTexturePrototypeTag, MAX_PATH);
			ZeroMemory(m_wszAlphaTextureFilePath, MAX_PATH);
		}

		_bool				isSave;

		_float2				vCombinedXY;
		_float				m_fX;
		_float				m_fY;
		_float				m_fZ;

		_float				m_fSizeX;
		_float				m_fSizeY;

		_tchar				m_wszTextureName[MAX_PATH];
		_tchar				m_wszTexturePath[MAX_PATH];

		CDummy_UI::UI_ID	m_eType;

		_bool				m_isParent;

		_bool				m_isAlpha;
		_float4				m_vColor;
		_tchar				m_wszAlphaTexturePrototypeTag[MAX_PATH] = TEXT("");
		_tchar				m_wszAlphaTextureFilePath[MAX_PATH] = TEXT("");
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
	HRESULT Read_File_In_Directory_Tree(_Inout_ UI_Tree& Root, const _tchar* pFilePath, const _tchar* pExt); // 폴더 경로 트리 알고리즘 통해 탐색.

public: // Tree & Image
	void	Open_File_Path_Tree(UI_Tree* pTree); // 트리 노드 생성.
	void	Create_UI(UI_Tree* pTree); // Open_File_Path_Tree 함수 내 Create 버튼 상호작용.

public: // Object List
	void	Object_List();
	void	Open_Object_List();
	void	Select_Obejct(CGameObject* pGameObject);
	void	Object_List_Button();
	void	Add_Group(CGameObject* pGameObject);
	void	Select_Object_ID(CGameObject* pGameObject);

public: // UI Gruop
	void	Input_Text();
	void	Create_UI_Gruop(string _pGroupName);
	void	UI_Gruop_Combo();
	void	UI_Group_Tree();

public: // Font
	void	Create_Font();
	void	Open_Font_List();

public: // Function
	void	Interaction_UI();
	void	Move_UI();
	void	Correction_Pick();
	void	Save_Load_Button();
	HRESULT	Save_Data(_tchar* pFilePath);
	void	Load_UI();
	HRESULT	Load(_tchar* pFilePath);
	void	Capture();
	void	Select_AlphaTexture(CDummy_UI* pDummyUI);


public:
	_bool Load_ImTexture(const _char* pFilePath, ID3D11ShaderResourceView** out_srv, _int* out_width, _int* out_height);
	HRESULT	Initialize_Font();

private:
	vector <wstring>					m_FilePaths;
	
	// For Tree
	UI_Tree								m_TreeDesc;
	// 아래는 동적할당한 객체들 모아두고 삭제하는 리스트.
	list <UI_Tree*>						m_Trees;
	list <ID3D11ShaderResourceView*>	m_SRVs;

private:
	_uint		m_iInitIndex = { 0 };
	_uint		m_iTreeIndex = { 0 };


private: // Tree & Image 변수
	_bool		m_isShowImageList;

private: // Object List 변수
	_bool		m_isObjectSelected = { false };
	_int		m_iObjectListIndex = { 0 };
	_bool		m_isOpenList= { false };
	_bool		m_isAddGruopSelected = { false };
	_int		m_AddGroupIndex = { 0 };
	_char		m_szObjectListInputText[256] = "";
	_bool		m_isParent = { false };
	_int		m_ParentChildComboIndex = { 0 }; // 선택된 항목의 인덱스

	_bool		m_isListMouseInteraction = { false };

private:
	class CLayer*					m_pUILayer = { nullptr };
	vector <class CGameObject*>		m_pUIVector = { nullptr };

	class CLayer*					m_pUIGroupLayer = { nullptr };
	vector <class CGameObject*>		m_pGroupVector = { nullptr };
	class CDummy_UI_Group*			m_pDummy_UI_Group = { nullptr }; // 현재 마우스로 잡은 녀석 보관

	class CLayer*					m_pFontLayer = { nullptr };
	vector <class CGameObject*>		m_pFontVector = { nullptr };
	class CDummy_Font*				m_pDummy_Font = { nullptr };


private:
	class CDummy_UI*			m_pDummy_UI = { nullptr }; // 현재 마우스로 잡은 녀석 보관
	POINT						m_MousePos = {}; // 이전 마우스 위치

private:
	_char		m_szInputText[MAX_PATH] = "";
	_bool		m_isEnterGroupName = { false };
	_int		m_GroupComboIndex = { 0 }; // 선택된 항목의 인덱스

	_char		m_szCaptureText[MAX_PATH] = "";
	_bool		m_isCapture = { false };

	_tchar		m_wszFontText[MAX_PATH] = TEXT("");
	_char		m_szFontText[MAX_PATH] = "";
	_bool		m_isEnterTextName = { false };
	_int		m_FontComboIndex = { 0 }; // 선택된 항목의 인덱스

	_int		m_isFontSelected = { false };
	_int		m_iFontListIndex = { 0 };
	FONT		m_eFont = { FONT_END };

	_int		m_iIDIndex = { (_int)CDummy_UI::UI_ID_END };

private:
	_bool		m_isSave = { false };
	_bool		m_isLoad = { false };

	_bool		m_isOpenSelectAlphaTexture = { false };
	_tchar		m_wszAlphaTexturePrototypeTag[MAX_PATH] = TEXT("");
	_tchar		m_wszAlphaTextureFilePath[MAX_PATH] = TEXT("");


private:
	UI_DATA		m_UIDesc;

public:
	static CUI_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END