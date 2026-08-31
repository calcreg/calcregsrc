
/* Includes */
#include <windows.h>			// system
#include <stdio.h>
#include    <stdlib.h>
//#include    <string.h>
#include    <commdlg.h>


//serial-Oscillo
#define OscilloSize 1000
#define DataToSendSize 10

int  waitfordata(int NbrTrialToGet);
float Oscilloscope(float a);
void GetOscilloData(int info);
void PutSerialData();
HANDLE OpenAndPrepareSerial(char *COM_Name);

extern void PrintCmd(char * s) ;
void Wait(int Time);
extern int CheckForBreak();


//Serial
int SerialReady=0; //Serial device opened =1, closed =0
unsigned char Oscillo[2*OscilloSize];//2* because of the several trails to get the data
unsigned char SerialDataToSend[DataToSendSize];//data to send
int SerialOpened=0;

int  NbrTrialSerial=20; //Nbr of trials to get the full Tablesize data loaded
int BaudRate = 28800;
int SerFlag = 0;

extern int BreakActivated;
extern int StopProgram;

char msg_s[100]; // This is a msg char for display error messages

//Serial Device PC
HANDLE hSerialPC;




void Wait(int Time){
	int nbrticks,nbrticks2;
	/*
	nbrticks=TimGetTicks();
	nbrticks2=nbrticks;
	while (nbrticks2<nbrticks+Time){
	if (BreakActivated==1) if( CheckForBreak() == 1) return;
	nbrticks2=TimGetTicks();}
	*/
	PrintCmd ("Wait not yet implemented\n");
	}



float Oscilloscope(float a){
	if (a > OscilloSize) {PrintCmd("Serial Data Overflow index!\n"); return 0;}
	if (SerialReady ==0) {PrintCmd("Serial Device not opened!\n"); return 0;}
	//return  5.0*(float)Oscillo[(int) a]/255.0 - 2.5;
	return  (float)Oscillo[(int) a];
}

void GetOscilloData(int SizeToRead){
			// Open the Serial Port
	if (SerialOpened==0){ //Always do here
		hSerialPC = OpenAndPrepareSerial("COM9");
		if (hSerialPC==0) return;	
	SerialOpened=1;
	}

//***************Read Operation******************//
    DWORD dwBytesRead = 0;
    int nread = OscilloSize;
	int reallyread=0;
	int NbrTrials=0;
	while(reallyread<OscilloSize){
	NbrTrials++;
	if (StopProgram ==1){
		PrintCmd("Acquisition Aborded\n");
		CloseHandle(hSerialPC);
		return ;
	}
	
	if (NbrTrials>10){
		PrintCmd("Exceeded 10 Cycles for Serial reading\n");
		PrintCmd(" Data Acquisition\n   -- problem! --\n");
		CloseHandle(hSerialPC);
		return ;
	}
    if (!ReadFile(hSerialPC, Oscillo+reallyread, nread, &dwBytesRead, NULL)) 
    {
        PrintCmd("error reading from input buffer \n");
		PrintCmd("  Data Acquisition\n   -- problem! --\n");
		if (SerialReady == 1) PrintCmd("keeping old data\n");
		CloseHandle(hSerialPC);
		return ;
	}
    else {
		reallyread=dwBytesRead+reallyread;
	}
	}
	if (SizeToRead!=0){
	sprintf(msg_s,"Nbr Bytes read is: %d\n",reallyread);
	PrintCmd(msg_s);
	}
	SerialReady = 1; //here it means that OSC(x) has got the data	
}

