#include "stdafx.h"
#include "..\Headers\Monster.h"
#include "ObjMgr.h"
#include "SceneMgr.h"
#include "Animator.h"
#include "Shader.h"
#include "Clock.h"
#include "DialogMgr.h"
#include "HpBar.h"
#include "BuffController.h"
#include "Item.h"
USING(MyGame)

CMonster::CMonster(CMonster & _rhs)
	:CCharacter(_rhs)
{
	SCENEID eSceneID = CSceneMgr::Get_Instance()->Get_CurrScene();
	m_pFocus = (CCharacter*)CObjMgr::Get_Instance()->Get_Player(eSceneID);
	m_pBuffCon = CBuffController::Create(m_pGraphic_Device);

	//멤버변수 셋팅
	m_iRecogRange = 6;
	m_iAwakeRange = 4;
	m_iAttackRange = 1;


}

HRESULT CMonster::Initialize(void * _param)
{
	if (nullptr == m_pTransform)
		return E_FAIL;



	//Hp바 셋팅
	m_pHpBar = CHpBar::Create(m_pGraphic_Device, m_pTransform->Get_Position(), Vector2(30.f, 5.f), L"hp_bar_monster", SCENE_STAGE);
	m_pHpBar->Set_UI(false);
	m_pHpBar->Set_State(m_tStat.m_fMaxHp->GetValue(), m_tStat.m_fHP);
	//처음엔 안보이게
	m_pHpBar->Set_Active(false);
	CTransform* pTransform = (CTransform*)m_pHpBar->Get_Module(L"Transform");
	if (nullptr == pTransform)
		return E_FAIL;
	pTransform->Set_Parent(m_pTransform);
	pTransform->Set_Position(Vector2(0.f, -15.f));
	CObjMgr::Get_Instance()->Add_GO_To_Layer(L"UI", SCENE_STAGE, m_pHpBar);

	Safe_AddRef(m_pHpBar);
	return S_OK;
}

_int CMonster::Update(_double _timeDelta)
{

	m_pHpBar->Set_State(m_tStat.m_fMaxHp->GetValue(), m_tStat.m_fHP);
	if (m_bDying)
	{
		if (nullptr == m_pDeadClock)
			m_bDead = true;

		if (m_pDeadClock->isThreashHoldReached(3.0))
			m_bDead = true;

		return 0;
	}

	if (!m_bActive)
		return 0;


	m_pTransform->Update_Route(_timeDelta);

	if (!m_bVisuable)
		m_pHpBar->Set_Active(false);

	return 0;
}

_int CMonster::LateUpate(_double _timeDelta)
{


	if (!m_bActive)
		return 0;

	m_pTransform->Update_Transform();

	if (nullptr == m_pRenderer)
		return E_FAIL;

	if (FAILED(m_pRenderer->Add_To_RenderGrop(this, CRenderer::RENDER_YSORT)))
		return E_FAIL;



	return 0;
}

HRESULT CMonster::Render()
{
	if (nullptr == m_pVIBuffer ||
		nullptr == m_pAnimator ||
		nullptr == m_pTransform ||
		nullptr == m_pShader)
		return E_FAIL;

	if (!m_bActive)
		return 0;

	int pass = m_iPass;
	if (SCENE_EDITOR != CSceneMgr::Get_Instance()->Get_CurrScene())
	{
		//보이지 않으면
		if (!m_bVisuable)
		{
			float alpha = 0.f;
			if (FAILED(m_pShader->Set_Value("g_Alpha", &alpha, sizeof(float))))
				return E_FAIL;

			pass = 4;
		}
		//보이면
		else
		{
			pass = m_iPass;
		}

	}

	if (FAILED(m_pVIBuffer->Set_Transform(m_pTransform->Get_Matrix() * m_pPipline->Get_ViewMatrix())))
		return E_FAIL;

	ALPHABLEND;

	if (FAILED(m_pAnimator->Render(m_pShader)))
		return E_FAIL;


	if (FAILED(m_pShader->Begin()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin_Pass(pass)))
		return E_FAIL;

	//반짝거리는 거였으면 원상복귀
	if (m_iPass == 3)
		m_iPass = 0;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

#ifdef MYDEBUG
	//콜라이더 렌더
	m_pTransform->Render_Collider();
#endif // MYDEBUG


	if (FAILED(m_pShader->End_Pass()))
		return E_FAIL;

	if (FAILED(m_pShader->End()))
		return E_FAIL;


	ALPHABLEND_END;

	float alpha = 1.f;
	if (FAILED(m_pShader->Set_Value("g_Alpha", &alpha, sizeof(float))))
		return E_FAIL;


	m_bVisuable = false;

	return S_OK;
}

_bool CMonster::IsAwake()
{
	if (nullptr == m_pFocus)
		return false;

	return IsTargetInRange(m_pFocus, m_iAwakeRange);
}

_float CMonster::CalulateSpeed(_int movePerturn)
{
	if (nullptr == m_pFocus)
		return -1.f;

	CTransform*	pHeroTransform = (CTransform*)m_pFocus->Get_Module(L"Transform");
	if (nullptr == pHeroTransform)
		return -1.f;

	return (_float)pHeroTransform->Get_Desc().speedPerSec;

}

HRESULT CMonster::Throw_Item(CItem * pItem)
{
	if (nullptr == m_pFocus) return E_FAIL;
	CTransform* pTargetTransform = (CTransform*)m_pFocus->Get_Module(L"Transform");
	RETURN_FAIL_IF_NULL(pTargetTransform);
	pItem->Throw(pTargetTransform->Get_Position(), m_pName);

	return S_OK;
}

HRESULT CMonster::OnRender()
{
	return S_OK;
}

void CMonster::OnDead()
{
	CDialogMgr::Get_Instance()->Log_Main(MSG_WIN(m_pName));
}



void CMonster::OnTakeDamage(float _fDamage)
{
	CCharacter::OnTakeDamage(_fDamage);
	m_pHpBar->Set_Active(true);
}

void CMonster::Free()
{
	if(m_pHpBar != nullptr)
		m_pHpBar->Set_Dead();
	Safe_Release(m_pHpBar);
	Safe_Release(m_pAnimator);
	CCharacter::Free();
}
