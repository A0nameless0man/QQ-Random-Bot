#pragma once

#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#define Check(a) (std::cout<<#a<<':'<<(a)<<std::endl);
namespace bit
{
	using Pos = short;
	using Digt = int;
	using singleDigt = short;
	using IntStore = std::vector<singleDigt>;
	const Pos BIN = 2;
	const Pos OCT = 8;
	const Pos DEC = 10;
	const Pos HXE = 16;
	char itoc(int);
	int ctoi(char);
	class BigInt
	{
	public:
		BigInt(Pos pos = DEC, long long def = 0);
		~BigInt();
		std::string string(void)const;
		BigInt operator+(const BigInt&b)const;
		BigInt operator+=(const BigInt&b);
		BigInt operator++(int);
		BigInt operator-(const BigInt&b)const;
		BigInt operator*(const singleDigt&)const;
		BigInt operator*(const BigInt&b)const;
		BigInt operator/(const BigInt&b)const;
		bool operator<(const BigInt&b)const;
		bool operator==(const BigInt&b)const;
		friend std::ostream & operator<<(std::ostream & os, const BigInt & in);
		friend std::istream & operator>>(std::istream&os, BigInt & in);
	private:
		IntStore ReverseInt;
		Pos pos;
		bool postive;
		BigInt move(Digt);
		bool absSmaller(const BigInt&)const;
		inline void correctPostive(void);
		inline bool isZero(void)const;
	};
}