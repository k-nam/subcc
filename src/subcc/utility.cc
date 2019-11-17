#include "utility.h"
#include <string>
#include <deque>
#include <iostream>

namespace subcc
{
using namespace std;

string
stringOfInt(int number)
{
	bool minus = false;
	if (number < 0)
	{
		minus = true;
		number = -number;
	}
	deque<char> digits; // store each digit
	while (true)
	{
		digits.push_front(number % 10 + '0'); // int -> character
		number /= 10;
		if (number == 0)
			break;
	}
	if (minus)
	{
		digits.push_front('-');
	}
	size_t len = digits.size();
	char *pString = new char[len];
	for (int i = 0; i < len; ++i)
	{
		pString[i] = digits[i];
	}
	return string(pString, len);
}

char charOf(string const &string)
{
	if (string.size() == 3)
	{ // normal character
		return string[1];
	}
	else if (string.size() == 4)
	{ // escape character
		if (string[1] != '\\')
		{
			cout << "charOf: escape char should begin with a '\\' \n";
			exit(0);
		}
		switch (string[2])
		{
		case 'a':
			return '\a';
		case 'b':
			return '\b';
		case 'f':
			return '\f';
		case 'v':
			return '\v';
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case '\'':
			return '\'';
		case '"':
			return '"';
		case '\\':
			return '\\';
		default:
			cout << "unregistered escape char\n";
			exit(0);
		}
	}
	else
	{ // error
		cout << "charOf: length should be 3 or 4\n";
		exit(0);
	}
}

int intOfDec(string const &string)
{
	int value = 0;
	for (size_t i = 0; i < string.size(); ++i)
	{
		value *= 10;
		value += string[i] - '0';
	}
	return value;
}

int intOfHex(string const &string)
{
	using std::cout;
	if (string[0] != '0' || (string[1] != 'x' && string[1] != 'X'))
	{
		cout << "intOfHex: error, input is not hex constant\n";
		exit(0);
	}
	int value = 0;
	for (size_t i = 2; i < string.size(); ++i)
	{
		value <<= 4;
		char digit = string[i];
		if (digit >= '0' && digit <= '9')
		{
			value += (digit - '0');
		}
		else if (digit >= 'a' && digit <= 'f')
		{
			value += (digit - 'a' + 10);
		}
		else
		{ // error
			cout << "intOfHex error: input hex contains illegal char\n";
			exit(0);
		}
	}
	return value;
}

float floatOf(string const &string)
{
	return static_cast<float>(1.1);
}
} // namespace subcc
