#include "stdafx.h"
#include "IceFlower.h"
#include "ObjMgr.h"
#include "Effect.h"
#include "Terrain.h"
USING(MyGame)
CIceFlower::CIceFlower(PDIRECT3DDEVICE9 _pGrahic_Device)
	:CSeed(_pGrahic_Device)
{
}

CIceFlower::CIceFlower(CIceFlower & _rhs)
	: CSeed(_rhs)
{
}

HRESULT CIceFlower::Initialize(void * _param)
{
	CSeed::Initialize(_param);

	Set_Module(L"Transform", SCENE_STATIC, (CModule**)&m_pTransform, L"Transform", &CTransform::STATEDESC(300.0, 300.0));
	Set_Module(L"VIBuffer", SCENE_STATIC, (CModule**)&m_pVIBuffer);

	if (nullptr != _param)
		m_pTransform->Set_Position((*(Vector3*)_param));

	m_vOriginalSize = Vector2(30.f, 25.f);
	m_pTransform->Set_Size(m_vOriginalSize);
	m_pTransform->Set_ColliderSize(m_vOriginalSize);


	m_pDescription = L"�������� �����̴�. ������ ����� ��������.";
	m_iTextureID = 7;
	m_pItemName = L"�������� ����";

	return S_OK;
}


void CIceFlower::OnCollisionEnter(CGameObject * _pOther)
{
	//������ �ε�ģ�� �ƴϰ�, �� ���¶��
	if (nullptr == dynamic_cast<CTerrain*>(_pOther) && m_bFlower)
	{
		CObjMgr* pObjMgr = CObjMgr::Get_Instance();

		if (m_pTransform == nullptr)
			return;

		CEffect*  pEffect = nullptr;
		BASEDESC desc = BASEDESC(m_pTransform->Get_Position(), m_pTransform->Get_Size());
		//����Ʈ ����
		pEffect = dynamic_cast<CEffect*>(pObjMgr->Add_GO_To_Layer(L"Effect_Fire", SCENE_STAGE, L"Effect", SCENE_STAGE, &desc));

		//����Ʈ ���
		if (pEffect)
			pEffect->Play();

		//����
		m_bDead = true;

	}
}

void CIceFlower::Free()
{
	CSeed::Free();
}
