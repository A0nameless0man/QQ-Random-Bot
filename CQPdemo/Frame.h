#pragma once
#include "stdafx.h"
#include"PublicRely.h"

//This part is responsible for authority Check and services as DataBase

const int MAX_BUFFER = 100;
extern int ac;

class Operate;
class Options;
class Member;
class Group;

using UserID = int64_t;
using GroupID = int64_t;
using SuperviserLevel = int64_t;//1 = 用户 2 = 群成员 4 = 群管理 8 = 群主 16 = 超级管理员
using Argus = std::vector<std::string>;
using Function = std::string(*)(const Argus&, GroupID, UserID);
using CommendList = std::map<std::string, Operate>;
using NameList = std::set<Member>;
using Lock = std::mutex;
using Locker = std::lock_guard<Lock>;

std::string noThing(const Argus& arg, GroupID group, UserID user);
std::string voidOperate(const std::string& msg, GroupID group, UserID user);
std::string voidOperate(const Argus& arg, GroupID group, UserID user);

namespace SLevel
{
	const SuperviserLevel none = 0x00;
	const SuperviserLevel user = 0x01;
	const SuperviserLevel member = 0x02;
	const SuperviserLevel manager = 0x04;
	const SuperviserLevel owner = 0x08;
	const SuperviserLevel super = 0x10;
	const SuperviserLevel all = 0x1f;
}

//bool IsAdmin(int64_t user, bool NeedSuperViser);
bool isManager(UserID user);

class Operate
{
public:
	Operate(/*int id, bool na,*/ Function function = noThing, SuperviserLevel superviserLevelNeeded = SLevel::user);
	//int ID;
	std::string excute(const std::string& msg, GroupID group, UserID user);
	//Function getFunction(void);
private:
	//bool needAdmin;
	//bool needSuperviser;//DONE 用supervisorLevel代替这个
	SuperviserLevel superviserLevelNeeded;
	Function function;
};

class Options
{
public:
	void addCommand(const std::string& command, Operate op);
	Operate getOperate(const std::string& command);
	//TODO alies
	CommendList getCommendList(void);
	std::string tryExcuteCommend(const std::string& commend, const std::string& msg, GroupID group, UserID user);
	bool isCommand(const std::string& command);
	void addAdmin(UserID user);
	bool isAdmin(UserID user, GroupID group, bool needSuperviser);
	SuperviserLevel getSuperviserLevel(UserID user, GroupID group);
private:
	std::set<UserID> admins;
	Lock adminLock;
	CommendList operates;
	Lock operateLock;
};

class Member
{
public:
	Member(bit::BigInt = bit::BigInt(bit::DEC, (long long)-1), std::string = "");
	bool operator < (const Member b)const;
	std::string getID(void)const;
	std::string getName(void)const;
	bool getChooseAble(void)const;
	void setChooseAble(bool);
private:
	bit::BigInt studentID;
	std::string name;
	bool canBeChoose;
};

class Group
{
public:
	Group(void);
	~Group(void);
	void addMember(Member member);
	void removeMember(Member member);
	size_t setChooseAble(Member member,bool chooseble);
	size_t setChooseAble(NameList list,bool chooseable);
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
