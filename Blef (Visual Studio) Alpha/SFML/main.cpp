#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <ctime>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iostream>
#include <codecvt>

#define M_PI       3.14159265358979323846
using namespace sf;
using namespace std;

RenderWindow window;
double scale, rScale;
Sprite cards[52], backCards[52], table, back, rightSpr, MMback, MMleft, MMright;
Texture tex[52], Table, Back, RightTex, LeftOn, LeftOff, LeftD, LeftH, RightOn, RightOff, RightD, RightH;
Texture CheckOn, CheckOff, CheckH, MMOn, MMOff, MMH, MMBack, MMLeft, MMRight, MMD;
int players = 0;
int beginCard = 0;
int endCard = 0;
int numPlayers=0;
int actualPlayer = 0;
bool isBegin=true;
int numCards = 13;
vector<wstring> playerName;
vector<vector<int>> card;
vector<int>cardNumber;
vector<int>isComputer; //0 - human player, more - computer player
Font font;
vector<Text> playerNames;
Text maxCardText, continueText, checkText, totalNumCardText;
Text myText[25];
wstring optionName = L"Options.txt";
int cardsPlay;

enum contract_type
{
	checkContract = -100,
	undefined_type = -1,
	high_card=0,
	a_pair=1,
	two_pairs=2,
	three=3,
	street=4,
	colour=5,
	full_house=6,
	four=7,
	flushh=8
};
enum contract_colour
{
	undefined_colour = -1,
	club=0,
	diamond=1,
	heart=2,
	spade=3
};

enum Button_isActive
{
	on, off, highlighted, disabled, crossed, blocked
};

const bool mySort(const int &i, const int &j)
{
	if (i / 13 > j / 13 && (i / 13 != 3 || j / 13 != 2))
		return false;
	if (i / 13 > j / 13 && i / 13 == 3 && j / 13 == 2)
		return true;
	if (j / 13 > i / 13 && (j / 13 != 3 || i / 13 != 2))
		return true;
	if (j / 13 > i / 13 && j / 13 == 3 && i / 13 == 2)
		return false;
	if (i < j && i % 13 != 0)
		return false;
	if (i > j && j % 13 != 0)
		return true;
	if (i % 13 == 0)
		return true;
	if (j % 13 == 0)
		return false;
	return false;
}

const bool fictSort(const int &i, const int &j)
{
	if ((i+12) % 13 != (j+12) % 13)
		return (i+12) % 13 > (j+12) % 13;
	return i > j;
}

/*wstring to_wstring(const string& s)
{
	wstring tempr(s.length(), L' ');
	copy(s.begin(), s.end(), tempr.begin());
	return temp;
}

string to_string(const wstring& s)
{
	string tempr(s.length(), ' ');
	copy(s.begin(), s.end(), tempr.begin());
	return tempr;
}*/

bool contains(wstring mainString, wchar_t b)
{
	for (int i = 0;i < mainString.size();i++)
	{
		if (wchar_t(mainString[i]) == b)
			return true;
	}
	return false;
}

struct Contract
{
	
	contract_type type;
	int height1;
	int height2;
	contract_colour color;
	Text tekst;
	bool fullTen;
	Contract()
	{
		type = high_card;
		height1 = 0;
		height2 = 0;
		color = club;
		fullTen = false;
	}
	void clear()
	{
		type = undefined_type;
		height1 = 0;
		height2 = 0;
		color = undefined_colour;
		tekst.setFillColor(Color::Black);
		tekst.setString("");
	}
	wstring height7(int a, int pom=0)
	{
		if (a < 2 && pom == 0)
			return L"X";
		if (a < 2 && pom == 1)
			return L"Y";
		if (a < 2 && pom == 2)
			return L"Z";
		if (a < 2 && pom == 3)
			return L"V";
		if (a < 1 && pom == 4)
			return L"W";
		if (a == 1 && pom == 4)
			return L"A";
		if (a == 2)
			return L"2";
		if (a == 3)
			return L"3";
		if (a == 4)
			return L"4";
		if (a == 5)
			return L"5";
		if (a == 6)
			return L"6";
		if (a == 7)
			return L"7";
		if (a == 8)
			return L"8";
		if (a == 9)
			return L"9";
		if (a == 10)
			if (fullTen)
				return L"10";
			else
				return L"0";
		if (a == 11)
			return L"J";
		if (a == 12)
			return L"Q";
		if (a == 13)
			return L"K";
		return L"A";
	}
	wstring color15(int a)
	{
		switch (a)
		{
		case 0:
			return L"♣";
		case 1:
			return L"♦";
		case 2:
			return L"♥";
		case 3:
			return L"♠";
		}
		return L"O";
	}
	void actualize()
	{
		switch (type)
		{
		case checkContract:
			tekst.setString(L"CHECK");
			break;
		case undefined_type:
			tekst.setString("TURN");
			break;
		case high_card:
			tekst.setString(height7(height1, 0));
			break;
		case a_pair:
			tekst.setString(height7(height1) + height7(height1));
			break;
		case two_pairs:
			tekst.setString(height7(height1) + height7(height1) + height7(height2, 1) + height7(height2, 1));
			break;
		case three:
			tekst.setString(height7(height1) + height7(height1) + height7(height1));
			break;
		case street:
			tekst.setString(height7(height1) + height7(height1 - 1, 1) + height7(height1 - 2, 2) + height7(height1 - 3, 3) + height7(height1 - 4, 4));
			break;
		case colour:
			tekst.setString(color15(color) + color15(color) + color15(color) + color15(color) + color15(color));
			break;
		case full_house:
			tekst.setString(height7(height1) + height7(height1) + height7(height1) + height7(height2, 1) + height7(height2, 1));
			break;
		case four:
			tekst.setString(height7(height1) + height7(height1) + height7(height1) + height7(height1));
			break;
		case flushh:
			tekst.setString(height7(height2) + height7(height2 - 1,1) + height7(height2 - 2,2) + height7(height2 - 3,3) + height7(height2 - 4,4) + color15(color));
			break;
		}
		if (contains(tekst.getString().toWideString(), L'♦') || contains(tekst.getString().toWideString(), L'♥'))
		{
			if (tekst.getFillColor() == Color::Black || tekst.getFillColor() == Color::Red)
				tekst.setFillColor(Color::Red);
			else
				tekst.setFillColor(Color::Yellow);
		}
		else if (tekst.getFillColor() == Color::Yellow)
			tekst.setFillColor(Color::Green);
	}
	void operator = (Contract other)
	{
		type = other.type;
		height1 = other.height1;
		height2 = other.height2;
		color = other.color;
	}
};

struct Button
{
	Sprite onn, of, cross, highL;
	string name;
	Button_isActive isActive;
	int pom;
	Text tekst;
	int xSize, ySize;
	Button()
	{
		name = "";
		isActive = off;
	}
	bool isClicked(Event event)
	{
		Vector2i localPosition = Mouse::getPosition(window);
		Vector2f up_left = onn.getPosition();
		if (localPosition.x >= up_left.x && localPosition.y >= up_left.y && localPosition.x <= up_left.x + xSize && localPosition.y <= up_left.y + ySize)
		{
			if (event.type == Event::MouseButtonPressed && Mouse::isButtonPressed(Mouse::Left))
			{
				isActive = on;
				return true;
			}
			else
				isActive = highlighted;
		}
		else if (isActive == highlighted)
			isActive = off;
		return false;
	}
};

Button MMButton[5];
Contract contracts[20][3];
Contract previousContract;

int actualRound = 0;
int contractPart = 0;

Button contractTypesButton[9], checkButton, contractNrButton[2][13], contractColorButton[4], backButton, StreetsDown;

