#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include "DOS.h"
#include "Editor.h"
 using namespace std;

char prompt = '>';

string getTime()
{
	char str[26];
	time_t result = time(NULL);
	ctime_s(str, sizeof str, &result);
	return str;
}
bool is_number(const string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}
vector<string> breakDownCommand(string in)
{
	string temp;
	vector<string> ret;
	for (int i = 0; i <= in.length(); i++)
	{
		if (in[i] == ' ' || i == in.length())
		{
			ret.push_back(temp);
			temp.clear();
			i++;
			if (i < in.length() && in[i] == '\"' )
			{
				i++;
				while (in[i] != '\"')
				{
					temp += in[i];
					i++;
				}
				continue;
			}
		}
		if(i<in.length())
		temp += in[i];
	}
	return ret;
}
void detectCommand(DOS& td, vector<string> in)
{
	transform(in[0].begin(), in[0].end(), in[0].begin(), ::tolower);
	if (in[0] == "cd")
	{
		if (in.size() == 1 || in[1] == ".")
			td.CD(".");
		else
			td.CD(in[1]);
	}
	else if (in[0] == "mkdir")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of Directory To Create One";
			return;
		}
		else if (in.size() == 3)
			td.MKDIR(in[1], in[2]);
		else
			td.MKDIR(in[1]);
	}
	else if (in[0] == "dir")
	{
		td.DIR();
	}
	else if (in[0] == "cls")
	{
		td.CLS();
	}
	else if (in[0] == "exit")
	{
		td.EXIT();
	}
	else if (in[0] == "create")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of File To Create One";
			return;
		}
		else
			td.CREATE(in[1]);
	}
	else if (in[0] == "edit")
	{
		if (in.size() == 1)
			td.EDIT();
		else
			td.EDIT(in[1]);
	}
	else if (in[0] == "del")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of File To Delete";
			return;
		}
		else
			td.DEL(in[1]);
	}
	else if (in[0] == "copy")
	{
		if (in.size() <= 2)
		{
			cout << "\nPlease Use Proper Format. >COPY [filename] [Path(D:/folder/folder1)].\n";
		}
		else
			td.COPY(in[1], in[2]);
	}
	else if (in[0] == "rmdir")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of Directory To Delete One";
			return;
		}
		else if (in.size() == 3)
			td.RMDIR(in[1], in[2]);
		else
			td.RMDIR(in[1]);
	}
	else if (in[0] == "find")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of File To Find One";
			return;
		}
		else
			td.FIND(in[1]);
	}
	else if (in[0] == "findf")
	{
		if (in.size() <= 2)
		{
			cout << "\n Please Use Proper Syntax [findf filename \"string\"]\n";
			return;
		}
		else
			td.FINDF(in[1], in[2]);
	}
	else if (in[0] == "findstr")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Use Proper Syntax [findstr \"string\"]\n";
			return;
		}
		else
			td.FINDSTR(in[1]);
	}
	else if (in[0] == "prompt")
	{
		if (in.size() == 1)
		{
			cout << "\nPlease Enter A New Character For Prompt\n";
			return;
		}
		else
			prompt = in[1][0];
	}
	else if (in[0] == "attrib")
	{
		if (in.size() == 1)
			td.ATTRIB();
		else if (in.size() == 2)
			td.ATTRIB(in[1]);
		else if (in.size() == 3)
			td.ATTRIB(in[1], in[2]);
	}
	else if (in[0] == "format")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Use Proper Syntax [format \"DIR\"]\n";
			return;
		}
		else
			td.FORMAT(in[1]);
	}
	else if (in[0] == "move")
	{
		if (in.size() <= 2)
		{
			cout << "\nPlease Use Proper Format. >MOVE [filename] [Path(D:/folder/folder1)].\n";
		}
		else
			td.MOVE(in[1], in[2]);
	}
	else if (in[0] == "rename")
	{
		if (in.size() <= 2)
		{
			cout << "\nPlease Use Proper Syntax: \"rename [filename] [new filename]\"";
			return;
		}
		else
			td.RENAME(in[1], in[2]);

	}
	else if (in[0] == "tree")
	{
		td.TREE();
	}
	else if (in[0] == "save")
	{
		td.SAVE();
	}
	else if (in[0] == "close")
	{
		td.CLOSE();
	}
	else if (in[0] == "open")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of File To Open\n";
			return;
		}
		else
			td.OPEN(in[1]);
	}
	else if (in[0] == "debug")
	{
		system("color");
	}
	else if (in[0] == "print")
	{
		if (in.size() == 1)
		{
			cout << "\n Please Enter The Name Of File To Print\n";
			return;
		}
		else
			td.PRINT(in[1]);
	}
	else if (in[0] == "queue")
	{
		td.QUEUE();
	}
	else if (in[0] == "pprint")
	{
		if (in.size() <= 2 || !is_number(in[2]))
		{
			cout << "\n Please Enter Proper Syntax: PPRINT [filename] [priority(1-100)]\n";
			return;
		}
		else
			td.PPRINT(in[1], stoi(in[2]));
	}
	else if (in[0] == "pqueue")
	{
		td.PQUEUE();
	}
	else if (in[0] == "savetree")
	{
	if (in.size() == 1)
	{
		cout << "\n Please Enter The Name Of File To Save Tree";
		return;
	}
	else
		td.SAVETREE(in[1]);
	}
	else if (in[0] == "loadtree")
	{
	if (in.size() == 1)
	{
		cout << "\n Please Enter The Name Of File To Save Tree";
		return;
	}
	else
		td.LOADTREE(in[1]);
	}


}

int main()
{
	DOS td;
	td.printInitialMessage();

	while (true)
	{
		string input;
		cout << "\n" << td.getCurrentPath() << prompt;
		getline(cin, input);

		vector<string> ret = breakDownCommand(input);
		detectCommand(td,ret);
	}
	return 0;
}

//int main()
//{
//	editor editor;
//	fstream test("newfile.txt", ios::in | ios::out);
//	editor.edit(test, true);
//}
