#include "QLearning.h"
#include "EnvModel.h"
#include "State.h"
#include "Action.h"
#include "../../Agent.h"


QLearning::QLearning(/*Prey* owner, */float gamma_in, float beta_in, float exploration_in)
{
	gamma = gamma_in;
	beta = beta_in;
	exploration = exploration_in;

	//m_pOwner = owner;
	curIter = 0;
	maxIter = 100000;

	pioQtable = new QLearningDataIO(&QFunctions,&StateList);
}

QLearning::~QLearning(void)
{
	for (int i=0;i<ActionList.size();i++)
	{
		CAction* act = ActionList[i];
		delete act;
		act = NULL;
	}

	for (std::vector<CState*>::iterator iter=StateList.begin();iter!=StateList.end();iter++)
	{
		CState* st = *iter;
		delete st;
		st = NULL;
	}
	delete pioQtable;
	pioQtable = NULL;

	pEnvModel = NULL;
}

void QLearning::setParameters(float newgamma, float newbeta, float newexploration) 
{
	gamma = newgamma;
	beta = newbeta;
	exploration = newexploration;
}

void QLearning::setActionSet( std::vector<CAction*>& ActionSet)
{
	ActionNum = ActionSet.size();

	for (std::vector<CAction*>::iterator it=ActionSet.begin();it!=ActionSet.end();it++)
	{
		ActionList.push_back(*it);

		QFunction* qf = new QFunction();
		QFunctions.insert(std::make_pair(*it,qf));
	}
}

void QLearning::setStateSet( std::vector<CState*>& StateSet)
{
	for (std::vector<CState*>::iterator it=StateSet.begin();it!=StateSet.end();it++)
	{
		StateList.push_back(*it);
		bestActionID.insert(std::make_pair(*it,0));

		for(std::map <CAction*, QFunction*>::iterator iter = QFunctions.begin();iter!=QFunctions.end();iter++)
		{
			QFunction* vf = iter->second;
			vf->qf.insert(std::make_pair(*it,0));
		}
	}
}

CAction* QLearning::chooseAction(Agent* owner,CState* state)
{
	double r = RandFloat(); 

	if (r < exploration)
	{
		int act = getRandomRLAction(ActionNum);
		pEnvModel->pAction = ActionList[act];
	}
	else
	{
		CAction* pbestA = getBestQValueOnState(state);
		if (pbestA->getAction() >= 0)
		{
			pEnvModel->pAction = pbestA;
		}
	}
	return pEnvModel->pAction;
}

CAction* QLearning::getBestQValueOnState(CState* state)
{
	float qValue = -MaxDouble;

	CAction *pAction = NULL;

	for(std::map <CAction*, QFunction*>::iterator it = QFunctions.begin();it!=QFunctions.end();it++)
	{
		QFunction* vf = it->second;

		float temp = vf->getQValue(state);

		if (temp > qValue)
		{
			qValue = temp;
			pAction = it->first;
		}
	}

	return pAction;
}

float QLearning::getQValue(CState* state, CAction* action)
{
	float value = 0.0;

	assert (QFunctions[action]);

	value = QFunctions[action]->getQValue(state);

	return value;
}

void QLearning::setQValue(CState* state,CAction* action, float v)
{
	float value = 0.0;

	assert(QFunctions[action]);

	(QFunctions[action])->setQValue(state, v);

}

void QLearning::updateValue(CState* pState, CAction* pAction, float reward, CState* pNextState, CAction* pNextAction)
{
	//Sarsa
	//float nextValue = getQValue(pNextState, pNextAction);

	//Q
	float nextValue = getQValue(pNextState, getBestQValueOnState(pNextState));

	float curValue = getQValue(pState, pAction);

	float error = reward + pow(gamma,pAction->getDuration()) * nextValue - curValue;

	setQValue(pState, pAction, curValue + beta * error);

 	//beta = beta*(1-episode/(float)maxIter);
 	//exploration = max(exploration*(1-episode/(float)maxIter),0.01);
}


CState* QLearning::updateEnvModelForRL(Agent* owner)
{
	CState* ps = NULL;
	ps = pEnvModel->UpdateCurState();
	CState* pret = NULL;
	pret = findStateinList(ps);
	if (NULL == pret)
	{
		pret = pEnvModel->MemyCopyState(ps);
		StateList.push_back(pret);
	}
	pEnvModel->pState = pret;

	return pEnvModel->pState;
}