pair<string, bool> MMBegin()
{
	string debug = "";
	bool isError = false;
	// loading main menu icons
	if (true)
	{
		if (!MMOff.loadFromFile("Images/Menu/MM_off.png"))
		{
			debug += "ERROR: MM_off.png cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_off.png loaded succesfully.\n";
		MMOff.setSmooth(true);
		if (!MMOn.loadFromFile("Images/Menu/MM_on.png"))
		{
			debug += "ERROR: MM_on.png cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_on.png loaded succesfully.\n";
		MMOn.setSmooth(true);
		if (!MMH.loadFromFile("Images/Menu/MM_highlighted.png"))
		{
			debug += "ERROR: MM_highlighted cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_highlighted loaded succesfully.\n";
		MMH.setSmooth(true);
		if (!MMBack.loadFromFile("Images/Menu/MM_back.png"))
		{
			debug += "ERROR: MM_back cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_back loaded succesfully.\n";
		MMBack.setSmooth(true);
		if (!MMLeft.loadFromFile("Images/Menu/MM_left.png"))
		{
			debug += "ERROR: MM_left cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_left loaded succesfully.\n";
		MMLeft.setSmooth(true);
		if (!MMRight.loadFromFile("Images/Menu/MM_right.png"))
		{
			debug += "ERROR: MM_right cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_right loaded succesfully.\n";
		MMRight.setSmooth(true);
		if (!MMD.loadFromFile("Images/Menu/MM_cross.png"))
		{
			debug += "ERROR: MM_cross cannot be loaded!!!\n";
			isError = true;
		}
		else
			debug += "MM_cross loaded succesfully.\n";
		MMD.setSmooth(true);
	}
	//creating main menu buttons
	if (true)
	{
		for (int i = 0; i < 5; i++)
		{
			MMButton[i].of.setTexture(MMOff);
			MMButton[i].of.setScale(scale * 0.5, scale * 0.25);
			MMButton[i].of.setPosition(double(window.getSize().x) / 2.0 - 256.0 * scale, (384.0 + 72.0 * double(i)) * scale);
			MMButton[i].onn.setTexture(MMOn);
			MMButton[i].onn.setScale(MMButton[i].of.getScale());
			MMButton[i].onn.setPosition(MMButton[i].of.getPosition());
			contractTypesButton[i].cross.setTexture(MMD);
			contractTypesButton[i].cross.setScale(contractTypesButton[i].of.getScale());
			contractTypesButton[i].cross.setPosition(contractTypesButton[i].of.getPosition());
			MMButton[i].highL.setTexture(MMH);
			MMButton[i].highL.setScale(MMButton[i].of.getScale());
			MMButton[i].highL.setPosition(MMButton[i].of.getPosition());
			MMButton[i].isActive = off;
			MMButton[i].xSize = 256.0 * scale;
			MMButton[i].ySize = 64.0 * scale;
			MMButton[i].tekst.setFont(font); 
			MMButton[i].tekst.setCharacterSize(scale * 24.0);
			MMButton[i].tekst.setFillColor(Color::Black);
			MMButton[i].tekst.setStyle(Text::Regular);
			MMButton[i].tekst.setPosition(MMButton[i].of.getPosition().x + 64.0 * rScale, MMButton[i].of.getPosition().y);
		}
		MMButton[0].tekst.setString(L"One player");
		MMButton[1].tekst.setString(L"-----------"); //Multiplayer
		MMButton[2].tekst.setString(L"Load game");
		MMButton[3].tekst.setString(L"Settings");
		MMButton[4].tekst.setString(L"Exit");
	}
	//creating main menu background
	if (true)
	{
		MMback.setTexture(MMBack);
		MMback.setPosition(rScale * 128.0, 0.0);
		MMback.setScale(scale * 0.5, scale * 0.5);
		MMleft.setTexture(MMLeft);
		MMleft.setPosition(0.0, 0.0);
		MMleft.setScale(rScale * 0.5, scale * 0.5);
		MMright.setTexture(MMRight);
		MMright.setPosition(rScale * 128.0 + scale * 768.0, 0.0);
		MMright.setScale(rScale * 0.5, scale * 0.5);
	}
	pair<string, bool> retValue;
	retValue.first = debug;
	retValue.second = isError;
	return retValue;
}

void shuffleCards()
{
	card.clear();
	int tab139[52];
	for (int i = 0;i < 60;i++)
		contracts[i / 3][i % 3].clear();
	for (int i = 0;i < 52;i++)
	{
		tab139[i] = i;
		cards[i].setPosition(352 * scale, 344 * scale);
		backCards[i].setPosition(352 * scale, 344 * scale);
	}
	sort(tab139, tab139 + 52, fictSort);
	for (int i = 0;i < 52;i++)
		wcout << tab139[i] << L" ";
	wcout << L"\n";
	random_shuffle(tab139, tab139 + 4 * numCards);
	int pom13 = 0;
	for (int i = 0; i < players; i++)
	{
		vector<int> ab;
		if (cardNumber[i] < endCard)
		{
			for (int j = 0;j < cardNumber[i]; j++)
			{
				double alpha = 2 * (double)i / (double)players * M_PI;
				cards[tab139[pom13]].setPosition(352.0 * scale - 234.0 * scale * sin(alpha), 344.0 * scale + 234.0 * scale * cos(alpha));
				backCards[tab139[pom13]].setPosition(352.0 * scale - 234.0 * scale * sin(alpha), 344.0 * scale + 234.0 * scale * cos(alpha));
				ab.push_back(tab139[pom13]);
				pom13++;
			}
			sort(ab.begin(), ab.end(), mySort);
			for (int j = 0;j < ab.size();j++)
			{
				cards[ab[j]].move((-16.0 * (double)cardNumber[i] + 32.0 * (double)j) * scale, 0);
				backCards[ab[j]].move((-16.0 * (double)cardNumber[i] + 32.0 * (double)j) * scale, 0);
			}
			
		}
		else
			ab.push_back(-1);
		card.push_back(ab);
	}
}

bool CardBegin()
{
	for (int i = 0;i < 52;i++)
		tex[i].setSmooth(true);
	if (!tex[0].loadFromFile("Images/Cards/AS.png"))
		return false;
	cards[0].setTexture(tex[0]);
	if (!tex[1].loadFromFile("Images/Cards/2S.png"))
		return false;
	cards[1].setTexture(tex[1]);
	if (!tex[2].loadFromFile("Images/Cards/3S.png"))
		return false;
	cards[2].setTexture(tex[2]);
	if (!tex[3].loadFromFile("Images/Cards/4S.png"))
		return false;
	cards[3].setTexture(tex[3]);
	if (!tex[4].loadFromFile("Images/Cards/5S.png"))
		return false;
	cards[4].setTexture(tex[4]);
	if (!tex[5].loadFromFile("Images/Cards/6S.png"))
		return false;
	cards[5].setTexture(tex[5]);
	if (!tex[6].loadFromFile("Images/Cards/7S.png"))
		return false;
	cards[6].setTexture(tex[6]);
	if (!tex[7].loadFromFile("Images/Cards/8S.png"))
		return false;
	cards[7].setTexture(tex[7]);
	if (!tex[8].loadFromFile("Images/Cards/9S.png"))
		return false;
	cards[8].setTexture(tex[8]);
	if (!tex[9].loadFromFile("Images/Cards/10S.png"))
		return false;
	cards[9].setTexture(tex[9]);
	if (!tex[10].loadFromFile("Images/Cards/JS.png"))
		return false;
	cards[10].setTexture(tex[10]);
	if (!tex[11].loadFromFile("Images/Cards/QS.png"))
		return false;
	cards[11].setTexture(tex[11]);
	if (!tex[12].loadFromFile("Images/Cards/KS.png"))
		return false;
	cards[12].setTexture(tex[12]);
	
	if (!tex[13].loadFromFile("Images/Cards/AH.png"))
		return false;
	cards[13].setTexture(tex[13]);
	if (!tex[14].loadFromFile("Images/Cards/2H.png"))
		return false;
	cards[14].setTexture(tex[14]);
	if (!tex[15].loadFromFile("Images/Cards/3H.png"))
		return false;
	cards[15].setTexture(tex[15]);
	if (!tex[16].loadFromFile("Images/Cards/4H.png"))
		return false;
	cards[16].setTexture(tex[16]);
	if (!tex[17].loadFromFile("Images/Cards/5H.png"))
		return false;
	cards[17].setTexture(tex[17]);
	if (!tex[18].loadFromFile("Images/Cards/6H.png"))
		return false;
	cards[18].setTexture(tex[18]);
	if (!tex[19].loadFromFile("Images/Cards/7H.png"))
		return false;
	cards[19].setTexture(tex[19]);
	if (!tex[20].loadFromFile("Images/Cards/8H.png"))
		return false;
	cards[20].setTexture(tex[20]);
	if (!tex[21].loadFromFile("Images/Cards/9H.png"))
		return false;
	cards[21].setTexture(tex[21]);
	if (!tex[22].loadFromFile("Images/Cards/10H.png"))
		return false;
	cards[22].setTexture(tex[22]);
	if (!tex[23].loadFromFile("Images/Cards/JH.png"))
		return false;
	cards[23].setTexture(tex[23]);
	if (!tex[24].loadFromFile("Images/Cards/QH.png"))
		return false;
	cards[24].setTexture(tex[24]);
	if (!tex[25].loadFromFile("Images/Cards/KH.png"))
		return false;
	cards[25].setTexture(tex[25]);

	if (!tex[26].loadFromFile("Images/Cards/AD.png"))
		return false;
	cards[26].setTexture(tex[26]);
	if (!tex[27].loadFromFile("Images/Cards/2D.png"))
		return false;
	cards[27].setTexture(tex[27]);
	if (!tex[28].loadFromFile("Images/Cards/3D.png"))
		return false;
	cards[28].setTexture(tex[28]);
	if (!tex[29].loadFromFile("Images/Cards/4D.png"))
		return false;
	cards[29].setTexture(tex[29]);
	if (!tex[30].loadFromFile("Images/Cards/5D.png"))
		return false;
	cards[30].setTexture(tex[30]);
	if (!tex[31].loadFromFile("Images/Cards/6D.png"))
		return false;
	cards[31].setTexture(tex[31]);
	if (!tex[32].loadFromFile("Images/Cards/7D.png"))
		return false;
	cards[32].setTexture(tex[32]);
	if (!tex[33].loadFromFile("Images/Cards/8D.png"))
		return false;
	cards[33].setTexture(tex[33]);
	if (!tex[34].loadFromFile("Images/Cards/9D.png"))
		return false;
	cards[34].setTexture(tex[34]);
	if (!tex[35].loadFromFile("Images/Cards/10D.png"))
		return false;
	cards[35].setTexture(tex[35]);
	if (!tex[36].loadFromFile("Images/Cards/JD.png"))
		return false;
	cards[36].setTexture(tex[36]);
	if (!tex[37].loadFromFile("Images/Cards/QD.png"))
		return false;
	cards[37].setTexture(tex[37]);
	if (!tex[38].loadFromFile("Images/Cards/KD.png"))
		return false;
	cards[38].setTexture(tex[38]);

	if (!tex[39].loadFromFile("Images/Cards/AC.png"))
		return false;
	cards[39].setTexture(tex[39]);
	if (!tex[40].loadFromFile("Images/Cards/2C.png"))
		return false;
	cards[40].setTexture(tex[40]);
	if (!tex[41].loadFromFile("Images/Cards/3C.png"))
		return false;
	cards[41].setTexture(tex[41]);
	if (!tex[42].loadFromFile("Images/Cards/4C.png"))
		return false;
	cards[42].setTexture(tex[42]);
	if (!tex[43].loadFromFile("Images/Cards/5C.png"))
		return false;
	cards[43].setTexture(tex[43]);
	if (!tex[44].loadFromFile("Images/Cards/6C.png"))
		return false;
	cards[44].setTexture(tex[44]);
	if (!tex[45].loadFromFile("Images/Cards/7C.png"))
		return false;
	cards[45].setTexture(tex[45]);
	if (!tex[46].loadFromFile("Images/Cards/8C.png"))
		return false;
	cards[46].setTexture(tex[46]);
	if (!tex[47].loadFromFile("Images/Cards/9C.png"))
		return false;
	cards[47].setTexture(tex[47]);
	if (!tex[48].loadFromFile("Images/Cards/10C.png"))
		return false;
	cards[48].setTexture(tex[48]);
	if (!tex[49].loadFromFile("Images/Cards/JC.png"))
		return false;
	cards[49].setTexture(tex[49]);
	if (!tex[50].loadFromFile("Images/Cards/QC.png"))
		return false;
	cards[50].setTexture(tex[50]);
	if (!tex[51].loadFromFile("Images/Cards/KC.png"))
		return false;
	cards[51].setTexture(tex[51]);
	return true;
}

bool Begin()
{
	srand(time(0));
	fstream optionFile;
	string debug="";
	bool isError = false;
	//creating window
	if (true)
	{
		int ScreenX = 1024, ScreenY = 768;
		bool isFullScreen = false;
		optionFile.open(optionName, ios::in);
		if (!optionFile.good())
		{
			isError = true;
			debug += "ERROR: Options.txt cannot be loaded!!!\n";
		}
		else
		{
			debug += "Options.txt loaded succesfully.\n";
			string pomOpt;
			optionFile >> ScreenX >> ScreenY >> pomOpt;
			if (pomOpt == "Fullscreen:_YES")
				isFullScreen = true;
		}
		optionFile.close();
		if (isFullScreen)
			window.create(VideoMode(ScreenX, ScreenY, 32), "Blef", Style::Fullscreen);
		else
			window.create(VideoMode(ScreenX, ScreenY, 32), "Blef");
		window.setFramerateLimit(60);
		if (window.getSize().x < window.getSize().y)
			scale = window.getSize().x / 768.0;
		else
			scale = window.getSize().y / 768.0;
	}
	//loading icons
	if (true) 
	{
		if (!Table.loadFromFile("Images/Table.png"))
		{
			isError = true;
			debug += "ERROR: Table.png cannot be loaded!!!\n";
		}
		else
			debug += "Table.png loaded succesfully.\n";
		Table.setSmooth(true);
		table.setTexture(Table);
		table.setPosition(0, 0);
		table.setScale(scale * 0.5, scale * 0.5);
		if (!Back.loadFromFile("Images/Back.png"))
		{
			isError = true;
			debug += "ERROR: Back.png cannot be loaded!!!\n";
		}
		else
			debug += "Back.png loaded succesfully.\n";
		Back.setSmooth(true);
		back.setTexture(Back);
		if (!LeftD.loadFromFile("Images/Icons/Left_cross.png"))
		{
			isError = true;
			debug += "ERROR: Left_cross.png cannot be loaded!!!\n";
		}
		else
			debug += "Left_cross.png loaded succesfully.\n";
		LeftD.setSmooth(true);
		if (!RightD.loadFromFile("Images/Icons/Right_cross.png"))
		{
			isError = true;
			debug += "ERROR: Right_cross.png cannot be loaded!!!\n";
		}
		else
			debug += "Right_cross.png loaded succesfully.\n";
		RightD.setSmooth(true);
		if (!LeftH.loadFromFile("Images/Icons/Left_highlighted.png"))
		{
			isError = true;
			debug += "ERROR: Left_highlighted.png cannot be loaded!!!\n";
		}
		else
			debug += "Left_highlighted.png loaded succesfully.\n";
		LeftH.setSmooth(true);
		if (!RightH.loadFromFile("Images/Icons/Right_highlighted.png"))
		{
			isError = true;
			debug += "ERROR: Right_highlighted.png cannot be loaded!!!\n";
		}
		else
			debug += "Right_highlighted.png loaded succesfully.\n";
		RightH.setSmooth(true);
		if (!CheckH.loadFromFile("Images/Icons/Check_highlighted.png"))
		{
			isError = true;
			debug += "ERROR: Check_highlighted.png cannot be loaded!!!\n";
		}
		else
			debug += "Check_highlighted.png loaded succesfully.\n";
		CheckH.setSmooth(true);
		if (!LeftOn.loadFromFile("Images/Icons/Left_on.png"))
		{
			isError = true;
			debug += "ERROR: Left_on.png cannot be loaded!!!\n";
		}
		else
			debug += "Left_on.png loaded succesfully.\n";
		LeftOn.setSmooth(true);
		if (!LeftOff.loadFromFile("Images/Icons/Left_off.png"))
		{
			isError = true;
			debug += "ERROR: Left_off.png cannot be loaded!!!\n";
		}
		else
			debug += "Left_off.png loaded succesfully.\n";
		LeftOff.setSmooth(true);
		if (!RightOn.loadFromFile("Images/Icons/Right_on.png"))
		{
			isError = true;
			debug += "ERROR: Right_on.png cannot be loaded!!!\n";
		}
		else
			debug += "Right_on.png loaded succesfully.\n";
		RightOn.setSmooth(true);
		if (!RightOff.loadFromFile("Images/Icons/Right_off.png"))
		{
			isError = true;
			debug += "ERROR: Right_off.png cannot be loaded!!!\n";
		}
		else
			debug += "Right_off.png loaded succesfully.\n";
		CheckOff.setSmooth(true);
		if (!CheckOn.loadFromFile("Images/Icons/Check_on.png"))
		{
			isError = true;
			debug += "ERROR: Check_on.png cannot be loaded!!!\n";
		}
		else
			debug += "Check_on.png loaded succesfully.\n";
		CheckOn.setSmooth(true);
		if (!CheckOff.loadFromFile("Images/Icons/Check_off.png"))
		{
			isError = true;
			debug += "ERROR: Check_off.png cannot be loaded!!!\n";
		}
		else
			debug += "Check_off.png loaded succesfully.\n";
		CheckOff.setSmooth(true);
		for (int i = 0;i < 52;i++)
		{
			backCards[i].setTexture(Back);
			backCards[i].setScale(scale * 0.8, scale * 0.8);
			backCards[i].setPosition(352.0 * scale, 344.0 * scale);
		}
		if (!CardBegin())
		{
			isError = true;
			debug += "ERROR: Cards cannot be loaded!!!\n";
		}
		else
			debug += "Cards loaded succesfully.\n";
		back.setPosition(352.0 * scale, 344.0 * scale);
		back.setScale(scale * 0.8, scale * 0.8);
		for (int i = 0;i < 52;i++)
		{
			cards[i].setScale(scale * 0.8, scale * 0.8);
			cards[i].setPosition(344.0 * scale * 0.8, 334.0 * scale * 0.8);
		}
		if (!RightTex.loadFromFile("Images/Right.png"))
		{
			isError = true;
			debug += "ERROR: Right.png cannot be loaded!!!\n";
		}
		else
			debug += "Right.png loaded succesfully.\n";
		rightSpr.setTexture(RightTex);
		rightSpr.setPosition(768.0 * scale, 0);
		rScale = double(window.getSize().x - window.getSize().y) / 256.0;
		rightSpr.setScale(rScale * 0.5, scale * 0.5);
	}
	//loading main menu
	if (true)
	{
		pair<string, bool> apd = MMBegin();
		if(apd.second)
			isError = true;
		debug += apd.first;
	}
	//loading fonts
	if (true)
	{
		if (!font.loadFromFile("mainFont.ttf"))
		{
			isError = true;
			debug += "ERROR: mainFont.ttf doesn't exist!!!\n";
		}
		else
			debug += "mainFont.ttf loaded succesfully\n";
	}
	//creating buttons
	if (true)
	{
		for (int i = 0;i < 9;i++)
		{
			contractTypesButton[i].of.setTexture(LeftOff);
			contractTypesButton[i].of.setScale(rScale * 0.5, scale * 0.5);
			contractTypesButton[i].of.setPosition(768.0 * scale + 16.0 * rScale, (671.0 - 37.0 * double(i + 1)) * scale);
			contractTypesButton[i].onn.setTexture(LeftOn);
			contractTypesButton[i].onn.setScale(contractTypesButton[i].of.getScale());
			contractTypesButton[i].onn.setPosition(contractTypesButton[i].of.getPosition());
			contractTypesButton[i].cross.setTexture(LeftD);
			contractTypesButton[i].cross.setScale(contractTypesButton[i].of.getScale());
			contractTypesButton[i].cross.setPosition(contractTypesButton[i].of.getPosition());
			contractTypesButton[i].highL.setTexture(LeftH);
			contractTypesButton[i].highL.setScale(contractTypesButton[i].of.getScale());
			contractTypesButton[i].highL.setPosition(contractTypesButton[i].of.getPosition());
			contractTypesButton[i].isActive = off;
			contractTypesButton[i].pom = i;
			contractTypesButton[i].xSize = 128.0 * rScale;
			contractTypesButton[i].ySize = 32.0 * scale;
			contractTypesButton[i].tekst.setFont(font);
			contractTypesButton[i].tekst.setCharacterSize(scale * 24.0);
			contractTypesButton[i].tekst.setFillColor(Color::Black);
			contractTypesButton[i].tekst.setStyle(Text::Regular);
			contractTypesButton[i].tekst.setPosition(contractTypesButton[i].of.getPosition().x + 64.0 * (rScale - scale), contractTypesButton[i].of.getPosition().y);
		}
		contractTypesButton[0].tekst.setString("High card");
		contractTypesButton[1].tekst.setString("Pair");
		contractTypesButton[2].tekst.setString("2 pairs");
		contractTypesButton[3].tekst.setString("3 of a kind");
		contractTypesButton[4].tekst.setString("Street");
		contractTypesButton[5].tekst.setString("Color");
		contractTypesButton[6].tekst.setString("Full house");
		contractTypesButton[7].tekst.setString("4 of a kind");
		contractTypesButton[8].tekst.setString("Flush");

		checkButton.of.setTexture(CheckOff);
		checkButton.of.setScale(rScale * 0.5, scale * 0.5);
		checkButton.of.setPosition(768.0 * scale + 16.0 * rScale, 721.0 * scale);
		checkButton.onn.setTexture(CheckOn);
		checkButton.onn.setScale(checkButton.of.getScale());
		checkButton.onn.setPosition(checkButton.of.getPosition());
		checkButton.highL.setTexture(CheckH);
		checkButton.highL.setScale(checkButton.of.getScale());
		checkButton.highL.setPosition(checkButton.of.getPosition());
		checkButton.isActive = disabled;
		checkButton.xSize = 128.0 * rScale;
		checkButton.ySize = 32.0 * scale;
		checkButton.tekst.setFont(font);
		checkButton.tekst.setCharacterSize(scale * 24.0);
		checkButton.tekst.setFillColor(Color::Black);
		checkButton.tekst.setStyle(Text::Regular);
		checkButton.tekst.setString("CHECK");
		checkButton.tekst.setPosition(checkButton.of.getPosition().x + 64.0 * (rScale - scale), checkButton.of.getPosition().y);

		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 13;j++)
			{
				contractNrButton[i][j].of.setTexture(RightOff);
				contractNrButton[i][j].of.setScale(rScale * 0.5, scale * 0.5);
				contractNrButton[i][j].of.setPosition(768.0 * scale + (160.0 + 48.0 * double(i)) * rScale, (734.0 - 36.0 * double(j)) * scale);
				contractNrButton[i][j].onn.setTexture(RightOn);
				contractNrButton[i][j].onn.setScale(contractNrButton[i][j].of.getScale());
				contractNrButton[i][j].onn.setPosition(contractNrButton[i][j].of.getPosition());
				contractNrButton[i][j].cross.setTexture(RightD);
				contractNrButton[i][j].cross.setScale(contractNrButton[i][j].of.getScale());
				contractNrButton[i][j].cross.setPosition(contractNrButton[i][j].of.getPosition());
				contractNrButton[i][j].highL.setTexture(RightH);
				contractNrButton[i][j].highL.setScale(contractNrButton[i][j].of.getScale());
				contractNrButton[i][j].highL.setPosition(contractNrButton[i][j].of.getPosition());
				contractNrButton[i][j].isActive = disabled;
				contractNrButton[i][j].pom = j + 2;
				contractNrButton[i][j].xSize = 32.0 * rScale;
				contractNrButton[i][j].ySize = 32.0 * scale;
				contractNrButton[i][j].tekst.setFont(font);
				contractNrButton[i][j].tekst.setCharacterSize(scale * 24.0);
				contractNrButton[i][j].tekst.setFillColor(Color::Black);
				contractNrButton[i][j].tekst.setStyle(Text::Regular);
				contractNrButton[i][j].tekst.setPosition(contractNrButton[i][j].of.getPosition().x + 16.0 * (rScale - scale), contractNrButton[i][j].of.getPosition().y);
				if (j == 8)
					contractNrButton[i][j].tekst.setString("10");
				else
					contractNrButton[i][j].tekst.setString(char('0' + (j + 2) % 10));
			}
			contractNrButton[i][9].tekst.setString("J");
			contractNrButton[i][10].tekst.setString("Q");
			contractNrButton[i][11].tekst.setString("K");
			contractNrButton[i][12].tekst.setString("A");
		}
		for (int i = 0;i < 4;i++)
		{
			contractColorButton[i].of.setTexture(RightOff);
			contractColorButton[i].of.setScale(rScale * 0.5, scale * 0.5);
			contractColorButton[i].of.setPosition(768.0 * scale + 160.0 * rScale, (734.0 - 36.0 * double(i + 9)) * scale);
			contractColorButton[i].onn.setTexture(RightOn);
			contractColorButton[i].onn.setScale(contractColorButton[i].of.getScale());
			contractColorButton[i].onn.setPosition(contractColorButton[i].of.getPosition());
			contractColorButton[i].cross.setTexture(RightD);
			contractColorButton[i].cross.setScale(contractColorButton[i].of.getScale());
			contractColorButton[i].cross.setPosition(contractColorButton[i].of.getPosition());
			contractColorButton[i].highL.setTexture(RightH);
			contractColorButton[i].highL.setScale(contractColorButton[i].of.getScale());
			contractColorButton[i].highL.setPosition(contractColorButton[i].of.getPosition());
			contractColorButton[i].isActive = disabled;
			contractColorButton[i].pom = i;
			contractColorButton[i].xSize = 32.0 * rScale;
			contractColorButton[i].ySize = 32.0 * scale;
			contractColorButton[i].tekst.setFont(font);
			contractColorButton[i].tekst.setCharacterSize(scale * 24.0);
			if (i % 3 == 0)
				contractColorButton[i].tekst.setFillColor(Color::Black);
			else
				contractColorButton[i].tekst.setFillColor(Color::Red);
			contractColorButton[i].tekst.setStyle(Text::Regular);
			contractColorButton[i].tekst.setPosition(contractColorButton[i].of.getPosition().x + 16.0 * (rScale - scale), contractColorButton[i].of.getPosition().y);
		}
		contractColorButton[0].tekst.setString(L"♣");
		contractColorButton[1].tekst.setString(L"♦");
		contractColorButton[2].tekst.setString(L"♥");
		contractColorButton[3].tekst.setString(L"♠");

		backButton.of.setTexture(RightOff);
		backButton.of.setScale(rScale * 0.5, scale * 0.5);
		backButton.of.setPosition(768.0 * scale + 16.0 * rScale, 684.0 * scale);
		backButton.onn.setTexture(RightOn);
		backButton.onn.setScale(backButton.of.getScale());
		backButton.onn.setPosition(backButton.of.getPosition());
		backButton.highL.setTexture(RightH);
		backButton.highL.setScale(backButton.of.getScale());
		backButton.highL.setPosition(backButton.of.getPosition());
		backButton.isActive = disabled;
		backButton.xSize = 32.0 * rScale;
		backButton.ySize = 32.0 * scale;
		backButton.tekst.setFont(font);
		backButton.tekst.setCharacterSize(scale * 24.0);
		backButton.tekst.setFillColor(Color::Black);
		backButton.tekst.setStyle(Text::Regular);
		backButton.tekst.setString(L"←");
		backButton.tekst.setPosition(backButton.of.getPosition().x + 16.0 * (rScale - scale), backButton.of.getPosition().y);

		StreetsDown.of.setTexture(RightOn);
		StreetsDown.of.setScale(rScale * 0.5, scale * 0.5);
		StreetsDown.of.setPosition(768.0 * scale + 208.0 * rScale, 698.0 * scale);
		StreetsDown.isActive = disabled;
		StreetsDown.xSize = 32.0 * rScale;
		StreetsDown.ySize = 32.0 * scale;
		StreetsDown.tekst.setFont(font);
		StreetsDown.tekst.setCharacterSize(scale * 24.0);
		StreetsDown.tekst.setFillColor(Color::Black);
		StreetsDown.tekst.setStyle(Text::Regular);
		StreetsDown.tekst.setString(L"↓");
		StreetsDown.tekst.setPosition(StreetsDown.of.getPosition().x + 16.0 * (rScale - scale), StreetsDown.of.getPosition().y);
	}
	//creating bidding schema
	if (true)
	{
		for (int i = 0;i < 20;i++)
		{
			for (int j = 0; j < 3;j++)
			{
				contracts[i][j].tekst.setPosition(768.0 * scale + (95.0 + 54.0 * double(j)) * rScale, scale * (42.0 + 26.0 * double(i)));
				contracts[i][j].tekst.setFont(font);
				contracts[i][j].tekst.setCharacterSize(24.0 * scale);
				contracts[i][j].tekst.setFillColor(Color::Black);
				contracts[i][j].type = undefined_type;
				contracts[i][j].color = undefined_colour;
				contracts[i][j].height1 = 0;
				contracts[i][j].height2 = 0;
				if (double(window.getSize().x) / double(window.getSize().y) >= 1.65)
					contracts[i][j].fullTen = true;
			}
		}
		previousContract.clear();
	}
	//creating texts
	if (true)
	{
		maxCardText.setPosition(20.0 * scale, 15.0 * scale);
		maxCardText.setFillColor(Color::Color(127, 0, 0));
		maxCardText.setFont(font);
		maxCardText.setCharacterSize(30.0 * scale);
		totalNumCardText.setPosition(20.0 * scale, 50.0 * scale);
		totalNumCardText.setFillColor(Color::Black);
		totalNumCardText.setFont(font);
		totalNumCardText.setCharacterSize(30.0 * scale);
		continueText.setPosition(0.0, 324.0 * scale);
		continueText.setFillColor(Color(0, 128, 0));
		continueText.setFont(font);
		continueText.setCharacterSize(60.0 * scale);
		checkText.setPosition(0.0, 324.0 * scale);
		checkText.setFillColor(Color(0, 128, 0));
		checkText.setFont(font);
		checkText.setCharacterSize(60.0 * scale);
	}

	fstream debugFile;
	debugFile.open("Debug.txt", ios::out);
	debugFile << debug;
	debugFile.close();
	return isError;
}

void WinCheck()
{
	contracts[actualPlayer][actualRound].actualize();
	cardsPlay++;
	totalNumCardText.setString(L"Number of cards: " + to_wstring(cardsPlay) + L" of " + to_wstring(numCards * 4));
	do
	{
		actualPlayer--;
		if(actualPlayer < 0)
			actualPlayer+=players;
	} while (cardNumber[actualPlayer] >= endCard);
	cardNumber[actualPlayer]++;
	if (cardNumber[actualPlayer] == endCard - 1)
		playerNames[actualPlayer].setFillColor(Color::Color(255, 127, 0));
	if (cardNumber[actualPlayer] == endCard - 2)
		playerNames[actualPlayer].setFillColor(Color::Color(255, 255, 0));
	if (cardNumber[actualPlayer] >= endCard)
	{
		cardsPlay -= endCard;
		totalNumCardText.setString(L"Number of cards: " + to_wstring(cardsPlay) + L" of " + to_wstring(numCards * 4));
		numPlayers--;
		playerNames[actualPlayer].setFillColor(sf::Color::Red);
		do
		{
			actualPlayer++;
			if (actualPlayer >= players)
				actualPlayer -= players;
		} while (cardNumber[actualPlayer] >= endCard);
	}
	shuffleCards();
	actualRound = 0;
	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 13 - numCards; j++)
		{
			contractNrButton[i][j].isActive = blocked;
		}
		for (int j = 13 - numCards;j < 13;j++)
		{
			contractNrButton[i][j].isActive = disabled;
		}
	}
	for (int i = 0;i < 4;i++)
	{
		contractColorButton[i].isActive = disabled;
	}
	for (int i = 0;i < 9;i++)
		contractTypesButton[i].isActive = off;
	isBegin = true;
	checkButton.isActive = disabled;
	contractPart = 0;
	for (int i = 0;i < actualPlayer; i++)
		contracts[i][0].tekst.setString("-----");
	contracts[actualPlayer][0].tekst.setFillColor(Color::Green);
	contracts[actualPlayer][0].actualize();
	previousContract.clear();
}

void LoseCheck()
{
	contracts[actualPlayer][actualRound].actualize();
	cardNumber[actualPlayer]++;
	cardsPlay++;
	totalNumCardText.setString(L"Number of cards: " + to_wstring(cardsPlay) + L" of " + to_wstring(numCards * 4));
	if (cardNumber[actualPlayer] == endCard - 1)
		playerNames[actualPlayer].setFillColor(Color::Color(255, 127, 0));
	if (cardNumber[actualPlayer] == endCard - 2)
		playerNames[actualPlayer].setFillColor(Color::Color(255, 255, 0));
	if (cardNumber[actualPlayer] >= endCard)
	{
		numPlayers--;
		cardsPlay -= endCard;
		totalNumCardText.setString(L"Number of cards: " + to_wstring(cardsPlay) + L" of " + to_wstring(numCards * 4));
		playerNames[actualPlayer].setFillColor(sf::Color::Red);
		do
		{
			actualPlayer++;
			if (actualPlayer >= players)
				actualPlayer -= players;
		} while (cardNumber[actualPlayer] >= endCard);
	}
	shuffleCards();
	actualRound = 0;
	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 13 - numCards; j++)
		{
			contractNrButton[i][j].isActive = blocked;
		}
		for (int j = 13 - numCards;j < 13;j++)
		{
			contractNrButton[i][j].isActive = disabled;
		}
	}
	for (int i = 0;i < 4;i++)
		contractColorButton[i].isActive = disabled;
	for (int i = 0;i < 9;i++)
		contractTypesButton[i].isActive = off;
	isBegin = true;
	checkButton.isActive = disabled;
	contractPart = 0;
	for (int i = 0;i < actualPlayer; i++)
			contracts[i][0].tekst.setString("-----");
	contracts[actualPlayer][0].tekst.setFillColor(Color::Green);
	contracts[actualPlayer][0].actualize();
	previousContract.clear();
}

