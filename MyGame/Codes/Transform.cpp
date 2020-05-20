#include "stdafx.h"
#include "..\Headers\Transform.h"
#include "Terrain.h"
#include "TurnMgr.h"
#include "LevelMgr.h"
#include "Texture.h"
#include "ModuleMgr.h"
#include "Pipline.h"
USING(MyGame)

_int CTransform::m_iTurnCnt = 0;
CTransform::CTransform(LPDIRECT3DDEVICE9 _pGraphic_Device)
	:CModule(_pGraphic_Device)
{
	m_tStateDesc.movePerTurn = 1;
	m_tStateDesc.radianPerSec = 1;
	m_tStateDesc.speedPerSec = 1;
}

CTransform::CTransform(CTransform & _module)
	: CModule(_module)
{

}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void * _pArg)
{
	if (nullptr != _pArg)
		memcpy(&m_tStateDesc, _pArg, sizeof(STATEDESC));


	m_vSize = Vector3(1.f, 1.f, 1.f);
	m_vRotation = Vector3(0.f, 0.f, 0.f);
	m_vPosition = Vector3(0.f, 0.f, 0.f, 1.f);
	m_vDir = Vector3();
	m_vLook = Vector3(1.f, 0.f, 0.f);
	m_vColliderSize = Vector3(1.f, 1.f, 1.f);
	D3DXMatrixIdentity(&m_StateMatrix);

	//콜라이더를 위한 VIBuffer
	m_pVIBuffer = dynamic_cast<CVIBuffer*>(CModuleMgr::Get_Instance()->Get_Module(L"VIBuffer", SCENE_STATIC));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
	//콜라이더를 위한 텍스쳐
	m_pTexture = dynamic_cast<CTexture*>(CModuleMgr::Get_Instance()->Get_Module(L"empty_bound", SCENE_STATIC));
	if (nullptr == m_pTexture)
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::Update_Route(_double _timeDelta)
{
	if (m_bStop)
		return E_FAIL;


	//모든 루트를 이동하면 멈춤
	if ((_int)m_iCurrRouteIndex >= m_Route.size() ||
		(m_iTotalMoveCnt >= (_int)m_tStateDesc.movePerTurn * m_iTurnCnt))
	{
		//초기화
		m_bStop = true;
		m_iCurrRouteIndex = 0;
		m_iTotalMoveCnt = 0;
		m_iCntForTurn = 0;
		m_Route.swap(vector<CTerrain*>());
		m_pTarget = nullptr;
		// isTurnEnd는 false여야 한다. true면 계속 턴이 넘어가서 몬스터가 GO_Route계속부름.
		return TURN_END;
	}

	CTransform* pTransform = (CTransform*)m_Route[m_iCurrRouteIndex]->Get_Module(L"Transform");
	if (nullptr == pTransform)
		return E_FAIL;

	m_vDir = pTransform->Get_Position() - m_vPosition;

	_float dist = m_vDir.magnitude();
	_float fSpeed = m_tStateDesc.speedPerSec;


	if (m_vDir.magnitude() > 2.f)
	{
		//가려는 경로를 다시 체크해서 갈 수 있는 곳이면
		if (m_Route[m_iCurrRouteIndex]->IsMovable(this))
		{
			m_vPosition += m_vDir.nomalize() * float(fSpeed * _timeDelta);
		}
		//갈 수 없으면 루트갱신
		else
		{
			//루트 삭제
			m_Route.swap(vector<CTerrain*>());

			CLevel* pLevel = CLevelMgr::Get_Instance()->Get_CurrLevel();
			RETURN_FAIL_IF_NULL(pLevel);
			//타겟이 있으면 타겟을 따라가고, 아니면 목표위치로 간다.
			if (m_pTarget != nullptr)
			{

				pLevel->Get_Route(m_vPosition, m_pTarget->Get_Position(), m_Route, this);
			}
			else
			{

				pLevel->Get_Route(m_vPosition, m_vDst, m_Route, this);
			}

			//루트 인덱스 초기화
			m_iCurrRouteIndex = 0;
		}

		FaceDir(m_vDir);
	}
	//타일에 도달했으면
	else
	{
		//지나간 타일 수 세기
		++m_iCntForTurn;
		//턴에 상관없이 지금까지 이동한 타일 수
		++m_iTotalMoveCnt;
		//한 타일지나면 인덱스 더하기
		++m_iCurrRouteIndex;

		//타깃을 쫒는 상태라면 (주로 몬스터)
		if (nullptr != m_pTarget)
		{
			//타깃의 위치는 변하기 때문에 루트를 매번 갱신해줘야한다.(한칸 갈때마다)
			m_Route.swap(vector<CTerrain*>());
			CLevel* pLevel = CLevelMgr::Get_Instance()->Get_CurrLevel();
			RETURN_FAIL_IF_NULL(pLevel);
			pLevel->Get_Route(m_vPosition, m_pTarget->Get_Position(), m_Route, this);

			//갱신시에는 루트 인덱스도 초기화
			m_iCurrRouteIndex = 0;
		}
	}

	//행동력만큼 이동했는지 체크
	//m_iCntForTurn은 한턴에 이동한 타일 수를 의미함.
	if ((m_iCntForTurn >= (_int)m_tStateDesc.movePerTurn))
	{
		m_bTurnEnd = true;
		m_iCntForTurn = 0;
	}

	return S_OK;
}

HRESULT CTransform::Update_Normal(_double _timeDelta)
{
	m_vPosition += m_vDir_Normal * float(m_tStateDesc.speedPerSec * _timeDelta);
	return S_OK;
}

_int CTransform::Update_Transform()
{
	_matrix scalingMatrix, rotationXMatrix, rotationYMatrix, rotationZMatrix, translationMatrix,
		revolveXMatrix, revolveYMatrix, revolveZMatrix, parentMatrix;

	D3DXMatrixScaling(&scalingMatrix, m_vSize.x, m_vSize.y, m_vSize.z);
	D3DXMatrixRotationX(&rotationXMatrix, m_vRotation.x);
	D3DXMatrixRotationX(&rotationYMatrix, m_vRotation.y);
	D3DXMatrixRotationX(&rotationZMatrix, m_vRotation.z);
	D3DXMatrixTranslation(&translationMatrix, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	D3DXMatrixRotationX(&revolveXMatrix, m_vRevolveAngle.x);
	D3DXMatrixRotationX(&revolveYMatrix, m_vRevolveAngle.y);
	D3DXMatrixRotationX(&revolveZMatrix, m_vRevolveAngle.z);
	
	//부모 행렬셋팅
	if (nullptr != m_pParent)
	{
		_matrix parent = m_pParent->Get_Matrix();
		D3DXMatrixTranslation(&parentMatrix, parent.m[3][0], parent.m[3][1], parent.m[3][2]);
	}
	else
		D3DXMatrixIdentity(&parentMatrix);


	m_StateMatrix = scalingMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix * translationMatrix
		* parentMatrix;

	return 0;
}

HRESULT CTransform::Render_Collider()
{
	if (nullptr == m_pTexture ||
		nullptr == m_pVIBuffer)
		return E_FAIL;

	ALPHATEST;
	m_pTexture->Set_Texture(0);

	//사이즈를 잠깐 늘려서 렌더
	Vector3 originSize = m_vSize;
	m_vSize = m_vColliderSize;
	Update_Transform();
	m_pVIBuffer->Set_Transform(m_StateMatrix * CPipline::Get_Instance()->Get_ViewMatrix());
	m_vSize = originSize;
	Update_Transform();

	m_pVIBuffer->Render();

	ALPHATEST_END;
	return S_OK;

}

HRESULT CTransform::Set_Position(Vector3 _vPosition)
{
	m_vPosition = _vPosition;
	m_vPosition.w = 1;
	return S_OK;
}

HRESULT CTransform::Set_Size(Vector3 _vSize)
{
	m_vSize = _vSize;
	return S_OK;
}

HRESULT CTransform::Set_Rotation(Vector3 _vRotation)
{
	m_vRotation = _vRotation;
	return S_OK;
}



Vector3 CTransform::Get_WorldPos()
{
	//부모가 없으면 그냥 포지션 리턴
	if (nullptr == m_pParent)
		return m_vPosition;

	Vector3 result = m_vPosition + m_pParent->Get_WorldPos();
	result.z = 0.f;
	result.w = 1.f;

	//부모가 있으면 부모의 월드포지션에 자신의 로컬포지션을 더한 값을 리턴
	return result;
}

RECT CTransform::Get_Collider()
{
	RECT rc = {};
	float fX = m_vPosition.x;
	float fY = m_vPosition.y;
	int iCX = (int)m_vColliderSize.x;
	int iCY = (int)m_vColliderSize.y;

	rc.left = (LONG)fX - (iCX >> 1);
	rc.right = (LONG)fX + (iCX >> 1);
	rc.top = (LONG)fY - (iCY >> 1);
	rc.bottom = (LONG)fY + (iCY >> 1);
	return rc;
}

RECT CTransform::Get_RECT()
{
	RECT rc = {};
	float fX = m_vPosition.x;
	float fY = m_vPosition.y;
	int iCX = (int)m_vSize.x;
	int iCY = (int)m_vSize.y;

	rc.left = (LONG)fX - (iCX >> 1);
	rc.right = (LONG)fX + (iCX >> 1);
	rc.top = (LONG)fY - (iCY >> 1);
	rc.bottom = (LONG)fY + (iCY >> 1);
	return rc;
}


void CTransform::FaceDir(Vector2 _vDir)
{
	float sizeX = abs(m_vSize.x);

	//사이즈가 음수면 왼쪽, 양수면 오른쪽을 바라본다.
	if (_vDir.x < 0)
		m_vSize.x = -1 * sizeX;
	else
		m_vSize.x = sizeX;
}

HRESULT CTransform::LookAt(CTransform * pTargetTransform)
{
	Vector3		vDirection = pTargetTransform->m_vPosition - m_vPosition;

	_float		fCosTheta = D3DXVec4Dot(&m_vLook.Nomalize(), &vDirection.Nomalize());

	if (pTargetTransform->m_vPosition.y >= m_vPosition.y)
		m_vRotation.z = acosf(fCosTheta);
	else
		m_vRotation.z = D3DXToRadian(360.0f) - acosf(fCosTheta);

	return S_OK;
}


void CTransform::Set_Parent(CTransform * pParent)
{
	if (nullptr == pParent)
		return;
	
	//로컬좌표 변환
	m_vPosition = m_vPosition - pParent->Get_WorldPos();
	m_vPosition.z = 0.f;
	m_vPosition.w = 1.f;
	//부모 매트릭스
	m_pParent = pParent;
}

HRESULT CTransform::MoveToTarget(CTransform * _pTransform, _double _timeDelta, _double _StopDistance)
{

	Vector3 vDir = _pTransform->Get_Position() - m_vPosition;

	if (vDir.magnitude() >= m_StopDistance)
	{
		m_vPosition += vDir.nomalize() * float(m_tStateDesc.speedPerSec * _timeDelta);
	}
	return S_OK;

}

HRESULT CTransform::MoveToTarget(CTransform * _pTransform, _double _timeDelta, _double _StopDistance, _double _Speed)
{

	Vector3 vDir = _pTransform->Get_Position() - m_vPosition;

	if (vDir.magnitude() >= m_StopDistance)
	{
		m_vPosition += vDir.nomalize() * float(_Speed * _timeDelta);
	}
	return S_OK;
}

HRESULT CTransform::MoveToDirAuto(Vector3 _vDir, _double _timeDelta)
{

	m_vDir_Normal = _vDir;

	return S_OK;
}

HRESULT CTransform::MoveToDirAuto(Vector3 _vDir, _double _timeDelta, _double _Speed)
{

	m_vDir_Normal = _vDir;
	m_tStateDesc.speedPerSec = _Speed;

	return S_OK;
}

HRESULT CTransform::MoveToDir(Vector3 _vDir, _double _timeDelta)
{
	m_vPosition += _vDir.nomalize() * float(m_tStateDesc.speedPerSec * _timeDelta);

	return S_OK;
}

HRESULT CTransform::MoveToDir(Vector3 _vDir, _double _timeDelta, _double _Speed)
{
	m_vPosition += _vDir.nomalize() * float(_Speed * _timeDelta);
	return S_OK;
}

HRESULT CTransform::MoveToDst(Vector3 _vDst, _double _timeDelta, _double _fStopDistance)
{
	Vector3 vDir = _vDst - m_vPosition;

	if (vDir.magnitude() >= m_StopDistance)
	{
		m_vPosition += vDir.nomalize() * float(m_tStateDesc.speedPerSec * _timeDelta);
	}
	return S_OK;
}

HRESULT CTransform::MoveToDst(Vector3 _vDst, _double _timeDelta, _double _fStopDistance, _double _Speed)
{

	Vector3 vDir = _vDst - m_vPosition;

	if (vDir.magnitude() >= m_StopDistance)
	{
		m_vPosition += vDir.nomalize() * float(_Speed * _timeDelta);
	}
	return S_OK;
}

HRESULT CTransform::Add_Froce(Vector3 _vDir, _float _fForce, _double _timeDelta)
{
	m_vPosition += _vDir * _fForce * float(_timeDelta);
	return S_OK;
}



HRESULT CTransform::Go_Route(vector<CTerrain*> _route, _double _StopDistance, _int _iTurnCnt)
{

	if (_route.empty())
		return E_FAIL;

	m_iTurnCnt = _iTurnCnt;
	m_iCurrRouteIndex = 0;
	m_Route.swap(vector<CTerrain*>());
	m_bTurnEnd = false;
	m_bStop = false;
	m_Route = _route;
	m_StopDistance = _StopDistance;
	CTransform* pTransform = (CTransform*)m_Route.back()->Get_Module(L"Transform");
	if (nullptr == pTransform)
		return E_FAIL;
	m_vDst = pTransform->Get_Position();


	return S_OK;
}

HRESULT CTransform::Go_Target(CTransform * _pTarget, _double _StopDistance)
{
	if (nullptr == _pTarget)
		return E_FAIL;

	m_iCurrRouteIndex = 0;
	m_Route.swap(vector<CTerrain*>());
	m_bTurnEnd = false;
	m_bStop = false;
	m_pTarget = _pTarget;
	m_StopDistance = _StopDistance;
	//루트 설정
	CLevel* pLevel = CLevelMgr::Get_Instance()->Get_CurrLevel();
	RETURN_FAIL_IF_NULL(pLevel);
	pLevel->Get_Route(m_vPosition, m_pTarget->Get_Position(), m_Route, this);

	return S_OK;
}




CTransform * CTransform::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CTransform* pInstance = new CTransform(_pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Fail to create CTransform");
		delete pInstance;
		pInstance = nullptr;
	}
	return pInstance;
}

CModule* CTransform::Clone(void * _pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Fail to clone CTransform");
		delete pInstance;
		pInstance = nullptr;
	}
	return pInstance;
}

void CTransform::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);
	CModule::Free();

}

