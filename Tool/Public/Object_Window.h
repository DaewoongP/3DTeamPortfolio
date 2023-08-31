#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;

class CObject_Window final : public CImWindow
{
private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void PickingMenu(); // ��ŷ �޴�
	void SelectModel(); // �� ���� �޴�
	void Deep_Copy_Name(); // �� �̸� ���ڿ� ���� ����
	void Deep_Copy_Path(const _tchar* wszPath); // �� ��� ���ڿ� ���� ����

	_float3 Find_PickingPos(); // ������ ��ŷ ��ġ�� ã�� �Լ�
	HRESULT Create_Dummy(); // ���� 1�� Dummy ����

private:
	CMapDummy* m_pDummy = { nullptr }; // �����ص� Dummy�� �ּ�

	string m_strCurrentModel = { "Prototype_Component_Model_Tree" }; // ���� Ȱ��ȭ�� �� �̸�, �ʱⰪ�� ����

	_int m_iModelIndex = { 0 }; // ���õ� �� �ε���
	vector<string> m_vecModelList; // ���� �߰��ص� �� �̸� ����Ʈ
	vector<const _tchar*> m_vecModelList_t; // �� �̸� ����Ʈ�� _tchar�� �����ص�
	vector<const _tchar*> m_vecModelPath_t; // �� ���� ��θ� _tchar�� �����ص�

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END