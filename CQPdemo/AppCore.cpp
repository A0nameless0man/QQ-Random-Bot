#include "stdafx.h"
#include"AppCore.h"
#include"PublicRely.h"

extern int ac;
extern Options options;

std::map<GroupID, Group> groups;
const std::string standHelp = "help";

void loadCommand(void)
{
	options.addCommand("start", Operate(/*true,*/ start,SLevel::super|SLevel::owner|SLevel::manager));
	options.addCommand("clear", Operate(/*true,*/ clear,SLevel::super));
	options.addCommand("addMember", Operate(/*true, */addmember, SLevel::super | SLevel::owner | SLevel::manager));
	options.addCommand("removeMember", Operate(/*true, */noThing, SLevel::super | SLevel::owner | SLevel::manager));
	options.addCommand("reffer", Operate(/*false, */reffer, SLevel::super | SLevel::owner | SLevel::manager|SLevel::member));
	options.addCommand("showLuckyList", Operate(/*false,*/ listLucky, SLevel::super | SLevel::owner | SLevel::manager));
	options.addCommand("ListLucky", options.getOperate("showLuckyList"));
	options.addCommand("help", Operate(/*false,*/ help,SLevel::all));
	return;
}

void loadAdmin(void)
{
	options.addAdmin(1395943920);
	return;
}

void loadGroups(void)
{
	;
}

void saveAdmin(void)
{
	;
}

void saveGroups(void)
{
	;
}

std::string help(const Argus& arg, GroupID group, UserID user)
{
	std::string reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "help");
	if (arg.size() == 1)
	{
		CQ_addLog(ac, CQLOG_DEBUG, "funrun", "commen help");
		reply += "欢迎使用随机抽签系统\n";
		reply += "所有命令前需带'#'\n";
		reply += "通用格式为：\"#命令 参数\"\n";
		reply += "此外，您可以使用以下命令:\n";
		CommendList tempList = options.getCommendList();
		for (auto it = tempList.begin(); it != tempList.end(); it++)
		{
			reply += "\""; reply += it->first; reply += "\"	";
			//reply += "ID："; reply += std::to_string(it->second.ID); reply += "\n";
		}
		reply += "如需具体命令的说明，请将其作为help命令的参数。\n";
		reply += "或者直接使用\"#命令 " + standHelp + "\"";
	}
	else
	{
		CQ_addLog(ac, CQLOG_DEBUG, "funrun", "commend help");
		std::map<std::string, Operate>::iterator iter;
		for (size_t i = 1; i < arg.size(); i++)
		{
			reply += options.tryExcuteCommend(arg[i], arg[i] + standHelp, group, user);
		}
	}
	return reply;
}

std::string start(const Argus& arg, GroupID groupid, UserID user)
{

	std::string reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "start");
	if (arg.size() < 2 || arg[1] == standHelp)
	{
		reply += "开始抽签\n参数为抽取人数\n默认为1";
	}
	else
	{
		std::map<GroupID, Group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())//检查是否存在群聊的记录
		{
			srand((unsigned int)time(NULL));//设置随机种子
			Group& currentGroup = iter_group->second;//建立引用方便调用.
			std::vector<Member> randlist;//预计的受害者名单
			int num = 0;
			//currentGroup.chosen.clear();
			for (auto u: currentGroup.getNameList(true))
			{
				randlist.push_back(u);
			}
			if (arg.size() == 2)
			{
				std::stringstream Op;
				Op << arg[1];
				Op >> num;
				Op.clear();
			}
			if (num == 0)
			{
				num = 1;
			}//默认为1
			std::random_shuffle(randlist.begin(), randlist.end());
			reply += "计划抽取人数:" + std::to_string(num) + "\n班级人数:" + std::to_string(currentGroup.size()) + "\n实际可抽取人数:" + std::to_string(randlist.size()) + "\n";
			NameList result;
			for (int i = 0; i < num && i < (int)randlist.size(); i++)
			{
				Member& luckyguy = randlist[i];
				result.insert(luckyguy);
			}
			currentGroup.setChosen(result);
			reply += listLucky(Argus(), groupid, user);
		}
		else
		{
			reply += "没有成员名单,请创建后重试";
		}

	}
	return reply;
}

std::string clear(const Argus& arg, GroupID groupid, UserID user)
{
	std::string Reply;
	std::stringstream Op;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "CL");
	if (arg.size() == 2 && arg[1] == standHelp)
	{
		Reply += "清空抽签池\n参数为群号\n无参数则默认为本群";
	}
	else
	{
		GroupID id = 0;//不能转换时返回0
		if (arg.size() == 2)
		{
			Op << arg[1];
			Op >> id;
			Op.clear();
		}
		if (id == 0)
		{
			id = groupid;
		}
		if (options.getSuperviserLevel(user,groupid)&SLevel::super)//Double check
		{
			try
			{
				groups.erase(id);
				Reply += "";//TODO
			}
			catch (const std::exception&)
			{
				Reply += "";//TODO
			}
		}
		else
		{
			voidOperate(arg[0], groupid, user);
		}

	}
	return Reply;
}

