#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;
class CMapObject;
class CMapObject_Ins;

class CObject_Window final : public CImWindow
{
	enum DUMMY_MATRIX { DUMMY_SCALE, DUMMY_ROT, DUMMY_TRANS, DUMMY_END }; // ������ ����
	enum INSTALL_METHOD { ONE_METHOD, CONTINUOUS_METHOD, MULTI_METHOD, END_METHOD }; // ��ġ ���

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
	void Install_Multi_Object(_float3 vPos); // ������Ʈ ����(�ν��Ͻ�) ��ġ �޴�
	void Select_Model(); // �� ���� �޴�
	void Current_MapObject(); // ���� ��ġ�Ǿ� �ִ� �� ������Ʈ Ȯ��
	void Save_Load_Menu(); // ������Ʈ ���̺� �ε� �޴�
	void Ins_Save_Load_Menu(); // �ν���Ʈ ������Ʈ ���̺� �ε� �޴�
	void Delete_Object_Menu(); // ������Ʈ ���� ���� �޴�
	void Mesh_Picking_Menu(); // �޽� ��ŷ �޴�
	void Delete_Picking_Object(); // ��ŷ�� ������Ʈ ���� �޴�
	HRESULT Save_MapObject(); // MapObject ����
	HRESULT Load_MapObject(); // MapObject �ε�
	HRESULT Save_MapObject_Ins(); // MapObject_Ins ����
	HRESULT Load_MapObject_Ins(); // MapObject_Ins �ε�

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

	// ��ư On Off _bool ���� ����
	_bool m_isDeleteObject = { false };

	// Radio ��ư _int ���� ����
	_int m_iInstallMethod = { 0 }; // MapObject ��ġ ��� ���� ����

	CMapDummy* m_pDummy = { nullptr }; // �����ص� Dummy�� �ּ�
	CMapObject* m_pObject = { nullptr }; // ��ġ�� MapObject�� �ּ�
	CMapObject_Ins* m_pObjIns = { nullptr }; // ��ġ�� MapObejct_Ins�� �ּ�
	_uint m_iMapObjectIndex = { 0 }; // ���� �ʿ� ��ġ�� �� ������Ʈ�� ����

	string m_strCurrentModel = { "Dummy" }; // ���� Ȱ��ȭ�� �� �̸�, �ʱⰪ�� ����
	_float m_vDummyMatrix[DUMMY_END][3];	// ������ ���� ���, �� ���� .ddd ���Ͽ� ������

	_int m_iModelIndex = { 0 }; // ���õ� �� �ε���
	_int m_iTagIndex = { 0 }; // ���õ� �� ������Ʈ �±� �ε���
	vector<string> m_vecModelList; // ���� �߰��ص� �� �̸� ����Ʈ
	vector<const _tchar*> m_vecModelList_t; // �� �̸��� _tchar�� �����ص�
	vector<const _tchar*> m_vecModelPath_t; // �� ��θ� _tchar�� �����ص�
	vector<wstring> m_vecMapObjectTag; // �ʿ� �߰��� ������Ʈ���� �ѹ��� �±׸� ����
	vector<string> m_vecObjectTag_s; // �ѹ��� �±׸� string���� ������
 	vector<SAVEOBJECTDESC> m_vecSaveObject; // ������ �� ������Ʈ�� ���� ����

	vector<SAVEINSOBJECTDESC> m_vecSaveInsObject; // ������ �ν��Ͻ��� �� ������Ʈ�� ���� ����

	_bool m_bOne = { true };

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END