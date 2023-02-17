#pragma once

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>
#include "DOS.h"
using namespace std;

void gotoRowCol(int rpos, int cpos)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	int xpos = cpos, ypos = rpos;
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos;
	scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}
void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);

}

struct state {

	list<list<char>> data;
	int cr, cc;
};

class editor {

	list<list<char>> data;
	list<list<char>>::iterator ritr;
	list<char>::iterator citr;
	deque<state> undos;
	deque<state> redos;
	int cr, cc;
	bool isEmpty;

	void swap(list<list<char>>::iterator& r, list<list<char>>::iterator& l)
	{
		auto temp = *r;
		*r = *l;
		*l = temp;
	}
	void spliceLists(list<char>& oldLine, list<char>& newLine, int startIndex)
	{
		list<char>::iterator titr = oldLine.begin();
		advance(titr, startIndex);
		newLine.splice(newLine.begin(), oldLine, titr, oldLine.end());
	}

public:

	editor()
	{
		data.push_back(list<char>());
		ritr = data.begin();
		cr = 0; cc = 0;
		isEmpty = true;
	}

	void closefile(fstream& edt)
	{
		edt.close();
	}
	void loadfile(fstream& edt)
	{
		list<list<char>>::iterator tempr = ritr;
		char* c = new char;
		while (!edt.eof())
		{
			edt.read(c,1);
			if (c[0] == '\r' || c[0] == '\n')
			{
				while(c[0] == '\r' || c[0] == '\n' && !edt.eof())
					edt.read(c, 1);
				if (edt.eof())
					break;
				data.push_back(list<char>());
				tempr++;
			}
			tempr->push_back(c[0]);
		}
	}
	void savefile(fstream& edt)
	{
		for (list<char> r : data)
		{
			for (char c : r)
			{
				edt << c;
			}
			edt << endl;
		}
	}

	void printData(int r = 0, int c = 0)
	{
		system("cls");
		gotoRowCol(0, 0);
		for (list<char> r : data)
		{
			cc = 0;
			for (char c : r)
			{
				cout << c;
				cc++;
			}
			cout << endl;
			cr++;
		}
		cr--;
		if (c == 0 && r == 0)
		{
			while (*ritr != data.back())
				ritr++;
			citr = ritr->end();
			if(ritr->size() > 0)
				citr--;
		}
		else
		{
			ritr = data.begin();
			cc = c, cr = r;
			for (int i = 0; i < cr; i++)
				ritr++;
			citr = ritr->begin();
			for (int i = 0; i < cc - 1; i++)
				citr++;
		}
	}
	void clearLine(int r)
	{
		auto temp = ritr;
		temp = data.begin();
		advance(temp, r);
		gotoRowCol(r, 0);
		for (int i = 0; i <= temp->size(); i++)
		{
			cout << " ";
		}
		gotoRowCol(cr, cc);
	}
	void printLine(list<char> line)
	{
		clearLine(cr);
		gotoRowCol(cr, 0);
		for (char x : line)
		{
			cout << x;
		}
		gotoRowCol(cr, cc);
	}
	void SaveState(deque<state>& que)
	{
		list<list<char>> copyList(data);

		state temp;
		temp.data = copyList;
		temp.cr = cr;
		temp.cc = cc;

		if (que.size() > 5)
			que.pop_front();
		que.push_back(temp);
		
	}
	void LoadState(deque<state>& que)
	{
		if (que.empty())
			return;
		state temp = que.back();
		list<list<char>> copyList(temp.data);
		data = copyList;
		cr = temp.cr;
		cc = temp.cc;
		que.pop_back();
	}

