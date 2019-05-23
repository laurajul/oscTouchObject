#include "ofApp.h"
#include <fstream>   // std::ofstream, std::ifstream
#include <vector>    // std::vector
#include <utility>   // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream>   // std::stringstream
#include <iostream>  // std::cout, std::cin


/*
string temp;
string line;

typedef float temper[24];
temper Jan[31];
int j = 0;

using namespace std;

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

//#include "ofAppGlutWindow.h"

*/

int main() {



	/*

	typedef float fHour[24];
	typedef fHour fDay[31];
	std::vector<std::string> lineSplitted;
	std::vector<float> allTemperatures;
	//float allTemperatures[8736];
	float Temperatur[12][31][24], Niederschlag[12];
	int iAnzahlTage[12];



	iAnzahlTage[0] = 31;
	iAnzahlTage[1] = 28;
	iAnzahlTage[2] = 31;
	iAnzahlTage[3] = 30;
	iAnzahlTage[4] = 31;
	iAnzahlTage[5] = 30;
	iAnzahlTage[6] = 31;
	iAnzahlTage[7] = 31;
	iAnzahlTage[8] = 30;
	iAnzahlTage[9] = 31;
	iAnzahlTage[10] = 30;
	iAnzahlTage[11] = 31;


	ifstream ip("data/data.csv");
	if (!ip.is_open()) std::cout << "ERROR: no file" << '\n';
	//getline(ip, temp, ',');
	float temp;


	for (int i = 0; i < 8786; i++)
	{
		getline(ip, line);
		lineSplitted = split(line, ',');
		//cout << lineSplitted[0] << endl;
		temp = stof(lineSplitted[0]);
		allTemperatures.push_back(temp);

	}

	allTemperatures[0];
	int i = 0;
	for (int m = 0; m < 12; m++) {

		for (int t = 0; t < iAnzahlTage[m]; t++)
		{
			for (int s = 0; s < 24; s++)
			{
				Temperatur[m][t][s] = allTemperatures[i];
				i++;
			}
		}

	}

	for (int j = 0; j < 31; j++) {

	for (int i = 0; i < 24; i++) {


		std::cout << Temperatur[2][j][i] << endl;
		//ofDrawRectangle(100, 100, 100, 100);
	
	}
	}


    */

	ofSetupOpenGL(1920, 1080, OF_WINDOW);
	ofRunApp(new ofApp());
	
	
}
