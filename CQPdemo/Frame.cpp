#include "stdafx.h"
#include"Frame.h"
struct options Options;
member::member(bit::BigInt id, std::string n):StID(id),Name(n)
{
	CanBeChose = true;
}
bool member::operator < (const member b)const
{
	return StID < b.StID;
}

std::string member::getID(void) const
{
	return StID.string();
}

std::string member::getName(void)const
{
	return Name;
}

bool member::Choseable(void)const
{
	return CanBeChose;
}


group::group(void) { ; }
group::~group(void) { ; }


operate::operate(int id, bool na, Function function) :ID(id), NeedAdmin(na), NeedSuperviser(false), function(function)
{ 
	; 
}

std::string operate::excute(const std::string& msg, GroupID group, UserID user)
{
	CQ_addLog(ac, CQLOG_DEBUG, "Frame", "excute");
	if (function!=NULL&&(!NeedAdmin || IsAdmin(user, NeedSuperviser)))
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
			Reply+= function(argus, group, user);
		}
		catch (const std::exception& e)
		{
			Reply += e.what();
		}
		return Reply;
	}
	else
	{
		return VoidOperate(msg, group, user);
	}

}


bool IsManager(UserID user)
{
	return false;
}

bool IsAdmin(UserID user, bool NeedSuperViser)
{
	return (Options.Admins.count(user) == 1) || !NeedSuperViser && IsManager(user);
}

std::string NoThing(const Argus& arg, GroupID group, UserID user)
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

std::string VoidOperate(const std::string& msg, GroupID group, UserID user)
{
	std::string Reply;
	CQ_addLog(ac, CQLOG_DEBUG, "voidCOMMEND", ("user=" + std::to_string(user) + ";InGroup:" + std::to_string(group) + "withCOMMEND:" + msg).c_str());
	Reply += "\"";
	Reply += msg;
	Reply += "\"\n";
	Reply += "是无效命令或者您的权限不足，请使用help命令查询可用命令。";
	return Reply;
}
