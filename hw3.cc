#include <iostream>
#include <vector>
#include <time.h> 
#include <unistd.h>
#include <fstream>
using namespace std;

vector<string> readInC();
vector<string> readInF(istream &file);
string formatIt(string outFmt, int year, int day, int mnth = -1);
string decipher(string str, string fmt, string err);
string localDays(string fmt, string outFmt);
string isoFmter(string fmt, string outFmt, string err);
string dotDay(string str, string outFmt, string err);
bool isValidForm(string str);
string strToLower(string str);
bool isLepYr(int year);

string prgmName;

int main(int argc, char *argv[]) {
	prgmName = argv[0];
	bool v = false, iso = false, f = false;
	// Defalut format looks somethign like this Wed Feb 12 2020
	string outFmt, err = "", erk = "(File: ";
	char op;

        while((op = getopt(argc, argv, "+vif:"))!= -1){
		switch(op){
			case'i':
				iso = true;
				break;
                        case'v':
                                v = true;
                                break;
                        case'f':
                                f = true;
                                if(!outFmt.empty()){
                                        cerr << prgmName << " - ERROR: invalid multiple f formating options detected\n";
					exit(1);
                                }
                                outFmt = optarg;
                                break;
                        default:
                                exit(1);

                }
        }

	if(iso){
		outFmt = "%04Y-%m-%d";
	}
	if(outFmt.empty()){
		outFmt = "%a %b %d %04Y";
	}
        if(f && iso){
		cerr << prgmName << " - ERROR: invalid two different formats detected in the input\n";
                exit(1);
        }
	vector<string> dates;
	if(optind == argc){
		dates = readInC();
		if(v){
			cout << "*** Processing standard input" << '\n';
		}
		for(int i = 0; i < int(dates.size()); i++){
			cout << decipher(dates[i], outFmt, err) << '\n'; 
		}
	}else{
		for(int i = optind; i < argc; i++){
			ifstream file(argv[i]);
			dates = readInF(file);
			err = erk+ argv[i] + ")";
			if(v){
				cout << "*** Processing " << argv[i] << '\n';
			}
			for(int j = 0; j < int(dates.size()); j++){
				cout << decipher(dates[j], outFmt, err) << '\n';
			}
		}
	}
	return 0;
}
vector<string> readInF(istream &file){
	string sline;
        vector<string> data;
        while(getline(file, sline)){
                data.push_back(sline);
        }
        if(data.empty()){
                cerr << prgmName << " - Error: " << "No Values Found" << "\n";
                exit (1);
        }
        return data;
}
vector<string> readInC(){
	string sline;
	vector<string> data;
	while(getline(cin, sline)){
                data.push_back(sline);
        }
        if(data.empty()){
                cerr << prgmName << " - Error: " << "No Values Found" << "\n";
                exit (1);
        }
        return data;
}
string formatIt(string outFmt, int year, int day, int mnth){
	struct tm timeinfo = {};
	char buff[1000];
	if(mnth != -1){
		timeinfo.tm_mon = mnth;
	}
  	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mday = day;

  	mktime(&timeinfo);
	strftime(buff,1000,outFmt.c_str(),&timeinfo);
	outFmt = buff;
	return outFmt;	
}
string decipher(string strIn, string fmt, string err){
	string orig = strIn;
	strIn = strToLower(strIn);
	if(strIn.size() < 3){
		cerr << prgmName << " - ERROR"<<err<<": data entry incomplete in line -> " << strIn << '\n';
		exit(1);
	}
	//Entering here means we know it just wants one of the days now, prior, or next
	if(strIn.compare("today") == 0 || strIn.compare("yesterday") == 0 || strIn.compare("tomorrow") == 0){
		return localDays(strIn, fmt);			
	}
	//Entering here means there is potential for it to be given as year-month-day
	if(strIn.size() == 10 && strIn[4] == '-' && strIn[7] == '-'){
		return isoFmter(orig, fmt, err);
	}
	//Anything else must be in the formate year.day
	return dotDay(orig, fmt, err);
}
string localDays(string fmt, string outFmt){
	time_t rawtime;
        struct tm timeinfo;
  	time (&rawtime);
  	timeinfo = *localtime (&rawtime);
	if(fmt.compare("today")==0){
		return formatIt(outFmt, timeinfo.tm_year + 1900, timeinfo.tm_yday + 1);
	}
	if(fmt.compare("yesterday")==0){
		return formatIt(outFmt, timeinfo.tm_year + 1900, timeinfo.tm_yday);
	}
	if(fmt.compare("tomorrow")==0){
		return formatIt(outFmt, timeinfo.tm_year + 1900, timeinfo.tm_yday + 2);
	}
	return fmt;
}
string isoFmter(string fmt, string outFmt, string err){
	int day, mnth, year;
	for(int i = 0; i < int(fmt.size()); i++){
		if(!(isdigit(fmt[i]) || fmt[i] == '-')){
			cerr << prgmName << " - ERROR"<<err<<": invalid iso input format in this line -> "<< fmt << '\n';
			exit(1);
		}
	}
	try{
		day = stoi(fmt.substr(8,10));
		mnth = stoi(fmt.substr(5,7));
		year = stoi(fmt.substr(0,4));
	}catch(const std::exception& e){
		cerr << prgmName << " - ERROR"<<err<<": invalid iso input format in this line -> " << fmt << '\n';
		exit(1);
	}
	if(mnth > 12 || mnth < 1){
		cerr << prgmName << " - ERROR"<<err<<": invalid iso input(month out of range) format in this line -> " << fmt << '\n';
		exit(1);
	}
	if(year > 9999 || year < 1){
		cerr << prgmName << " - ERROR"<<err<<": invalid iso input(year out of range) format in this line -> " << fmt << '\n';
                exit(1);	
	}
	int leap = 0;
	if(isLepYr(year)){leap = 1;}
	int mnthNum[] = {31,28+leap,31,30,31,30,31,31,30,31,30,31};
	if(mnthNum[mnth - 1] < day || day < 0){
		cerr << prgmName << " - ERROR"<<err<<": invalid iso input(day out of range for month) format in this line -> " << fmt << '\n';
		exit(1);	
	}
	
	return formatIt(outFmt, year, day, mnth - 1);
}
string dotDay(string str, string outFmt, string err){
	int year, day, leap;
	if(!(isValidForm(str))){
		cerr << prgmName << " - ERROR"<<err<<": invalid year.day input format in this line -> " << str << '\n';
                exit(1);
	}
	size_t index = str.find('.');
	try{
		year = stoi(str.substr(0, index));
		day = stoi(str.substr(index + 1, str.size()));
	}catch(const std::exception& e){
		cerr << prgmName << " - ERROR"<<err<<": invalid year.day input format in this line -> " << str << '\n';
		exit(1);
	}
	if(year > 9999 || year < 1){
		cerr << prgmName << " - ERROR"<<err<<": year out of range in this line -> " << str << '\n';
		exit(1);
	}
	leap = 0;
	if(isLepYr(year)){
		leap = 1;
	}
	if(day > 365 + leap || day < 1){
		cerr << prgmName << " - ERROR"<<err<<": day out of range in this line -> " << str << '\n';
		exit(1);
	}
	return formatIt(outFmt, year, day);
}
bool isValidForm(string str){
        int dotCnt = 0;
        size_t search = str.find('.');
        if(search == string::npos){return false;}
        for(int i = 0; i < int(str.length()); i++){
                char s = str[i];
                if(!((s >= '0' && s <= '9') || s == '.')){
                        return false;
                }if(s == '.'){
                        dotCnt++;
                }
        }
        if(dotCnt > 1){
                return false;
        }
        return true;
}
string strToLower(string str){
	for(int i = 0; i < int(str.size()); i++){
		str[i] = tolower(str[i]);
	}
	return str;
}
bool isLepYr(int year){
        if(year%4 == 0){
                if(year%100 == 0){
                        if(year%400 == 0){
                                return true;
                        }
                        return false;
                }
                return true;
        }
        return false;
}
