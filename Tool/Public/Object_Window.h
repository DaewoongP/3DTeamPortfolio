#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;
class CMapObject;
class CMapObject_Ins;

class CObject_Window final : public CImWindow
{
	enum DUMMY_MATRIX { DUMMY_SCALE, DUMMY_ROT, DUMMY_TRANS, DUMMY_END }; // ������ ����
	enum INSTALL_METHOD { ONE_METHOD, CONTINUOUS_METHOD, RANDOM_METHOD, MULTI_METHOD, END_METHOD }; // ��ġ ���

	typedef struct SaveObjectDesc
	{
		_float4x4 matTransform; // ���� ���
		_uint iTagLen; // ���ڿ� ����
		_tchar wszTag[MAX_PATH]; // ������Ʈ ����(�� ������Ʈ �̸�)
	}SAVEOBJECTDESC;

	typedef struct SaveInsObjectDesc
	{
		_uint iInstanceCnt; // �ν��Ͻ� ����
		_float4x4* pMatTransform; // ���� ���� ��ĵ��� �ּ�
		_float4x4 matTransform; // ���� ���
		_uint iTagLen; // ���ڿ� ����
		_tchar wszTag[MAX_PATH]; // ������Ʈ ����(�� ������Ʈ �̸�)
	}SAVEINSOBJECTDESC;

private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// �޴� ����
	void Picking_Menu(); // ��ŷ �޴�
	void Install_Object(_float3 vPos); // ������Ʈ ��ġ �޴�
	void Install_Continuous_Object(_float3 vPos); // ������Ʈ ���� ��ġ �޴�
	void Install_Random_Object(_float3 vPos); // ������Ʈ (���� ��) ���� ��ġ �޴�
	void Install_Multi_Object(_float3 vPos); // ������Ʈ ����(�ν��Ͻ�) ��ġ �޴�
	void Select_Model(); // �� ���� �޴�
	void Current_MapObject(); // ���� ��ġ�Ǿ� �ִ� �� ������Ʈ Ȯ��
	void Save_Load_Menu(); // ������Ʈ ���̺� �ε� �޴�
	void Ins_Save_Load_Menu(); // �ν���Ʈ ������Ʈ ���̺� �ε� �޴�
	void Delete_Object_Menu(); // ������Ʈ ���� ���� �޴�
	void Mesh_Picking_Menu(); // �޽� ��ŷ �޴�
	void Change_Picking_Menu(const _tchar* wszTag, _uint iTagNum); // ��ŷ�� �޽� ����
	void Delete_Picking_Object(); // ��ŷ�� ������Ʈ ���� �޴�
	void Map_Brushing_Menu(); // �� �׸��� �޴�
	HRESULT Save_MapObject(string szMapDataPath); // MapObject ����
	HRESULT Load_MapObject(const _tchar* wszMapDataPath); // MapObject �ε�
	HRESULT Save_MapObject_Ins(string szMapDataPath); // MapObject_Ins ����
	HRESULT Load_MapObject_Ins(const _tchar* wszMapDataPath); // MapObject_Ins �ε�

	void Deep_Copy_Name(const _tchar* wszName = nullptr); // �� �̸� ���ڿ� ���� ����
	void Deep_Copy_Path(const _tchar* wszPath); // �� ��� ���ڿ� ���� ����
	void Deep_Copy_Tag(const _tchar* wszTag); // �� ������Ʈ �±� ���ڿ� ���� ����
	const _tchar* Deep_Copy(const _tchar* wszString); // ������ ���ڿ��� ���� �����ϴ� �Լ�

	_float3 Find_PickingPos(); // ������ ��ŷ ��ġ�� ã�� �Լ�
	HRESULT Create_Dummy(); // Dummy ����
	HRESULT Delete_Dummy(); // Dummy ����
	HRESULT Save_Model_Path(_uint iType, const _tchar* pFilePath); // ���α׷��� ����� �� ȣ����Ѽ� ���� ��ε��� ������

private:
	// �޴� On Off _bool ���� ����
	_bool m_isCheckPicking = { false };
	_bool m_isSelectModel = { false };
	_bool m_isCurrentMapObject = { false };
	_bool m_isSaveLoad = { false };
	_bool m_isInsSaveLoad = { false };
	_bool m_isInstallObject = { true };
	_bool m_isPickingObject = { false };
	_bool m_isChangeObject = { false };
	_bool m_isBrushingMap = { false };

	// ��ư On Off _bool ���� ����
	_bool m_isDeleteObject = { false }; // �� ������Ʈ ��ü ���� ������
	_bool m_isDeleteBrush = { false }; // �귯�� ��ü ���� ������

