#pragma once
#include "Action.h"

class Prey;


/*  zhangqi 2017/02/24
	���ܣ���������������ѧϰ�Ļ���ģ�ͻ��࣬��Ϊѧϰ�㷨ʹ�õĹ����ӿڣ�
	��ֻӦ��������ʵ�塢״̬��ָ�룬������ָ�룬�Լ����������ӿ�
*/

class EnvModelLearning
{
public:
	EnvModelLearning(Prey* pA)
	{
		m_pOwner = pA;
		pState = NULL;
		preState = NULL;
		pAction = NULL;
		preAction = NULL;
		rewardFeedback = 0;
	}
	~EnvModelLearning(void){}
protected:
	Prey* m_pOwner;

public:
	//state
	CState* pState;
	CState* preState;
	//action
	CAction* pAction;
	CAction* preAction;
	//action data
	CActionData localActionData;
	CActionData preActionData;
	//reward
	float rewardFeedback;

public:
	virtual float CalculateRewardsEachStep(){return 0;}
	virtual CState* UpdateCurState(){return NULL;}
	virtual void SetCurAction(CAction* pA){pAction = pA;}
	virtual CState* MemyCopyState(CState* temp){return NULL;}
	virtual void bindActionData(CAction* pA, CActionData* pAD){pA->setActionData(pAD);}
};
