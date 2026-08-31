
/* Includes */
#include <windows.h>			// system
#include <Stdio.h>


//serial-Oscillo
#define OscilloSize 1000
#define DataToSendSize 10

int  waitfordata(int NbrTrialToGet);
float Oscilloscope(float a);
void GetOscilloData(int info);
void PutSerialData();

extern void PrintCmd(char * s) ;
void Wait(int Time);
extern int CheckForBreak();


//Serial
int SerialReady=0; //Serial device opened =1, closed =0
unsigned char Oscillo[OscilloSize];
unsigned char SerialDataToSend[DataToSendSize];//data to send
int SerialLibOpened=0;

int  NbrTrialSerial=20; //Nbr of trials to get the full Tablesize data loaded
int BaudRate = 28800;
int SerFlag = 0;
char	*strErrSerLibOpen = "Error opening serial library";
char	*strErrSerPortOpen = "Error opening serial port";

extern int BreakActivated;


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
	return  5.0*(float)Oscillo[(int) a]/255.0 - 2.5;
}

void GetOscilloData(int info){
	/*
	Err		Error;
	SerSettingsType sstSetup;

	//Open the Serial library , then the Serial Port 
	if (SerialLibOpened==0){ //Always do here
		Error = SysLibFind("Serial Library", &SerIORef);
		if (Error) {
			FrmCustomAlert (AlertGenericAlert, strErrSerLibOpen, "can't open library","can't open library" );
			SerClearErr(SerIORef);
			return;		
		}
	SerialLibOpened=1;
	}

	Error = SerOpen(SerIORef, 0,BaudRate);
	if (Error) {
		FrmCustomAlert (AlertGenericAlert, strErrSerPortOpen, "can't open port","can't open port" );
		SerClearErr(SerIORef);
		return;
	}	
	
	
   SerIOConnected = true;	
	sstSetup.baudRate = BaudRate;

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


		if (!waitfordata(NbrTrialSerial)){//try 10 times to get proper data
				PrintCmd("  Data Acquisition\n   -- problem! --\n");
				if (SerialReady == 1) PrintCmd("keeping old data\n");
				return;
			}
		else if (info!=0) {PrintCmd("Data Acquired.\n");}
	SerialReady = 1; //here it means that OSC(x) has got the data

	if (!SerIOConnected)
		return;
		
	Error = SerClose (SerIORef); //close the port
	*/
	PrintCmd ("Oscillo serial not yet implemented\n");
	}

void PutSerialData(int info){
/*	Err		Error;
	SerSettingsType sstSetup;

		// Open the Serial library , then the Serial Port
	if (SerialLibOpened==0){ //Always do here
		Error = SysLibFind("Serial Library", &SerIORef);
		if (Error) {
			FrmCustomAlert (AlertGenericAlert, strErrSerLibOpen, "can't open library","can't open library" );
			SerClearErr(SerIORef);
			return;		
		}
	SerialLibOpened=1;
	}

	Error = SerOpen(SerIORef, 0,BaudRate);
	if (Error) {
		FrmCustomAlert (AlertGenericAlert, strErrSerPortOpen, "can't open port","can't open port" );
		SerClearErr(SerIORef);
		return;
	}	
	
	
   SerIOConnected = true;	
	sstSetup.baudRate = BaudRate;

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

	//unsigned char SerialDataToSend[DataToSendSize] 
	SerSend(SerIORef, SerialDataToSend, 1, &Error); //At the moment just send one byte
	if (Error !=errNone ) PrintCmd("putserial Sending Error\n");

	if (!SerIOConnected)
		return;
		
	Error = SerClose (SerIORef); //close the port
*/
	PrintCmd ("PutSerialdata Not yet implemented\n");
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
