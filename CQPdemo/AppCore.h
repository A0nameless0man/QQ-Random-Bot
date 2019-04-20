#pragma once
#include"PublicRely.h"
#include "stdafx.h"
#include"Frame.h"

std::string Help(const Argus& arg, int64_t group, int64_t user);
std::string Start(const Argus& arg, int64_t groupid, int64_t user);
std::string Clear(const Argus& arg, int64_t groupid, int64_t user);
std::string addmember(const Argus& arg, int64_t groupid, int64_t user);
std::string reffer(const Argus& arg, int64_t groupid, int64_t user);
std::string ListLucky(const Argus& arg, int64_t groupid, int64_t user);