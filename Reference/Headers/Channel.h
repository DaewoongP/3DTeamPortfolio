#pragma once
/* =============================================== */
// 
//	�� : ��ö��
//	�� :
//
/* =============================================== */

#include "Model.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;

public:
	_uint Get_BoneIndex() const { return m_iBoneIndex; }
	// �����ϴ� Ű������ �� �ε����� �ش��ϴ� Ű�������� �ð��� ��ȯ
	_float Get_CurrentKeyFrameTime(_uint iCurrentIndex) const { return m_MatrixKeyFrames[iCurrentIndex].fTime; }
	// ä�� Ű������ �� ���� ū�� ��ȯ
	_uint	Get_NumKeyFrames() const { return m_iNumKeyFrames; }

	void	Set_CurrentKeyFrame_Zero() {}
	void Delete_Translation();
	void Delete_Rotation();

public:
	HRESULT Initialize(const Engine::CHANNEL& Channel, const CModel::BONES& Bones);
	// Time Acc�� �ش��ϴ� ���� ä���� Ű������ ���°��� ������� ���������Ͽ� ���� ��������� �����ϴ� �Լ�.
	void	Invalidate_TransformationMatrix(CModel::BONES& Bones, _float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex);
	void	Invalidate_TransformationMatrix_Lerp(CModel::BONES& Bones, _float fTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex, _double LerpTimeAcc);
	void	Lerp_TransformationMatrix(CModel::BONES& Bones, class CChannel* pCurrentChannel, _float fDuration, _float fTimeAcc, _uint iCurrentKeyFrameIndex);

private:
	_tchar				m_szName[MAX_STR] = TEXT("");
	// ������, �����̼�, ������ �� ���� �������� ���� ���� ����.
	_uint				m_iNumKeyFrames = { 0 };
	vector<MATRIXFRAME>	m_MatrixKeyFrames;

	// ���� ä�ΰ� �̸��� ���� ���� �ε��� ��
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const Engine::CHANNEL& Channel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END