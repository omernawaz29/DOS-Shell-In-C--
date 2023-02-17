#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <string>
#include <string.h>
#include <time.h>
#include "Editor.h"
using namespace std;


struct dir;

struct file {
	
	string name;
	string datemodified;
	char attribs[4] = { 'R','A','S','H' };
	bool astates[4] = { false,false,false,false };
	int size;
	dir* parent;

};
struct dir {

	string name;
	string datemodified;
	int size;

	dir* parent;
	list<dir*> childDir;
	list<file*> files;

};
struct printTask
{
	file* task;
	int priority;
	int timestarted;
	int printTime;
	bool isActive;
	printTask(file* _task, int _prior = 0)
	{
		task = _task;
		priority = _prior;
		printTime = task->size * 5;
		isActive = false;
		timestarted = time(0);
	}
};
struct Comparator
{
	bool operator()(const printTask* t1, const printTask* t2)
	{
		return t1->priority < t2->priority;
	}
};

class DOS {

	//data members
	dir* vDir;
	dir* currDir;
	editor teditor;
	fstream filehandler;
	file* openfile{};
	queue<printTask> printqueue;
	priority_queue<printTask*, vector<printTask*>, Comparator> priorityqueue;
	float ver = 0.1;


	//helper functions
	string getTime()
	{
		char str[26];
		time_t result = time(NULL);
		ctime_s(str, sizeof str, &result);
		return str;
	}
	dir* findDir(dir* DIR, string _name)
	{
		for (dir* x : DIR->childDir)
		{
			if (x->name == _name)
				return x;
		}
		return nullptr;
	}
	file* findFile(dir* DIR, string _name)
	{
		for (file* x : DIR->files)
		{
			if (x->name == _name)
				return x;
		}
		return nullptr;
	}
	void  findFileInTree(dir* DIR, string _name,file*& ret)
	{
		if (DIR == nullptr)
			return;

		for (file* x : DIR->files)
		{
			if (x->name == _name)
				ret = x;
		}

		if (DIR->childDir.size() != 0)
		{
			for (dir* x : DIR->childDir)
			{
				findFileInTree(x, _name,ret);
			}
		}
	}
	void findFileInTreeWithStr(dir* DIR, string searchString, file*& ret)
	{
		if (DIR == nullptr)
			return;

		for (file* x : DIR->files)
		{
			if (FINDF(DIR,x->name,searchString))
				ret = x;
		}

		if (DIR->childDir.size() != 0)
		{
			for (dir* x : DIR->childDir)
			{
				findFileInTreeWithStr(x, searchString, ret);
			}
		}
	}
	bool copyFile(file*& destFile, file* orgFile)
	{
		destFile->name = orgFile->name;
		destFile->size = orgFile->size;
		return true;
	}
	bool isPath(string loc)
	{
		if (loc[0] == vDir->name[0] && loc[1] == ':')
			return true;
		return false;
	}
	vector<string> breakDownPath(string path)
	{
		string temp;
		vector<string> ret;
		for (int i = 0; i <= path.length(); i++)
		{
			if (path[i] == '/' || i == path.length())
			{
				ret.push_back(temp);
				temp.clear();
				i++;
			}
			if (i < path.length())
				temp += path[i];
		}
		return ret;
	}
	dir* gotoPath(string path)
	{
		vector<string> pathvec = breakDownPath(path);
		auto temp = vDir;
		for (int i = 1; i < pathvec.size(); i++)
		{
			temp = findDir(temp, pathvec[i]);
			if (!temp)
				return temp;
		}
		return temp;
	}
	bool isSubstring(string subString, string mainString)
	{
		int s1 = subString.length();
		int s2 = mainString.length();

		for (int i = 0; i <= s2 - s1; i++) {
			int j;

			
			for (j = 0; j < s1; j++)
				if (mainString[i + j] != subString[j])
					break;

			if (j == s1)
				return true;
		}

		return false;
	}
	bool FINDF(dir* DIR, string _name, string searchString)
	{
		file* temp = findFile(DIR, _name);
		if (!temp)
		{
			return false;
		}

		ifstream rdr(_name);
		string tempstring;

		while (getline(rdr, tempstring))
		{
			if (isSubstring(searchString, tempstring))
			{
				return true;
			}
		}

		return false;

	}
	int getAttribIndex(char attrib)
	{
		attrib = toupper(attrib);
		char attribsarray[4] = { 'R','A','S','H' };
		for (int i = 0; i < 4; i++)
			if (attribsarray[i] == attrib)
				return i;
		return -1;
		
	}
	void setAttrib(file* fil, int attrib)
	{
		fil->astates[attrib] = true;
	}
	void removeAttrib(file* fil, int attrib)
	{
		fil->astates[attrib] = false;
	}

