#pragma once
#include "Base.h"
#include "Buff.h"
BEGIN(MyGame)
class CBuffController final :public CBase
{
private:
	explicit CBuffController(PDIRECT3DDEVICE9 _pGraphic_Device);
	virtual ~CBuffController() = default;

private:
	PDIRECT3DDEVICE9 m_pGraphic_Device = nullptr;
	list<CBuff*>	m_listBuff;

public:
	HRESULT	Initialize();
	_int	Act(CCharacter* _pTarget);
public:
	HRESULT Add_Buff(CBuff* _pBuff);

public:
	static CBuffController* Create(PDIRECT3DDEVICE9 _pGraphic_Device);

public:
	virtual void Free() override;

};
END