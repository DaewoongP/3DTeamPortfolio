#pragma once
#include "ImWindow.h"
#include "Model.h"
#include "MeshGenerationPoint.h"

BEGIN(Tool)

//���� triangleMesh�� �̿��� �� ���� ������ ���� triangleMesh�� �����س��� â�Դϴ�.
//�ҷ��� �𵨰� ������ ������ ������ ���� ���� �����ϰ� Generate ��ư�� ���� ���� �𵨿� �������� ������ �۾��� �����մϴ�.
//������ ���� ���� �Ŀ� discard ��ư�� ���� �浹�� �Ͼ�� ���� �������� ������ �� ������,
//���ÿ� indices�� �����ǰ�, ��ŷ�� ���� �� �������� ��ġ�� ������ �� �ֽ��ϴ�.

class CMeshGeneration_Window final : public CImWindow
{
private:
	explicit CMeshGeneration_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMeshGeneration_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//�� �������� �����
	void OpenFile_Button();
	void Select_Model();
	void AddModel_Button();

	//CMeshGenerationPoint�� �������� ������
	void Setting_VerticesPoints();

private:
	class CDummy_Generation*	m_pDummyObject = { nullptr };
	class CRectDebug*			m_pRectDebug = { nullptr };
	class CPhysXRender*			m_pPhysXRender = { nullptr };
	vector<vector<CMeshGenerationPoint*>> m_VertciesPoint;

	//���� ���õ� ���� �ε��� �� �ִ� ������. 
	_int			m_iMaxModelIndex = { 0 };
	_int		    m_iModelIndex = { 0 };

	//�ҷ��� �𵨵��� �±���.
	vector<string>  m_vecModelList;
	vector<wstring> m_vecModelList_t;

	//�ҷ��� ���� �±���. ���嶧 ����Ϸ��� ���ܵ�.
	_tchar			m_wszCurrentDummyModelTag[MAX_PATH] = {};

	//������ �����ִ� rect debug Buffer
	_int			m_vPoint_Range[2] = {};
	_int			m_vPoint_Count[2] = {};
	
public:
	static CMeshGeneration_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END