	void  deleteTree(dir* DIR)
	{
		if (DIR == nullptr)
			return;

		for (file* x : DIR->files)
		{
			delete x;
		}

		DIR->files.clear();

		if (DIR->childDir.size() != 0)
		{
			for (dir* x : DIR->childDir)
			{
				deleteTree(x);
				delete x;
			}
		}

		DIR->childDir.clear();
	}

	char* convertToChar(string in)
	{
		char* filename = new char[in.length() + 1];
		int i;
		for (i = 0; i < in.length(); i++)
		{
			filename[i] = in[i];
		}
		filename[i] = '\0';
		return filename;
	}
	void displayTree(dir* DIR,int depth = 0)
	{
		if (DIR == nullptr)
			return;
		
		for (dir* x : DIR->childDir)
		{
			for (int i = 0; i < depth; i++)
cout << "\t";
cout << x->name << endl;

displayTree(x, depth + 1);
		}

		for (file* j : DIR->files)
		{
			for (int i = 0; i < depth; i++)
				cout << "\t";
			cout << j->name << endl;
		}
	}

	void updatePrintQueue()
	{
		if (printqueue.empty())
			return;

		printTask temp = printqueue.front();
		if (time(0) - temp.timestarted > temp.printTime)
		{
			printqueue.pop();
			printTask& temp2 = printqueue.front();
			temp2.isActive = true;
			temp2.timestarted = time(0);
		}

	}
	void updatePriorityQueue()
	{
		printTask* temp = priorityqueue.top();
		if (time(0) - temp->timestarted > temp->printTime)
		{
			priorityqueue.pop();
		}
		temp = priorityqueue.top();
		if (!temp->isActive)
		{
			temp->isActive = true;
			temp->timestarted = time(0);
		}
	}

	vector<string> spliceStr(string str, char delim)
	{
		string temp;
		vector<string> ret;
		for (int i = 0; i <= str.length(); i++)
		{
			if (str[i] == delim || i == str.length())
			{
				ret.push_back(temp);
				temp.clear();
				i++;
			}
			if (i < str.length())
				temp += str[i];
		}
		return ret;
	}

