/*
This application uses Open Source components.
You can find the source code of the open source project along with license information below.
I acknowledge and I'm grateful to the developer for his contribution to open source.

Project: oscTouchObject https://github.com/paul-ferragut/oscTouchObject

*/

#include "touchObject.h"
#include <iostream>
#include <string>    // std::string
#include <fstream>   // std::ofstream, std::ifstream
#include <vector>    // std::vector
#include <utility>   // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream>   // std::stringstream

//--------------------------------------------------------------
string temp;
string line;


int j = 0;



ofPolyline TemperaturLinie;





typedef float fHour[24];
typedef fHour fDay[31];
std::vector<std::string> lineSplitted;
std::vector<std::string> lineSplittedSun;
std::vector<float> allTemperatures;
std::vector<float> allGlobalRadiation;
//float allTemperatures[8736];
float Temperatur[12][31][24]; 
float Globalstrahlung[12][31][24];
int iAnzahlTage[12];





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


void touchObject::setup() {

	
	

	//FETCH DATA WETTERDATEN////

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
	float globalRadiation;


	for (int i = 0; i < 8786; i++)
	{
		getline(ip, line);
		lineSplitted = split(line, ',');
		//cout << lineSplitted[0] << endl;
		temp = stof(lineSplitted[0]);
		globalRadiation = stof(lineSplitted[6]);

		//cout << lineSplitted[6] << endl;
		allTemperatures.push_back(temp);
		allGlobalRadiation.push_back(globalRadiation);

	}

	/**/

	allTemperatures[0];
	allGlobalRadiation[0];
	int i = 0;
	for (int m = 0; m < 12; m++) {

		for (int t = 0; t < iAnzahlTage[m]; t++)
		{
			for (int s = 0; s < 24; s++)
			{
				Temperatur[m][t][s] = allTemperatures[i];
				Globalstrahlung[m][t][s] = allGlobalRadiation[i];
				i++;

			}
		}

	}


	

	//FETCH DATA WETTERDATEN////
	

	//radialc.load("radialc.png");
	settings.loadFile("settings.xml");
	cout << "angles used:" << endl;
	for (int i = 0;i < settings.getNumTags("angle");i++) {
		cout << settings.getValue("angle", 0, i) << " - ";
		degrees.push_back(settings.getValue("angle", 0, i));
		triangleTracker tri;
		triangleT.push_back(tri);
	}
	cout <<  endl;
	
	//SET RANGE IN PIXEL
	for (int i = 0;i < triangleT.size();i++) {
		if (i == 0) {
			triangleT[i].rangeLow = MARKER_LOW_RANGE;
			triangleT[i].rangeHigh = (degrees[i]+ degrees[i+1])/2;
		}
		else if (i == triangleT.size() - 1) {
			triangleT[i].rangeLow = (degrees[i - 1] + degrees[i]) / 2;
			triangleT[i].rangeHigh = MARKER_HIGH_RANGE;
		}
		else {
			triangleT[i].rangeLow = (degrees[i-1] + degrees[i]) / 2;
			triangleT[i].rangeHigh = (degrees[i] + degrees[i+1]) / 2;
		}
	}

#ifdef USE_OSC
 
  sender.setup(settings.getValue("host", "localhost", 0), settings.getValue("port", 12345, 0));
#endif

}


