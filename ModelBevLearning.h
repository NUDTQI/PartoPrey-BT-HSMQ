#pragma once
#include "Prey.h"
#include "AI/rl/EnvModel.h"
#include "AI/rl/Action.h"
#include "AI/rl/CPrimitiveAction.h"
#include "AI/rl/State.h"
#include "StateBevLearning.h"

//���ļ���Ҫ�û�Ϊ������Ҫѧϰ����Ϊ���ڵ㶨���佱������ģ�ͣ���Ӧ����Ƶ�״̬��
//���ڿ���ͳһ�Ľ���ѡ��������Ϊ�ӽڵ����ţ���˶���ģ������ʵ����


//����1��ʾretreat�ڵ㣻����2��ʾattack������3��ʾidle
class ModelSelRootBevLearning
	: public EnvModelLearning
{
public:

	ModelSelRootBevLearning(Prey* pA)
		: EnvModelLearning(pA){}

	~ModelSelRootBevLearning(void){}

	StateRootBevSel temp;

public:

	virtual CState* UpdateCurState()
	{
		temp.UpdateDiscreteState(m_pOwner);
		return &temp;
	}

	virtual void SetCurAction(CAction* pA)
	{
		pAction->setAction(pA->getAction());
	}

	virtual CState* MemyCopyState(CState* temp)
	{
		StateRootBevSel* ps = new StateRootBevSel();
		ps->CopyState(temp);
		return ps;
	}


	//��������Ҫ�����һ�����������״̬�����ռ��α����ֵ
	virtual float CalculateRewardsEachStep()
	{
		StateRootBevSel* pS = dynamic_cast<StateRootBevSel*>(pState);
		StateRootBevSel* preS = dynamic_cast<StateRootBevSel*>(preState);

		////////////////����ΪRoot�ڵ�ռ��ڶ���ĵ���α����ֵ����//////////////////////////////////////////////////////////
		float pseudoReward = 0;
		if (NonHl==pS->GetHealthlevel())
		{
			pseudoReward = pseudoReward - 10;
		}
		if (Inside==pS->GetDisToHaven())
		{
			pseudoReward = pseudoReward + 15;
		}

		//û�е��ˣ�û�з���havenѡ���˳ͷ�
		if (None==pS->GetDisToHaven() && None==pS->GetDisToEnemy() && preAction->getAction()==0)
		{
			pseudoReward = pseudoReward - 0.5;
		}

		//û�е��ˣ�ѡ������ͷ�
		if (None==pS->GetDisToEnemy() && preAction->getAction()==2)
		{
			pseudoReward = pseudoReward - 0.5;
		}

		//����˲�Զ��ѡ�����γͷ�
		if (Middle>pS->GetDisToEnemy() && preAction->getAction()==1)
		{
			pseudoReward = pseudoReward - 0.5;
		}

		//Ѫ�����ӣ�����
		if (pS->GetHealthlevel() > preS->GetHealthlevel())
		{
			pseudoReward = pseudoReward + 2;
		}

		if (LowHl == pS->GetHealthlevel())
		{
			if (Near==pS->GetDisToEnemy() && preAction->getAction()==0)
			{
				pseudoReward = pseudoReward + 0.5;
			}
		}

		//���Ѷ�Ѫ���߹�������
		if(HighHl==pS->GetHealthlevel() && LowAlly<pS->GetNumAllyNeighbor() && None>pS->GetDisToEnemy() && preAction->getAction()==2)
		{
			pseudoReward = pseudoReward + 0.5;
		}		
		return pseudoReward;
	}
};

class ModelSelRetreatBevLearning
	: public EnvModelLearning
{
public:

	ModelSelRetreatBevLearning(Prey* pA)
		: EnvModelLearning(pA){}

	~ModelSelRetreatBevLearning(void){}

	StateRetreatBevSel temp;

public:

	virtual CState* UpdateCurState()
	{
		temp.UpdateDiscreteState(m_pOwner);
		return &temp;
	}

	virtual void SetCurAction(CAction* pA)
	{
		pAction->setAction(pA->getAction());
	}

	virtual CState* MemyCopyState(CState* temp)
	{
		StateRetreatBevSel* ps = new StateRetreatBevSel();
		ps->CopyState(temp);
		return ps;
	}

	virtual float CalculateRewardsEachStep()
	{
		StateRetreatBevSel* pS = dynamic_cast<StateRetreatBevSel*>(pState);

		////////////////����Ϊretreat�ڵ�ռ��ڶ���ĵ���α����ֵ����//////////////////////////////////////////////////////////
		float pseudoReward = 0;
		//���ڳ�����Ϊ�����������������������͵��ﰲȫ��������
		if (NonHl == pS->GetHealth())
		{
			pseudoReward = pseudoReward - 10;
		}

		if (Inside == pS->GetDisToHaven())
		{
			pseudoReward = pseudoReward + 15;
		}

		//û��Haven��ѡ�񵽴�Haven�ͷ�
		if (None==pS->GetDisToHaven() && preAction->getAction()==1)
		{
			pseudoReward = pseudoReward - 0.5;
		}
		return pseudoReward;
	}
};

