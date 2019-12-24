#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
using namespace std;

// every bucket stores an N number of characters and the offset it is currently at
class hash_Bucket
{
public:
	void insert(string s, int i)
	{
		m_string += s;
		offSet = i;
	}

	string getM_s()
	{
		return m_string;
	}

	int getOffSet()
	{
		return offSet;
	}

	hash_Bucket()
	{
		m_string = "";
		offSet = 0;
	}
private:
	string m_string;
	int offSet;
};

// check whether the string is in the original table
// use a vector to store every offset which has identical string with the string passed in
// originally had hashTree as an array but I had to change it to a vector since later on I pass in a size
int checkIn(string s, vector<hash_Bucket*> hashTree, int size, vector<int>& v)
{
	for (int m = 0; m < size; m++)
	{
		if (hashTree[m]->getM_s() == s)
		{
			v.push_back(hashTree[m]->getOffSet());
		}
		// put a restriction here to accelerate
		if (v.size() == 50000)
		{
			break;
		}
	}

	// return a random number
	if (v.empty())
	{
		return -2;
	}
	// if not empty, return -1
	else
	{
		return -1;
	}
}


void createDiff(istream& fold, istream& fnew, ostream& fdiff)
{
	// take the text file and put it into a string
	std::string tempy;
	std::string utilit;
	while (getline(fold, utilit))
	{
		tempy += utilit + 'n';
	}

	// take the text file and put it into a string
	std::string tempy1;
	std::string utilit1;
	while (getline(fnew, utilit1))
	{
		tempy1 += utilit1 + 'n';
	}
	// takes out the extra new line character
	tempy1.pop_back();

	int N = 8;
	int SIZER;
	// find how many 8-bytes string are there in the tempy
	if (tempy.size() <= 8)
	{
		SIZER = 1;
	}
	else
	{
		SIZER = static_cast<int>(tempy.size() - 7);
	}

	// create a vector of buckets of sizeIn
	vector<hash_Bucket*> hashTree(SIZER);

	for (int m = 0; m < SIZER; m++)
	{
		string s;
		for (int n = 0; n < N; n++)
		{
			s = s + tempy[m + n];
		}
		hashTree[m] = new hash_Bucket();
		hashTree[m]->insert(s, m);
	}

	for (int j = 0; j < static_cast<int>(tempy1.size()); j++)
	{
		// get every 8-bytes string from the tempy1
		string s1;
		for (int i = 0; i < N && (j + i) < tempy1.size(); i++)
		{
			s1 = s1 + tempy1[j + i];
		}
		// create a vector here to store different offset which satisfy the condition
		vector<int> v;
		// if found, copy
		if (checkIn(s1, hashTree, SIZER, v) == -1)
		{
			int maxLength = 0;
			int offsetOut = -1;
			// for each offset, see how long is the matching length
			for (int i = 0; i != v.size(); i++)
			{
				int copyJ = j;
				int curC = v[i];
				int L = 0;
				while (tempy[curC] == tempy1[j])
				{
					L++;
					curC++;
					j++;
				}
				// find the longest matching length
				if (L > maxLength)
				{
					maxLength = L;
					offsetOut = v[i];
					j = copyJ;
				}
				// else, increment to next offset
				else
				{
					j = copyJ;
				}
			}
			j = j + maxLength;
			j--;
			fdiff << "C" << maxLength << "," << offsetOut;
		}
		// if not found, add
		else if (checkIn(s1, hashTree, SIZER, v) == -2)
		{
			int countAdd = 0;
			// store each first letter in a string
			// at the end, add the string
			string s2;
			while (checkIn(s1, hashTree, SIZER, v) == -2 && (countAdd + j) != static_cast<int>(tempy1.size()))
			{
				s2 += s1[0];
				countAdd++;
				s1 = "";
				for (int i = 0; (i < N && (countAdd + j + i) < tempy1.size()); i++)
				{
					s1 = s1 + tempy1[countAdd + j + i];
				}
			}
			j = j + static_cast<int>(s2.size()) - 1;
			fdiff << "A" << static_cast<int>(s2.size()) << ":" << s2;
		}
	}
	// delete whatever we created using NEW
	for (int m = 0; m < SIZER; m++)
	{
		delete hashTree[m];
	}
}

bool getInt(istream& inf, int& n)
{
	char ch;
	if (!inf.get(ch) || !isascii(ch) || !isdigit(ch))
		return false;
	inf.unget();
	inf >> n;
	return true;
}

bool getCommand(istream& inf, char& cmd, int& length, int& offset)
{
	if (!inf.get(cmd))
	{
		cmd = 'x';  // signals end of file
		return true;
	}
	char ch;
	switch (cmd)
	{
	case 'A':
		return getInt(inf, length) && inf.get(ch) && ch == ':';
	case 'C':
		return getInt(inf, length) && inf.get(ch) && ch == ',' && getInt(inf, offset);
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

bool applyDiff(istream& fold, istream& fdiff, ostream& fnew)
{
	char comnd;
	char nexter;
	int length = 0;
	int offset = 0;

	// while the fdiff is not empty
	do
	{
		if (!getCommand(fdiff, comnd, length, offset))
		{
			return false;
		}

		// return false if you can't do the function anymore
		if ((comnd != 'A' && comnd != 'C' && comnd != 'x') || length < 0 || offset < 0)
		{
			return false;
		}

		// command is add, add the string to newfile from difffile
		if (comnd == 'A')
		{
			for (int i = 1; i < length + 1; i++)
			{
				fdiff.get(nexter);
				fnew << nexter;
			}
		}

		// command is copy, copy the string from oldfile to newfile
		else if (comnd == 'C')
		{
			fold.seekg(offset);

			for (int i = 1; i < length + 1; i++)
			{
				fold.get(nexter);
				fnew << nexter;
			}
		}

	} while (comnd != 'x');

	return true;
}

void runtest(string oldtext, string newtext)
{
	istringstream oldFile(oldtext);
	istringstream newFile(newtext);
	ostringstream diffFile;
	createDiff(oldFile, newFile, diffFile);
	string result = diffFile.str();
	cout << "The diff file length is " << result.size()
		<< " and its text is " << endl;
	cout << result << endl;

	oldFile.clear();   // clear the end of file condition
	oldFile.seekg(0);  // reset back to beginning of the stream
	istringstream diffFile2(result);
	ostringstream newFile2;
	assert(applyDiff(oldFile, diffFile2, newFile2));
	assert(newtext == newFile2.str());
}

int main()
{
	runtest("There's a bathroom on the right.",
		"There's a bad moon on the rise.sddsfsdffkdsf");
	runtest("ABCDEFGHIJBLAHPQRSTUVPQRSTUV",
		"XYABCDEFGHIJBLETCHPQRSTUVPQRSTQQELF");
	cout << "All tests passed" << endl;
}