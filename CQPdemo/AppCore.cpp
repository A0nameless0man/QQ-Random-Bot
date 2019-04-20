#include "stdafx.h"
#include"AppCore.h"
extern int ac;
extern options Options;
std::map<int64_t, group> groups;
const std::string standHelp = "help";
std::string Help(const Argus& arg, int64_t group, int64_t user)
{
	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "help");
	if (arg.size()==1)
	{
		Reply += "欢迎使用随机抽签系统\n";
		Reply += "所有命令前需带'#'\n";
		Reply += "通用格式为：\"#命令 参数\"\n";
		Reply += "此外，您可以使用以下命令：";
		std::map<std::string, operate>::iterator iter;
		for (auto it = Options.Operate.begin();it!=Options.Operate.end();it++)
		{
				Reply += "\""; Reply += iter->first; Reply += "\"	";
				Reply += "ID："; Reply += std::to_string(iter->second.ID); Reply += "\n";
		}
		Reply += "如需具体命令的说明，请将其作为help命令的参数。\n";
		Reply += "或者直接使用\"#命令 "+standHelp+"\"";
	}
	else
	{
		std::map<std::string, operate>::iterator iter;
		for (size_t i =1; i < arg.size(); i++)
		{
			iter = Options.Operate.find(arg[i]);
			Reply+=iter->second.excute(iter->first + " "+standHelp, group, user);
		}
	}
	return Reply;
}
std::string Start(const Argus& arg, int64_t groupid, int64_t user)
{
	
	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "start");
	if (arg.size() < 2||arg[1]==standHelp)
	{
		Reply += "开始抽签\n参数为抽取人数\n默认为1";
	}
	else
	{
		std::map<int64_t, group>::iterator iter_group = groups.find(groupid);
		if (iter_group!=groups.end())
		{
			group& currentGroup = iter_group->second;
			std::vector<member> randlist;
			int num = 0;
			currentGroup.Chosen.clear();
			for (std::set<member>::iterator i = currentGroup.NameList.begin(); i != currentGroup.NameList.end(); i++)
			{
				randlist.push_back(*i);
			}//加载抽签名单
			std::stringstream Op;
			Op << arg[1];
			Op >> num;
			if (num == 0)
			{
				num = 1;
			}//默认为1
			std::random_shuffle(randlist.begin(), randlist.end());
			Reply += "以下人员中签：\n";
			for (size_t i = 0; i < num && i<randlist.size(); i++)
			{
				member &luckyguy = randlist[i];
				if (!luckyguy.Choseable())
				{
					num++;
					continue;
				}
				else
				{
					currentGroup.Chosen.insert(luckyguy);
				}
			}
			Reply += ListLucky(Argus(), groupid, user);
		}
		else
		{
			Reply += "没有成员名单,请创建后重试";
		}

	}
	return Reply;
}
std::string Clear(const Argus & arg, int64_t groupid, int64_t user)
{
	std::string Reply;
	std::stringstream Op;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "CL");
	if (arg.size()==2&&arg[1]==standHelp)
	{
		Reply += "清空抽签池\n参数为群号\n无参数则默认为本群";
	}
	else
	{
		int64_t id = 0;//不能转换时返回0
		if (arg.size()==2)
		{
			Op << arg[1];
			Op >> id;
		}
		if (id == 0)
		{
			id = groupid;
		}
		if (IsAdmin(user,true))
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
			VoidOperate(arg[0], groupid, user);
		}

	}
	return Reply;
}
std::string addmember(const Argus & arg, int64_t groupid, int64_t user)
{
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "addmem");
	std::string Reply;
	std::stringstream Op;
	int incount = 0;
	if (arg.size()<2||arg[1]==standHelp)
	{
		Reply += "添加抽签人员\n可以\"#addmember \'号码\' \' 名字\'\n也可以#addmember range \'起始号码\' \' 终止号码\'\"";
	}
	else
	{
		if (arg.size()==4&&arg[1]=="range")
		{
			bit::BigInt from, to;
			Op << arg[2];
			Op >> from;
			Op << arg[3];
			Op << to;
			if (Op||to<from)
			{
				VoidOperate(arg[0], groupid, user);
			}
			else
			{
				group& currentGroup = groups[groupid];
				for (; from < to; from++)
				{
					currentGroup.NameList.insert(member(from));
					incount++;
				}
			}
		}
		else
		{
			if ((arg.size() - 1)%2==0)
			{
				bit::BigInt i;
				std::string name;
				std::map<int64_t, group>::iterator iter = groups.find(groupid);
				for (size_t j = 1; j < arg.size(); j+=2)
				{
					Op << arg[j];
					Op >> i;
					Op << arg[j + 1];
					Op >> name;
					iter->second.NameList.insert(member(i, name));
					incount++;
				}
			}
		}
		//Reply += "" + std::to_string(incount) + "";//TODO
	}
	return Reply;
}
std::string reffer(const Argus& arg, int64_t groupid, int64_t user)
{
	std::string Reply;
	std::stringstream Op;
	if (arg.size() < 2 || arg[1] == standHelp)
	{
		Reply += "查找幸运儿\n参数为查询目标的ID";
	}
	else
	{
		std::map<int64_t, group>::iterator iter = groups.find(groupid);
		if (iter != groups.end())
		{
			group& currentGroup = iter->second;
			bit::BigInt tar = 0;
			bool IsLucky;
			Op << arg[1];
			Op >> tar;
			if (IsAdmin(user, false))
			{
				IsLucky = (iter->second.Chosen.count(tar) == 1);
			}
			else
			{
				VoidOperate(arg[0], groupid, user);
			}
			Reply += "所查询的用户"; if (IsLucky) Reply += "是"; else Reply += "不是"; Reply += "幸运儿";
		}
		else
		{
			Reply += "";//TODO
		}
	}
	return Reply;
}
std::string ListLucky(const Argus& arg, int64_t groupid, int64_t user)
{

	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "funrun", "List");
	if (arg.size() >1 && arg[1] == standHelp)
	{
		Reply += "查询本群所有中签者\n无需参数";
	}
	else
	{
		std::map<int64_t, group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())
		{
			group& currentGroup = iter_group->second;
			Reply += "以下人员中签：\n";
			for (auto meb:currentGroup.Chosen)
			{
				Reply += "学号：";
				Reply += meb.getID();
				if (meb.getName().length() > 0)
				{
					Reply += "	姓名：";
					Reply += meb.getName();
				}
				Reply += "\n";
			}
		}
		else
		{
			Reply += "没有中签名单,请检查后重试";
		}
	}
	return Reply;
}