void Graphic2(bool areCardShown)
{
	window.clear();
	window.draw(rightSpr);
	window.draw(table);
	window.draw(back);
	window.draw(maxCardText);
	window.draw(totalNumCardText);
	for (int i = 0;i < card.size();i++)
	{
		if (cardNumber[i] < endCard)
		{
			for (int j = 0;j < card[i].size();j++)
				window.draw(backCards[card[i][j]]);
		}
	}
	if (areCardShown)
	{
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					window.draw(cards[card[i][j]]);
		window.draw(checkText);
	}
	else
		window.draw(continueText);
	for (int i = 0;i < players;i++)
		window.draw(playerNames[i]);
	//displaying bidding schema
	for (int i = 0; i < players; i++)
		for (int j = 0; j <= actualRound; j++)
			window.draw(contracts[i][j].tekst);
	window.display();
}

bool RealCheck()
{
	//true if check is good
	//false if check is wrong
	wcout << L"\n";
	for (int i = 0; i < players; i++)
	{
		if (cardNumber[i] < endCard)
		{
			wcout << playerName[i] << L"\n";
			for (int j = 0;j < card[i].size();j++)
			{
				wcout << card[i][j]%13+1 << L"." << card[i][j]/13 << L" ";
			}
			wcout << L"\n";
		}
	}
	int pom213 = 0, pom214 = 0;
	switch (previousContract.type)
	{
	case high_card:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (card[i][j] % 13 + 1 == previousContract.height1)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14)
						pom213++;
		if (pom213 >= 1)
			return true;
		break;
	case a_pair:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (card[i][j] % 13 + 1 == previousContract.height1)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14)
						pom213++;
		if (pom213 >= 2)
			return true;
		break;
	case two_pairs:
		for (int i = 0;i < players; i++)
		{
			if (cardNumber[i] < endCard)
			{
				for (int j = 0;j < card[i].size();j++)
				{
					if (card[i][j] % 13 + 1 == previousContract.height1)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14)
						pom213++;
					if (card[i][j] % 13 + 1 == previousContract.height2)
						pom214++;
					else if (card[i][j] % 13 == 0 && previousContract.height2 == 14)
						pom214++;
				}
			}
		}
		if (pom213 >= 2 && pom214 >= 2)
			return true;
		break;
	case three:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (card[i][j] % 13 + 1 == previousContract.height1)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14)
						pom213++;
		if (pom213 >= 3)
			return true;
		break;
	case street:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (card[i][j] % 13 + 1 == previousContract.height1 && pom213 % 2 == 0)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14 && pom213 % 2 == 0)
						pom213++;
					else if (card[i][j] % 13 + 1 == previousContract.height1 - 1 && (pom213 / 2) % 2 == 0)
						pom213 += 4;
					else if (card[i][j] % 13 + 1 == previousContract.height1 - 2 && (pom213 / 4) % 2 == 0)
						pom213 += 8;
					else if (card[i][j] % 13 + 1 == previousContract.height1 - 3 && (pom213 / 8) % 2 == 0)
						pom213 += 16;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 5 && (pom213 / 8) % 2 == 0)
						pom213 += 16;
		if (pom213 >= 31)
			return true;
		break;
	case colour:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (3 - (card[i][j] / 13) == previousContract.color)
						pom213++;
		if (pom213 >= 5)
			return true;
		break;
	case full_house:
		for (int i = 0;i < players; i++)
		{
			if (cardNumber[i] < endCard)
			{
				for (int j = 0;j < card[i].size();j++)
				{
					if (card[i][j] % 13 + 1 == previousContract.height1)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14)
						pom213++;
					if (card[i][j] % 13 + 1 == previousContract.height2)
						pom214++;
					else if (card[i][j] % 13 == 0 && previousContract.height2 == 14)
						pom214++;
				}
			}
		}
		if (pom213 >= 3 && pom214 >= 2)
			return true;
		break;
	case four:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (card[i][j] % 13 + 1 == previousContract.height1)
						pom213++;
					else if (card[i][j] % 13 == 0 && previousContract.height1 == 14)
						pom213++;
		if (pom213 >= 4)
			return true;
		break;
	case flushh:
		for (int i = 0;i < players; i++)
			if (cardNumber[i] < endCard)
				for (int j = 0;j < card[i].size();j++)
					if (card[i][j] % 13 == 3 - previousContract.color)
						if (card[i][j] % 13 + 1 == previousContract.height1 && pom213 % 2 == 0)
							pom213++;
						else if (card[i][j] % 13 == 0 && previousContract.height1 == 14 && pom213 % 2 == 0)
							pom213++;
						else if (card[i][j] % 13 + 1 == previousContract.height1 - 1 && (pom213 / 2) % 2 == 0)
							pom213 += 4;
						else if (card[i][j] % 13 + 1 == previousContract.height1 - 2 && (pom213 / 4) % 2 == 0)
							pom213 += 8;
						else if (card[i][j] % 13 + 1 == previousContract.height1 - 3 && (pom213 / 8) % 2 == 0)
							pom213 += 16;
						else if (card[i][j] % 13 == 0 && previousContract.height1 == 5 && (pom213 / 8) % 2 == 0)
							pom213 += 16;
		if (pom213 >= 31)
			return true;
		break;
	default:
		return true;
	}
	return false;
}

