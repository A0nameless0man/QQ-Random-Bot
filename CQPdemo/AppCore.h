#pragma once
#include"PublicRely.h"
#include "stdafx.h"
#include"Frame.h"

//This part is responsible for interact with users

void loadCommand(void);
void loadAdmin(void);
void loadGroups(void);
void saveAdmin(void);
void saveGroups(void);


std::string help(const Argus& arg, GroupID group, UserID user);
std::string start(const Argus& arg, GroupID groupid, UserID user);
std::string clear(const Argus& arg, GroupID groupid, UserID user);
std::string addmember(const Argus& arg, GroupID groupid, UserID user);
std::string reffer(const Argus& arg, GroupID groupid, UserID user);
std::string listLucky(const Argus& arg, GroupID groupid, UserID user);
std::string removeMember(const Argus& arg, GroupID groupid, UserID user);
std::string setChooseable(const Argus& arg, GroupID groupid, UserID user);