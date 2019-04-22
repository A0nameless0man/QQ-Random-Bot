#pragma once
#include "stdafx.h"
#include"PublicRely.h"
const int MAX_BUFFER = 100;
extern int ac;

using UserID = int64_t;
using GroupID = int64_t;
using Argus = std::vector<std::string>;

using Function = std::string(*)(const Argus&, GroupID, UserID);

std::string NoThing(const Argus& arg, GroupID group, GroupID user);
std::string VoidOperate(const std::string& msg, GroupID group, GroupID user);

bool IsAdmin(int64_t user, bool NeedSuperViser);
bool IsManager(int64_t user);

class operate
{
public:
	operate(int id, bool na, Function function = NoThing);
	int ID;
	std::string excute(const std::string& msg, GroupID group, UserID user);
private:
	bool NeedAdmin;
	bool NeedSuperviser;
	Function function;
};

class options
{
public:
	//TODO set private:
	std::set<unsigned long long int> Admins;
	std::map<std::string, operate> Operate;
	void addCommand(const std::string& command, operate op);
	//TODO alies
	bool isCommand(const std::string&);
	void addAdmin(UserID target, UserID user);
	bool isAdmin(UserID user, GroupID group,bool needSuperviser);
};

class member
{
public:
	member(bit::BigInt = bit::BigInt(bit::DEC,(long long)-1), std::string = "");
	bool operator < (const member b)const;
	std::string getID(void)const;
	std::string getName(void)const;
	bool Choseable(void)const;
private:
	bit::BigInt StID;
	std::string Name;
	bool CanBeChose;
};

class group
{
public:
	std::set<member>  Chosen;
	std::set<member> NameList;
	group(void);
	~group(void);
};