CState* QLearning::findStateinList(CState* state)
{
	if (StateList.empty())
	{
		return NULL;
	}
	else
	{
		for(std::vector <CState*>::iterator it = StateList.begin();it!=StateList.end();it++)
		{
			if ((*it)->IsSameState(state))
			{
				return *it;
			}
		}
		return NULL;
	}
}

int   QLearning::getActionIndex(CAction* action)
{
	int index = -1;
	for(std::vector <CAction*>::iterator it = ActionList.begin();it!=ActionList.end();it++)
	{
		index++;
		if ((*it)->isSameAction(action))
		{
			break;
		}
	}
	return index;
}

int QLearning::selectActionByRL(Agent* owner)
{
	pEnvModel->pAction = chooseAction(owner,pEnvModel->pState);
	pEnvModel->bindActionData(pEnvModel->pAction,&pEnvModel->localActionData);

	pEnvModel->pAction->setDuration(0);
	pEnvModel->pAction->setisFinished(false);

	pEnvModel->rewardFeedback = 0;

	// here must make sure the accordance of action index in actionlist and BT  
	int ret = getActionIndex(pEnvModel->pAction);

	int s = pEnvModel->pState->PrintStateValues()[3];
	return ret;
}

void QLearning::AccuRewardsOption(Agent* owner, float& rdsignal)
{
	//ִ����һ�Σ���������ʱ������
	pEnvModel->bindActionData(pEnvModel->pAction,&pEnvModel->localActionData);
	pEnvModel->bindActionData(pEnvModel->preAction,&pEnvModel->preActionData);

	pEnvModel->preAction->setDuration(pEnvModel->preAction->getDuration()+1);

	updateEnvModelForRL(owner);

	//ִ�ж�����õ��ۼƽ������˴����в��
	//����Option�ķ���ϸ��ÿһ���ѧϰ���Ľ���ֵ�����ִ��һ��CalculateRewardsEachStep��������������
	//��HSMQ�������ϲ㽱�����²��ۻ���ã����ִ��һ��CalculateRewardsEachStep��������¼����صĽ���

	//����ֵ���԰���������Σ�
	//һ�Ƕ�������Ľ��������ռ䣬���ÿһ����α����ֵ
	//����ÿһ�����¼�ִ�еĽڵ㴦���صĽ���ֵ
	//�����ԭ�Ӷ��������¼�ִ�нڵ㷵�صĽ���ֵΪ0��������õ�α����ֵ����Ϊ�������������ص�reward siginal����¼��tree��output��ͬʹ��
	//�����ԭ�Ӷ�������ȡ��tree��output�еײ�ԭ�Ӷ�������ִ�еĽ����������ۻ�

	float pseudoreward = 0;
	pseudoreward = pEnvModel->CalculateRewardsEachStep();

	float reward = pseudoreward + rdsignal;
	pEnvModel->rewardFeedback = pEnvModel->rewardFeedback + reward* pow(gamma,pEnvModel->preAction->getDuration()-1);
}

void QLearning::RecordMemory(Agent* owner)
{
	pEnvModel->preAction = pEnvModel->pAction;
	pEnvModel->preActionData = pEnvModel->localActionData;
	pEnvModel->bindActionData(pEnvModel->preAction,&pEnvModel->preActionData);
	pEnvModel->preState = pEnvModel->pState;
}

void QLearning::KnowledgeLearn(Agent* owner)
{
	// update Q table
	pEnvModel->preAction->setisFinished(true);

	int ret = pEnvModel->preState->PrintStateValues()[3];
	int act = pEnvModel->preAction->getAction();
	updateValue(pEnvModel->preState, pEnvModel->preAction, pEnvModel->rewardFeedback, pEnvModel->pState, pEnvModel->pAction);

	//for debug and control experiment
	curIter ++;
	//for(std::vector <CState*>::iterator it = StateList.begin();it!=StateList.end();it++)
	//{
	//	bestActionID[*it] = getBestQValueOnState(*it)->getAction();
	//}
}

void QLearning::saveQTableToFile( char* filename)
{
	pioQtable->SaveData(filename);
}

void QLearning::loadQTableFromFile( char* filename)
{
	pioQtable->LoadData(filename);
}