	void writeTree(ofstream& wrt, dir* DIR, int depth = 0)
	{
		if (DIR == nullptr)
			return;
		for (dir* x : DIR->childDir)
		{
			for (int i = 0; i < depth; i++)
				wrt << "\t-";
			wrt << "DIR-" << x->name << "-" << x->size << "-" << x->datemodified << endl;

			writeTree(wrt, x, depth + 1);
		}

		for (file* j : DIR->files)
		{
			for (int i = 0; i < depth; i++)
				wrt << "\t-";
			wrt << "FILE-" << j->name << "-" << j->size << "-" << j->datemodified << endl;
		}
	}
	void loadTree(ifstream& rdr, dir* DIR)
	{
		string rec;
		int depth{};
		while (!rdr.eof())
		{
			int newDepth{};
			getline(rdr, rec);
			vector<string> vRec = spliceStr(rec, '-');
			int i{};
			while (vRec[i] == "\t")
			{
				newDepth++;
				i++;
			}

			if ((newDepth == depth) && newDepth == 0 || newDepth == depth)
			{
				if (vRec[i] == "DIR")
				{
					dir* temp = new dir;
					temp->name = vRec[i + 1];
					temp->size = stoi(vRec[i + 2]);
					temp->datemodified = vRec[i + 3];
					temp->parent = DIR;
					DIR->childDir.push_back(temp);
				}
				if (vRec[i] == "FILE")
				{
					file* temp = new file;
					temp->name = vRec[i + 1];
					temp->size = stoi(vRec[i + 2]);
					temp->datemodified = vRec[i + 3];
					temp->parent = DIR;
					DIR->files.push_back(temp);
				}
				depth = newDepth;
			}
			else if (depth < newDepth)
			{
				DIR = DIR->childDir.back();
				if (vRec[i] == "DIR")
				{
					dir* temp = new dir;
					temp->name = vRec[i + 1];
					temp->size = stoi(vRec[i + 2]);
					temp->datemodified = vRec[i + 3];
					temp->parent = DIR;
					DIR->childDir.push_back(temp);
				}
				if (vRec[i] == "FILE")
				{
					file* temp = new file;
					temp->name = vRec[i + 1];
					temp->size = stoi(vRec[i + 2]);
					temp->datemodified = vRec[i + 3];
					temp->parent = DIR;
					DIR->files.push_back(temp);
				}
				depth = newDepth;

			}
			else if (depth > newDepth)
			{
				int tempdep = newDepth;
				while (DIR->parent && tempdep != depth)
				{
					DIR = DIR->parent;
					tempdep++;
				}

				if (vRec[i] == "DIR")
				{
					dir* temp = new dir;
					temp->name = vRec[i + 1];
					temp->size = stoi(vRec[i + 2]);
					temp->datemodified = vRec[i + 3];
					temp->parent = DIR;
					DIR->childDir.push_back(temp);
				}
				if (vRec[i] == "FILE")
				{
					file* temp = new file;
					temp->name = vRec[i + 1];
					temp->size = stoi(vRec[i + 2]);
					temp->datemodified = vRec[i + 3];
					temp->parent = DIR;
					DIR->files.push_back(temp);
				}
				depth = newDepth;
			}


		}

	}

public:


	DOS(string _name = "V")
	{
		_name += ":";
		vDir = new dir;
		vDir->name = _name;
		vDir->parent = nullptr;
		vDir->datemodified = getTime();
		vDir->datemodified.pop_back();
		vDir->size = 0;
		currDir = vDir;
		filehandler.close();
	}

	string getCurrentPath()
	{
		string tempstr;
		string retstr;
		dir* tptr = currDir;
		while (tptr)
		{
			tempstr = tptr->name + "/";
			retstr = tempstr + retstr;
			tptr = tptr->parent;
		}
		return retstr;
	}
	string getPath(dir* DIR)
	{
		string tempstr;
		string retstr;
		dir* tptr = DIR;
		while (tptr)
		{
			tempstr = tptr->name + "/";
			retstr = tempstr + retstr;
			tptr = tptr->parent;
		}
		return retstr;
	}
	void printInitialMessage()
	{
		string initialmessage = "Virtual DOS. Muhammad Omer Nawaz. BSCS20010.\nData Structures And Algorithms/";
		cout << initialmessage << " Version: " << ver << endl << endl;
	}

