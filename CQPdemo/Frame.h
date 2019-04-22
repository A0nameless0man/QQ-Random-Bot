#pragma once
#include "stdafx.h"
#include"PublicRely.h"
const int MAX_BUFFER = 100;
extern int ac;

class Operate;
class Options;
class Member;
class Group;

using UserID = int64_t;
using GroupID = int64_t;
using Argus = std::vector<std::string>;
using Function = std::string(*)(const Argus&, GroupID, UserID);
using CommendList = std::map<std::string, Operate>;
using NameList = std::set<Member>;
using Lock = std::mutex;
using Locker = std::lock_guard<Lock>;

std::string noThing(const Argus& arg, GroupID group, UserID user);
std::string voidOperate(const std::string& msg, GroupID group, UserID user);

//bool IsAdmin(int64_t user, bool NeedSuperViser);
bool isManager(UserID user);

class Operate
{
public:
	Operate(/*int id, */bool na, Function function = noThing);
	//int ID;
	std::string excute(const std::string& msg, GroupID group, UserID user);
private:
	bool needAdmin;
	bool needSuperviser;
	Function function;
};

class Options
{
public:
	void addCommand(const std::string& command, Operate op);
	//TODO alies
	CommendList getCommendList(void);
	std::string tryExcuteCommend(const std::string& commend, const std::string& msg, GroupID group, UserID user);
	bool isCommand(const std::string& command);
	void addAdmin(UserID user);
	bool isAdmin(UserID user, GroupID group,bool needSuperviser);
private:
	std::set<UserID> admins;
	Lock adminLock;
	CommendList operates;
	Lock operateLock;
};

class Member
{
public:
	Member(bit::BigInt = bit::BigInt(bit::DEC,(long long)-1), std::string = "");
	bool operator < (const Member b)const;
	std::string getID(void)const;
	std::string getName(void)const;
	bool choseable(void)const;
private:
	bit::BigInt studentID;
	std::string name;
	bool canBeChose;
};

class Group
{
public:
	Group(void);
	~Group(void);
	void addMember(Member member);
	void removeMember(Member member);
	NameList getNameList(bool needChoseable);
	size_t size(void);
	bool isLucky(bit::BigInt stuID);
	NameList getChosen(void);
	void setChosen(const NameList& chosed);
private:
	NameList  chosen;
	NameList nameList;
	Lock thisLock;

};
