#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;
class CMapObject;

class CObject_Window final : public CImWindow
{
	typedef struct SaveObjectDesc
	{
		//_float4x4 matTransform; // ���� ���
		_float3 vPos; // ��ġ ��
		_tchar wszTag[MAX_PATH]; // ������Ʈ ����
	}SAVEOBJECTDESC;

private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Picking_Menu(); // ��ŷ �޴�
	void Install_Object(_float3 vPos); // ������Ʈ ��ġ �޴�
	void Select_Model(); // �� ���� �޴�
	void Save_Load_Menu(); // ���̺� �ε� �޴�
	HRESULT Save_MapObject();
	HRESULT Load_MapObject();

	void Deep_Copy_Name(); // �� �̸� ���ڿ� ���� ����
	void Deep_Copy_Path(const _tchar* wszPath); // �� ��� ���ڿ� ���� ����
	void Deep_Copy_Tag(const _tchar* wszTag); // �� ������Ʈ �±� ���ڿ� ���� ����

	_float3 Find_PickingPos(); // ������ ��ŷ ��ġ�� ã�� �Լ�
	HRESULT Create_Dummy(); // ���� 1�� Dummy ����

private:
	CMapDummy* m_pDummy = { nullptr }; // �����ص� Dummy�� �ּ�
	CMapObject* m_pObject = { nullptr }; // ��ġ�� MapObject�� �ּ�
	_uint m_iMapObjectIndex = { 0 }; // ���� �ʿ� ��ġ�� �� ������Ʈ�� ����

	string m_strCurrentModel = { "Prototype_Component_Model_Tree" }; // ���� Ȱ��ȭ�� �� �̸�, �ʱⰪ�� ����

	_int m_iModelIndex = { 0 }; // ���õ� �� �ε���
	vector<string> m_vecModelList; // ���� �߰��ص� �� �̸� ����Ʈ
	vector<const _tchar*> m_vecModelList_t; // �� �̸� ����Ʈ�� _tchar�� �����ص�
	vector<const _tchar*> m_vecModelPath_t; // �� ���� ��θ� _tchar�� �����ص�
	vector<const _tchar*> m_vecMapObjectTag_t; // �ʿ� �߰��� ������Ʈ���� ����

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END