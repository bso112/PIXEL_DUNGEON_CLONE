#pragma once
#include "Actor.h"
#include "Transform.h"
#include "VIBuffer.h"
#include "Animator.h"
#include "Stat.h"
BEGIN(MyGame)
class CStat;
class CClock_Single;

class CCharacter abstract : public CGameObject
{
protected:
	explicit CCharacter(PDIRECT3DDEVICE9 _pGraphic_Device) :CGameObject(_pGraphic_Device){};
	explicit CCharacter(CCharacter& _character);
	virtual ~CCharacter() = default;

protected:
	typedef struct tagStats
	{
		float	m_fHP		= 0.f;
		CStat*	m_fMaxHp	= nullptr;
		CStat*	m_fArmor	= nullptr;
		CStat*	m_fAtt		= nullptr;
		int		m_iGold		= 0;
		float	m_fExp		= 0.f;
		float	m_fMaxExp	= 0.f;
	}STATS;

	//저항
	enum IMMUNE {IMMUNE_FIRE, IMMUNE_ICE, IMMUNE_END};

protected:
	CTransform*	m_pTransform = nullptr;
	CVIBuffer*	m_pVIBuffer = nullptr;
	//포커스하고 있는 대상
	CCharacter*	m_pFocus = nullptr;

public:
	CCharacter*	Get_Focus() { return m_pFocus; }

protected:
	_int	m_iCurFrame = 0;
	STATS	m_tStat = {};
	vector<IMMUNE> m_vecImmune;
	
	bool	m_bDead = false;
	bool	m_bInvisible = false;
	//이동할 목표지점
	Vector4			m_vDst = {};

	//공격범위
	_uint	m_iAttackRange = 0;

public:
	virtual void TakeDamage(float _fDamage);
	void SetInvisible(bool _bInvisible) { m_bInvisible = _bInvisible; }
	bool IsAlive();
	bool IsImmune(IMMUNE _eImmune);
	
public:
	//어느 지형을 밟고있는지 인덱스를 리턴한다.
	HRESULT	Get_TerrainIndex(pair<_int, _int>& _out);
	//타겟이 이 객체를 중심으로 해당 범위에 있는가?
	bool	IsTargetInRange(CCharacter* pTarget, _int _iRange);
	const STATS&	Get_Stat() { return m_tStat; }

protected:
	virtual void Process() = 0;
	virtual void Update_State() = 0;
	virtual void OnDead() = 0;
	virtual void OnTakeDamage() = 0;
	virtual void Scene_Change() = 0;


public:
	virtual void Free() override;
};

END