//--------------------------------------------------------------
void touchObject::update(vector<ofPoint>touchPt,float distanceTriangle=150) {

	vector<trianglePt>tPt;

	for (int i = 0;i < touchPt.size();i++) {

		trianglePt t;
		t.paired = false;
		t.pos = touchPt[i];

		tPt.push_back(t);
	
	}

  //FIND ALL PAIRS
 
  int pairNum = 0;
  for (int i = 0;i < tPt.size();i++) {
	  for (int j = 0;j < tPt.size();j++) {
		  if (ofDist(tPt[i].pos.x, tPt[i].pos.y, tPt[j].pos.x, tPt[j].pos.y)< distanceTriangle && j != i) {

			  if (tPt[i].paired == true && tPt[j].paired == true) {
			  }
			  else {
				  pairNum++;
				  tPt[i].groupBelonging.push_back(pairNum);
				  tPt[j].groupBelonging.push_back(pairNum);
				  tPt[i].paired = true;
				  tPt[j].paired = true;
			  }
		  }
	  }
  }


  //DRAW PAIRS DEBUG
  /*
  for (int i = 0;i < tPt.size();i++) {
	  for (int i2 = 0;i2 < tPt[i].groupBelonging.size();i2++) {
		  for (int j = 0;j < tPt.size();j++) {
			  for (int j2 = 0;j2 < tPt[j].groupBelonging.size();j2++) {
			  }
		  }
	  }
  }
  */

  //GROUP PAIRS INTO POLYGONS
  vector < vector<ofPoint>> poly;
  for (int i = 0;i < tPt.size();i++) {
	  if (tPt[i].groupBelonging.size()>1) { //a point is part of two pairs at least or discarded, note 3 as it's multiplied
		  vector<ofPoint>nT;
		  for (int i2 = 0;i2 < tPt[i].groupBelonging.size();i2++) {
			  for (int j = 0;j < tPt.size();j++) {
				  for (int j2 = 0;j2 < tPt[j].groupBelonging.size();j2++) {
					  if (tPt[i].groupBelonging[i2] == tPt[j].groupBelonging[j2]) {
						  ofLine(tPt[i].pos, tPt[j].pos);
						  nT.push_back(tPt[j].pos);
					  }
				  }
			  }
		  }
		  if (nT.size() > 0) {
			  nT.push_back(tPt[i].pos);
			  poly.push_back(nT);
		  }
	  }
  }

  vector < vector<ofPoint>> polyNoDuplicate;

  for (int i = 0;i < poly.size();i++) {
	  vector<ofPoint> nP;
	  if (poly[i].size()>0) {
		  nP.push_back(poly[i][0]);
		  for (int j = 0;j < poly[i].size();j++) {
			  bool exist = false;
			  for (int k = 0;k < nP.size();k++) {
				  if (nP[k] == poly[i][j]) {
					  exist = true;
				  }
				  if (j == 0) {
					  exist = true;
				  }
			  }
			  if (exist == false) {
				  nP.push_back(poly[i][j]);
			  }
		  }
	  }
	  polyNoDuplicate.push_back(nP);
  }

  for (int i = 0;i < triangleT.size();i++) {
	  triangleT[i].visible = false;
  }

  ofFill();

  for (int i = 0;i < polyNoDuplicate.size();i++) {

	  vector<ofPoint> tTriangle;
	  if (polyNoDuplicate[i].size() == 3) {


		  triangleT[i].pos[0] = polyNoDuplicate[i][0];
		  triangleT[i].pos[1] = polyNoDuplicate[i][1];
		  triangleT[i].pos[2] = polyNoDuplicate[i][2];

		  //ofSetColor(ofColor::purple);

		  int topCircle = findTop(triangleT[i].pos);

		  ofPoint average((triangleT[i].pos[0].x + triangleT[i].pos[1].x + triangleT[i].pos[2].x) / 3, (triangleT[i].pos[0].y + triangleT[i].pos[1].y + triangleT[i].pos[2].y) / 3);
		  triangleT[i].center = average;
		  triangleT[i].apexAngle =  abs(findAngleApex(triangleT[i].pos, topCircle));
		  triangleT[i].orientation = findOrientation(triangleT[i].pos, topCircle);
		  triangleT[i].apex = triangleT[i].pos[topCircle];
		  triangleT[i].widthTri =findWidth(triangleT[i].pos, topCircle);
		  triangleT[i].heightTri = findAltitude(triangleT[i].pos, topCircle);

		  for (int j = 0;j < triangleT.size();j++) {
			  if (triangleT[i].apexAngle > triangleT[j].rangeLow &&  triangleT[i].apexAngle < triangleT[j].rangeHigh) {
				  triangleT[i].index = j;
				  triangleT[i].visible = true;
			  }
		  }
	  }
  }

  //TO DO EXTRACT TRIANGLES FROM POLYGONS

#ifdef USE_OSC
  sendOsc();
#endif



}