bool Check()
{
	
	bool isGoodCheck = !RealCheck();
	if (isGoodCheck)
	{
		checkText.setString("\t\tThe check is good.\nTo continue press any button.");
		checkText.setFillColor(Color(0, 128, 0));
	}
	else
	{
		checkText.setString("\t\tThe check is wrong.\nTo continue press any button.");
		checkText.setFillColor(Color(128, 0, 0));
	}
	bool pr153 = false;
	while (true)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
				pr153 = true;
			if (event.type == Event::Closed)
			{
				window.close();
				return true;
			}
		}
		Graphic2(true);
		if (pr153)
			break;
	}
	return isGoodCheck;
}

void GraphicUpdate(bool isComputer = false)
{
	window.clear();
	window.draw(rightSpr);
	window.draw(table);
	window.draw(back);
	window.draw(maxCardText);
	window.draw(totalNumCardText);
	for (int i = 0;i < players;i++)
		window.draw(playerNames[i]);
	for (int i = 0;i < card.size();i++)
	{
		if ((isComputer || i!= actualPlayer) && cardNumber[i] < endCard)
			for (int j = 0;j < card[i].size();j++)
				window.draw(backCards[card[i][j]]);
	}
	if (!isComputer)
	{
		//displaying buttons
		if (true)
		{
			for (int i = 0; i < 9;i++)
			{
				if (contractTypesButton[i].isActive != disabled)
				{
					if (contractTypesButton[i].isActive == off)
						window.draw(contractTypesButton[i].of);
					else if (contractTypesButton[i].isActive == highlighted)
						window.draw(contractTypesButton[i].highL);
					else if (contractTypesButton[i].isActive == on)
						window.draw(contractTypesButton[i].onn);
					window.draw(contractTypesButton[i].tekst);
					if (contractTypesButton[i].isActive == crossed)
						window.draw(contractTypesButton[i].cross);
				}
			}
			for (int j = 0; j < 2;j++)
			{
				for (int i = 13 - numCards; i < 13;i++)
				{
					if (contractNrButton[j][i].isActive != disabled)
					{
						if (contractNrButton[j][i].isActive == off)
							window.draw(contractNrButton[j][i].of);
						else if (contractNrButton[j][i].isActive == highlighted)
							window.draw(contractNrButton[j][i].highL);
						else if (contractNrButton[j][i].isActive == on)
							window.draw(contractNrButton[j][i].onn);
						window.draw(contractNrButton[j][i].tekst);
						if (contractNrButton[j][i].isActive == crossed)
							window.draw(contractNrButton[j][i].cross);
					}
				}
			}
			for (int i = 0; i < 4;i++)
			{
				if (contractColorButton[i].isActive != disabled)
				{
					if (contractColorButton[i].isActive == off)
						window.draw(contractColorButton[i].of);
					else if (contractColorButton[i].isActive == highlighted)
						window.draw(contractColorButton[i].highL);
					else if (contractColorButton[i].isActive == on)
						window.draw(contractColorButton[i].onn);
					window.draw(contractColorButton[i].tekst);
					if (contractColorButton[i].isActive == crossed)
						window.draw(contractColorButton[i].cross);
				}
			}
			if (checkButton.isActive == off)
				window.draw(checkButton.of);
			else if (checkButton.isActive == highlighted)
				window.draw(checkButton.highL);
			else if (checkButton.isActive == on)
				window.draw(checkButton.onn);
			if (checkButton.isActive != disabled)
				window.draw(checkButton.tekst);
			if (backButton.isActive != disabled)
			{
				if (backButton.isActive == off)
					window.draw(backButton.of);
				else if (backButton.isActive == highlighted)
					window.draw(backButton.highL);
				else if (backButton.isActive == on)
					window.draw(backButton.onn);
				window.draw(backButton.tekst);
			}
			if (StreetsDown.isActive != disabled)
			{
				window.draw(StreetsDown.of);
				window.draw(StreetsDown.tekst);
			}
		}
		if (cardNumber[actualPlayer] < endCard)
			for (int j = 0;j < card[actualPlayer].size();j++)
				window.draw(cards[card[actualPlayer][j]]);
	}
	else
		window.draw(continueText);
	//displaying bidding schema
	for (int i = 0; i < players; i++)
		for (int j = 0; j <= actualRound; j++)
			window.draw(contracts[i][j].tekst);
	window.display();
}

