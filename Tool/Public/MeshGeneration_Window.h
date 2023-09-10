#pragma once
#include "ImWindow.h"
#include "Model.h"
#include "MeshGenerationPoint.h"

BEGIN(Tool)

//모델의 triangleMesh를 이용해 더 작은 정점을 가진 triangleMesh를 추출해내는 창입니다.
//불러온 모델과 설정된 정점의 개수를 통해 값을 지정하고 Generate 버튼을 통해 실제 모델에 정점들을 입히는 작업을 진행합니다.
//정점에 모델을 입힌 후엔 discard 버튼을 통해 충돌이 일어나지 않은 정점들을 제거할 수 있으며,
//동시에 indices가 설정되고, 피킹을 통해 각 정점들의 위치를 변경할 수 있습니다.

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
	//모델 가져오고 변경용
	void OpenFile_Button();
	void Select_Model();
	void AddModel_Button();

	//CMeshGenerationPoint의 생성관련 로직들
	void Setting_VerticesPoints();

private:
	class CDummy_Generation*	m_pDummyObject = { nullptr };
	class CRectDebug*			m_pRectDebug = { nullptr };
	class CPhysXRender*			m_pPhysXRender = { nullptr };
	vector<vector<CMeshGenerationPoint*>> m_VertciesPoint;

	//현재 선택된 모델의 인덱스 및 최대 개수임. 
	_int			m_iMaxModelIndex = { 0 };
	_int		    m_iModelIndex = { 0 };

	//불러온 모델들의 태그임.
	vector<string>  m_vecModelList;
	vector<wstring> m_vecModelList_t;

	//불러온 모델의 태그임. 저장때 사용하려고 냄겨둠.
	_tchar			m_wszCurrentDummyModelTag[MAX_PATH] = {};

	//범위를 보여주는 rect debug Buffer
	_int			m_vPoint_Range[2] = {};
	_int			m_vPoint_Count[2] = {};
	
public:
	static CMeshGeneration_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END