//--------------------------------------------------------------
void touchObject::draw() {


	//// ORIENTATION /// MAPPING VALUES
	///		b = at + b

	///360/(12 iAnzahlTage)

	///  	255 = a*-10 + b
	/// 	0 = a * 20 + b

	///		255 * 2 / 3 = b
	/// 	255 / (-30) = a

	float M_PI = 3.14159265358979323846264338327950288419716939937510582097494459;

	ofBackground(0, 0, 0);

	int q = 0;
	int markerDrawOffsetM = 175;
	//int markerDrawOffsetM_inner = 100;
	int markerDrawOffsetG = 179;
	//int markerDrawOffsetG_inner = 10;

	int markerDrawOffsetY = 375;
	int magT = 3;
	float magG = 0.3;
	float magGY = 0.4;
	int fontsize = 16;

	float degM = M_PI / 2.00;
	float degY = M_PI / 2.00;
	int m = 1;
	int mo = 1;
	int tri = 0;
	float degQ = 0.00;
	float markerDrawOffsetQ = 245;

	

	float markerDrawOffsetMonthsStr = 400;

	int offsetMonthStringY = 330;

	//if (triangleT[0].orientation < 90) {
	////	m = 0;
	//	mo = 0;
	//	ofDrawBitmapString("January", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY + 150, fontsize);
	//}
	string Monate[12];
    Monate[0] = "January";
	Monate[1] = "February";
	Monate[2] = "March";
	Monate[3] = "April";
	Monate[4] = "May";
	Monate[5] = "June";
	Monate[6] = "July";
	Monate[7] = "August";
	Monate[8] = "September";
	Monate[10] = "October";
	Monate[11] = "November";
	Monate[12] = "December";
	float degQms = 90.0;
	int fi = 0;
	
	for (int fi = 0; fi < 12; fi++) {
		
ofDrawBitmapString("Monate[fi]" , triangleT[0].center.x + cos(degQms)*(1 * magT + markerDrawOffsetQ), triangleT[0].center.y - sin(degQms)*(1 * magT + markerDrawOffsetMonthsStr), fontsize);
//ofDrawBitmapString("Monat", triangleT[0].center.x + cos(degQms)*(1 * magT + markerDrawOffsetQ), triangleT[0].center.y - sin(degQms)*(1 * magT + markerDrawOffsetQ), fontsize);

//degQms += 2 * M_PI / 12;
degQms += 2 * M_PI/12;
}
	



	if (triangleT[0].orientation > 90 && triangleT[0].orientation < 120) {
		m = 0;
		mo = 0;
		ofDrawBitmapString("January", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}
	
	if (triangleT[0].orientation > 60 && triangleT[0].orientation < 90) {
		m = 0;
		mo = 0;
		ofDrawBitmapString("Feb", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < -120) {
		m = 1;
		mo = 1;
		ofDrawBitmapString("February", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);

	}

	else if (triangleT[tri].orientation < -90) {
		m = 2;
		mo = 2;
		ofDrawBitmapString("March", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < -60) {
		m = 3;
		mo = 3;
		ofDrawBitmapString("April", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < -30) {
		m = 4;
		mo = 4;
		ofDrawBitmapString("May", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < 0) {
		m = 5;
		mo = 5;
		ofDrawBitmapString("June", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < 30) {
		m = 6;
		mo = 6;
		ofDrawBitmapString("July", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < 60) {
		m = 7;
		mo = 7;
		ofDrawBitmapString("August", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < 90) {
		m = 8;
		mo = 8;
		ofDrawBitmapString("September", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < 120) {
		m = 9;
		mo = 9;
		ofDrawBitmapString("October", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
	}

	else if (triangleT[tri].orientation < 150) {
		ofDrawBitmapString("November", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
		m = 10;
		mo = 10;
	}

	else if (triangleT[tri].orientation < 180) {
		ofDrawBitmapString("December", triangleT[0].center.x, triangleT[0].center.y + offsetMonthStringY, fontsize);
		m = 11;
		mo = 11;
	}


	else { m = 12; }

	
	cout << triangleT[0].orientation << endl;





	////DRAW MONTH////---------------------------------------------------------------------------------------------------------------------
	

	/// NUMBERS DAYS /////



for (int q = 0; q < iAnzahlTage[m]; q++) {

	stringstream ss;
	ss << q;
	string str = ss.str();
	ofDrawBitmapString(ss.str() , triangleT[0].center.x + cos(degQ)*(1 * magT + markerDrawOffsetQ), triangleT[0].center.y - sin(degQ)*( 1 * magT + markerDrawOffsetQ), fontsize);

	degQ += 2 * M_PI / iAnzahlTage[m];

}

///  YEAR //// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
for (int mon = 0; mon < 12; mon++) {

for (int tag = 0; tag < 30; tag++) {

	for (int std = 0; std < 24; std++) {


		float r_T = 255 / 33 * Temperatur[mon][tag][std] + 235 * 2 / 9;
		float g_T = 255 / (-45.00)*Temperatur[mon][tag][std] + 245 * 2 / 3;
		//float g_T = 255 / (-65.00)*Temperatur[mon][tag][std] + 245 * 2 / 3;
		float b_T = 255 / (-45.0)*Temperatur[mon][tag][std] + 255 * 2 / 3;
		float a_T = 255 / -66 * Temperatur[mon][tag][std] + 255 * 2 / 3;

		/////DRAW GLOBALSTRAHLUNG
		ofSetColor(255, 255, 255, 80); ///YELLOW
		ofSetLineWidth(1);
		ofLine(triangleT[0].center.x + cos(degY)*(0.2 * magGY + markerDrawOffsetY), triangleT[0].center.y - sin(degY)* (0.2 * magGY + markerDrawOffsetY), triangleT[0].center.x + cos(degY)*(Globalstrahlung[mon][tag][std] * magGY + markerDrawOffsetY), triangleT[0].center.y - sin(degY)*(Globalstrahlung[mon][tag][std] * magGY + markerDrawOffsetY)); ////// 'SUN RAYS'

		ofSetColor(r_T, g_T, b_T);
		ofSetLineWidth(4);
		ofCircle(triangleT[0].center.x + cos(degY)*(Temperatur[mon][tag][std] * magT + markerDrawOffsetY), triangleT[0].center.y - sin(degY)*(Temperatur[mon][tag][std] * magT + markerDrawOffsetY), 0.5, 0.5);

		ofSetColor(r_T, g_T, b_T, a_T);
		ofLine(triangleT[0].center.x + cos(degY)*(Temperatur[mon][tag][std] * magT + markerDrawOffsetY), triangleT[0].center.y - sin(degY)*(Temperatur[mon][tag][std] * magT + markerDrawOffsetY), triangleT[0].center.x + cos(degY)*(1 * magT + markerDrawOffsetY), triangleT[0].center.y - sin(degY)*(1 * magT + markerDrawOffsetY));
		ofSetLineWidth(1.0);  // Line widths apply to polylines

		degY += 2 * M_PI / (24.00 * 365.00);
	}
}


}




/// MONTH  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	for (int j = 0; j < 30; j++) {

		for (int i = 0; i < 24; i++) {


			float r_T = 255 / 33 * Temperatur[m][j][i] + 235 * 2/9;
			float g_T = 255 / (-45.00)*Temperatur[m][j][i] + 245 * 2 / 3;
			float b_T = 255 / (-45.0)*Temperatur[m][j][i] + 255 * 2 / 3;
			float a_T = 255 / -66 * Temperatur[m][j][i] + 255 * 2/3;



			//float r_T = 255 / 10 * Temperatur[m][j][i] + 50 / 3;
			//float g_T = 255 / (9)*Temperatur[m][j][i] + 180 * 2 / 3;
			//float b_T = 255 / (-45.0)*Temperatur[m][j][i] + 255 * 2 / 3;
	
			/////DRAW GLOBALSTRAHLUNG
			ofSetColor(255, 255, 255, 150); ///YELLOW
			ofSetLineWidth(1);
			ofLine(triangleT[0].center.x + cos(degM)*(1* magG + markerDrawOffsetG), triangleT[0].center.y - sin(degM)* ( 1 * magG + markerDrawOffsetG), triangleT[0].center.x + cos(degM)*(Globalstrahlung[m][j][i] * magG + markerDrawOffsetG), triangleT[0].center.y - sin(degM)*(Globalstrahlung[m][j][i] * magG + markerDrawOffsetG)); ////// 'SUN RAYS'
			//ofSetColor(0);
			//ofCircle(triangleT[0].center.x, triangleT[0].center.y, 200, 200);
			//ofLine(triangleT[0].center.x + cos(degM)* magT + markerDrawOffsetG, triangleT[0].center.y - sin(degM)* magT + markerDrawOffsetG, triangleT[0].center.x + cos(degM)*(Globalstrahlung[m][j][i] * magG + markerDrawOffsetG), triangleT[0].center.y - sin(degM)*(Globalstrahlung[m][j][i] * magG + markerDrawOffsetG));
			// Define the circle in X and Y coordinates40
			ofSetColor(r_T, g_T, b_T);
			ofSetLineWidth(4);
			//// MONTH////
			//ofCircle(ofGetMouseX() + cos(degM)*(Temperatur[m][j][i]* magT + markerDrawOffsetM), ofGetMouseY() - sin(degM)*(Temperatur[m][j][i]* magT + markerDrawOffsetM), 2, 2);
			ofCircle(triangleT[0].center.x + cos(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM), 2.5,2.5);

			ofSetColor(r_T, g_T, b_T, a_T);
			//ofLine(triangleT[0].center.x + cos(degM)*(50 * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(50 * magT + markerDrawOffsetM), triangleT[0].center.x + cos(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM));  ////// LINE TEMPERATURES
			ofLine(triangleT[0].center.x + cos(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM), triangleT[0].center.x + cos(degM)*(1 * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(1 * magT + markerDrawOffsetM));

			//ofLine(triangleT[0].center.x + cos(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(50 * magT + markerDrawOffsetM), triangleT[0].center.x + cos(degM)*(50 * magT + markerDrawOffsetM), triangleT[0].center.y - sin(degM)*(Temperatur[m][j][i] * magT + markerDrawOffsetM));
			ofSetLineWidth(1.0);  // Line widths apply to polylines

			////BLUE/// MAPPING VALUES
			///		b = at + b
			
			///  	255 = a*-10 + b
			/// 	0 = a * 20 + b

			///		255 * 2 / 3 = b
			/// 	255 / (-30) = a
			
			////RED/// MAPPING VALUES
			///             r = at + b 
			/// 			255 = a* 45 + b
			///             0 = a * 5 + b
           ///
			///	255 = -8b--> b = -255 / 8
			///	255 = 40a--> a = 255 / 40



			
			
			


			degM += 2 * M_PI / (24.00 * iAnzahlTage[m]) ;
		}
	}

	


	//// DAY /// ----------------------------------------------------------------------------------------
	int magnitudeD = 3;
	int markerDrawOffsetD = 50;
	int markerDrawOffsetGD = 55;
	//int mo = 7;
	int da = 9;
	float degD =  M_PI/2;
	float magnitudeGD = 0.15;


	ofPath path;


	int idd = 0;
	
	//cmath cos function takes RADIANS not DEGREES - so we need to convert it 
		for (int id = 0, ida= -1; id < 24, ida < 23; id++, ida++) {

		
			

			
			ofCircle(triangleT[0].center.x + cos(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetGD), triangleT[0].center.y - sin(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetGD), 2, 2);
			
			//ofSetColor(255 / 40 * Temperatur[mo][da][id] - 255 / 8, 75, 255 / (-30.0)*Temperatur[mo][da][id] + 255 * 2 / 3);

			/////DRAW GLOBALSTRAHLUNG
			ofSetColor(255, 255, 255, 255); ///YELLOW
			ofSetLineWidth(1);
			ofLine(triangleT[0].center.x + cos(degD)*(0.2 * magnitudeGD + markerDrawOffsetGD), triangleT[0].center.y - sin(degD)* (0.2 * magnitudeGD + markerDrawOffsetGD), triangleT[0].center.x + cos(degD)*(Globalstrahlung[mo][da][id] * magnitudeGD + markerDrawOffsetGD), triangleT[0].center.y - sin(degD)*(Globalstrahlung[mo][da][id] * magnitudeGD + markerDrawOffsetGD)); ////// 'SUN RAYS'


			ofSetColor(255, 255, 255, 70); ///YELLOW
			ofSetLineWidth(9);
			ofLine(triangleT[0].center.x + cos(degD)*(0.2 * magnitudeGD + markerDrawOffsetGD), triangleT[0].center.y - sin(degD)* (0.2 * magnitudeGD + markerDrawOffsetGD), triangleT[0].center.x + cos(degD)*(Globalstrahlung[mo][da][id] * magnitudeGD + markerDrawOffsetGD), triangleT[0].center.y - sin(degD)*(Globalstrahlung[mo][da][id] * magnitudeGD + markerDrawOffsetGD)); ////// 'SUN RAYS'

			float r_T = 255 / 33 * Temperatur[m][j][id] + 235 * 2 / 9;
			float g_T = 255 / (-45.00)*Temperatur[m][j][id] + 245 * 2 / 3;
			float b_T = 255 / (-45.0)*Temperatur[m][j][id] + 255 * 2 / 3;
			float a_T = 255 / -66 * Temperatur[m][j][id] + 255 * 2 / 3;


			ofSetColor(r_T, g_T, b_T, 100);
			ofLine(triangleT[0].center.x + cos(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetD), triangleT[0].center.y - sin(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetD), triangleT[0].center.x + cos(degD+ 2 * M_PI / 24.00)*(Temperatur[mo][da][id+1] * magnitudeD + markerDrawOffsetD), triangleT[0].center.y - sin(degD+ 2 * M_PI / 24.00)*(Temperatur[mo][da][id+1] * magnitudeD + markerDrawOffsetD));  ////// LINE TEMPERATURES
			ofSetLineWidth(8);
			//ofLine(triangleT[0].center.x + cos(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetD), triangleT[0].center.y - sin(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetD), triangleT[0].center.x + cos(degM)*(1 * magnitudeD + markerDrawOffsetD), triangleT[0].center.y - sin(degD)*(1 * magT + markerDrawOffsetD));


			ofSetColor(r_T, g_T, b_T, 255);
			ofSetLineWidth(1);
			ofLine(triangleT[0].center.x + cos(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetD), triangleT[0].center.y - sin(degD)*(Temperatur[mo][da][id] * magnitudeD + markerDrawOffsetD), triangleT[0].center.x + cos(degD + 2 * M_PI / 24.00)*(Temperatur[mo][da][id + 1] * magnitudeD + markerDrawOffsetD), triangleT[0].center.y - sin(degD + 2 * M_PI / 24.00)*(Temperatur[mo][da][id + 1] * magnitudeD + markerDrawOffsetD));  ////// LINE TEMPERATURES
			


			degD +=  2*M_PI / 24.00;


		}
	
	
	///// --------------------------------------------------------------------------------------------

		



	//radialc.draw(450, 0);

	

	
	ofRectangle myRect(0, 0, 75, 100);
	
	ofSetColor(ofColor::greenYellow);

  for (int i = 0;i < triangleT.size();i++) {

	  if (triangleT[i].visible ) {


		  			  ofSetColor(ofColor::blue);
		  			  //ofDrawTriangle(triangleT[i].pos[0], triangleT[i].pos[1], triangleT[i].pos[2]);


		ofSetColor(ofColor::whiteSmoke);
		ofPoint dirNorm = triangleT[i].apex - triangleT[i].center ;
		dirNorm.normalize();
		  
		ofPoint scaledPt = triangleT[i].apex+(dirNorm.scale(5));
		ofPoint scaledPtText = triangleT[i].apex + (dirNorm.scale(5));
		ofDrawArrow(triangleT[i].center, scaledPt,10);
		ofDrawBitmapStringHighlight(ofToString(triangleT[i].index), scaledPtText);

	  }
  }
}

//--------------------------------------------------------------
void touchObject::sendOsc() {
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
		
			ofxOscMessage m;
			m.setAddress("/object/");
			m.addStringArg("  index: ");
			m.addIntArg(triangleT[i].index);			
			m.addStringArg("  x: ");
			m.addFloatArg(triangleT[i].center.x);
			m.addStringArg("  y: ");
			m.addFloatArg(triangleT[i].center.y);
			m.addStringArg("  w: ");
			m.addIntArg(triangleT[i].widthTri);
			m.addStringArg("  h: ");
			m.addIntArg(triangleT[i].heightTri);
			m.addStringArg("  angle: ");
			m.addIntArg(triangleT[i].orientation);

			sender.sendMessage(m, false);
		}
	}
}

vector<ofPoint> touchObject::getCenter()
{
	vector<ofPoint>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].center);
		}
	}
	return c;
}

vector<float> touchObject::getWidth()
{
	vector<float>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].widthTri);
		}
	}
	return c;
}

vector<float> touchObject::getHeight()
{
	vector<float>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].heightTri);
		}
	}
	return c;
}

