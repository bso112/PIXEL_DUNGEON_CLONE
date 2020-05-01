#include "stdafx.h"
#include "..\Headers\CollisionMgr.h"
#include "GameObject.h"
#include "Transform.h"

USING(MyGame)


CCollisionMgr::CCollisionMgr()
{
}

bool CCollisionMgr::Collision_Rect(list<CGameObject*> _Dst, list<CGameObject*> _Src)
{

	RECT rc = {};
	
	for (auto& Dst : _Dst)
	{	
		CTransform* pDstTransform = (CTransform*)Dst->Get_Module(L"Transform");
		
		if (nullptr == pDstTransform)
			return false;

		for (auto& Src : _Src)
		{	

			CTransform* pSrcTransform = (CTransform*)Src->Get_Module(L"Transform");
			
			if (nullptr == pSrcTransform)
				return false;


			if (IntersectRect(&rc, &pDstTransform->Get_Rect(), &pSrcTransform->Get_Rect()))
			{
				//������ �������, �浹ó���� ���� �ʴ´�.
				if (Src->Get_Dead())
					Dst->Erase_Collided(Src);
				if (Dst->Get_Dead())
					Src->Erase_Collided(Dst);
				if (Src->Get_Dead() || Dst->Get_Dead())
					continue;


				//���� ���� �浹�� ���� ������ ������ �浹����Ʈ�� ���θ� �߰��Ѵ�.
				if (!Dst->Contain_Collided(Src))
				{
					Dst->Add_Collided(Src);
					Dst->Set_isCollided(true);
					Dst->OnCollisionEnter(Src);
				}
				if (!Src->Contain_Collided(Dst))
				{
					Src->Add_Collided(Dst);
					Src->Set_isCollided(true);
					Src->OnCollisionEnter(Dst);
				}


				Dst->OnCollisionStay(Src);
				Src->OnCollisionStay(Dst);

			}
			else
			{
				//���� ������ ���� �浹�� ����Ʈ�� ������ �����Ѵ�.
				Dst->Erase_Collided(Src);
				Src->Erase_Collided(Dst);

				//�浹����Ʈ�� �����, ���� �浹������ ������ collisionExit�� �����Ѵ�.
				if (Dst->Get_CollidedSize() == 0 && Dst->Get_isCollided())
				{
					Dst->Set_isCollided(false);
					Dst->OnCollisionExit(Src);
				}
				if (Src->Get_CollidedSize() == 0 && Src->Get_isCollided())
				{
					Src->Set_isCollided(false);
					Src->OnCollisionExit(Dst);
				}
			}
		}
	}
	return false;
}

bool CCollisionMgr::Collision(list<CGameObject*> _Dst, list<CGameObject*> _Src, bool _bisCollided)
{
	RECT rc = {};

	for (auto& Dst : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (_bisCollided)
			{
				//������ �������, �浹ó���� ���� �ʴ´�.
				if (Src->Get_Dead())
					Dst->Erase_Collided(Src);
				if (Dst->Get_Dead())
					Src->Erase_Collided(Dst);
				if (Src->Get_Dead() || Dst->Get_Dead())
					continue;


				//���� ���� �浹�� ���� ������ ������ �浹����Ʈ�� ���θ� �߰��Ѵ�.
				if (!Dst->Contain_Collided(Src))
				{
					Dst->Add_Collided(Src);
					Dst->Set_isCollided(true);
					Dst->OnCollisionEnter(Src);
				}
				if (!Src->Contain_Collided(Dst))
				{
					Src->Add_Collided(Dst);
					Src->Set_isCollided(true);
					Src->OnCollisionEnter(Dst);
				}


				Dst->OnCollisionStay(Src);
				Src->OnCollisionStay(Dst);

			}
			else
			{
				//���� ������ ���� �浹�� ����Ʈ�� ������ �����Ѵ�.
				Dst->Erase_Collided(Src);
				Src->Erase_Collided(Dst);

				//�浹����Ʈ�� �����, ���� �浹������ ������ collisionExit�� �����Ѵ�.
				if (Dst->Get_CollidedSize() == 0 && Dst->Get_isCollided())
				{
					Dst->Set_isCollided(false);
					Dst->OnCollisionExit(Src);
				}
				if (Src->Get_CollidedSize() == 0 && Src->Get_isCollided())
				{
					Src->Set_isCollided(false);
					Src->OnCollisionExit(Dst);
				}
			}
		}
	}
	return false;
}

//bool CCollisionMgr::Intersect_LIne(const LINE _line1, const LINE _line2, Vector2& _result)
//{
//
//	Vector2 p0 = _line1.src;
//	Vector2 p1 = _line1.dst;
//	Vector2 p2 = _line2.src;
//	Vector2 p3 = _line2.dst;
//
//
//	float a1 = p1.fY - p0.fY;
//	float b1 = p0.fX - p1.fX;
//	float c1 = a1 * p0.fX + b1 * p0.fY;
//	float a2 = p3.fY - p2.fY;
//	float b2 = p2.fX - p3.fX;
//	float c2 = a2 * p2.fX + b2 * p2.fY;
//	float denominator = a1 * b2 - a2 * b1;
//
//	float intersectX = (b2 * c1 - b1 * c2) / denominator;
//	float intersectY = (a1 * c2 - a2 * c1) / denominator;
//
//	float distX0 = (intersectX - p0.fX) / (p1.fX - p0.fX);
//	float distY0 = (intersectY - p0.fY) / (p1.fY - p0.fY);
//	float distX1 = (intersectX - p2.fX) / (p3.fX - p2.fX);
//	float distY1 = (intersectY - p2.fY) / (p3.fY - p2.fY);
//
//	if (((distX0 >= 0 && distX0 <= 1) || (distY0 >= 0 && distY0 <= 1))
//		&& ((distX1 >= 0 && distX1 <= 1) || (distY1 >= 0 && distY1 <= 1)))
//	{
//		_result.fX = intersectX;
//		_result.fY = intersectY;
//		return true;
//	}
//
//	return false;
//}


void CCollisionMgr::Free()
{
}