	void MKDIR(string _name)
	{
		dir* temp = new dir;
		temp->name = _name;
		temp->parent = currDir;
		temp->datemodified = getTime();
		temp->datemodified.pop_back();
		temp->size = 0;

		currDir->childDir.push_back(temp);
	}
	void MKDIR(string path, string _name)
	{
		if (!isPath(path))
		{
			cout << "\nInvalid Path. Please Enter Proper Path e.g [D:/Dir1/Dir2]\n";
			return;
		}

		dir* tempdir = gotoPath(path);
		if (!tempdir)
		{
			cout << "\nPath Not Found. Please Enter Valid Path.\n";
			return;
		}

		dir* temp = new dir;
		temp->name = _name;
		temp->parent = tempdir;
		temp->datemodified = getTime();
		temp->datemodified.pop_back();
		temp->size = 0;

		tempdir->childDir.push_back(temp);
	}
	void RMDIR(string _name)
	{
		dir* tempdir = findDir(currDir, _name);
		if (!tempdir)
		{
			cout << "\nDIR not found. Please enter a valid DIR.\n";
			return;
		}

		if (tempdir->childDir.size() != 0 || tempdir->files.size() != 0)
		{
			cout << "\nDIR not empty.\n";
			return;
		}
		delete tempdir;
		currDir->childDir.remove(tempdir);
	}
	void RMDIR(string path, string _name)
	{
		dir* temploc = gotoPath(path);
		if (!temploc)
		{
			cout << "\nPath Not Found. Please Enter Valid Path.\n";
			return;
		}
		dir* tempdir = findDir(temploc, _name);
		if (!tempdir)
		{
			cout << "\DIR Not Found. Please Enter Valid DIR.\n";
			return;
		}

		if (tempdir->childDir.size() != 0 || tempdir->files.size() != 0)
		{
			cout << "\nDIR not empty.\n";
			return;
		}

		delete tempdir;
		temploc->childDir.remove(tempdir);
	}

