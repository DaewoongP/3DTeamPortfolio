#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;
class CMapObject;

class CObject_Window final : public CImWindow
{
		//_float4x4 matTransform; // ���� ���
	typedef struct SaveObjectDesc
	{
		_float3 vPos; // ��ġ ��
		_uint iTagLen; // ���ڿ� ����
		_tchar wszTag[MAX_PATH]; // ������Ʈ ����(�� ������Ʈ �̸�)
	}SAVEOBJECTDESC;

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
	void Select_Model(); // �� ���� �޴�
	void Current_MapObject(); // ���� ��ġ�Ǿ� �ִ� �� ������Ʈ Ȯ��
	void Save_Load_Menu(); // ���̺� �ε� �޴�
	HRESULT Save_MapObject(); // MapObject ����
	HRESULT Load_MapObject(); // MapObject �ε�

	void Deep_Copy_Name(const _tchar* wszName = nullptr); // �� �̸� ���ڿ� ���� ����
	void Deep_Copy_Path(const _tchar* wszPath); // �� ��� ���ڿ� ���� ����
	void Deep_Copy_Tag(const _tchar* wszTag); // �� ������Ʈ �±� ���ڿ� ���� ����
	const _tchar* Deep_Copy(const _tchar* wszString); // ������ ���ڿ��� ���� �����ϴ� �Լ�

	_float3 Find_PickingPos(); // ������ ��ŷ ��ġ�� ã�� �Լ�
	HRESULT Create_Dummy(); // ���� 1�� Dummy ����
	HRESULT Save_Model_Path(_uint iType, const _tchar* pFilePath); // ���α׷��� ����� �� ȣ����Ѽ� ���� ��ε��� ������

private:
	// �޴� On Off _bool ���� ����
	_bool m_isCheckPicking = { false };
	_bool m_isSelectModel = { false };
	_bool m_isCurrentMapObject = { false };
	_bool m_isSaveLoad = { false };
	_bool m_isInstallObject = { true };

	CMapDummy* m_pDummy = { nullptr }; // �����ص� Dummy�� �ּ�
	CMapObject* m_pObject = { nullptr }; // ��ġ�� MapObject�� �ּ�
	_uint m_iMapObjectIndex = { 0 }; // ���� �ʿ� ��ġ�� �� ������Ʈ�� ����

	string m_strCurrentModel = { "Dummy" }; // ���� Ȱ��ȭ�� �� �̸�, �ʱⰪ�� ����

	_int m_iModelIndex = { 0 }; // ���õ� �� �ε���
	_int m_iTagIndex = { 0 }; // ���õ� �� ������Ʈ �±� �ε���
	vector<string> m_vecModelList; // ���� �߰��ص� �� �̸� ����Ʈ
	vector<const _tchar*> m_vecModelList_t; // �� �̸��� _tchar�� �����ص�
	vector<const _tchar*> m_vecModelPath_t; // �� ��θ� _tchar�� �����ص�
	vector<wstring> m_vecMapObjectTag; // �ʿ� �߰��� ������Ʈ���� �ѹ��� �±׸� ����
	vector<string> m_vecObjectTag_s; // �ѹ��� �±׸� _char�� ������
 	vector<SAVEOBJECTDESC> m_vecSaveObject; // ������ �� ������Ʈ�� ���� ����

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END