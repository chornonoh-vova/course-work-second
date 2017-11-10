#pragma once

#include<string>

using namespace std;

class TokenStream
{
public:
	TokenStream();

	void ResetStream();

	void SetTokenStream(char* data);
	bool GetNextToken(string* buffer, char* delimiters, int totalDelimiters);
	bool MoveToNextLine(string *buffer);

private:
	int startIndex_, endIndex_;
	string data_;
};