class ModelSelAttackBevLearning
	: public EnvModelLearning
{
public:

	ModelSelAttackBevLearning(Prey* pA)
		: EnvModelLearning(pA){}

	~ModelSelAttackBevLearning(void){}

	StateAttackBevSel temp;

public:

	virtual CState* UpdateCurState()
	{
		temp.UpdateDiscreteState(m_pOwner);
		return &temp;
	}

	virtual void SetCurAction(CAction* pA)
	{
		pAction->setAction(pA->getAction());
	}

	virtual CState* MemyCopyState(CState* temp)
	{
		StateAttackBevSel* ps = new StateAttackBevSel();
		ps->CopyState(temp);
		return ps;
	}

	virtual float CalculateRewardsEachStep()
	{
		float pseudoReward = 0; 
		StateAttackBevSel* pS = dynamic_cast<StateAttackBevSel*>(pState);

		////////////////����Ϊattack�ڵ�ռ��ڶ���ĵ���α����ֵ����//////////////////////////////////////////////////////////
		//float pseudoReward = 0;
		//���ڽ�����Ϊ�����������������������͵��ﰲȫ��������

		if (NonHl == pS->GetHealthlevel())
		{
			pseudoReward = pseudoReward - 10;
		}

		if (m_pOwner->GetNearestPredator() && m_pOwner->GetNearestPredator()->isDead())
		{
			pseudoReward = pseudoReward + 2;
		}

		return pseudoReward;
	}
};


//general learning 
class ModelSelGeneralBevLearning
	: public EnvModelLearning
{
public:

	ModelSelGeneralBevLearning(Prey* pA)
		: EnvModelLearning(pA){}

	~ModelSelGeneralBevLearning(void){}

	StateGeneralBevSel temp;

public:

	virtual CState* UpdateCurState()
	{
		temp.UpdateDiscreteState(m_pOwner);
		return &temp;
	}

	virtual void SetCurAction(CAction* pA)
	{
		pAction->setAction(pA->getAction());
	}

	virtual CState* MemyCopyState(CState* temp)
	{
		StateGeneralBevSel* ps = new StateGeneralBevSel();
		ps->CopyState(temp);
		return ps;
	}

	virtual float CalculateRewardsEachStep()
	{
		StateGeneralBevSel* pS = dynamic_cast<StateGeneralBevSel*>(pState);
		StateGeneralBevSel* preS = dynamic_cast<StateGeneralBevSel*>(preState);

		////////////////����ΪRoot�ڵ�ռ��ڶ���ĵ���α����ֵ����//////////////////////////////////////////////////////////
		float reward = 0.0f;
		if (HealthLevel::NonHl == pS->GetHealthlevel())
		{
			reward = reward - 200;
		}

		if (DisLevel::Inside == pS->GetDisToHaven())
		{
			reward = reward + 200;
		}

		if (m_pOwner->m_CurEnemy && m_pOwner->m_CurEnemy->isDead())
		{
			reward = reward + 10;
		}
		
		//if (preS->GetHealthlevel() < pS->GetHealthlevel())
		//{
		//	reward = reward + 5;
		//}

		//if (preS->GetHealthlevel() > pS->GetHealthlevel())
		//{
		//	reward = reward - 5;
		//}

		//û��Haven��ѡ�񵽴�Haven�ͷ�
		//if (DisLevel::None==pS->GetDisToHaven() && m_pOwner->curPrimaryAct=="SeekSafety")
		//{
		//	reward = reward - 1;
		//}

		////û�е��ˣ�ѡ������ͷ�
		//if (DisLevel::None==pS->GetDisToEnemy() && (m_pOwner->curPrimaryAct=="Charge"||m_pOwner->curPrimaryAct=="Assist"))
		//{
		//	reward = reward - 1;
		//}

		////û��Food��ѡ��Forge�ͷ�
		//if (DisLevel::None==pS->GetDisToFood() && m_pOwner->curPrimaryAct=="Forage")
		//{
		//	reward = reward - 1;
		//}
		//
		//if (HealthLevel::LowHl >= pS->GetHealthlevel() && DisLevel::Inside==pS->GetDisToFood() && m_pOwner->curPrimaryAct=="Eat")
		//{
		//	reward = reward + 1;
		//}

		/*if (HealthLevel::LowHl == pS->GetHealthlevel())
		{
			if (DisLevel::Near==pS->GetDisToEnemy() && 
				DisLevel::Near==pS->GetDisToHaven() && m_pOwner->curPrimaryAct=="SeekSafety")
			{
				reward = reward + 15;
			}

			if (DisLevel::Inside==pS->GetDisToFood() && m_pOwner->curPrimaryAct=="Eat")
			{
				reward = reward + 10;
			}

			if (DisLevel::Inside<pS->GetDisToFood() && m_pOwner->curPrimaryAct=="Forage")
			{
				reward = reward + 5;
			}

			if (DisLevel::Near==pS->GetDisToEnemy() && DisLevel::None==pS->GetDisToHaven() && m_pOwner->curPrimaryAct=="Flee")
			{
				reward = reward + 1;
			}
		}

		if (HealthLevel::HighHl==pS->GetHealthlevel() && NumAllyLevel::HighAlly==pS->GetNumAllyNeighbor() && m_pOwner->curPrimaryAct=="Charge" )
		{
			reward = reward + 0.6;
		}

		if (HealthLevel::LowHl<pS->GetHealthlevel() && NumAllyLevel::MidAlly==pS->GetNumAllyNeighbor() && m_pOwner->curPrimaryAct=="Assist")
		{
			reward = reward + 0.5;
		}*/
		m_pOwner->m_dReward = m_pOwner->m_dReward + reward;
		return reward;
	}
};