#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <fstream>
#include <list>
#include <stdio.h>
#include <iterator>
#include <algorithm>
#include <math.h>

using namespace std;

//Cursor vars
int cursor_X_Start = 3;
int cursor_Y_Start = 1;
int cursor_X = 0;
int cursor_Y = 0;
CONSOLE_SCREEN_BUFFER_INFO csbi;

//Editor vars
int lastC = -1;
int line = 1;
string dataLine = "";
list <string> allData;
string fileName;

//Character code list 
//TODO: change to another file
const int DEL = 8;
const int CR  = 13; //line jump
const int ESC = 27;
const int AR  = 224; //arrow
const int LAR = 75;  //Left Arrow
const int RAR = 77;  //Right Arrow
const int UAR = 72;  //Up Arrow
const int DAR = 80;  //Down Arrow


void save(string name) {
	ofstream file;
	file.open(name);
	for  (string line : allData)
	{
		file << line << endl;
	}
	file.close();
}

void gotoxy(int x, int y) {
	HANDLE hcon;
	hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD dwpos;
	dwpos.X = x;
	dwpos.Y = y;
	SetConsoleCursorPosition(hcon, dwpos);
}
int getDigits(int num) {
	int digits = 0;
	if (num < 0) digits = 1;
	while (num) {
		num /= 10;
		digits++;
	}
	return digits;
}

void addPrefix() {
	cursor_X_Start = getDigits(line) + 2;
	cursor_X = cursor_X_Start;
	gotoxy(0, cursor_Y);
	std::cout << line << ". ";
	gotoxy(cursor_X, cursor_Y);
}

void open(string name) {

	ifstream file;
	file.open(name);
	if (file.bad()) {
		file.close();
		delete &file;
		ofstream file;
		file.open(name);
		return;
	}
	cout << name << endl;
	string SLine;
	cursor_Y = cursor_Y_Start;
	while (getline(file,SLine)) { //TODO improve tab render
		allData.push_back(SLine);
		addPrefix();
		line++;
		replace(SLine.begin(),SLine.end(),'\t',' ');
		cout << SLine << endl;
		cursor_Y++;
	}
	file.close();

}

void editor() {
	
	int c;
	while (true) {
		c = _getch();
		if (c == ESC) {
			system("cls");
			int response = 0;
			int Max_Rows = trunc((csbi.srWindow.Bottom - csbi.srWindow.Top + 1) / 2);
			int count = 0;
			do {
				if (count >= Max_Rows) { system("cls"); count = 0; }
				if (response != 0) { cout << "please type Y or N" << endl; count++; }
				count++;
				cout << "Save file [Y/n]";
				response = _getch();
				cout << endl;

			} while (response != 'y' && response != 'Y' && response != 'N' && response != 'n');
			

			if (response == 'y' || response == 'Y') {
				if (allData.size() != line && dataLine != "") {
					if (cursor_Y == line) allData.push_back(dataLine);
					

					
				}
				//else {

				//	auto it = next(allData.begin(), cursor_Y - cursor_Y_Start);
				//	*it = dataLine;
				//}
				if (fileName == "") {
					cout << "File Name: ";
					cin >> fileName;

				}
				save(fileName);
			}
			break;
		}
		if (lastC == AR) {
			switch (c)
			{
			case LAR:
				if (cursor_X > cursor_X_Start) {
					cursor_X--;
					gotoxy(cursor_X, cursor_Y);
				}
				break;
			case RAR:
				if ((cursor_X - cursor_X_Start) <= (dataLine.length() + 1)) {
					cursor_X++;
					gotoxy(cursor_X, cursor_Y);

				}
				break;
			case UAR:
				if (cursor_Y > cursor_Y_Start) {
					if (allData.size() != line) {
						allData.push_back(dataLine);
					}
					else {

						auto it0 = next(allData.begin(), cursor_Y - cursor_Y_Start);
						*it0 = dataLine;
					}
					cursor_Y--;
					gotoxy(cursor_X, cursor_Y);
					auto it = next(allData.begin(), cursor_Y - cursor_Y_Start);
					dataLine = *it;
				}
				break;
			case DAR:
				if (cursor_Y < line) {
					if (allData.size() != line) {
						allData.push_back(dataLine);
					}
					else {

						auto it0 = next(allData.begin(), cursor_Y - cursor_Y_Start);
						*it0 = dataLine;
					}
					cursor_Y++;
					gotoxy(cursor_X, cursor_Y);
					auto it = next(allData.begin(), cursor_Y - cursor_Y_Start);
					dataLine = *it;
				}
				break;
			default:
				break;
			}
			lastC = -1;
			continue;
		}


		switch (c)
		{
		case DEL:
			if (cursor_X > cursor_X_Start) {
				if (cursor_X - cursor_X_Start >= dataLine.size()) {
					cursor_X--;
					gotoxy(cursor_X, cursor_Y);
					_putch(0);
					gotoxy(cursor_X, cursor_Y);
					dataLine = dataLine.substr(0, dataLine.length() - 1);
				}
				else {

					string partA = dataLine.substr(0, cursor_X - cursor_X_Start);
					string partB = dataLine.substr(cursor_X - cursor_X_Start, dataLine.length());
					cursor_X--;
					gotoxy(cursor_X, cursor_Y);
					cout << partB << " ";
					gotoxy(cursor_X, cursor_Y);
					dataLine = dataLine.substr(0, partA.length() - 1) + partB;
				}

			}
			break;
		case CR:
			if (allData.size() != line && cursor_Y == line) 	allData.push_back(dataLine);
			if (cursor_Y != line) {

				auto it = next(allData.begin(), cursor_Y - cursor_Y_Start);
				*it = dataLine;
			}
			cursor_Y++;
			if (cursor_Y - cursor_Y_Start == line) {
				dataLine = "";
				line++;
				gotoxy(cursor_X, cursor_Y);
				addPrefix();
			}
			else gotoxy(cursor_X, cursor_Y);
		case AR:
			break;
		default:
			if (c == 9) c = 32;
			if (cursor_X - cursor_X_Start == dataLine.length()) {
				cursor_X++;
				_putch(c);
				dataLine.append(string(1, c));
			}
			else {

				string partA = dataLine.substr(0, cursor_X - cursor_X_Start);
				string partB = dataLine.substr(cursor_X - cursor_X_Start, dataLine.length());
				cursor_X++;
				_putch(c);
				cout << partB << " ";
				dataLine = partA + string(1, c) + partB;
			}

			gotoxy(cursor_X, cursor_Y);
			break;
		}
		lastC = c;
	}
}

int main(int argc,char *argv[])
{
	system("cls");
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
	if (argc == 1) {
		//new file
		cursor_X = cursor_X_Start;
		cursor_Y = cursor_Y_Start;
		std::cout << (fileName == "" ? "Untitled" : fileName) << endl;
		addPrefix();
		editor();
	}
	else {
		//open file
		open(string(argv[1]));
		addPrefix();
		editor();
	}
	
	
}

