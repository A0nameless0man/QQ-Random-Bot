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
		Reply += "��ӭʹ�������ǩϵͳ\n";
		Reply += "��������ǰ���'#'\n";
		Reply += "ͨ�ø�ʽΪ��\"#���� ����\"\n";
		Reply += "���⣬������ʹ���������";
		std::map<std::string, operate>::iterator iter;
		for (auto it = Options.Operate.begin();it!=Options.Operate.end();it++)
		{
				Reply += "\""; Reply += iter->first; Reply += "\"	";
				Reply += "ID��"; Reply += std::to_string(iter->second.ID); Reply += "\n";
		}
		Reply += "������������˵�����뽫����Ϊhelp����Ĳ�����\n";
		Reply += "����ֱ��ʹ��\"#���� "+standHelp+"\"";
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
		Reply += "��ʼ��ǩ\n����Ϊ��ȡ����\nĬ��Ϊ1";
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
			}//���س�ǩ����
			std::stringstream Op;
			Op << arg[1];
			Op >> num;
			if (num == 0)
			{
				num = 1;
			}//Ĭ��Ϊ1
			std::random_shuffle(randlist.begin(), randlist.end());
			Reply += "������Ա��ǩ��\n";
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
			Reply += "û�г�Ա����,�봴��������";
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
		Reply += "��ճ�ǩ��\n����ΪȺ��\n�޲�����Ĭ��Ϊ��Ⱥ";
	}
	else
	{
		int64_t id = 0;//����ת��ʱ����0
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
		Reply += "��ӳ�ǩ��Ա\n����\"#addmember \'����\' \' ����\'\nҲ����#addmember range \'��ʼ����\' \' ��ֹ����\'\"";
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
		Reply += "�������˶�\n����Ϊ��ѯĿ���ID";
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
			Reply += "����ѯ���û�"; if (IsLucky) Reply += "��"; else Reply += "����"; Reply += "���˶�";
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
		Reply += "��ѯ��Ⱥ������ǩ��\n�������";
	}
	else
	{
		std::map<int64_t, group>::iterator iter_group = groups.find(groupid);
		if (iter_group != groups.end())
		{
			group& currentGroup = iter_group->second;
			Reply += "������Ա��ǩ��\n";
			for (auto meb:currentGroup.Chosen)
			{
				Reply += "ѧ�ţ�";
				Reply += meb.getID();
				if (meb.getName().length() > 0)
				{
					Reply += "	������";
					Reply += meb.getName();
				}
				Reply += "\n";
			}
		}
		else
		{
			Reply += "û����ǩ����,���������";
		}
	}
	return Reply;
}