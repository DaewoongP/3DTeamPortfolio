#pragma once

#include "Base.h"
#include "Tool_Defines.h"


BEGIN(Tool)

class CCamera_Point;
#ifdef _DEBUG
class CCamera_Line;
#endif
class CCutScene_Camera_Tool :
    public CBase
{
public:
	enum CUTSCENEPOINT
	{
		CUTSCENEPOINT_SELECT,
		CUTSCENEPOINT_CREATE,
		CUTSCENEPOINT_DELETE,
		CUTSCENEPOINT_SECTION,
		CUTSCENEPOINT_END
	};

	enum CUTSCENE_EYE_OR_AT
	{
		CUTSCENE_AT,
		CUTSCENE_EYE,
		CUTSCENE_END
	};

	enum SECTION_SELECT
	{
		SECTION_START,
		SECTION_END
	};

	typedef struct tagCameraPointInfoDesc
	{
		CCamera_Point* pEyePoint{ nullptr };
		CCamera_Point* pAtPoint{ nullptr };
		_bool isLerp{ true };
		_float fDuration{ 1.0f };			//���ö��� / ���ǵ�
		_float fSplineLength{ 0.0f };		//���� ������ ���ö��� ���� �� �Ÿ� 10���� ���� 
	}CAMERAPOINTINFODESC;

private:
	explicit CCutScene_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutScene_Camera_Tool() = default;

public:
	HRESULT Initialize(void* pArg);
	void Tick(_float _fTimeDelta);
	HRESULT Render();




private:
	
	//����Ʈ�� ���� �ϱ� ����
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	//�ƾ��� ���� ������
	list<CAMERAPOINTINFODESC> m_CameraInfoList;
	//���� At
	list<CCamera_Point*> m_OriginAtList;

	//�˻��� �ݺ���
	list<CAMERAPOINTINFODESC>::iterator m_CurrentIterater = { m_CameraInfoList.end() };

	//���� ���õ� ������Ʈ(�����ϱ� ����)
	CCamera_Point* m_pCurrentPoint = { nullptr };

	//���� ���õ� AtPoint(�����ϱ� ����)
	CCamera_Point* m_pAtCurrentPoint = { nullptr };

	//���� ���̵� ��
	CCamera_Point* m_pCreateGuidePoint = { nullptr };
	
	//IMGUI ���� ��ư��
	_int m_iPointRadio = { 0 };
	//Eye,At ���п�
	_int m_iEyeOrAt = { 0 };

	//����Ʈ ���� �Ÿ�
	_float m_fDistance = { 10.0f };

	//�ʱ�ȭ ����üũ ��
	_bool m_isClearDoubleCheck{ false };

#ifdef _DEBUG

	//Eye ����
	CCamera_Line* m_pEyeLine = { nullptr };

	//At ����
	CCamera_Line* m_pAtLine = { nullptr };

	//Look ����
	CCamera_Line* m_pLookLine = { nullptr };
	
#endif

	//Line_Update��
	_bool m_isLineUpdate{ false };

	//���ǵ�(���� ũ�⸸ŭ �������� ���� �ϸ�...�����ٰ� ���� �������� �͵� ������ ��?)
	_float m_fSpeed{ 1.0f };				

	//���ǵ� �������� ����...
	_uint m_iSpeedLerp{ 0 };

	//���� ����
	//���� 
	list<CAMERAPOINTINFODESC>::iterator m_iterStart{ m_CameraInfoList.end() };
	//��
	list<CAMERAPOINTINFODESC>::iterator m_iterEnd{ m_CameraInfoList.end() };

	//���� ���� �ð� ����
	_float m_fSectionPlayTime{ 1.0f };

	_int m_iSectionRadio{ 0 };

private:
	//���콺�� ��ġ ���� ���
	void Fix_Point();

	//����Ʈ �ʱ�ȭ ��ü �ʱ�ȭ
	void Clear_CutSceneList();

	//AtPoint �ٲٱ�
	void Change_AtPoint(CCamera_Point* _pAtPoint);

private:
	//Ŭ�� �� RayPos, RayDir ��ȯ
	void Return_RayInfo(_Out_ _float4& _vRayPos, _Out_ _float4& _vRayDir);
	
	//���� ��ġ
	_float4 Point_Create_Position(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//����
	void Create_CameraInfo(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	void Create_OriginAt(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//��ü ��ȸ ����
	_bool Select_Eye_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);
	_bool Select_OriginAt_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//���� (������Ʈ ���ú��� �ʰ� ������ �Ѵ�.)
	void Delete_Eye_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);
	void Delete_OriginAt_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//ī�޶� ����Ʈ �� ���� ����
	void Fix_CameraPointInfo(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//���� ���� ������Ʈ
	void Create_Tick(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//���� ���� ������Ʈ
	void Select_Tick(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//���� ���� ������Ʈ
	void Delete_Tick(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//����Ʈ ������Ʈ
	void List_Tick(_float _TimeDelta);
	
	void Clear_DoubleCheck();

	HRESULT Save_CutSceneInfo(const _tchar* _wszFilePath);

	HRESULT Load_CutSceneInfo(const _tchar* _wszFilePath);

	void Save_And_Load();

	void Play_CutScene();

	void Add_CutScene(const CAMERAPOINTINFODESC& _CameraPointInfoDesc);

#ifdef _DEBUG

	//���� �غ�
	HRESULT Ready_Line();

	//�߰� �� ���� �ÿ�
	HRESULT Line_Update();

	//Eye���� ������Ʈ//���� �Ÿ� ����//ù��°�� 0 ������ ����
	HRESULT EyeLine_Update();

	//At���� ������Ʈ
	HRESULT AtLine_Update();

	//Look���� ������Ʈ
	HRESULT LookLine_Update();
#endif
	//������ ���ϴ� ��....(�� ������ �� �ð��ȿ� ���� �ϰڴ�.)(���� / �ѱ��� * �ð�)
	void Select_Start_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);
	void Select_End_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//���� ����Ʈ ����
	void Swap_Section_Point();

	//���� ������Ʈ
	void Section_Point_Update(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//���� �ӵ�
	void Set_Section_Point_Duration();

	//���� ���
	void Play_Section();

	


public:
	static CCutScene_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void * pArg = nullptr);
	virtual void Free() override;

};

END