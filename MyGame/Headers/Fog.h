#pragma once
#include "GameObject.h"
BEGIN(MyGame)
class CTransform;
class CVIBuffer;
class CTexture;
class CHero;
class CShader;
//�Ȱ��� �������� �ʴ� ������ ���� ������ ������ ����.
class CFog : public CGameObject
{
protected:
	explicit CFog(PDIRECT3DDEVICE9 _pGraphic_Device);
	explicit CFog(CFog& _rhs);
	virtual ~CFog() = default;

private:
	CTransform*	m_pTransform = nullptr;
	CVIBuffer*	m_pVIBuffer = nullptr;
	CTexture*	m_pTexture = nullptr;
	CShader*	m_pShader = nullptr;
private:
	CTransform*	m_pHeroTransform = nullptr;
public:
	HRESULT	Initiailzie();
	_int	Upate();
	_int	LateUpdate();
	HRESULT	Render();

public:
	static	CFog* Create(PDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject * Clone(void * _pArg = nullptr) override;
	void	Free();
};
END