	void edit(fstream& edt, bool load)
	{
		system("color f0");
		system("cls");

		int getc;

		if (load)
		{
			loadfile(edt);
		}
		printData();
		gotoRowCol(cr, cc);
		SaveState(undos);
		while (true)
		{
			getc = _getch();

			if (getc == 224)
			{
				getc = _getch();
				if (getc == 72) // up
				{
					if (cr == 0)
						continue;
					cr--;
					ritr--;
					if (cc > ritr->size())
						cc = ritr->size();
					citr = ritr->begin();
					for (int i = 0; i < cc - 1; i++)
						citr++;

				}
				else if (getc == 80) //down
				{
					if (cr + 1 == data.size())
						continue;
					cr++;
					ritr++;
					if (cc > ritr->size())
						cc = ritr->size();
					citr = ritr->begin();
					for (int i = 0; i < cc - 1; i++)
						citr++;
				}
				else if (getc == 75) //left
				{
					if (cc == 0)
						continue;
					cc--;
					if(cc != 0)
						citr--;
				}
				else if (getc == 77) //right
				{
					if (cc == ritr->size())
						continue;
					cc++;
					if(cc > 1)
						citr++;
				}
				else if (getc == 83) //del
				{
					if (cc == ritr->size())
						continue;
					SaveState(undos);

					if (cc == 0)
					{
						ritr->erase(ritr->begin());
						cc = 0;
						printLine(*ritr);
						gotoRowCol(cr, cc);
						continue;
					}
					auto temp = citr;
					temp++;
					ritr->erase(temp);
					printLine(*ritr);
					gotoRowCol(cr, cc);

				}
			}
			else if (getc == 13)
			{
				SaveState(undos);
				if (cr == data.size() - 1)
				{
					auto temp = ritr;
					data.push_back(list<char>());
					ritr++;
					if (cc != temp->size())
					{
						spliceLists(*temp, *ritr, cc);
					}
					cr++;
					cc = 0;
					printData(cr, cc);
				}
				else
				{
					auto temp = ritr;
					data.insert(ritr, list<char>());
					temp--;
					swap(temp, ritr);
					if (cc != temp->size())
					{
						spliceLists(*temp, *ritr, cc);
					}
					cr++;
					cc = 0;
					citr = ritr->begin();
					
					printData(cr,cc);
				}
				
			}
			else if (getc == 8)
			{
				if (cc == 0 && cr == 0)
					continue;

				SaveState(undos);

				if (cc == 0 && cr > 0)
				{
					auto temp = ritr;
					temp--;
					cc = temp->size();
					spliceLists(*temp, *ritr, 0);
					swap(temp, ritr);
					data.erase(ritr);
					ritr = temp;
					citr = ritr->begin();
					advance(citr, cc);
					cr--;
					printData(cr, cc);
					continue;
				}

				auto temp = citr;
				if (temp != ritr->begin())
					temp--;
				ritr->erase(citr);
				citr = temp;
				cc--;
				printLine(*ritr);
				gotoRowCol(cr, cc);
				
			}
			else if (getc == 26)
			{
				SaveState(redos);
				LoadState(undos);
				system("cls");
				printData(cr, cc);
			}
			else if (getc == 25)
			{
				SaveState(undos);
				LoadState(redos);
				system("cls");
				printData(cr, cc);
			}
			else if (getc == 27)
			{
				system("cls");
				system("color 0f");
				return;
			}
			else
			{
				if (isEmpty)
					isEmpty = false;
				if (ritr->size() == 0)
				{
					ritr->push_back(getc);
					citr = ritr->begin();
					cc++;
				}
				else if (cc == 0 && ritr->size() != 0)
				{
					ritr->push_front((char)getc);
					citr = ritr->begin();
					cc++;
				}
				else if (cc == ritr->size())
				{
					ritr->push_back(char(getc));
					citr++;
					cc++;
				}
				else
				{
					citr++;
					ritr->insert(citr, char(getc));
					cc++;
					citr--;
				}
				printLine(*ritr);
			}


			gotoRowCol(cr, cc);

		}
	}

	bool empty()
	{
		return isEmpty;
	}
	void clear()
	{
		isEmpty = true;
		data.clear();
		data.push_back(list<char>());
		ritr = data.begin();
		cr = 0; cc = 0;
	}
	int getsize()
	{
		int sum = 0;
		for (list<char> x : data)
		{
			sum += x.size();
		}
		return sum;
	}
	
};