HANDLE OpenAndPrepareSerial(char *COM_Name){
    HANDLE hSerial;
    COMMTIMEOUTS timeouts;
    COMMCONFIG dcbSerialParams;
   // hSerial = CreateFile("COM9",GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

   hSerial = CreateFile(COM_Name,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 
    if ( hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND){
            sprintf(msg_s,"serial port %s does not exist\n",COM_Name);
			PrintCmd(msg_s);
        }
        PrintCmd("some other errors occured.\n");
    }
 
 
    //DCB    dcbSerialParams ;
    //GetCommState( hSerial, &dcbSerialParams.dcb);
    if (!GetCommState(hSerial, &dcbSerialParams.dcb)) {
        PrintCmd("Serial error getting state \n");
		CloseHandle(hSerial);
		return 0;
    }
/* from PALMOS program
 // SerFlag = 0	8bits; 
//				=1	8bits|parityOdd;
//				=2	8bits|parityEven
//				=3	8bits|parityEven|BitStop1
//				=4	8bits|parityOdd|bitStop1|AutoM-RTS-CTS
//				=5	8bits|parityEven|bitStop1|AutoM-RTS-CTS


	if (SerFlag==0) sstSetup.flags = serSettingsFlagBitsPerChar8;
	if (SerFlag==1) sstSetup.flags = serSettingsFlagBitsPerChar8 | serSettingsFlagParityOnM;		
	if (SerFlag==2) sstSetup.flags = serSettingsFlagBitsPerChar8 |
													serSettingsFlagParityOnM | serSettingsFlagParityEvenM;		
	if (SerFlag==3){ sstSetup.flags = serSettingsFlagBitsPerChar8 |
													serSettingsFlagParityOnM | serSettingsFlagParityEvenM|
														serSettingsFlagStopBits1;
							sstSetup.ctsTimeout = SysTicksPerSecond() / 2;}
	if (SerFlag==4){ sstSetup.flags = serSettingsFlagBitsPerChar8 |
													serSettingsFlagParityOnM | serSettingsFlagParityEvenM|
														serSettingsFlagStopBits1 | serSettingsFlagRTSAutoM |
															serSettingsFlagCTSAutoM;
							sstSetup.ctsTimeout = SysTicksPerSecond() / 2;}
	if (SerFlag==5){ sstSetup.flags = serSettingsFlagBitsPerChar8 |
													serSettingsFlagParityOnM |
														serSettingsFlagStopBits1 | serSettingsFlagRTSAutoM |
															serSettingsFlagCTSAutoM;
							sstSetup.ctsTimeout = SysTicksPerSecond() / 2;}
								
	
//Example and description
//SerSettingsType serSettings;
//serSettings.baudRate = 19200;
//serSettings.flags = serSettingsFlagBitsPerChar8 |
//serSettingsFlagParityOnM | serSettingsFlagParityEvenM |
//serSettingsFlagStopBits1 | serSettingsFlagRTSAutoM |
//serSettingsFlagCTSAutoM;
//serSettings.ctsTimeout = SysTicksPerSecond() / 2;
//err = SerSetSettings(refNum, &serSettings);

*/
    dcbSerialParams.dcb.DCBlength = sizeof(dcbSerialParams.dcb);
    dcbSerialParams.dcb.BaudRate = CBR_9600;
    dcbSerialParams.dcb.ByteSize = 8;
    dcbSerialParams.dcb.StopBits = ONESTOPBIT;
    dcbSerialParams.dcb.Parity = NOPARITY;
 
    dcbSerialParams.dcb.fBinary = TRUE;
    dcbSerialParams.dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcbSerialParams.dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcbSerialParams.dcb.fOutxCtsFlow = FALSE;
    dcbSerialParams.dcb.fOutxDsrFlow = FALSE;
    dcbSerialParams.dcb.fDsrSensitivity= FALSE;
    dcbSerialParams.dcb.fAbortOnError = FALSE;//TRUE;
 
    if (!SetCommState(hSerial, &dcbSerialParams.dcb)) 
    {
        PrintCmd("Serial error setting serial port Params \n");
		CloseHandle(hSerial);
		return 0;
    }
 
 
    GetCommTimeouts(hSerial,&timeouts);
    //COMMTIMEOUTS timeouts = {0};
 
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier= 0;
 
    if(!SetCommTimeouts(hSerial, &timeouts)) 
    {
        PrintCmd("Serial error setting port Timeouts \n");
		CloseHandle(hSerial);
		return 0;
    }
 return hSerial;
}

void PutSerialData(){
		// Open the Serial Port
	if (SerialOpened==0){ //Always do here
		hSerialPC = OpenAndPrepareSerial("COM9");
		if (hSerialPC==0) return;	
	SerialOpened=1;
	}

    //****************Write Operation*********************//
    int nwrite = 1;
    DWORD dwBytesWritten = 0;
 
    if (!WriteFile(hSerialPC, SerialDataToSend, nwrite, &dwBytesWritten, NULL)){ 
        PrintCmd("error writing to output buffer \n");
    }

}
	

int  waitfordata(int NbrTrialToGet){
/*
Boolean ret;
ULong nbytes=3;
Byte anbytes;
Err err;
int i;
UInt32 nbrticks,nbrticks2;


for (i=0;i<OscilloSize;i++){Oscillo[i]=0;}
//SerReceiveWait(SerIORef, 3, 10);  // Regan: Changé de 1000 à 10 timout
//SerReceiveCheck(SerIORef, &nbytes);//Checks the receive FIFO and returns the number of bytes in the serial receive queue
//anbytes = SerReceive(SerIORef, data, nbytes, 100, &err);

	for (i=0;i<NbrTrialToGet;i++){
	nbrticks=TimGetTicks();
	anbytes = SerReceive(SerIORef, Oscillo,OscilloSize, 20, &err);//Wait 20 max for the timeout between bytes
	nbrticks2=TimGetTicks();
	//DeltaTicks=nbrticks2-nbrticks;
	if (err ==0) {goto OutFor;}// got one set without error => out
	SerReceiveFlush(SerIORef, 0);//Flush remaining data.
	}

OutFor:

if (err != 0)
ret = false;
   
else
ret = true;
SerReceiveFlush(SerIORef, 0);//Put to 0 otherwise it waits

//ret=false;//modif regan //Ne pas générer d'erreur intempestive
return(ret);
*/
	PrintCmd ("wait for data oscillo not yet implemented\n"); 
}