	// Radio ��ư _int ���� ����
	_int m_iInstallMethod = { 0 }; // MapObject ��ġ ��� ���� ����

	CMapDummy* m_pDummy = { nullptr }; // �����ص� Dummy�� �ּ�
	CMapObject* m_pObject = { nullptr }; // ��ġ�� MapObject�� �ּ�
	CMapObject_Ins* m_pObjIns = { nullptr }; // ��ġ�� MapObejct_Ins�� �ּ�
	_uint m_iMapObjectIndex = { 0 }; // ���� �ʿ� ��ġ�� �� ������Ʈ�� ����

	string m_strPath = "";// ���Ӱ� ������ ���̺� ���� �̸� �޾ƿ��� �κ�
	_char m_szPath[MAX_PATH] = ""; // ���Ӱ� ������ ���̺� ���� �̸� �޾ƿ��� �κ�
	string m_strPath_Ins = "";// ���Ӱ� ������ �ν��Ͻ� ���̺� ���� �̸� �޾ƿ��� �κ�
	_char m_szPath_Ins[MAX_PATH] = ""; // ���Ӱ� ������ �ν��Ͻ� ���̺� ���� �̸� �޾ƿ��� �κ�
	string m_strFindModel = ""; // ã�� �� �̸�
	_char m_szFindModel[MAX_PATH] = ""; // ã�� �� �̸��� �޾ƿ� ���ڿ�
	_tchar m_wszMapLoadDataPath[MAX_PATH] = TEXT(""); // ImGuiFileDialog�� �о�� ������Ʈ ���
	_tchar m_wszMapSaveDataPath[MAX_PATH] = TEXT(""); // �� ������Ʈ�� Save�� ���
	_tchar m_wszMapInsLoadDataPath[MAX_PATH] = TEXT(""); // ImGuiFileDialog�� �о�� �ν��Ͻ� ������Ʈ ���
	_tchar m_wszMapInsSaveDataPath[MAX_PATH] = TEXT(""); // �ν��Ͻ� �� ������Ʈ�� Save�� ���
	
	_float m_vDummyMatrix[DUMMY_END][3];	// ������ ���� ���, �� ���� .ddd ���Ͽ� ������

	_float3 m_vRandomInstall[2] = { { -1.f, -1.f, -1.f }, { -1.f, -1.f, -1.f } }; // ���� ��ġ�� �� ���� ����
	_uint m_iRandomInstall_Seq = { 0 }; // ���� ����

	_int m_iModelIndex = { 0 }; // ���õ� �� �ε���
	_int m_iTagIndex = { 0 }; // ���õ� �� ������Ʈ �±� �ε���
	string m_strCurrentModel = { "Dummy" }; // ���� Ȱ��ȭ�� �� �̸�, �ʱⰪ�� ����
	vector<string> m_vecModelList; // ���� �߰��ص� �� �̸� ����Ʈ
	vector<const _tchar*> m_vecModelList_t; // �� �̸��� _tchar�� �����ص�
	vector<const _tchar*> m_vecModelPath_t; // �� ��θ� _tchar�� �����ص�
	vector<wstring> m_vecMapObjectTag; // �ʿ� �߰��� ������Ʈ���� �ѹ��� �±׸� ����
	vector<string> m_vecObjectTag_s; // �ѹ��� �±׸� string���� ������
 	vector<SAVEOBJECTDESC> m_vecSaveObject; // ������ �� ������Ʈ�� ���� ����

	_float m_fTimeAcc = { 0.f }; // MapObject ���� ��ġ �ð� ��Ʈ�� ����
	_float m_fTimeAccBrush = { 0.f }; // Brush ���� ��ġ �ð� ��Ʈ�� ����

	_uint m_iInsObjectCnt = { 0 }; // �ν��Ͻ��� �� ������Ʈ ����
	vector<SAVEINSOBJECTDESC> m_vecSaveInsObject; // ����� �ν��Ͻ� �� ������Ʈ�� ���� ������ �޾ƿ� ����
	vector<_float4x4> m_vecSaveInsObjectWorld; // �ν��Ͻ����� ������ ��ü���� ���� ���� ���

	_float m_fBrushSize = { 10.f }; // �귯�� ������
	_int m_iDiffuseIndex = { 0 }; // � �ؽ�ó�� Diffuse�� �����ߴ���
	_int m_iBrushIndex = { 1 };	// � �ؽ�ó�� Brush�� �����ߴ���

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END