void Continue()
{
	
	backButton.isActive = disabled;
	previousContract = contracts[actualPlayer][actualRound];
	if (contracts[actualPlayer][actualRound].type == street)
	{
		StreetsDown.tekst.move(48.0 * rScale, 0);
		StreetsDown.of.move(48.0 * rScale, 0);
		StreetsDown.onn.move(48.0 * rScale, 0);
	}
	StreetsDown.isActive = disabled;
	contracts[actualPlayer][actualRound].tekst.setFillColor(Color::Black);
	contracts[actualPlayer][actualRound].actualize();
	int nextPlayer = actualPlayer;
	do
	{
		nextPlayer = (nextPlayer + 1) % players;
	} while (cardNumber[nextPlayer] >= endCard);
	if (isBegin)
	{
		isBegin = false;
		checkButton.isActive = off;
	}
	for (int j = 0;j < 13 - numCards; j++)
	{
		contractNrButton[0][j].isActive = blocked;
		contractNrButton[1][j].isActive = blocked;
	}
	for (int j = 13 - numCards;j < 13;j++)
	{
		contractNrButton[0][j].isActive = disabled;
		contractNrButton[1][j].isActive = disabled;
	}
	for (int i = 0;i < 4;i++)
		contractColorButton[i].isActive = disabled;
	for (int i = 0;i < 9;i++)
	{
		if (contracts[actualPlayer][actualRound].type < contract_type(i))
			contractTypesButton[i].isActive = off;
		else if (contracts[actualPlayer][actualRound].type > contract_type(i))
			contractTypesButton[i].isActive = crossed;
		else
		{
			switch (contracts[actualPlayer][actualRound].type)
			{
			case high_card:
			case a_pair:
			case three:
			case street:
			case four:
				if (contracts[actualPlayer][actualRound].height1 >= 14)
					contractTypesButton[i].isActive = crossed;
				else
					contractTypesButton[i].isActive = off;
				break;
			case colour:
				if (contracts[actualPlayer][actualRound].color == spade)
					contractTypesButton[i].isActive = crossed;
				else
					contractTypesButton[i].isActive = off;
				break;
			case two_pairs:
			case full_house:
				if (contracts[actualPlayer][actualRound].height1 >= 14 && contracts[actualPlayer][actualRound].height2 >= 13)
					contractTypesButton[i].isActive = crossed;
				else
					contractTypesButton[i].isActive = off;
				break;
			case flushh:
				if (contracts[actualPlayer][actualRound].color == spade && contracts[actualPlayer][actualRound].height2 >= 14)
					contractTypesButton[i].isActive = crossed;
				else
					contractTypesButton[i].isActive = off;
				break;
			}
		}
	}
	do
	{
		actualPlayer++;
		if (actualPlayer >= players)
		{
			actualPlayer -= players;
			if (actualRound < 2)
				actualRound++;
			else
			{
				for (int i = 0;i < 20; i++)
				{
					contracts[i][0].tekst.setString(contracts[i][1].tekst.getString());
					contracts[i][0].color = contracts[i][1].color;
					contracts[i][0].height1 = contracts[i][1].height1;
					contracts[i][0].height2 = contracts[i][1].height2;
					contracts[i][0].type = contracts[i][1].type;
					contracts[i][1].tekst.setString(contracts[i][2].tekst.getString());
					contracts[i][1].color = contracts[i][2].color;
					contracts[i][1].height1 = contracts[i][2].height1;
					contracts[i][1].height2 = contracts[i][2].height2;
					contracts[i][1].type = contracts[i][2].type;
					contracts[i][2].clear();
				}
			}
		}
	} while (cardNumber[actualPlayer] >= endCard);
	contractPart = 0;
	checkButton.tekst.setString("CHECK");
	contracts[actualPlayer][actualRound].tekst.setFillColor(Color::Green);
	contracts[actualPlayer][actualRound].tekst.setString("V");
	if (isComputer[actualPlayer] == 0)
	{
		continueText.setString("Next player: " + playerName[nextPlayer] + "\nTo continue press any button.");
	}
	else
	{
		continueText.setString("WAIT! Computer is thinking.");
	}
	bool pr153 = false;
	do
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed || isComputer[actualPlayer]>0)
				pr153 = true;
			if (event.type == Event::Closed)
			{
				window.close();
				return;
			}
		}
		Graphic2(false);
	} while (!pr153);
}

