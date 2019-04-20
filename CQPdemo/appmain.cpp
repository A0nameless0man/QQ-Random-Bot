/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/
#include "stdafx.h"
#include"appmain.h"
#include"AppCore.h"
extern options Options;
int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;
/*
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}
/*
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}
/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}
/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}
/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	int i = 0;
	Options.Operate.insert(std::pair<std::string, operate>("start", operate(i++, true, Start)));
	Options.Operate.insert(std::pair<std::string, operate>("clear", operate(i++, true, Clear)));
	Options.Operate.insert(std::pair<std::string, operate>("addmember", operate(i++, true, addmember)));
	Options.Operate.insert(std::pair<std::string, operate>("removemember", operate(i++, true, NoThing)));
	Options.Operate.insert(std::pair<std::string, operate>("reffer", operate(i++, false,reffer)));
	Options.Operate.insert(std::pair<std::string, operate>("shownamelist", operate(i++, false)));
	Options.Operate.insert(std::pair<std::string, operate>("help", operate(i++, false, Help)));
	Options.Admins.insert(1395943920);
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {

	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_IGNORE;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (*msg == '#')
	{
		std::string	reply;
		std::string Operate = msg + 1;
		std::string commend;
		std::stringstream Op;
		Op << Operate;
		Op >> commend;//����Ϣ�����ָ��
		std::map<std::string, operate>::iterator iter;
		iter = Options.Operate.find(commend);
		if (iter != Options.Operate.end())
		{
			reply += iter->second.excute(std::string(msg), fromGroup, fromQQ);
		}
		else
		{
			reply += VoidOperate(commend, fromGroup, fromQQ);
		}
		CQ_sendPrivateMsg(ac, fromQQ, reply.c_str());
		CQ_sendGroupMsg(ac, fromGroup, reply.c_str());
		return EVENT_BLOCK;
	}
	else
	{
		return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
	}
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	/*int64_t fromGroup = fromDiscuss;
	if (*msg == '#')
	{
		string	reply;
		string Operate = msg + 1;
		string commend;
		stringstream Op;
		Op << Operate;
		Op >> commend;
		getline(Op, Operate);
		if (Options.Operate.count(commend) == 1)
		{
			if (groups.count(fromGroup) != 0)
			{
				map<string, operate>::iterator iter;
				for (iter = Options.Operate.begin(); iter != Options.Operate.end(); iter++)
				{
					CQ_addLog(ac, CQLOG_DEBUG, "breakpoint", iter->first.c_str());
					if (iter->first == commend)
					{
						if (iter->second.fun != NULL)
						{
							CQ_addLog(ac, CQLOG_DEBUG, "usefun", iter->first.c_str());
							(*(iter->second.fun))(msg, fromGroup, fromQQ);
						}
						CQ_addLog(ac, CQLOG_DEBUG, "NULLfun", iter->first.c_str());

						break;
					}
					else
					{
						CQ_addLog(ac, CQLOG_DEBUG, "notusefun", iter->first.c_str());
					}
				}
			}
			else
			{
				groups.insert(pair<int64_t, group>(fromGroup, group()));
				reply += "��Ϊ����ͨ��ǩ�����������ԡ�";
			}
			CQ_sendPrivateMsg(ac, fromQQ, reply.c_str());
			CQ_sendDiscussMsg(ac, fromGroup, reply.c_str());
		}
		else
		{
			VoidOperate(msg, fromGroup, fromQQ);
		}
		return EVENT_BLOCK;
	}
	else*/
	{
		return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
	} //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "", 0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "", 0);
	return 0;
}
