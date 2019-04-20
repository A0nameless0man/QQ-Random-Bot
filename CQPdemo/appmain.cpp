/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include<set>
#include<map>
//#include<vector>
#include<random>
#include<string.h>
#include<iostream>
#include<sstream>
using namespace std;
const int MAX_BUFFER = 100;
int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;
class member
{
public:
	member();
	member(int);
	member(int, string);
	~member();
	bool operator>(const member b)
	{
		return this->StID > b.StID;
	}
	bool operator<(const member b)
	{
		return this->StID < b.StID;
	}
	unsigned long long int StID;
	string name;
	bool CanBeChose;
private:

};
static bool operator < (const member a, const member b)
{
	return a.StID < b.StID;
}
member::member()
{
	StID = -1;
	CanBeChose = false;
}
member::member(int id)
{
	StID = id;
	CanBeChose = true;
}
member::member(int id, string n)
{
	StID = id;
	name = n;
	CanBeChose = true;
}
member::~member()
{
}
class group
{
public:
	std::set<member>  Chosen;
	set<member> NameList;
	group(void)
	{

	}
	~group(void)
	{

	}
};
map<int64_t, group> groups;
void NoThing(const char* msg, int64_t group, int64_t user);
class operate
{
public:
	operate(int id, bool na, void(*f)(const char*, int64_t, int64_t))
	{
		ID = id;
		NeedAdmin = na;
		fun = f;
	}
	operate(int id, bool na)
	{
		operate(id, na, NoThing);
	}
	~operate()
	{

	}
	int ID;
	bool NeedAdmin;
	void(*fun)(const char*, int64_t, int64_t);
};
struct options
{
	set<unsigned long long int> Admins;
	map<string, operate> operate;
}Options;
bool IsManager(int64_t user)
{
	return false;
}
bool IsAdmin(int64_t user,bool NeedSuperViser)
{
	return (Options.Admins.count(user) == 1) || !NeedSuperViser&&IsManager(user);
}
void NoThing(const char* msg, int64_t group, int64_t user)
{
	string replay;
	CQ_addLog(ac, CQLOG_DEBUG, "unFinishedCOMMEND", ("user=" + to_string(user) + ";InGroup:" + to_string(group) + "withCOMMEND" + msg).c_str());
	replay += "\"";
	replay += msg;
	replay += "\"\n";
	replay += "������ܻ�δ֧�֣������ڴ���";
	CQ_sendGroupMsg(ac, group, replay.c_str());
	CQ_sendPrivateMsg(ac, user, replay.c_str());
	return;
}
void VoidOperate(const char* msg, int64_t group, int64_t user)
{
	string reply;
	CQ_addLog(ac, CQLOG_DEBUG, "voidCOMMEND",("user="+to_string(user)+";InGroup:"+to_string(group)+"withCOMMEND"+msg).c_str() );
	reply += "\"";
	reply += msg;
	reply += "\"\n";
	reply += "����Ч�����������Ȩ�޲��㣬��ʹ��help�����ѯ�������";
	CQ_sendPrivateMsg(ac, user, reply.c_str());
	CQ_sendGroupMsg(ac, group, reply.c_str());
	return;
}
void Help(const char* msg, int64_t group, int64_t user)
{
	string Reply;
	string Msg = msg;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "help");
	Reply += "��ӭʹ�������ǩϵͳ\n";
	Reply += "��������ǰ���'#'\n";
	Reply += "ͨ�ø�ʽΪ��\"#���� ����\"\n";
	Reply += "���⣬������ʹ���������";
	map<string, operate>::iterator iter;
	for (iter = Options.operate.begin(); iter != Options.operate.end(); iter++)
	{
		if (Msg.find(iter->first) != string::npos&&iter->second.fun != Help)
		{
			(*(iter->second.fun))("help", group, user);
		}
		else
		{
			Reply += "\""; Reply += iter->first; Reply += "\"	";
			Reply += "ID��"; Reply += to_string(iter->second.ID); Reply += "\n";
		}
	}
	Reply += "������������˵�����뽫����Ϊhelp����Ĳ�����\n";
	Reply += "����ֱ��ʹ��\"#���� help\"";
	CQ_sendPrivateMsg(ac, user, Reply.c_str());
	CQ_sendGroupMsg(ac, group, Reply.c_str());
}
void Start(const char* msg, int64_t groupid, int64_t user)
{
	string Msg = msg;
	string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "start");
	if (Msg.find("help") != string::npos)
	{
		Reply += "��ʼ��ǩ\n����Ϊ��ȡ����\nĬ��Ϊ1";
	}
	else
	{
		Reply += "������Ա��ǩ��\n";
		map<int64_t, group>::iterator iter = groups.find(groupid);
		int size = iter->second.NameList.size();
		vector<member> randlist;
		for (set<member>::iterator i = iter->second.NameList.begin(); i != iter->second.NameList.end(); i++)
		{
			randlist.push_back(*i);
		}//���س�ǩ����
		int mun = 0;
		stringstream Op;
		Op << Msg;
		Op >> Msg;
		Op >> mun;
		if (mun == 0)
		{
			mun = 1;
		}//Ĭ��Ϊ1
		for (size_t i = 0; i < mun&&size>0; i++)
		{
			member luckyguy;
			int luckint;
			luckyguy = randlist[luckint = rand() % size];
			if (!luckyguy.CanBeChose)
			{
				i--;
				continue;
			}
			else
			{
				iter->second.Chosen.insert(luckyguy);
				Reply += "ѧ�ţ�";
				Reply += to_string(luckyguy.StID);
				if (luckyguy.name.length() > 0)
				{
					Reply += "	������";
					Reply += luckyguy.name;
				}
				Reply += "\n";
				randlist.erase(randlist.begin() + luckint);
				size--;
			}
		}
	}
	CQ_sendPrivateMsg(ac, user, Reply.c_str());
	CQ_sendGroupMsg(ac, groupid, Reply.c_str());
}
void Clear(const char*msg, int64_t groupid, int64_t user)
{
	string Msg = msg;
	string Reply;
	stringstream Op;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "CL");
	Op << Msg;
	Op >> Msg;
	if (Msg.find("help") != string::npos)
	{
		Reply += "��ճ�ǩ��\n����ΪȺ��\n�޲�����Ĭ��Ϊ��Ⱥ";
	}
	else
	{
		int id = 0;//����ת��ʱ����0
		Op >> id;
		if (id == 0)
		{
			if (Options.Admins.count(user) > 0)
			{
				groups.erase(groupid);
			}
			else
			{
				VoidOperate(msg, groupid, user);
			}
		}
		else
		{
			if (Options.Admins.count(user) > 0)
			{
				groups.erase(id);
			}
			else
			{
				VoidOperate(msg, groupid, user);
			}
		}
	}
	CQ_sendPrivateMsg(ac, user, Reply.c_str());
	CQ_sendGroupMsg(ac, groupid, Reply.c_str());
}
void addmember(const char*msg, int64_t groupid, int64_t user)
{
	string Msg = msg;
	string Reply;
	stringstream Op;
	int incount = 0;
	Op << Msg;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "addmem");
	if (Msg.find("help") != string::npos)
	{
		Reply += "��ӳ�ǩ��Ա\n����\"#addmember \'����\' \' ����\'��ӵ�������\nҲ����#addmember range \'��ʼ����\' \' ��ֹ����\'\"";
		CQ_sendPrivateMsg(ac, user, Reply.c_str());
		CQ_sendGroupMsg(ac, groupid, Reply.c_str());
		return;
	}
	else
	{
		if (Msg.find("range") != string::npos)
		{
			int from = 0, to = 0;
			Op >> Msg >> Msg >> from >> to;
			if (from == 0 || to == 0 || from == to)
			{
				VoidOperate(msg, groupid, user);
			}
			else
			{
				map<int64_t, group>::iterator iter = groups.find(groupid);
				for (; from <= to; from++)
				{
					iter->second.NameList.insert(member(from));
					incount++;
				}
			}
		}
		else
		{
			int i;
			string name;
			Op >> Msg ;
			while (Op>> i >> name)
			{
				if (i == 0 || name.length() == 0)
				{
					VoidOperate(msg, groupid, user);
					break;
				}
				else
				{
					map<int64_t, group>::iterator iter = groups.find(groupid);
					iter->second.NameList.insert(member(i, name));
					incount++;
				}
			}
		}
	}
	CQ_addLog(ac, CQLOG_DEBUG, "addmember", to_string(incount).c_str());
}
void reffer(const char*msg, int64_t groupid, int64_t user)
{
	string Msg = msg;
	string Reply;
	stringstream Op;
	Op << Msg;
	if (Msg.find("help")!=string::npos||groups.count(groupid)==0)
	{
		Reply += "�������˶�������Ϊ��ѯĿ�꣬û��Ĭ�ϲ��Լ�";
	}
	else
	{
		map<int64_t, group>::iterator iter = groups.find(groupid);
		int tar = 0;
		bool IsLucky;
		Op >> Msg >> tar;
		if (tar==0)
		{
			IsLucky = (iter->second.Chosen.count(user) == 1);
		}
		else
		{
			if (IsAdmin(user,false))
			{
				IsLucky= (iter->second.Chosen.count(tar) == 1);
			}
			else
			{
				VoidOperate(msg, groupid, user);
			}
		}
		Reply += "����ѯ���û�"; if (IsLucky) Reply += "��"; else Reply += "����"; Reply+="���˶�";
	}
	CQ_sendPrivateMsg(ac, user, Reply.c_str());
	CQ_sendGroupMsg(ac, groupid, Reply.c_str());
}
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
	Options.operate.insert(pair<string, operate>("start", operate(i++, true, Start)));
	Options.operate.insert(pair<string, operate>("clear", operate(i++, true, Clear)));
	Options.operate.insert(pair<string, operate>("addmember", operate(i++, true, addmember)));
	Options.operate.insert(pair<string, operate>("removemember", operate(i++, true, NoThing)));
	Options.operate.insert(pair<string, operate>("reffer", operate(i++, false,reffer)));
	Options.operate.insert(pair<string, operate>("shownamelist", operate(i++, false)));
	Options.operate.insert(pair<string, operate>("help", operate(i++, false, Help)));
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
		string	reply;
		string Operate = msg + 1;
		string commend;
		stringstream Op;
		Op << Operate;
		Op >> commend;//����Ϣ�����ָ��
		getline(Op, Operate);
		if (Options.operate.count(commend) == 1)//��ע�����ָ����
		{
			if (groups.count(fromGroup) != 0)//��������Ⱥ�ļ�¼����д���//û���򴴽���¼
			{
				map<string, operate>::iterator iter;
				for (iter = Options.operate.begin(); iter != Options.operate.end(); iter++)//����ָ�Ѱ��ƥ��//Ϲ�㣬�Ȼ����find
				{
					CQ_addLog(ac, CQLOG_DEBUG, "breakpoint", iter->first.c_str());
					if (iter->first == commend)
					{
						if (!iter->second.NeedAdmin||IsAdmin(fromQQ,false))//�Ƿ���л�����Ȩ��
						{
							if (iter->second.fun != NULL)//�����Ƿ��Ѿ���ȷע��
							{
								CQ_addLog(ac, CQLOG_DEBUG, "usefun", iter->first.c_str());
								(*(iter->second.fun))(msg, fromGroup, fromQQ);
							}
							else
							{
								CQ_addLog(ac, CQLOG_DEBUG, "NULLfun", iter->first.c_str());
							}
							
						}
						else
						{
							VoidOperate(msg, fromGroup, fromQQ);//���Ȩ�޲�����ܾ�����
						}
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
			CQ_sendGroupMsg(ac, fromGroup, reply.c_str());
		}
		else
		{
			VoidOperate(msg, fromGroup, fromQQ);
		}
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
		if (Options.operate.count(commend) == 1)
		{
			if (groups.count(fromGroup) != 0)
			{
				map<string, operate>::iterator iter;
				for (iter = Options.operate.begin(); iter != Options.operate.end(); iter++)
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
