#include "stdafx.h"
#include"AppCore.h"
extern int ac;
extern Options options;

std::map<GroupID, Group> groups;
const std::string standHelp = "help";

void loadCommand(void)
{
	options.addCommand("start", Operate(true, start));
	options.addCommand("clear", Operate(true, clear));
	options.addCommand("addMember", Operate(true, addmember));
	options.addCommand("removeMember", Operate(true, noThing));
	options.addCommand("reffer", Operate(false, reffer));
	options.addCommand("showLuckyList", Operate(false, listLucky));
	options.addCommand("ListLucky", Operate(false, listLucky));
	options.addCommand("help", Operate(false, help));
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
		reply += "��ӭʹ�������ǩϵͳ\n";
		reply += "��������ǰ���'#'\n";
		reply += "ͨ�ø�ʽΪ��\"#���� ����\"\n";
		reply += "���⣬������ʹ���������";
		CommendList tempList = options.getCommendList();
		for (auto it = tempList.begin(); it != tempList.end(); it++)
		{
			reply += "\""; reply += it->first; reply += "\"	";
			//reply += "ID��"; reply += std::to_string(it->second.ID); reply += "\n";
		}
		reply += "������������˵�����뽫����Ϊhelp����Ĳ�����\n";
		reply += "����ֱ��ʹ��\"#���� " + standHelp + "\"";
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
		reply += "��ʼ��ǩ\n����Ϊ��ȡ����\nĬ��Ϊ1";
	}
	else
	{
		std::map<GroupID, Group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())
		{
			srand((unsigned int)time(NULL));
			Group& currentGroup = iter_group->second;
			std::vector<Member> randlist;
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
			}//Ĭ��Ϊ1
			std::random_shuffle(randlist.begin(), randlist.end());
			reply += "�ƻ���ȡ����:" + std::to_string(num) + "\n�༶����:" + std::to_string(currentGroup.size()) + "\nʵ�ʿɳ�ȡ����:" + std::to_string(randlist.size()) + "\n";
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
			reply += "û�г�Ա����,�봴��������";
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
		Reply += "��ճ�ǩ��\n����ΪȺ��\n�޲�����Ĭ��Ϊ��Ⱥ";
	}
	else
	{
		GroupID id = 0;//����ת��ʱ����0
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
		if (options.isAdmin(user,groupid, true))
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
		Reply += "��ӳ�ǩ��Ա\n����\"#addmember \'����\' \' ����\'\nҲ����#addmember range \'��ʼ����\' \' ��ֹ����\'\"";
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
					//reply += "ѧ�ţ�";
					//reply += i.string();
					//if (name.length() > 0)
					//{
					//	reply += "	������";
					//	reply += name;
					//}
					//reply += "\n";
					currentGroup.addMember(Member(i, name));
					incount++;
				}
			}
		}
		Reply += "�����:" + std::to_string(incount) + "��\n" + "�༶����:" + std::to_string(currentGroup.size()) + "��\n";
	}
	return Reply;
}

std::string reffer(const Argus& arg, GroupID groupid, UserID user)
{
	std::string reply;
	std::stringstream Op;
	if (arg.size() < 2 || arg[1] == standHelp)
	{
		reply += "�������˶�\n����Ϊ��ѯĿ���ID";
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
				reply += "����ѯ���û�"; if (currentGroup.isLucky(tar)) reply += "��"; else reply += "����"; reply += "���˶�";
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
		Reply += "��ѯ��Ⱥ������ǩ��\n�������";
	}
	else
	{
		std::map<int64_t, Group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())
		{
			Group& currentGroup = iter_group->second;
			Reply += "������Ա��ǩ��\n";
			int count = 0;
			for (auto meb : currentGroup.getChosen())
			{
				count++;
				Reply += "ѧ�ţ�";
				Reply += meb.getID();
				if (meb.getName().length() > 0)
				{
					Reply += "	������";
					Reply += meb.getName();
				}
				Reply += "\n";
			}
			Reply += "��" + std::to_string(count) + "����ǩ";
		}
		else
		{
			Reply += "û����ǩ����,���������";
		}
	}
	return Reply;
}