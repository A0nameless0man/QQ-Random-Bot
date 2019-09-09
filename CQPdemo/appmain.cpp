/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/
#include "stdafx.h"
#include"appmain.h"
#include"AppCore.h"
extern Options options;
int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;
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
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	loadCommand();
	loadAdmin();
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
	saveGroups();
	saveAdmin();
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
		std::string	reply;
		std::string operate = msg + 1;
		std::string commend;
		std::stringstream Op;
		Op << operate;
		Op >> commend;//从信息分离出指令
		CQ_addLog(ac, CQLOG_DEBUG, "msg", commend.c_str());
		if (options.isCommand(commend))
		{
			reply += options.tryExcuteCommend(commend, operate, fromGroup, fromQQ);
		}
		else
		{
			reply += voidOperate(commend, fromGroup, fromQQ);
		}
		//CQ_sendPrivateMsg(ac, fromQQ, reply.c_str());
		CQ_sendGroupMsg(ac, fromGroup, reply.c_str());
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
		string operates = msg + 1;
		string commend;
		stringstream Op;
		Op << operates;
		Op >> commend;
		getline(Op, operates);
		if (options.operates.count(commend) == 1)
		{
			if (groups.count(fromGroup) != 0)
			{
				map<string, Operate>::iterator iter;
				for (iter = options.operates.begin(); iter != options.operates.end(); iter++)
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
				groups.insert(pair<int64_t, Group>(fromGroup, Group()));
				reply += "已为您开通抽签容器，请重试。";
			}
			CQ_sendPrivateMsg(ac, fromQQ, reply.c_str());
			CQ_sendDiscussMsg(ac, fromGroup, reply.c_str());
		}
		else
		{
			voidOperate(msg, fromGroup, fromQQ);
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
