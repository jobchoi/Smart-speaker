#include <iostream>
#include <thread>
#include <string>
#include <string.h>
#include <fstream>
#include <Python.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 

#define SHM_KEY   0x12345 
#define MEMORY_SIZE 200

#define PARSING "0"
#define YOUTUBE "1"
#define WEATHER "2"
#define CALENDAR "3"

using namespace std;

char* google_string;
int mutex = 0;
int countdown_recoding = 0;
int Interrup = 0;

void InteruptFunc()	{
	while(1)	{
		cin >> Interrup;
		if (Interrup == 1)
			google_string = "종료";
	}
}

int SttFunc()	{
	std::cout << "start stt" << std::endl;

	PyObject *pName, *pModule;	//for load python script
	PyObject *pgoogle_stt;	//for python functions
	PyObject *g_pArgs, *g_pValue;	//for get/set python function parameters
	Py_Initialize();
	//start set path
	PyObject *sys = PyImport_ImportModule("sys");
	PyObject *path = PyObject_GetAttrString(sys, "path");
	PyList_Append(path, PyString_FromString("."));
	
	//get python script
	pName = PyString_FromString("stt");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	
	if (pModule != NULL)
	{
		//module load
		pgoogle_stt = PyObject_GetAttrString(pModule, "google_stt");	
		//module load error check
        if ( !(pgoogle_stt && PyCallable_Check(pgoogle_stt)) )
        {
            if (PyErr_Occurred()) PyErr_Print();
            std::cout << "Cannot find function 'google_stt'" << std::endl;
        }
        g_pArgs = PyTuple_New(100); //make parameter list
		g_pArgs = PyObject_CallObject(pgoogle_stt, NULL);
		cout << "before stt API" << endl;
		//google stt API
		google_string = PyString_AsString(g_pArgs);
		cout << "after stt API" << endl;
	}
	else
	{
		PyErr_Print();
		std::cout << "Failed to load 'pModules'" << std::endl;
	}
	Py_Finalize();
	return 0;
}

int SttFunc_repeat()    {
    while(1)    {
        if (mutex == 0) {
            cout << "main record start" << endl;
            system("./record_to_wav_level_check");
			//start google stt func
	        SttFunc();
			system("rm input.wav");			
//			google_string = "레미제라블 노래 틀어줘 파이야 ";
            std::cout << "google_stt : " << google_string << std::endl;
			
            mutex = 1;
        }
		else 
			sleep(1);
		if (google_string == "종료")
            break;
    }
    return 0;
}

int main (int argc, char *const argv[])
{
	int shmid, Fornum;
	char *Function;
	char *String;
	string FunctionS;
	string YOUTUBEsearch;
	string YOUTUBEfullURL;
	string YOUTUBEpartURL;
	char YOUTUBEomxplayer[1500];
	char YOUTUBEomxplayer2[1500];
	int fornum = 0, YOUTUBEparsing1 = 0, YOUTUBEparsing2 = 0;
	int YOUTUBEend;

	shmid = shmget((key_t)SHM_KEY, (size_t)MEMORY_SIZE, 0777 | IPC_CREAT); 
	if(shmid == -1) {
		perror("shmget( )");
		return -1; 
	}   

	Function = (char*)shmat(shmid, NULL, 0);
	if(Function == (void *)-1) {
		perror("shmat( )");
		return -1; 
	}   
	
	String = Function +1;

	//interupt
//    thread t_interupt(&InteruptFunc);
    // start Stt repeat
    thread t_stt(&SttFunc_repeat);

	//Text Parsing repeat
	while(1)	{
		if (google_string == "종료")
		break;
		if (mutex == 1)	{
			cout << "Main Start PLZ" << endl;
			//input SHM value
			cout << "Function : " << Function[0] << endl;
            cout << "String : " << String << endl;
			strcpy(Function,PARSING);
			strcpy(String,google_string);

			//parsing
			system("./parsing");
			cout << "Main parsingend String : " << String <<endl;

			FunctionS = Function;
			YOUTUBEsearch = Function;
			FunctionS.erase(1);
			cout << "Main FunctionS : " << FunctionS <<endl;

			//start YOUTUBE
			if (strcmp(FunctionS.c_str(),"1") ==0 )  {
				cout << "Main Youtube start" <<endl;
				
				ofstream outFile("youtube_search.txt");
				outFile << YOUTUBEsearch << endl;
				outFile.close();
				system("./youtubesearch.pyc"); //make play list youtube_feed.txt
				//YOUTUBE - DL Start
				YOUTUBEfullURL = "https://www.youtube.com/watch?v=";
				std::fstream YOUTUBEfile("youtube_feed.txt");
				if(YOUTUBEfile.fail())	{
					cout << "File open Fail" << endl;
				}

				//play YOUTUBE Number
//				while(!YOUTUBEfile.eof())	{
					getline(YOUTUBEfile,YOUTUBEpartURL);
//					cout << fornum << "YOUTUBEpartURL : " << YOUTUBEpartURL << endl;
//					fornum++;
//				}
				YOUTUBEfile.close();https://www.youtube.com/watch?v=RDtvoUZ4kD0
				YOUTUBEend = YOUTUBEpartURL.length();
				YOUTUBEpartURL = YOUTUBEpartURL.substr(YOUTUBEend-12,11);
				YOUTUBEfullURL = YOUTUBEfullURL + YOUTUBEpartURL;
				YOUTUBEfullURL = "sudo youtube-dl -g -f 18 " + YOUTUBEfullURL + " > youtube_omxplayer.txt";
				strcpy(YOUTUBEomxplayer,YOUTUBEfullURL.c_str());
				system(YOUTUBEomxplayer);

				//YOUTUBE omxplayer Start
				std::fstream YOUTUBEfile2("youtube_omxplayer.txt");
                if(YOUTUBEfile2.fail())  {
                    cout << "File open Fail" << endl;
                }
				getline(YOUTUBEfile2,YOUTUBEfullURL);
				YOUTUBEfile2.close();
				YOUTUBEfullURL = "omxplayer -b " + YOUTUBEfullURL;
				strcpy(YOUTUBEomxplayer2,YOUTUBEfullURL.c_str());
					//omxplayer URL parsing
				while(YOUTUBEomxplayer2[YOUTUBEparsing1])	{
					if(strchr("?&=",YOUTUBEomxplayer2[YOUTUBEparsing1]))	{
						YOUTUBEomxplayer[YOUTUBEparsing2++] = '\\';
					}
					YOUTUBEomxplayer[YOUTUBEparsing2++] = YOUTUBEomxplayer2[YOUTUBEparsing1++];
				}
				YOUTUBEomxplayer[YOUTUBEparsing2] = '\0';
				YOUTUBEparsing1 = 0; YOUTUBEparsing2 = 0;
				system(YOUTUBEomxplayer);
			}
			//shm read
			cout << "clear" <<endl;			
			mutex = 0;
		}
	}

	shmctl(shmid, IPC_RMID, 0);
//	t_interupt.join();
	t_stt.join();
	return 0;
}
