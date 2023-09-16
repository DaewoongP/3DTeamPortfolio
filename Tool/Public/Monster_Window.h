#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CDummy;

class CMonster_Window final : public CImWindow
{
	enum DUMMY_MATRIX { DUMMY_SCALE, DUMMY_ROT, DUMMY_TRANS, DUMMY_END }; // ������ ����

	typedef struct SaveObjectDesc
	{
		_float4x4 matTransform; // ���� ���
		_uint iTagLen; // ���ڿ� ����
		_tchar wszTag[MAX_PATH]; // ������Ʈ ����(�� ������Ʈ �̸�)
	}SAVEOBJECTDESC;


private:
	explicit CMonster_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMonster_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// �޴� ����
	void Picking_Menu(); // ��ŷ �޴�
	void Show_MonsterTable(); // ���� ��ġ�Ǿ� �ִ� �� ������Ʈ Ȯ��
	void Save_Load_Menu(); // ������Ʈ ���̺� �ε� �޴�
	HRESULT Save_Monsters(const string& szMapDataPath); // MapObject ����
	HRESULT Load_Monsters(const wstring& wstrMapDataPath); // MapObject �ε�

	_float3 Find_PickingPos(); // ������ ��ŷ ��ġ�� ã�� �Լ�
	HRESULT Create_Dummy(); // Dummy ����
	HRESULT Delete_Dummy(); // Dummy ����
	HRESULT Load_Model_Prototypes(const wstring& wstrFilePath); // ���α׷��� ���۵� �� ȣ����Ѽ� ��ü�� ������Ÿ�� ����

private:
	// �޴� On Off _bool ���� ����
	_bool m_isSaveLoad = { false };
	_bool m_isCheckPicking = { false };
	_bool m_isCreateMonster = { false };
	_bool m_isCurrentMapObject = { false };

	// ���콺 ��ŷ ���� ����
	_bool m_isLockMouseMove = { false }; // ���콺 ������ ���� ����
	_float3 m_vLockingMousePos = { 0.f, 0.f, 0.f }; // ���ѵ� ���콺 ��ġ

	CDummy* m_pCurrentDummy = { nullptr }; // ���� ���� ���� ������
	_int m_iTableIndex = { 0 }; // ������Ʈ ���̺� ���õ� ������Ʈ�� �ε���
	_int m_iPrototypeTableIndex = { 0 };
	_float m_vDummyMatrix[DUMMY_END][3];	// ���� ������ ���� ���, �� ���� .ddd ���Ͽ� ������

	string m_strFindModel = ""; // ã�� �� �̸�
	_char m_szFindModel[MAX_PATH] = ""; // ã�� �� �̸��� �޾ƿ� ���ڿ�

	_char m_szDataName[MAX_PATH] = { "" };
	vector<SAVEOBJECTDESC> m_SaveObjects; // ������ ������Ʈ�� ���� ����
	vector<wstring> m_wstrModelPaths;
	vector<wstring> m_wstrModelTags;
	vector<CDummy*> m_Dummies;

public:
	static CMonster_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END