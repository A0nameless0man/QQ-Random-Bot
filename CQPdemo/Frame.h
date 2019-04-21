#pragma once
#include "stdafx.h"
#include"PublicRely.h"
const int MAX_BUFFER = 100;
extern int ac;
using Argus = std::vector<std::string>;
using Function = std::string(*)(const Argus&, int64_t, int64_t);
std::string NoThing(const Argus& arg, int64_t group, int64_t user);
std::string VoidOperate(const std::string& msg, int64_t group, int64_t user);
bool IsAdmin(int64_t user, bool NeedSuperViser);
bool IsManager(int64_t user);
class operate
{
public:
	operate(int id, bool na, Function function = NoThing) :ID(id), NeedAdmin(na), NeedSuperviser(false), function(function) { ; }
	int ID;
	std::string excute(const std::string& msg, int64_t group, int64_t user);
private:
	bool NeedAdmin;
	bool NeedSuperviser;
	Function function;
};

struct options
{
	std::set<unsigned long long int> Admins;
	std::map<std::string, operate> Operate;
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