std::string addmember(const Argus& arg, GroupID groupid, UserID user)
{
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "addmem");
	std::string Reply;
	std::stringstream Op;
	int incount = 0;
	if (arg.size() < 2 || arg[1] == standHelp)
	{
		Reply += "添加抽签人员\n可以\"#addmember \'号码\' \' 名字\'\n也可以#addmember range \'起始号码\' \' 终止号码\'\"";
	}
	else
	{
		if (groups.find(groupid) == groups.end())
		{
			//Group temp();
			//groups.insert(std::pair<GroupID, Group>(groupid, Group()));
			groups[groupid];
		}
		Group& currentGroup = groups[groupid];
		if (arg.size() == 4 && arg[1] == "range")
		{
			CQ_addLog(ac, CQLOG_DEBUG, "funrun", "addmem by range");
			bit::BigInt from, to;
			Op << arg[2];
			Op >> from;
			Op << arg[3];
			Op << to;
			if (Op || to < from)
			{
				voidOperate(arg[0], groupid, user);
			}
			else
			{
				for (; from < to; from++)
				{
					currentGroup.addMember(Member(from));
					incount++;
				}
			}
		}
		else
		{
			CQ_addLog(ac, CQLOG_DEBUG, "funrun", "addmem by data");
			if ((arg.size() - 1) % 2 == 0)
			{
				bit::BigInt i;
				std::string name;

				for (size_t j = 1; j < arg.size(); j += 2)
				{
					Op << arg[j];
					Op >> i;
					Op.clear();
					name = arg[j + 1];
					//reply += "学号：";
					//reply += i.string();
					//if (name.length() > 0)
					//{
					//	reply += "	姓名：";
					//	reply += name;
					//}
					//reply += "\n";
					currentGroup.addMember(Member(i, name));
					incount++;
				}
			}
		}
		Reply += "已添加:" + std::to_string(incount) + "人\n" + "班级现有:" + std::to_string(currentGroup.size()) + "人\n";
	}
	return Reply;
}

std::string reffer(const Argus& arg, GroupID groupid, UserID user)
{
	std::string reply;
	std::stringstream Op;
	if (arg.size() < 2 || arg[1] == standHelp)
	{
		reply += "查找幸运儿\n参数为查询目标的ID";
	}
	else
	{
		std::map<int64_t, Group>::iterator iter = groups.find(groupid);
		if (iter != groups.end())
		{
			Group& currentGroup = iter->second;
			bit::BigInt tar = 0;
			bool IsLucky = false;
			Op << arg[1];
			Op >> tar;
			if (options.isAdmin(user,groupid ,false))
			{
				reply += "所查询的用户"; if (currentGroup.isLucky(tar)) reply += "是"; else reply += "不是"; reply += "幸运儿";
			}
			else
			{
				reply += voidOperate(arg[0], groupid, user);
			}
		}
		else
		{
			reply += "";//TODO
		}
	}
	return reply;
}

std::string listLucky(const Argus & arg, GroupID groupid, UserID user)
{

	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "List");
	if (arg.size() > 1 && arg[1] == standHelp)
	{
		Reply += "查询本群所有中签者\n无需参数";
	}
	else
	{
		std::map<int64_t, Group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())
		{
			Group& currentGroup = iter_group->second;
			Reply += "以下人员中签：\n";
			int count = 0;
			for (auto meb : currentGroup.getChosen())
			{
				count++;
				Reply += "学号：";
				Reply += meb.getID();
				if (meb.getName().length() > 0)
				{
					Reply += "	姓名：";
					Reply += meb.getName();
				}
				Reply += "\n";
			}
			Reply += "共" + std::to_string(count) + "人中签";
		}
		else
		{
			Reply += "没有中签名单,请检查后重试";
		}
	}
	return Reply;
}

std::string removeMember(const Argus& arg, GroupID groupid, UserID user)
{
	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "removeMember");
	if (arg.size() > 2)
	{
		std::stringstream Op;
		bit::BigInt j;
		std::map<int64_t, Group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())
		{
			Group& currentGroup = iter_group->second;
			for (size_t i = 2; i < arg.size(); i++)
			{
				Op << arg[i];
				Op >> j;
				currentGroup.removeMember(Member(j));
			}
			Reply += "";
		}
		else
		{
			Reply += "";
		}
	}
	else
	{
		
	}
}

std::string setChooseable(const Argus& arg, GroupID groupid, UserID user)
{
	return std::string();
}
