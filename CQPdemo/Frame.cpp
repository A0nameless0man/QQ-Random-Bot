#include "stdafx.h"
#include"Frame.h"
Operate notOperate(/*false,*/ voidOperate, SLevel::all);
Options options;

bool isManager(UserID user)
{
	return false;
}

//bool IsAdmin(UserID user, bool NeedSuperViser)
//{
//	return (options.admins.count(user) == 1) || !NeedSuperViser && isManager(user);
//}


std::string noThing(const Argus& arg, GroupID group, UserID user)
{
	std::string Reply;
	if (!arg.empty())
	{
		CQ_addLog(ac, CQLOG_DEBUG, "unFinishedCOMMEND", ("user:" + std::to_string(user) + ";InGroup:" + std::to_string(group) + "withCOMMEND:" + arg[0]).c_str());
		Reply += "\"";
		Reply += arg[0];
		Reply += "\"\n";
		Reply += "这个功能还未支持，敬请期待。";
	}
	else
	{
		CQ_addLog(ac, CQLOG_DEBUG, "unFinishedCOMMEND", ("user:" + std::to_string(user) + ";InGroup:" + std::to_string(group) + "withCOMMEND:" + "not a command").c_str());
	}
	return Reply;
}

std::string voidOperate(const std::string& msg, GroupID group, UserID user)
{
	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "voidCOMMEND", ("user=" + std::to_string(user) + ";InGroup:" + std::to_string(group) + "withCOMMEND:" + msg).c_str());
	Reply += "\"";
	Reply += msg;
	Reply += "\"\n";
	Reply += "是无效命令或者您的权限不足，请使用help命令查询可用命令。";
	return Reply;
}

std::string voidOperate(const Argus& arg, GroupID group, UserID user)
{
	return voidOperate(arg.size() ? arg[0] : "", group, user);
}


Operate::Operate(/*int id, bool na,*/ Function function, SuperviserLevel superviserLevelNeeded) :
	/*ID(id), needAdmin(na), needSuperviser(false), */function(function), superviserLevelNeeded(superviserLevelNeeded)
{
	;
}

std::string Operate::excute(const std::string& msg, GroupID group, UserID user)
{
	CQ_addLog(ac, CQLOG_DEBUG, "Frame", "excute");
	if (function != NULL && (superviserLevelNeeded & options.getSuperviserLevel(user, group)))
	{
		std::string arg;
		std::stringstream ss;
		Argus argus;
		ss << msg;
		while (ss >> arg)
		{
			argus.push_back(arg);
		}
		std::string Reply;
		try
		{
			Reply += function(argus, group, user);
		}
		catch (const std::exception& e)
		{
			Reply += e.what();
		}
		return Reply;
	}
	else
	{
		return voidOperate(msg, group, user);
	}

}

//Function Operate::getFunction(void)
//{
//	return function;
//}//abanded



void Options::addCommand(const std::string& command, Operate op)
{
	Locker lock(operateLock);
	operates.insert(std::pair<std::string, Operate>(command, op));
	return;
}

Operate Options::getOperate(const std::string& command)
{
	if (isCommand(command))
	{
		Locker lock(operateLock);
		return operates[command];
	}
	else
	{
		return notOperate;
	}
}

CommendList Options::getCommendList(void)
{
	//operateLock.lock();
	Locker lock(operateLock);
	return operates;
}

std::string Options::tryExcuteCommend(const std::string& commend, const std::string& msg, GroupID group, UserID user)
{
	if (isCommand(commend))
	{
		return operates.find(commend)->second.excute(msg, group, user);
	}
	else
	{
		return voidOperate(commend, group, user);

	}
}

bool Options::isCommand(const std::string& command)
{
	Locker lock(operateLock);
	return (operates.find(command) != operates.end());
}

void Options::addAdmin(UserID user)
{
	Locker lock(adminLock);
	admins.insert(user);
	return;
}

bool Options::isAdmin(UserID user, GroupID group, bool needSuperviser)
{
	Locker lock(adminLock);
	return  (admins.find(user) != admins.end()) || !needSuperviser && isManager(user);
}

SuperviserLevel Options::getSuperviserLevel(UserID user, GroupID group)
{
	SuperviserLevel level = SLevel::none;
	{
		level |= SLevel::member;
	}
	{
		Locker lock(adminLock);
		level |= (admins.find(user) != admins.end()) ? SLevel::super : SLevel::none;
	}
	return level;
}


Member::Member(bit::BigInt id, std::string n) :studentID(id), name(n)
{
	canBeChoose = true;
}

bool Member::operator < (const Member b)const
{
	return studentID < b.studentID;
}

std::string Member::getID(void) const
{
	return studentID.string();
}

std::string Member::getName(void)const
{
	return name;
}

bool Member::getChooseAble(void)const
{
	return canBeChoose;
}

void Member::setChooseAble(bool Op)
{
	canBeChoose = Op;
}


Group::Group(void) { nameList.clear(); chosen.clear(); }

Group::~Group(void) { ; }

void Group::addMember(Member member)
{
	Locker lock(thisLock);
	nameList.insert(member);
}

void Group::removeMember(Member member)
{
	Locker lock(thisLock);
	nameList.erase(member);
}

size_t Group::setChooseAble(Member member, bool chooseable)
{
	
}

size_t Group::setChooseAble(NameList list, bool chooseable)
{
	return size_t();
}

NameList Group::getNameList(bool needChoseable)
{
	Locker lock(thisLock);
	NameList tempList;
	for (auto u : nameList)
	{
		if (!needChoseable || u.getChooseAble())
		{
			tempList.insert(u);
		}
	}
	return tempList;
}

size_t Group::size(void)
{
	return nameList.size();
}

bool Group::isLucky(bit::BigInt stuID)
{
	if (chosen.find(Member(stuID)) != chosen.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

NameList Group::getChosen(void)
{
	//Locker lock(thisLock);
	return chosen;
}

void Group::setChosen(const NameList& chosed)
{
	chosen = chosed;
}