void Save(wstring fileName = L"Saves/Autosave.txt")
{
	wfstream saveFile;
	saveFile.open(fileName, ios::out, ios::trunc);
	if(saveFile.good())
	{
		saveFile << numCards << L" " << players << L" " << beginCard << L" " << endCard << L" " << actualPlayer << L"\n";
		for (int i = 0; i < players; i++)
			saveFile << playerName[i] << L"\n" << isComputer[i] << " " << cardNumber[i] << L"\n";
	}
	saveFile.close();
}

bool Load(wstring fileName = L"Saves/Autosave.txt")
{
	wfstream saveFile;
	saveFile.open(fileName, ios::in);
	if (!saveFile.good())
	{
		return false;
		/*for (int i = 0; i < 6; i++)
		{
			wstring abb = L"Player " + to_wstring(i + 1);
			playerName.push_back(abb);
			Text myText;
			myText.setFont(font);
			myText.setString(abb);
			myText.setColor(sf::Color::Black);
			myText.setStyle(sf::Text::Regular);
			myText.setPosition(768.0 * scale + 1.0 * rScale, (37.0 + 26.0 * (double)i) * scale);
			myText.setCharacterSize(scale * 24.0);
			if ((double)myText.getGlobalBounds().width > 92.0 * rScale)
				myText.setCharacterSize(rScale * 92.0 / (double)myText.getGlobalBounds().width);
			playerNames.push_back(myText);
			cardNumber.push_back(beginCard);
		}*/
	}
	else
	{
		saveFile >> numCards >> players >> beginCard >> endCard >> actualPlayer;
		wstring streamLine[25];
		int ab[25];
		numPlayers = players;
		playerNames.clear();
		cardNumber.clear();
		playerName.clear();
		isComputer.clear();
		maxCardText.setString(L"Excluding number of cards: " + to_wstring(endCard));
		cardsPlay = 0;
		int aennon;
		for (int i = 0;i < players;i++)
		{
			saveFile >> streamLine[i];
			saveFile >> ab[i];
			saveFile >> aennon;
			playerName.push_back(streamLine[i]);
			isComputer.push_back(aennon);
			if (ab[i] < beginCard)
				ab[i] = beginCard;
			/*for (int j = 0; j < streamLine[i].length();j++)
			if (streamLine[i][j] == '_')
			streamLine[i][j] = ' ';*/
			myText[i].setFont(font);
			myText[i].setString(streamLine[i]);
			myText[i].setFillColor(Color::Black);
			if(aennon == 0)
				myText[i].setStyle(Text::Regular);
			else
				myText[i].setStyle(Text::Italic);
			myText[i].setPosition(770.0 * scale + 1.0 * rScale, (37.0 + 26.0 * (double)i) * scale);
			myText[i].setCharacterSize(scale * 24.0);
			if ((double)myText[i].getGlobalBounds().width > 92.0 * rScale)
				myText[i].setCharacterSize(rScale * 92.0 / (double)myText[i].getGlobalBounds().width);
			cardsPlay += ab[i];
			if (ab[i] >= endCard)
			{
				cardsPlay -= ab[i];
				numPlayers--;
				myText[i].setFillColor(Color::Red);
			}
			if (ab[i] == endCard - 1)
				myText[i].setFillColor(Color::Color(255, 127, 0));
			if (ab[i] == endCard - 2)
				myText[i].setFillColor(Color::Color(255, 255, 0));
			playerNames.push_back(myText[i]);
			cardNumber.push_back(ab[i]);
		}
		totalNumCardText.setString(L"Number of cards: " + to_wstring(cardsPlay) + L" of " + to_wstring(numCards * 4));
	}
	saveFile.close();
	return true;
}