	void CREATE(string _name)
	{
		if(!isSubstring(".txt",_name))
			_name += ".txt";

		ofstream wrt(_name);
		wrt.close();

		file* temp = new file;
		temp->name = _name;
		temp->parent = currDir;
		temp->datemodified = getTime();
		temp->datemodified.pop_back();
		temp->size = 0;

		currDir->files.push_back(temp);
	}
	void COPY(string _name, string path)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* org_file = findFile(currDir, _name);
		if (!org_file)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}
		dir* destDir = gotoPath(path);
		if (!destDir)
		{
			cout << "\nPath Not Found. Please Enter Valid Path.\n";
			return;
		}

		file* copy_file = new file;
		copyFile(copy_file, org_file);
		copy_file->parent = destDir;

		copy_file->datemodified = getTime();
		copy_file->datemodified.pop_back();

		destDir->files.push_back(copy_file);

	}

	void DEL(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* temp = findFile(currDir, _name);
		if (!temp)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}

		currDir->files.remove(temp);

		char* filename = convertToChar(_name);
		remove(filename);
	}
	void EDIT(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* temp = findFile(currDir, _name);
		if (!temp)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}
		if (temp->astates[0])
		{
			cout << "\nFile is Read-Only. Please Enter Valid Name.\n";
			return;
		}

		if (!filehandler.is_open())
		{
			OPEN(_name);
			teditor.edit(filehandler, temp->size != 0);
		}
		else
		{
			cout << "\nAnother file is already open. Please close that file.\n";
			return;
		}

		
	}
	void EDIT()
	{
		if (!filehandler.is_open())
		{
			cout << "\nThere are no open files.\n";
			return;
		}
		else
			teditor.edit(filehandler, teditor.empty() && openfile->size != 0);


	}
	void RENAME(string _name, string newname)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";
		if (!isSubstring(".txt", newname))
			newname += ".txt";

		file* temp = findFile(currDir, _name);
		if (!temp)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}

		temp->name = newname;

		char* filename = convertToChar(_name);
		char* newfilename = convertToChar(newname);
		rename(filename,newfilename);
	}
	void DIR()
	{
		cout << "\n";
		for (dir* x : currDir->childDir)
		{
			cout << x->datemodified << "\t" << "<DIR>" << "\t" << " " << "\t" << x->name;
			cout << endl;
		}
		for (file* x : currDir->files)
		{
			if (!x->astates[3])
			{
				cout << x->datemodified << "\t" << "     " << "\t" << x->size << "\t" << x->name;
				cout << endl;
			}
		}
		cout << endl;
		cout << currDir->files.size() << "\tFile(s)\n" << currDir->childDir.size() << "\tDIR(s)\n" << currDir->size << "\tbyte(s)\n\n";
	}
	void CD(string _name)
	{
		if (isPath(_name))
		{
			dir* tempdir = gotoPath(_name);
			if (!tempdir)
				cout << "\nPath Not Found. Please Enter Valid Path.\n";
			else
				currDir = tempdir;
			return;
		}
		else if (_name == "..")
		{
			currDir = currDir->parent;
			return;
		}
		else if (_name == ".")
		{
			cout << getCurrentPath() << "\n\n";
			return;
		}
		else if (_name == "\\")
		{
			currDir = vDir;
			return;
		}

		dir* temp = findDir(currDir, _name);

		if (temp)
			currDir = temp;
		else
			cout << "The DIR does not exist. Please specify a valid DIR.\n\n";
	}

	void FIND(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* temp;
		findFileInTree(currDir, _name,temp);

		if (!temp)
		{
			cout << "\nFile Not Found. Please enter a valid file name.\n";
			return;
		}

		cout << "\nFile Found: " << getPath(temp->parent) << endl;
	}
	void FINDF(string _name, string searchString)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* temp = findFile(currDir, _name);
		if (!temp)
		{
			cout << "\nFile Not Found. Please enter a valid file name.\n";
			return;
		}

		ifstream rdr(_name);
		string tempstring;
		
		while (getline(rdr, tempstring))
		{
			if (isSubstring(searchString,tempstring))
			{
				cout << "\nThe Search String is present in the file.\n";
				return;
			}
		}

		cout << "\nThe Search String is NOT present in the file.\n";
		return;

	}
	void FINDSTR(string searchString)
	{
		file* temp = nullptr;
		findFileInTreeWithStr(currDir, searchString, temp);
		if (!temp)
		{
			cout << "\nNo file found with given string";
			return;
		}

		string ret = getPath(temp->parent);
		ret = ret + temp->name;

		cout << endl << "String Found In: " << ret << endl;
	}

	void ATTRIB()
	{
		for (file* x : currDir->files)
		{
			for (int i = 0; i < 4; i++)
			{
				if (x->astates[i])
					cout << x->attribs[i];
				cout << "\t";
			}

			cout << "\t" << x->name << endl;
		}
	}
	void ATTRIB(string attrib)
	{
		int ind = getAttribIndex(attrib[0]);

		if (ind == -1)
		{
			cout << "\nAttrib not found. Please enter a valid Attribute\n";
			return;
		}

		for (file* x : currDir->files)
		{
			if (x->astates[ind])
				cout << x << endl;
		}

	}
	void ATTRIB(string attrib, string _name)
	{
		
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* tempfile = findFile(currDir, _name);
		if (!tempfile)
		{
			cout << "\nFile not found. Please enter a valid filename\n";
			return;
		}

		if (attrib[0] == '+')
		{
			int ind = getAttribIndex(attrib[1]);
			if (ind == -1)
			{
				cout << "\nAttrib not found. Please enter a valid Attribute\n";
				return;
			}

			setAttrib(tempfile, ind);
		}
		else if (attrib[0] == '-')
		{
			int ind = getAttribIndex(attrib[1]);
			if (ind == -1)
			{
				cout << "\nAttrib not found. Please enter a valid Attribute\n";
				return;
			}

			removeAttrib(tempfile, ind);
		}
		else
		{
			cout << "\nPlease Use Proper Syntax. ATTRIB [+R] [filename] or ATTRIB [-H] [filename]\n";
			return;
		}
	}

	void FORMAT(string _name)
	{
		dir* tempdir = findDir(currDir, _name);
		if (!tempdir)
		{
			cout << "\nDIR not found. Please enter a valid DIR.\n";
			return;
		}

		deleteTree(tempdir);
		delete tempdir;
		currDir->childDir.remove(tempdir);

		cout << "\nDIR formatted successfully.\n";

	}
	void MOVE(string _name, string path)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		file* org_file = findFile(currDir, _name);
		if (!org_file)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}
		dir* destDir = gotoPath(path);
		if (!destDir)
		{
			cout << "\nPath Not Found. Please Enter Valid Path.\n";
			return;
		}

		currDir->files.remove(org_file);
		destDir->files.push_back(org_file);
		org_file->parent = destDir;
	}
	void TREE()
	{
		cout << currDir->name << endl;
		displayTree(currDir, 1);
	}

	void CLS()
	{
		system("cls");
		printInitialMessage();
	}
	void EXIT()
	{
		CLS();
		exit(0);
	}

	void OPEN(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";
		file* temp = findFile(currDir, _name);

		if (!temp)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}

		if (filehandler.is_open())
		{
			cout << "\nAnother file is already open. Please close that file.\n";
			return;
		}
		else
		{
			filehandler.open(_name, ios::in | ios::out);
			openfile = temp;
		}
			
		if (filehandler.is_open())
		{
			cout << "\n File Opened\n";
		}
			
	}
	void CLOSE()
	{
		if (!filehandler.is_open())
		{
			cout << "\nError. No File is Open.\n";
			return;
		}

		openfile = nullptr;
		filehandler.close();
		teditor.clear();
	}
	void SAVE()
	{
		if (!filehandler.is_open())
		{
			cout << "\nError. No File is Open.\n";
			return;
		}

		teditor.savefile(filehandler);
		openfile->size = teditor.getsize();

		auto tptr = currDir;
		while (tptr)
		{
			tptr->size += openfile->size;
			tptr = tptr->parent;
		}
	}

	void PRINT(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";
		file* temp = findFile(currDir, _name);

		if (!temp)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}

		updatePrintQueue();
		printTask tempTask(temp);
		if (printqueue.empty())
		{
			tempTask.isActive = true;
			tempTask.timestarted = time(0);
		}
		printqueue.push(tempTask);

	}
	void QUEUE()
	{
		updatePrintQueue();
		queue<printTask> temp = printqueue;
		int size = temp.size();
		cout << "\n Files in Queue: \n";
		while (temp.size() > 0)
		{
			printTask temptsk = temp.front();

			cout << size - temp.size() + 1 << ". " << temptsk.task->name << ": ";

			if (temptsk.isActive)
				cout << temptsk.printTime - (time(0) - temptsk.timestarted) << " seconds remaining." << endl;
			else
				cout << "Waiting for existing task...";
			
			temp.pop();
		}
	}

	void PPRINT(string _name, int priority)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";
		file* temp = findFile(currDir, _name);

		if (!temp)
		{
			cout << "\nFile Not Found. Please Enter Valid Name.\n";
			return;
		}


		printTask* tempTask = new printTask(temp, priority);

		if (priorityqueue.empty())
		{
			tempTask->isActive = true;
			tempTask->timestarted = time(0);
		}
		else
		{
			printTask* tempTask2 = priorityqueue.top();
			tempTask2->isActive = false;
			tempTask2->printTime = tempTask2->printTime - (time(0) - tempTask2->timestarted);
		}

		priorityqueue.push(tempTask);
		updatePriorityQueue();
	}
	void PQUEUE()
	{
		updatePriorityQueue();
		priority_queue<printTask*, vector<printTask*>, Comparator> temp = priorityqueue;
		int size = temp.size();
		cout << "\n Files in Queue: \n";
		while (temp.size() > 0)
		{
			printTask* temptsk = temp.top();

			cout << size - temp.size() + 1 << ". " << temptsk->task->name << ": ";

			if (temptsk->isActive)
				cout << temptsk->printTime - (time(0) - temptsk->timestarted) << " seconds remaining.";
			else
				cout << "Waiting for existing task...";
			
			cout << "\t (Priority: " << temptsk->priority << ")" << endl;

			temp.pop();
		}
	}

	void SAVETREE(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		ofstream wrt(_name);
		writeTree(wrt, vDir, 0);
	}
	void LOADTREE(string _name)
	{
		if (!isSubstring(".txt", _name))
			_name += ".txt";

		ifstream rdr(_name);
		if (rdr.good())
			loadTree(rdr, vDir);
		else
			cout << "\nError Loading Tree.\n";
	}



};