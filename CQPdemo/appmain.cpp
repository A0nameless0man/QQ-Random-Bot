/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include<set>
#include<map>
//#include<vector>
#include<random>
#include<string.h>
#include<iostream>
#include<sstream>
using namespace std;
const int MAX_BUFFER = 100;
int ac = -1; //AuthCode 调用酷Q的方法时需要用到
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
	replay += "这个功能还未支持，敬请期待。";
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
	reply += "是无效命令或者您的权限不足，请使用help命令查询可用命令。";
	CQ_sendPrivateMsg(ac, user, reply.c_str());
	CQ_sendGroupMsg(ac, group, reply.c_str());
	return;
}
void Help(const char* msg, int64_t group, int64_t user)
{
	string Reply;
	string Msg = msg;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "help");
	Reply += "欢迎使用随机抽签系统\n";
	Reply += "所有命令前需带'#'\n";
	Reply += "通用格式为：\"#命令 参数\"\n";
	Reply += "此外，您可以使用以下命令：";
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
			Reply += "ID："; Reply += to_string(iter->second.ID); Reply += "\n";
		}
	}
	Reply += "如需具体命令的说明，请将其作为help命令的参数。\n";
	Reply += "或者直接使用\"#命令 help\"";
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
		Reply += "开始抽签\n参数为抽取人数\n默认为1";
	}
	else
	{
		Reply += "以下人员中签：\n";
		map<int64_t, group>::iterator iter = groups.find(groupid);
		int size = iter->second.NameList.size();
		vector<member> randlist;
		for (set<member>::iterator i = iter->second.NameList.begin(); i != iter->second.NameList.end(); i++)
		{
			randlist.push_back(*i);
		}//加载抽签名单
		int mun = 0;
		stringstream Op;
		Op << Msg;
		Op >> Msg;
		Op >> mun;
		if (mun == 0)
		{
			mun = 1;
		}//默认为1
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
				Reply += "学号：";
				Reply += to_string(luckyguy.StID);
				if (luckyguy.name.length() > 0)
				{
					Reply += "	姓名：";
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
		Reply += "清空抽签池\n参数为群号\n无参数则默认为本群";
	}
	else
	{
		int id = 0;//不能转换时返回0
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
		Reply += "添加抽签人员\n可以\"#addmember \'号码\' \' 名字\'添加单个名字\n也可以#addmember range \'起始号码\' \' 终止号码\'\"";
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
		Reply += "查找幸运儿，参数为查询目标，没有默认查自己";
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
		Reply += "所查询的用户"; if (IsLucky) Reply += "是"; else Reply += "不是"; Reply+="幸运儿";
	}
	CQ_sendPrivateMsg(ac, user, Reply.c_str());
	CQ_sendGroupMsg(ac, groupid, Reply.c_str());
}
/*
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}
/*
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}
/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}
/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}
/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
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
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {

	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_IGNORE;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (*msg == '#')
	{
		string	reply;
		string Operate = msg + 1;
		string commend;
		stringstream Op;
		Op << Operate;
		Op >> commend;//从信息分离出指令
		getline(Op, Operate);
		if (Options.operate.count(commend) == 1)//是注册过的指令吗？
		{
			if (groups.count(fromGroup) != 0)//如果有这个群的记录则进行处理//没有则创建记录
			{
				map<string, operate>::iterator iter;
				for (iter = Options.operate.begin(); iter != Options.operate.end(); iter++)//遍历指令，寻找匹配//瞎搞，等会改用find
				{
					CQ_addLog(ac, CQLOG_DEBUG, "breakpoint", iter->first.c_str());
					if (iter->first == commend)
					{
						if (!iter->second.NeedAdmin||IsAdmin(fromQQ,false))//是否具有基本的权限
						{
							if (iter->second.fun != NULL)//函数是否已经正确注册
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
							VoidOperate(msg, fromGroup, fromQQ);//如果权限不足则拒绝操作
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
				reply += "已为您开通抽签容器，请重试。";
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
		return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
	}
}


/*
* Type=4 讨论组消息
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
				reply += "已为您开通抽签容器，请重试。";
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
		return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
	} //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "", 0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "", 0);
	return 0;
}