bool EventUpdate()
{
	Event event;
		while (window.pollEvent(event))
		{
			//keyword events
			if (event.type == Event::Closed)
			{
				window.close();
				return true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape))
				return true;
			/*if (event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::Return))
			{
				Continue();
			}
			if (event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::F5) && !isBegin)
			{
				LoseCheck();
			}
			if (event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::F6) && !isBegin)
			{
				WinCheck();
			}*/
			if (event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::S))
			{
				wstring fileName;
				wcout << "Write the name of file (without extension):\n";
				wcin >> fileName;
				Save(L"Saves/" + fileName + ".txt");
			}
			if (event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::L))
			{
				wstring fileName;
				wcout << "Write the name of file (without extension):\n";
				wcin >> fileName;
				while (!Load(L"Saves/" + fileName + ".txt"))
				{
					wcout << fileName << " doesn't exist. Write the name of file (without extension)\n or write \"exit\ to exit the loading\n";
					wcin >> fileName;
					if (fileName == "exit")
						return false;
				}
			}
			//button events
			if (true)
			{
				if (contractPart == 0)
				{
					if (checkButton.isActive != disabled)
					{
						if (checkButton.isClicked(event))
						{
							contracts[actualPlayer][actualRound].tekst.setString("CHECK");
							checkButton.isActive = on;
							contractPart = 100;
							if (Check() && window.isOpen())
								WinCheck();
							else if (window.isOpen())
								LoseCheck();
							else
								return true;
						}
					}
					for (int i = 0;i < 9;i++)
					{
						if (contractTypesButton[i].isActive != crossed && contractTypesButton[i].isClicked(event))
						{
							contracts[actualPlayer][actualRound].type = contract_type(contractTypesButton[i].pom);
							contractTypesButton[i].isActive = on;
							checkButton.isActive = disabled;
							if (contracts[actualPlayer][actualRound].type == colour || contracts[actualPlayer][actualRound].type == flushh)
							{
								for (int i = 0;i < 4;i++)
									contractColorButton[i].isActive = off;
								for (int i = 13-numCards;i < 13;i++)
								{
									contractNrButton[0][i].isActive = disabled;
									contractNrButton[1][i].isActive = disabled;
								}
								if (contracts[actualPlayer][actualRound].type == previousContract.type && previousContract.type == colour)
								{
									for (int i = 0;i < 4;i++)
										if(contract_colour(i) <= previousContract.color)
											contractColorButton[i].isActive = crossed;
								}
								else if (contracts[actualPlayer][actualRound].type == previousContract.type && previousContract.type == flushh)
								{
									for (int i = 0;i < 4;i++)
										if (contract_colour(i) <= previousContract.color && previousContract.height2 >= 14)
											contractColorButton[i].isActive = crossed;
								}
							}
							else
							{
								for (int i = 13 - numCards;i < 13;i++)
								{
									if (i == 13 - numCards && contracts[actualPlayer][actualRound].type == two_pairs)
										contractNrButton[0][i].isActive = disabled;
									else
										contractNrButton[0][i].isActive = off;
									contractNrButton[1][i].isActive = disabled;
									if (contracts[actualPlayer][actualRound].type == previousContract.type)
									{
										if(previousContract.height1 > i+2)
											contractNrButton[0][i].isActive = crossed;
										if(previousContract.height1 == i+2)
											if(previousContract.type != two_pairs && previousContract.type != full_house)
												contractNrButton[0][i].isActive = crossed;
											else if(previousContract.type == two_pairs && previousContract.height2 == previousContract.height1-1)
												contractNrButton[0][i].isActive = crossed;
											else if (previousContract.type == full_house && previousContract.height2 >= 14)
												contractNrButton[0][i].isActive = crossed;
									}
								}
								for (int j = 0;j < 4;j++)
									contractColorButton[j].isActive = disabled;
							}
							if (contracts[actualPlayer][actualRound].type == street)
							{
								StreetsDown.isActive = off;
								StreetsDown.tekst.move(-48.0 * rScale, 0);
								StreetsDown.of.move(-48.0 * rScale, 0);
								contractNrButton[0][13 - numCards].isActive = disabled;
								contractNrButton[0][14 - numCards].isActive = disabled;
								contractNrButton[0][15 - numCards].isActive = disabled;
								if(numCards < 13)
									contractNrButton[0][16 - numCards].isActive = disabled;
							}
							contracts[actualPlayer][actualRound].actualize();
							contractPart++;
							backButton.isActive = off;
						}
					}
				}
				else if (contractPart == 1)
				{
					for (int i = 13 - numCards;i < 13;i++)
					{
						if (contractNrButton[0][i].isActive != disabled && contractNrButton[0][i].isActive != crossed && contractNrButton[0][i].isClicked(event))
						{
							contracts[actualPlayer][actualRound].height1 = contractNrButton[0][i].pom;
							contracts[actualPlayer][actualRound].actualize();
							contractNrButton[0][i].isActive = on;
							switch (contracts[actualPlayer][actualRound].type)
							{
							case high_card:
							case a_pair:
							case three:
							case street:
							case four:
								contractPart = 5;
								checkButton.isActive = off;
								checkButton.tekst.setString("CONFIRM");
								break;
							case two_pairs:
								contractPart++;
								for (int j = 0;j < i;j++)
									if (contractNrButton[1][j].isActive != crossed)
										contractNrButton[1][j].isActive = off;
								if (previousContract.type == two_pairs && previousContract.height1 == contracts[actualPlayer][actualRound].height1)
									for (int j = 13 - numCards;j <= previousContract.height2 - 2;j++)
										contractNrButton[1][j].isActive = crossed;
								for (int j = i;j < 13;j++)
									contractNrButton[1][j].isActive = disabled;
								break;
							case full_house:
								contractPart++;
								for (int j = 13 - numCards;j < 13;j++)
									if (contractNrButton[1][j].isActive != crossed)
										contractNrButton[1][j].isActive = off;
								if (previousContract.type == full_house && previousContract.height1 == contracts[actualPlayer][actualRound].height1)
									for (int j = 13 - numCards;j <= previousContract.height2 - 2;j++)
										contractNrButton[1][j].isActive = crossed;
								contractNrButton[1][i].isActive = disabled;
								break;
							}
						}
					}
					for (int i = 0;i < 4;i++)
					{
						if (contractColorButton[i].isActive != disabled && contractColorButton[i].isActive != crossed && contractColorButton[i].isClicked(event))
						{
							contracts[actualPlayer][actualRound].color = contract_colour(contractColorButton[i].pom);
							contracts[actualPlayer][actualRound].actualize();
							if (contracts[actualPlayer][actualRound].type == colour)
							{
								contractPart = 5;
								checkButton.isActive = off;
								checkButton.tekst.setString("CONFIRM");
							}
							else
							{
								contractPart++;
								StreetsDown.isActive = off;
								contractNrButton[1][0].isActive = disabled;
								contractNrButton[1][1].isActive = disabled;
								contractNrButton[1][2].isActive = disabled;
								int a597 = 16 - numCards;
								if (numCards < 13)
									a597++;
								for (int j = a597;j < 13;j++)
									if(contractNrButton[1][j].isActive != crossed)
										if (contractNrButton[1][j].isActive == flushh)
											if (j + 2 < previousContract.height2 || (j + 2 == previousContract.height2 && contracts[actualPlayer][actualRound].color <= previousContract.color))
												contractNrButton[1][j].isActive = crossed;
											else
												contractNrButton[1][j].isActive = off;
										else
											contractNrButton[1][j].isActive = off;
							}
							contractColorButton[i].isActive = on;
						}
						
					}
				}
				else if (contractPart == 2)
				{
					for (int i = 13 - numCards;i < 13;i++)
					{
						if (contractNrButton[1][i].isActive != disabled && contractNrButton[1][i].isActive != crossed && contractNrButton[1][i].isClicked(event))
						{
							contracts[actualPlayer][actualRound].height2 = contractNrButton[1][i].pom;
							contractNrButton[1][i].isActive = on;
							checkButton.isActive = off;
							contractPart = 5;
							checkButton.tekst.setString("CONFIRM");
							contracts[actualPlayer][actualRound].actualize();
						}
					}
				}
				else if (contractPart == 5 && checkButton.isActive != disabled && checkButton.isClicked(event))
				{
					contracts[actualPlayer][actualRound].actualize();
					if (contracts[actualPlayer][actualRound].type == two_pairs && contracts[actualPlayer][actualRound].height1 < contracts[actualPlayer][actualRound].height2)
						swap(contracts[actualPlayer][actualRound].height1, contracts[actualPlayer][actualRound].height2);
					Continue();
				}
			}
			//cancel event
			if(contractPart > 0)
			{
				bool pom175 = backButton.isClicked(event);
				if ((event.type == Event::KeyPressed && Keyboard::isKeyPressed(Keyboard::Z)) || pom175)
				{
					if (pom175)
					{
						if (contractPart >= 2)
							backButton.isActive = off;
						else
							backButton.isActive = disabled;
					}
					if (contractPart == 1)
					{
						if (contracts[actualPlayer][actualRound].type == street)
						{
							StreetsDown.isActive = disabled;
							StreetsDown.tekst.move(48.0 * rScale, 0);
							StreetsDown.of.move(48.0 * rScale, 0);
							StreetsDown.onn.move(48.0 * rScale, 0);
						}
						for (int i = 0;i < 9;i++)
							if (contractTypesButton[i].isActive != crossed)
								contractTypesButton[i].isActive = off;
						for (int i = 0;i < 4;i++)
							contractColorButton[i].isActive = disabled;
						for (int i = 13 - numCards;i < 13;i++)
						{
							contractNrButton[0][i].isActive = disabled;
							contractNrButton[1][i].isActive = disabled;
						}
						contracts[actualPlayer][actualRound].type = undefined_type;
						if (!isBegin)
						{
							checkButton.isActive = off;
							checkButton.tekst.setString("CHECK");
						}
						contractPart--;
					}
					else if (contractPart == 2)
					{
						for (int i = 13 - numCards; i < 13;i++)
							contractNrButton[1][i].isActive = disabled;
						if (contracts[actualPlayer][actualRound].type == flushh)
						{
							StreetsDown.isActive = disabled;
							for (int i = 0; i < 4;i++)
								if(contractColorButton[i].isActive != crossed)
									contractColorButton[i].isActive = off;
						}
						else
							for (int i = 13 - numCards; i < 13;i++)
							{
								if((i > 13 - numCards || contracts[actualPlayer][actualRound].type != two_pairs) && contractNrButton[13-numCards][i].isActive != crossed)
									contractNrButton[0][i].isActive = off;
								else if(contractNrButton[0][i].isActive != crossed)
									contractNrButton[0][i].isActive = disabled;
							}
						contracts[actualPlayer][actualRound].height1 = -1;
						contracts[actualPlayer][actualRound].color = undefined_colour;
						contractPart--;
					}
					else if (contractPart == 5)
					{
						int a614;
						checkButton.isActive = disabled;
						if(contracts[actualPlayer][actualRound].type != flushh)
							contracts[actualPlayer][actualRound].color = undefined_colour;
						contracts[actualPlayer][actualRound].height2 = -1;
						switch (contracts[actualPlayer][actualRound].type)
						{
						case flushh:
							contractPart = 2;
							for (int i = 13 - numCards; i < 3;i++)
								contractNrButton[1][i].isActive = disabled;
							a614 = 16 - numCards;
							if (numCards < 13)
								a614++;
							for (int i = a614; i < 13;i++)
								if (contractNrButton[1][i].isActive == on)
									contractNrButton[1][i].isActive = off;
							break;
						case two_pairs:
						case full_house:
							contractPart = 2;
							for (int i = 13 - numCards;i < 13;i++)
								if(contractNrButton[1][i].isActive == on)
									contractNrButton[1][i].isActive = off;
							break;
						case colour:
							contractPart = 1;
							for (int i = 0;i < 4;i++)
								if (contractColorButton[i].isActive != crossed)
									contractColorButton[i].isActive = off;
							break;
						case street:
							contractPart = 1;
							a614 = 16 - numCards;
							if (numCards < 13)
								a614++;
							for (int i = 13 - numCards;i < a614;i++)
								contractNrButton[0][i].isActive = disabled;
							for (int i = a614;i < 13;i++)
								if (contractNrButton[0][i].isActive != crossed)
									contractNrButton[0][i].isActive = off;
							contracts[actualPlayer][actualRound].height1 = -1;
							break;
						default:
							contractPart = 1;
							for (int i = 13 - numCards;i < 13;i++)
								if (contractNrButton[0][i].isActive != crossed)
									contractNrButton[0][i].isActive = off;
							contracts[actualPlayer][actualRound].height1 = -1;
						}
					}
					contracts[actualPlayer][actualRound].actualize();
				}
			}
		}
	return false;
}

long long int Newton(int n, int k)
{
	double wyn = 1.0;
	if (k > n - k)
		k = n - k;
	for (int i = 1;i <= k;i++)
		wyn *= (n - i + 1)/ i;
	return (long long int) wyn;
}

double Probability(Contract con)
{
	int k = cardsPlay;
	int li = cardNumber[actualPlayer];
	int V = 0, W = 0;
	double A, Omega;
	Omega = 1.0;
	A = 0.0;
	for (unsigned int i = 1; i <= k-li; i++)
	{
		Omega *= (4 * numCards - li - i + 1) / (double)i;
	}
	switch (con.type)
	{
	case high_card:
		for(int i=0;i<card[actualPlayer].size();i++)
			if (card[actualPlayer][i] % 13 + 1 == con.height1)
				V++;
			else if (card[actualPlayer][i] % 13 == 0 && con.height1 == 14)
				V++;
		if (V >= 1)
			return 1.0;
		for (int j = 1;j <= 4;j++)
			A += Newton(4 - V, j - V)*Newton(4*numCards-li-4+V,k-li-j+V);
		break;
	case a_pair:
		for (int i = 0;i<card[actualPlayer].size();i++)
			if (card[actualPlayer][i] % 13 + 1 == con.height1)
				V++;
			else if (card[actualPlayer][i] % 13 == 0 && con.height1 == 14)
				V++;
		if (V >= 2)
			return 1.0;
		for (int j = 2;j <= 4;j++)
			A += Newton(4 - V, j - V)*Newton(4 * numCards - li - 4 + V, k - li - j + V);
		break;
	case two_pairs:
		for (int j = 0;j < cardNumber[actualPlayer];j++)
			if (card[actualPlayer][j] % 13 + 1 == con.height1)
				V++;
			else if (card[actualPlayer][j] % 13 == 0 && con.height1 == 14)
				V++;
			else if (card[actualPlayer][j] % 13 + 1 == con.height2)
				W++;
			else if (card[actualPlayer][j] % 13 == 0 && con.height2 == 14)
				W++;
		if (V >= 2 && W >= 2)
			return true;
		for (int i = 2;i <= 4;i++)
			for (int j = 2;j <= 4;j++)
				A += Newton(4 - V, i - V)*Newton(4 - W, j - W)*Newton(4 * numCards - li - 8 + V + W, k - li - i - j + V + W);
		break;
	case three:
		for (int i = 0;i<card[actualPlayer].size();i++)
			if (card[actualPlayer][i] % 13 + 1 == con.height1)
				V++;
			else if (card[actualPlayer][i] % 13 == 0 && con.height1 == 14)
				V++;
		if (V >= 3)
			return 1.0;
		for (int j = 3;j <= 4;j++)
			A += Newton(4 - V, j - V)*Newton(4 * numCards - li - 4 + V, k - li - j + V);
		break;
	case full_house:
		for (int j = 0;j < cardNumber[actualPlayer];j++)
			if (card[actualPlayer][j] % 13 + 1 == con.height1)
				V++;
			else if (card[actualPlayer][j] % 13 == 0 && con.height1 == 14)
				V++;
			else if (card[actualPlayer][j] % 13 + 1 == con.height2)
				W++;
			else if (card[actualPlayer][j] % 13 == 0 && con.height2 == 14)
				W++;
		if (V >= 3 && W >= 2)
			return true;
		for (int i = 3;i <= 4;i++)
			for (int j = 2;j <= 4;j++)
				A += Newton(4 - V, i - V)*Newton(4 - W, j - W)*Newton(4 * numCards - li - 8 + V + W, k - li - i - j + V + W);
		break;
	case colour:
		for (int j = 0;j < cardNumber[actualPlayer];j++)
			if (3 - (card[actualPlayer][j] / 13) == con.color)
				V++;
		if (V >= 5)
			return 1.0;
		for (int j = 3;j <= numCards;j++)
			A += Newton(numCards - V, j - V)*Newton(3 * numCards + V - li, k - li - j + V);
		break;
	case four:
		for (int i = 0;i<card[actualPlayer].size();i++)
			if (card[actualPlayer][i] % 13 + 1 == con.height1)
				V++;
			else if (card[actualPlayer][i] % 13 == 0 && con.height1 == 14)
				V++;
		if (V >= 4)
			return 1.0;
		A += Newton(4 * numCards - li - 4 + V, k - li - 4 + V);
		break;
	}
	wcout << A << " " << Omega << "\n";
	return A / Omega;
}