vector<float> touchObject::getAngle()
{
	vector<float>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].orientation);
		}
	}
	return c;
}

vector<int> touchObject::getIndex()
{
	vector<int>c;
	for (int i = 0;i < triangleT.size();i++) {
		if (triangleT[i].visible) {
			c.push_back(triangleT[i].index);
		}
	}
	return c;
}

//--------------------------------------------------------------
int  touchObject::findTop(ofPoint pt[3]) {

	float dist[3];
	dist[0] = ofDist(pt[0].x, pt[0].y, pt[1].x, pt[1].y);
	dist[1] = ofDist(pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	dist[2] = ofDist(pt[0].x, pt[0].y, pt[2].x, pt[2].y);

	float dist0to1 = dist[0];
	float dist1to2 = dist[1];
	float dist0to2 = dist[2];

	float diff01m02 = abs(dist0to1 - dist0to2);
	float diff02m12 = abs(dist0to2 - dist1to2);
	float diff01m12 = abs(dist0to1 - dist1to2);

	if (diff01m02 < diff02m12 && diff01m02 < diff01m12) {
		return 0;
	}
	else if (diff01m12<diff01m02 && diff01m12<diff02m12) {
		return 1;
	}
	else if (diff02m12<diff01m02 && diff02m12<diff01m12) {
		return 2;
	}

}

//--------------------------------------------------------------
float  touchObject::findOrientation(ofPoint pt[3], int top) {

	ofPoint a;
	ofPoint b;
	ofPoint c;
	b = pt[top];
	if (top == 0) {
		a = pt[1];
		c = pt[2];
	}
	else if (top == 1) {
		a = pt[0];
		c = pt[2];
	}
	else if (top == 2) {
		a = pt[0];
		c = pt[1];
	}

	ofPoint middlePt(ofLerp(a.x, c.x, 0.5), ofLerp(a.y, c.y, 0.5));

	ofPoint diff = b - middlePt;

	diff.normalize();

	return 	ofRadToDeg(atan2(diff.x, diff.y))*-1;

}

//--------------------------------------------------------------
float  touchObject::findAltitude(ofPoint pt[3], int top) {

	ofPoint a;
	ofPoint b;
	ofPoint c;
	b = pt[top];
	if (top == 0) {
		a = pt[1];
		c = pt[2];
	}
	else if (top == 1) {
		a = pt[0];
		c = pt[2];
	}
	else if (top == 2) {
		a = pt[0];
		c = pt[1];
	}

	ofPoint middlePt(ofLerp(a.x,c.x, 0.5), ofLerp(a.y, c.y, 0.5));

	return 	ofDist(middlePt.x, middlePt.y, b.x, b.y);
}

//--------------------------------------------------------------
float  touchObject::findAngleApex(ofPoint pt[3], int top) {

		ofPoint a;
		ofPoint b;
		ofPoint c;
		b = pt[top];
		if (top == 0) {
			a = pt[1];
			c = pt[2];
		}
		else if (top == 1) {
			a = pt[0];
			c = pt[2];
		}
		else if (top == 2) {
			a = pt[0];
			c = pt[1];
		}

	ofPoint ab = { b.x - a.x, b.y - a.y };
	ofPoint cb = { b.x - c.x, b.y - c.y };

	float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
	float cross = (ab.x * cb.y - ab.y * cb.x); // cross product

	float alpha = atan2(cross, dot);

	return (int)floor(alpha * 180. / PI + 0.5);

}

//--------------------------------------------------------------
float  touchObject::findWidth(ofPoint pt[3], int top) {

	ofPoint a;
	ofPoint b;
	ofPoint c;
	b = pt[top];
	if (top == 0) {

		a = pt[1];
		c = pt[2];

	}
	else if (top == 1) {

		a = pt[0];
		c = pt[2];

	}
	else if (top == 2) {

		a = pt[0];
		c = pt[1];

	}

	return ofDist(a.x, a.y, c.x, c.y);

}