bool ComputerAction()
{
	wcout << actualPlayer << "\n";
	Event event;
	while (window.pollEvent(event))
	{
		//keyword events
		if (event.type == Event::Closed)
		{
			window.close();
			return true;
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
			return true;
	}
	double checkProb;
	if (!isBegin)
		checkProb = 1.0 - Probability(previousContract);
	else
		checkProb = 0.0;
	vector<double> contractProb;
	vector<Contract> probContracts;
	Contract pom913;
	for (int i = previousContract.type;i <= flushh;i++)
	{
		pom913.type = contract_type(i);
		switch (i)
		{
		case high_card:
		case a_pair:
		case three:
		case four:
			pom913.color = undefined_colour;
			pom913.height2 = 0;
			if (contract_type(i) == previousContract.type)
				for (int j = previousContract.height1 + 1;j <= 14;j++)
				{
					pom913.height1 = j;
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
			else
				for (int j = 15 - numCards; j <= 14;j++)
				{
					pom913.height1 = j;
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
			break;
		case street:
			pom913.color = undefined_colour;
			pom913.height2 = 0;
			if (contract_type(i) == previousContract.type)
				for (int j = previousContract.height1 + 1;j <= 14;j++)
				{
					pom913.height1 = j;
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
			else
				for (int j = 10 - numCards; j <= 14;j++)
				{
					if (numCards < 13 && j == 10 - numCards)
						continue;
					pom913.height1 = j;
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
			break;
		case colour:
			pom913.height1 = 0;
			pom913.height2 = 0;
			if (contract_type(i) == previousContract.type)
				for (int j = previousContract.color + 1;j <= 3;j++)
				{
					pom913.color = contract_colour(j);
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
			else
				for (int j = 0; j <= 3;j++)
				{
					pom913.color = contract_colour(j);
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
			break;
		case two_pairs:
			pom913.color = undefined_colour;
			if (contract_type(i) == previousContract.type)
			{
				pom913.height1 = previousContract.height1 + 1;
				for (int k = previousContract.height2 + 1;k < previousContract.height1 + 1;k++)
				{
					pom913.height2 = k;
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
				for (int j = previousContract.height1 + 2; j <= 14;j++)
				{
					pom913.height1 = j;
					for (int k = 15 - numCards;k < j;k++)
					{
						pom913.height2 = k;
						contractProb.push_back(Probability(pom913));
						probContracts.push_back(pom913);
					}
				}
			}
			else
				for (int j = 16 - numCards; j <= 14;j++)
				{
					pom913.height1 = j;
					for (int k = 15 - numCards;k < j;k++)
					{
						pom913.height2 = k;
						contractProb.push_back(Probability(pom913));
						probContracts.push_back(pom913);
					}
				}
			break;
		case full_house:
			pom913.color = undefined_colour;
			if (contract_type(i) == previousContract.type)
			{
				pom913.height1 = previousContract.height1 + 1;
				for (int k = previousContract.height2 + 1;k <= 14;k++)
				{
					if (k == previousContract.height1 + 1)
						continue;
					pom913.height2 = k;
					contractProb.push_back(Probability(pom913));
					probContracts.push_back(pom913);
				}
				for (int j = previousContract.height1 + 2; j <= 14;j++)
				{
					pom913.height1 = j;
					for (int k = 15 - numCards;k <= 14;k++)
					{
						if (k == j)
							continue;
						pom913.height2 = k;
						contractProb.push_back(Probability(pom913));
						probContracts.push_back(pom913);
					}
				}
			}
			else
				for (int j = 16 - numCards; j <= 14;j++)
				{
					pom913.height1 = j;
					for (int k = 15 - numCards;k <= 14;k++)
					{
						if (k == j)
							continue;
						pom913.height2 = k;
						contractProb.push_back(Probability(pom913));
						probContracts.push_back(pom913);
					}
				}
			break;
		}
		wcout << i << "\n";
	}
	double max = checkProb;
	int ind = -1;
	for (int i = 0;i < contractProb.size();i++)
	{
		if (contractProb[i] >= max || contractProb[i]>=.75)
		{
			ind = i;
			max = contractProb[i];
		}
	}
	if (ind >= 0)
	{
		contracts[actualPlayer][actualRound] = probContracts[ind];
		contracts[actualPlayer][actualRound].actualize();
		Continue();
	}
	else
	{
		contracts[actualPlayer][actualRound].tekst.setString("CHECK");
		contractPart = 100;
		if (Check())
			WinCheck();
		else
			LoseCheck();
	}
	return false;
}

bool PrepareGame(bool isLoaded)
{
	//okno ustawień gry przed jej rozpoczęciem
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed || Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			break;
			return false;
		}
		break;
	}
	if (!isLoaded)
	{
		wcout << L"Write the number of players\n";
		wcin >> players;
		while (players < 2 || players > 10)
		{
			if (players < 2)
				wcout << L"Too few players. Write the number of players\n";
			else
				wcout << L"Too many players. Write the number of players\n";
			wcin >> players;
		}
		actualPlayer = 0;
		wcout << L"Write the number of cards in deck\n";
		wcin >> numCards;
		while (numCards > 52 || numCards < 24)
		{
			if (numCards > 52)
				wcout << L"Too many cards. Write the number of cards, when the player is excluded\n";
			else
				wcout << L"Too few cards. Write the number of cards, when the player is excluded\n";
			wcin >> numCards;
		}
		numCards /= 4;
		wcout << L"Write the beginning number of cards for each player\n";
		wcin >> beginCard;
		while (beginCard < 0 || beginCard >= numCards * 4 / players)
		{
			if(beginCard < 0)
				wcout << L"Too few cards. Write the beginning number of cards for each player\n";
			else
				wcout << L"Too many cards. Write the beginning number of cards for each player\n";
			wcin >> beginCard;
		}
		wcout << L"Write the number of cards, when the player is excluded\n";
		wcin >> endCard;
		while (endCard < beginCard || endCard-1 >= numCards * 4 / players)
		{
			if (endCard > numCards * 4 / players)
				wcout << L"Too many cards. Write the number of cards, when the player is exclude\n";
			else
				wcout << L"Too few cards. Write the number of cards, when the player is exclude\n";
			wcin >> endCard;
		}
		numPlayers = players;
	}
	playerNames.clear();
	cardNumber.clear();
	playerName.clear();
	isComputer.clear();
	maxCardText.setString(L"Excluding number of cards: " + to_wstring(endCard));
	cardsPlay = players * beginCard;
	totalNumCardText.setString(L"Number of cards: " + to_wstring(cardsPlay) + L" of " + to_wstring(numCards * 4));
	wstring pom187634;
	int pom1705;
	for (int i = 0;i < players;i++)
	{
		wcout << L"Write the name of player " << i + 1 << "\n";
		wcin >> pom187634;
		playerName.push_back(pom187634);
		wcout << L"Write if this player is a computer.\n0 - human player, more - computer player ";
		wcin >> pom1705;
		isComputer.push_back(pom1705);
		myText[i].setFont(font);
		myText[i].setString(pom187634);
		myText[i].setFillColor(Color::Black);
		if (pom1705 == 0)
			myText[i].setStyle(Text::Regular);
		else
			myText[i].setStyle(Text::Italic);
		myText[i].setPosition(770.0 * scale + 1.0 * rScale, (37.0 + 26.0 * (double)i) * scale);
		myText[i].setCharacterSize(scale * 24.0);
		if ((double)myText[i].getGlobalBounds().width > 92.0 * rScale)
			myText[i].setCharacterSize(rScale * 92.0 / (double)myText[i].getGlobalBounds().width);
		if (beginCard >= endCard)
		{
			numPlayers--;
			myText[i].setFillColor(Color::Red);
		}
		if (beginCard == endCard - 1)
			myText[i].setFillColor(Color::Color(255, 127, 0));
		if (beginCard == endCard - 2)
			myText[i].setFillColor(Color::Color(255, 255, 0));
		playerNames.push_back(myText[i]);
		cardNumber.push_back(beginCard);
	}
	return true;
}

void GameMain(bool isLoaded = false)
{
	wcout << "GAME\n";
	if (!isLoaded)
	{
		if (!PrepareGame(false))
			return;
	}
	else
	{
		wstring fileName;
		wcout << "Write the name of file (without extension):\n";
		wcin >> fileName;
		while (!Load(L"Saves/"+fileName+".txt"))
		{
			wcout << fileName << " doesn't exist. Write the name of file (without extension)\n or write \"exit\ to exit the loading\n";
			wcin >> fileName;
			if (fileName == "exit")
				return;
		}
	}
	shuffleCards();
	wcout << actualPlayer << " " << actualRound << "\n";
	contracts[actualPlayer][actualRound].actualize();
	contracts[actualPlayer][actualRound].tekst.setFillColor(Color::Green);
	while (window.isOpen())
	{
		GraphicUpdate(isComputer[actualPlayer]);
		if (isComputer[actualPlayer] == 0)
		{
			if (EventUpdate())
				return;
		}
		else
		{
			if (ComputerAction())
				return;
		}
		if (numPlayers <= 1)
			break;
	}
}

void Settings()
{
	wfstream optionFile;
	optionFile.open(optionName, ios::out, ios::trunc);
	if (optionFile.good())
	{
		int ScreenX, ScreenY;
		wcout << L"Write the size of screen:\n";
		wcin >> ScreenX >> ScreenY;
		optionFile << ScreenX << L" " << ScreenY;
		optionFile << L"\n";
		wcout << L"Write if fulscreen is active (y-yes, n-no)\n";
		wchar_t a;
		wcin >> a;
		while (a != 'y' && a != 'n')
		{
			wcout << "Written letter is wrong. Write if fulscreen is active (y-yes, n-no)\n";
			wcin >> a;
		}
		wstring b813;
		if (a == 'y')
			b813 = L"Fullscreen:_YES\n";
		else
			b813 = L"Fullscreen:_NO\n";
		optionFile << b813;
	}
	optionFile.close();
}

int main()
{
	if (Begin())
	{
		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed || Keyboard::isKeyPressed(sf::Keyboard::Escape))
					window.close();
			}	
		}
		return EXIT_FAILURE;
	}
	int clickedNr = -1;
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed || Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
		}
		window.clear();
		window.draw(MMleft);
		window.draw(MMright);
		window.draw(MMback);
		for (int i = 0;i < 5;i++)
		{
			if (MMButton[i].isClicked(event))
				clickedNr = i;
			if (MMButton[i].isActive == on)
				window.draw(MMButton[i].onn);
			else if (MMButton[i].isActive == highlighted)
				window.draw(MMButton[i].highL);
			else
				window.draw(MMButton[i].of);
			window.draw(MMButton[i].tekst);
		}
		switch (clickedNr)
		{
		case 0:
			//One player
			GameMain(false);
			break;
		case 1:
			//Multiplayer
			break;
		case 2:
			//Load game
			Load();
			GameMain(true);
			break;
		case 3:
			//Settings
			Settings();
			break;
		case 4:
			//Exit
			window.close();
			break;
		}
			clickedNr = -1;
		window.display();
	}
	if (!window.isOpen())
		return EXIT_SUCCESS;
	return EXIT_SUCCESS;
}
