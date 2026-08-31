/*------------------------------------------
 *
 *  Windows Sound Program
 *  plays a simple sound wave
 *  with your default sound card.
 *
 *  BUSSY-SOCRATE REGAN
 *
 *-----------------------------------------*/

  
     #include <windows.h>
     #include <math.h>
	 #include <stdio.h>
	 
typedef struct Matrix{
	int n;	//n line
	int p;	//p column
	float * ptr; //pointer on the matrix
	float * Cptr; //pointer on the complexe part
	}Matrix;

int PlaySoundMatrix(Matrix *, int NumM, float SamplesPerSecond,int mode, int NumM2);
int PlaySoundReg(float , float , float );
int GetAudioMicro(Matrix *MAccu,int NumM,float SamplesPerSecond);
int CloseAudioDevice(void);
int LoadSoundFile(Matrix *MAccu, int NumM, char *);
int SaveSoundFile(Matrix *MAccu, int NumM,int SamplesPerSecond, char*);
int SaveMFile(Matrix *MAccu, int NumM,char*);
int LoadMFile(Matrix *MAccu, int NumM,char*);

//threads:
DWORD WINAPI ThreadPlayHandleFinish(LPVOID lpParam);		
DWORD WINAPI ThreadHandleWavInEnd(LPVOID lpParam);


extern void PrintCmd(char *);
    
     //#define BUFFERSIZE    4860                // 4k sound buffer
    
     #define PI            3.14159265358979
          
        // A simple way to report Sound Errors
     void Message(LPCSTR message)
     {
        MessageBox(NULL, message, "Sound Error", MB_OK | MB_ICONSTOP);
     }
	 
//important for external use like closing	 
HWAVEOUT     hWaveOut;          // Handle to sound card output
int AudioDeviceState=0; // 0=closed; 1=opened
int AudioRecording=0;
int MultiTaskSnd=0;

          
int PlaySoundReg(float SoundFrequency, float SoundAmplitude, float SoundDuration){
              
        HWAVEOUT     hWaveOut;          // Handle to sound card output
        WAVEFORMATEX WaveFormat;        // The sound format
        WAVEHDR      WaveHeader;        // WAVE header for our sound data
              
 //       char         Data[BUFFERSIZE];  // Sound data buffer
        HANDLE       Done;              // Event Handle that tells us the sound has finished being played.
                // This is a real efficient way to put the program to sleep
                // while the sound card is processing the sound buffer
        double x;
        int i;
			// ** Initialize the sound format we will request from sound card **    
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;     // Uncompressed sound format
        WaveFormat.nChannels = 1;                    // 1=Mono 2=Stereo
        WaveFormat.wBitsPerSample = 8;               // Bits per sample per channel
        WaveFormat.nSamplesPerSec = 44100;           // Sample Per Second
        WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;    
        WaveFormat.cbSize = 0;

		int BUFFERSIZE = SoundDuration * WaveFormat.nSamplesPerSec;
		//WORD *Data=(WORD *)malloc(BUFFERSIZE*sizeof(WORD));
		char *Data=(char *)malloc(BUFFERSIZE*sizeof(char));
		if (Data == 0 ) {
			PrintCmd("Error While allocating memory for sound");
			return 0;
			}

		
        // ** Create our "Sound is Done" event **
        Done = CreateEvent (0, FALSE, FALSE, 0);
              
        // ** Open the audio device **
        if (waveOutOpen(&hWaveOut,0,&WaveFormat,(DWORD) Done,0,CALLBACK_EVENT) != MMSYSERR_NOERROR) 
              {        
              Message("Sound card cannot be opened.");
              return TRUE;
              }
              
        // ** Make the sound buffer **    
        for (i=0; i < BUFFERSIZE; i++)
        {        
          // ** Generate the sound wave based on FREQUENCY define
          // ** x will have a range of -SoundAmplitude to +SoundAmplitude
          x = SoundAmplitude * sin(i*2.0*PI*((double)SoundFrequency)/(double)WaveFormat.nSamplesPerSec); 
              
          // ** scale x to a range of 0-255 (signed char) for 8 bit sound reproduction **
        Data[i] = (char)(127*x+128);
        //Data[i] = (WORD)(511*x+512);
        }
              
              
        // ** Create the wave header for our sound buffer **
        WaveHeader.lpData=Data;
        WaveHeader.dwBufferLength=BUFFERSIZE;
        WaveHeader.dwFlags=0;
        WaveHeader.dwLoops=0;
              
        // ** Prepare the header for playback on sound card **
        if (waveOutPrepareHeader(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error preparing Header!");
              return TRUE;
        }
              
        // ** Play the sound! **
        ResetEvent(Done);  // Reset our Event so it is non-signaled, it will be signaled again with buffer finished
              
        if (waveOutWrite(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error writing to sound card!");
              return TRUE;
        }
              
        // ** Wait until sound finishes playing
        if (WaitForSingleObject(Done,INFINITE) != WAIT_OBJECT_0)
        {
              Message("Error waiting for sound to finish");
              return TRUE;
        }  
              
        // ** Unprepare our wav header **
        if (waveOutUnprepareHeader(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error unpreparing header!");
              return TRUE;
        }
              
        // ** close the wav device **
        if (waveOutClose(hWaveOut) != MMSYSERR_NOERROR)
        {
              Message("Sound card cannot be closed!");
              return TRUE;
        } 
              
        // ** Release our event handle **
        CloseHandle(Done);
        free (Data); //release memory
        return FALSE;
     }
	 


HANDLE		Done;              // Event Handle that tells us the sound has finished being played.
short int	*SaveDataPlay;
int SoundPlaying =0;
WAVEHDR      WaveHeader;        // WAVE header for our sound data
//------------------------------- Play Matrix ------------------
int PlaySoundMatrix(Matrix *MAccu, int NumM, float SamplesPerSecond,int mode, int NumM2){
//mode is 1=mono, 2=stereo
//        HWAVEOUT     hWaveOut;          // Handle to sound card output
		 
        WAVEFORMATEX WaveFormat;        // The sound format
              
        double x;
        int i,DataSize;
				
		if(MAccu[NumM].ptr == 0) {PrintCmd("Matrix not defined!\n Use defM n°,sizen=1,sizep\nSee also recsndM n°,NbrSamplesPerSec\n");return 0;}
		if (mode == 2) {
			//PrintCmd("Mode Stereo\n");
			//if(NumM==NumM2) PrintCmd("same mtx left and right\n");
			if(MAccu[NumM2].ptr == 0) {PrintCmd("Matrix not defined!\n Use defM n°,sizen=1,sizep\nSee also recsndM n°,NbrSamplesPerSec\n");return 0;}
			if (MAccu[NumM2].n!=MAccu[NumM].n ||MAccu[NumM2].p!=MAccu[NumM].p){PrintCmd("In Stereo mode:\nMtx1 and Mtx2 should have same size n and p!\n");return 0;}
		}
		DataSize = MAccu[NumM].n * MAccu[NumM].p; //Matrix Size, in Stereo in remains each matrix size

		 // ** Initialize the sound format we will request from sound card **    
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;     // Uncompressed sound format
        WaveFormat.nChannels = mode;                    // 1=Mono 2=Stereo
        WaveFormat.wBitsPerSample = 16;               // Bits per sample per channel
        WaveFormat.nSamplesPerSec = (int)SamplesPerSecond; //example: 44100 // Sample Per Second
        WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;    
        WaveFormat.cbSize = 0;

		short int *Data=(short int *)malloc(mode*DataSize*sizeof(short int));//twice as much in  mode=2 Stereo
		if (Data == 0 ) {
			Message("Error While allocating memory for sound");
			return 0;
			}
			
		if (MAccu[NumM].n !=1) Message("Warning:\nMtx1.n should be equal to 1\n");
		if (mode==2)if (MAccu[NumM2].n !=1) Message("Warning:\nMtx2.n should be equal to 1\n");

		if (mode==1){
			for (i=0; i<DataSize; i++) Data[i]=(short int)(MAccu[NumM].ptr[i]);
			}
		else{//PrintCmd("Dispatch Data mtx1 & 2\n"); //mode=2
			for (i=0; i<DataSize; i++) { //true size of Data is 2*datasize in Stereo
				Data[2*i]=(short int)(MAccu[NumM].ptr[i]);
				Data[2*i+1]=(short int)(MAccu[NumM2].ptr[i]);
				}
			}
		// ------------ Here wait for the previous sound to finish playing ------
		while ( SoundPlaying == 1){;;} 
		// -----------------------------------------------------------------------------
        // ** Create our "Sound is Done" event **
        Done = CreateEvent (0, FALSE, FALSE, 0);
              
        // ** Open the audio device **
        if (waveOutOpen(&hWaveOut,0,&WaveFormat,(DWORD) Done,0,CALLBACK_EVENT) != MMSYSERR_NOERROR) 
              {Message("Speaker Sound card cannot be opened.");return TRUE;}
		else AudioDeviceState=1; 
       
              
        // ** Create the wave header for our sound buffer **
        WaveHeader.lpData=(LPSTR)Data;
        //WaveHeader.dwBufferLength=BUFFERSIZE;
        WaveHeader.dwBufferLength=mode*DataSize*2; //because it is short int and Datasize is the size of one matrix, for stereo mode =2
        WaveHeader.dwFlags=0;
        WaveHeader.dwLoops=0;
              
        // ** Prepare the header for playback on sound card **
        if (waveOutPrepareHeader(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error preparing Header!");
              return TRUE;
        }
              
        // ** Play the sound! **
        ResetEvent(Done);  // Reset our Event so it is non-signaled, it will be signaled again with buffer finished

        if (waveOutWrite(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error writing to sound card!");
              return TRUE;
        }
              
		SoundPlaying=1;
	  //---- here we transfer into a Thread ------

		HANDLE hThreadSnd;
		DWORD ThreadSndID, ThreadSndParam = 100;
		hThreadSnd = CreateThread(NULL, 0, ThreadPlayHandleFinish, &ThreadSndParam, 0, &ThreadSndID);
		if (hThreadSnd == NULL){
			PrintCmd("Error on launching the Thread PlaySnd!\n");
	        if (WaitForSingleObject(Done,INFINITE) != WAIT_OBJECT_0){
			Message("Error waiting for sound to finish");
			//return TRUE;
			}  
			
			// ** Unprepare our wav header **
			if (waveOutUnprepareHeader(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
			{
              Message("Error unpreparing header!");
              return TRUE;
			}
   			SoundPlaying=0; //sound stopped here
			
			// ** close the wav device **
			/* FERMETURE A L'EXTERIEUR test AudioDeviceState
			if (waveOutClose(hWaveOut) != MMSYSERR_NOERROR)
			{
				Message("Sound card cannot be closed!");
				return TRUE;
			} 
			*/   
			// ** Release our event handle **
			CloseHandle(Done);
			free (Data); //release memory
			}else {
			SaveDataPlay=Data; // for later to free in the Thread when sound stops playing 
		}
        return FALSE;
}


DWORD WINAPI ThreadPlayHandleFinish(LPVOID lpParam){		
		// ** Wait until sound finishes playing
        if (WaitForSingleObject(Done,INFINITE) != WAIT_OBJECT_0)
        {
              Message("Error waiting for sound to finish");
              return TRUE;
        }  
         SoundPlaying =0; //Sound stopped playing here
        // ** Unprepare our wav header **
        if (waveOutUnprepareHeader(hWaveOut,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error unpreparing header!");
              return TRUE;
        }
              
        // ** close the wav device **
		/* FERMETURE A L'EXTERIEUR test AudioDeviceState
        if (waveOutClose(hWaveOut) != MMSYSERR_NOERROR)
        {
              Message("Sound card cannot be closed!");
              return TRUE;
        } 
         */   
        // ** Release our event handle **
        CloseHandle(Done);
        free (SaveDataPlay); //release memory
		return 0;
     }
	 
int CloseAudioDevice(void){
		while (SoundPlaying ==1){;;} //Sound has to stop in the Thread
        if (waveOutClose(hWaveOut) != MMSYSERR_NOERROR)
        {
              Message("Speaker Sound card cannot be closed!");
              return TRUE;
        }
		SoundPlaying=0;
}


//-------------------------- Audio Microphone -------------------------
float *AccuMptr; //pour le transfer de donnée à partir de Data
int DataSizeWavIn;	//Sauvegarde taille des données
short int *DataWavIn;	//Sauvegarde de Data pour WaveIn
HWAVEIN hWaveInSave;//Sauvegarde de hWaveIn
WAVEHDR WaveHeaderIn;	//sauvegarde de du Header de WaveIn

int GetAudioMicro(Matrix *MAccu,int NumM,float SamplesPerSecond){

      
    HWAVEIN     hWaveIn;          // Handle to sound card input		 
    WAVEFORMATEX WaveFormat;        // The sound format
    //WAVEHDR      WaveHeader;        // WAVE header for our sound data
    MMRESULT result;      
                // This is a real efficient way to put the program to sleep
                // while the sound card is processing the sound buffer
    double x;
    int i;
	unsigned char s[50];
		//do we set no limit for recording memory or is it set by matrix size
		//if (MAccu[NumM].ptr != 0) {free(MAccu[NumM].ptr);}
	int DataSize = MAccu[NumM].n*MAccu[NumM].p;
	if(MAccu[NumM].ptr == 0) {PrintCmd("Matrix not defined!\n Use defM n°,sizen=1,sizep for use with recsndM n°,NbrSamplesPerSec\n");return 0;}

	short int *Data=(short int *)malloc(DataSize*sizeof(short int));
	//unsigned char *Data=(char *)malloc(DataSize*sizeof(char));
	if (Data == 0 ) {
		Message("Error While allocating memory for sound");
		return 0;
		}

	while (AudioRecording == 1){;;} //wait that previou sound is recorded

		// ** Initialize the sound format we will request from sound card **    
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;     // Uncompressed sound format
    WaveFormat.nChannels = 1;                    // 1=Mono 2=Stereo
    WaveFormat.wBitsPerSample = 16;               // Bits per sample per channel
    WaveFormat.nSamplesPerSec =(int)SamplesPerSecond; //44100;           // Sample Per Second
    WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;    
    WaveFormat.cbSize = 0;
	
	result = waveInOpen(&hWaveIn, 0, &WaveFormat, 0, 0, CALLBACK_NULL);

if (result)
 {
  char fault[256];
  waveInGetErrorText(result, fault, 256);
	PrintCmd(fault);
	PrintCmd( "Failed to open waveform input device.");
	return 0;
 }

        
    // ** Create the wave header for our sound buffer **
    WaveHeaderIn.lpData=(LPSTR)Data;
    WaveHeaderIn.dwBufferLength=DataSize*2;//*2 because it is short int
    WaveHeaderIn.dwFlags=0;
    WaveHeaderIn.dwLoops=0;
              
    // ** Prepare the header for Recording with sound card **
    if (waveInPrepareHeader(hWaveIn,&WaveHeaderIn,sizeof(WaveHeaderIn)) != MMSYSERR_NOERROR)
		{Message("Micro Error preparing Header!");return TRUE;}
              
    if (waveInAddBuffer(hWaveIn,&WaveHeaderIn,sizeof(WaveHeaderIn)) != MMSYSERR_NOERROR)
    {Message("Error reading sound card!");return TRUE;}
           
// Commence sampling input
	result = waveInStart(hWaveIn);
	if (result){Message( "Failed to start recording");return 0;}

	//Loading data for the Thread
	AccuMptr=MAccu[NumM].ptr;
	DataWavIn=Data;
	DataSizeWavIn=DataSize;
	hWaveInSave=hWaveIn;
	if (MAccu[NumM].ptr != 0) {
		MAccu[NumM].n=1; MAccu[NumM].p=DataSize;
	}else {
		PrintCmd("Micro matrix not defined!\nData unavailable\n");
		goto CloseTheDevice;
	}
	
	
	HANDLE hThreadW;
	DWORD ThreadWavInID, ThreadWavInParam = 100;
	if (MultiTaskSnd==1){
	hThreadW = CreateThread(NULL, 0, ThreadHandleWavInEnd, &ThreadWavInParam, 0, &ThreadWavInID);
	if (hThreadW == NULL){
		PrintCmd("Error on launching the Thread EndWavIn!\n");
		goto NoMultiTaskSnd;
		}
		return FALSE;
	}

NoMultiTaskSnd:
		//We do the usual program.
	AudioRecording=0;

		// Wait until finished recording
 do {} while (waveInUnprepareHeader(hWaveIn, &WaveHeaderIn, sizeof(WAVEHDR))==WAVERR_STILLPLAYING);

	if (MAccu[NumM].ptr != 0) {
		for (i=0;i<DataSize;i++){
			MAccu[NumM].ptr[i] = (float) Data[i];
			}
	}
    // close the wav device **
	CloseTheDevice:
 if (waveInClose(hWaveIn) != MMSYSERR_NOERROR)
      {
        Message("Micro Sound card cannot be closed!");
        return TRUE;
        } 
    //Release our event handle **
        //CloseHandle(Done);
        free (Data); //release memory
		AudioRecording=0; //Now the full program can end, no risk for matrix free problems
        return FALSE;
}



DWORD WINAPI ThreadHandleWavInEnd(LPVOID lpParam){
	int i;
	AudioRecording=1; //Only here it has to be set because it is important to avoid ending program and freeing the matrix too early
	// Wait until finished recording
 do {} while (waveInUnprepareHeader(hWaveInSave, &WaveHeaderIn, sizeof(WAVEHDR))==WAVERR_STILLPLAYING);
	//PrintCmd("Hello From Micro Thread\n");
	if (AccuMptr != 0) {
		for (i=0;i<DataSizeWavIn;i++){
			AccuMptr[i] = (float) DataWavIn[i];
			}
	}
    // close the wav device **
    if (waveInClose(hWaveInSave) != MMSYSERR_NOERROR)
      {
        Message("Micro Sound card cannot be closed!");
	AudioRecording=0; //Now the full program can end, no risk for matrix free problems
        return 0;
        } 
	AudioRecording=0; //Now the full program can end, no risk for matrix free problems
           
    //Release our event handle **
        //CloseHandle(DoneWavIn);
        free (DataWavIn); //release memory
        return 0;
}



//----------------------------------- Load Sound File----------------

BOOL LoadSoundFile(Matrix* MAccu, int NumM,char *pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;
	
	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;
		char s[50];
		dwFileSize = GetFileSize(hFile, NULL);
		//sprintf(s,"file .wav size=%d\n",dwFileSize);
		//PrintCmd(s);
		if(dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				DWORD dwRead;

				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Add null terminator
					//We fill in the Matrix
					LPSTR  datastrt=0;
					int i;
					int datasize;
					int *dat;
					LPSTR psz = pszFileText;
					for (i=0;i<dwFileSize;i++) {
						if(*psz=='d' && *(psz+1)=='a' &&*(psz+2)=='t' && *(psz+3)=='a' ) 
								{datastrt = psz +8;
								//PrintCmd("found data chunk\n");
								goto datafound;}
								
							psz++;
						}
				datafound:
					//technic to get the int32 == int  with introduction of  int dat
					dat=psz+4;
					datasize = *dat;
					//sprintf(s,"data size=%d\n",datasize);
					//PrintCmd(s);

					if (datastrt == 0) {
						PrintCmd("Failed to find 'data' chunk!\n");
						CloseHandle(hFile);
						return FALSE;
					}
					if (MAccu[NumM].ptr !=0) free(MAccu[NumM].ptr);
					//the datasize is for bytes, the info is coded on short int 2bytes
					//so the real nbre of samples is datasize/2
					MAccu[NumM].ptr = (float*)malloc(datasize/2*sizeof(float) ); //datasize/2 because int short and not char
					if (MAccu[NumM].ptr == 0) {PrintCmd("Couldn't allocate memory for loading .wav file\n");return FALSE;}
					short int *datawav;
					datawav=datastrt;
					//for (i=0;i<datasize;i++) MAccu[NumM].ptr[i]=(float)datastrt[i];
					for (i=0;i<datasize/2;i++) MAccu[NumM].ptr[i]=(float)datawav[i];
					MAccu[NumM].n=1;MAccu[NumM].p=datasize/2;
					bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}else{PrintCmd("Couldn't load sound file!\n"); bSuccess=FALSE;}
	return bSuccess;
}

//------------------------------ Save Matrix ---------------------------
BOOL SaveSoundFile(Matrix* MAccu, int NumM,int SamplesPerSecond,char *pszFileName)
{

	HANDLE hFile;
	BOOL bSuccess = FALSE;
	typedef struct sndChunk{
		char nam[4];
		int filesizem8;
		char wavenam[4];
		char fmtnam[4];
		int wavefmt;
		short int wFormatTag;
		short int nChannels;
		int  nSamplesPerSec;
		int nAvgBytesPerSec;
		short int nBlockAlign;
		short int wBitsPerSample;
		char dat[4];
		int length;
		}sndChunk;
		
		int k;
		
	if (MAccu[NumM].ptr == 0) {PrintCmd("Matrix not defined\n");return 1;}

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;
		char s[50];
		//emplacement pour les donnees concernant le chunk de la matrice a copier
		sndChunk sndMChnk[1];
		PrintCmd("Step1\n");

		//prepare chunk 'matrix', 'data' -----------------------------------------		
		strcpy(sndMChnk[0].nam,"RIFF");
		sndMChnk[0].filesizem8=2*MAccu[NumM].p + sizeof(sndMChnk)- 8; //mono so mtx are dim1*p
		strcpy(sndMChnk[0].wavenam,"WAVE");
		strcpy(sndMChnk[0].fmtnam,"fmt ");
		sndMChnk[0].wavefmt=16;
        sndMChnk[0].wFormatTag = WAVE_FORMAT_PCM;     // Uncompressed sound format
        sndMChnk[0].nChannels = 1;                    // 1=Mono 2=Stereo
        sndMChnk[0].nSamplesPerSec =(int)SamplesPerSecond; //44100;           // Sample Per Second
        sndMChnk[0].nAvgBytesPerSec = (int)SamplesPerSecond * 2;    
        sndMChnk[0].nBlockAlign = 1*2; //nChannels * wBitsPerSample / 8;
        sndMChnk[0].wBitsPerSample = 16;  // Bits per sample per channel
		strcpy(sndMChnk[0].dat,"data");
		sndMChnk[0].length=2*MAccu[NumM].p;
		DWORD dwWritten;
		
		short int *SndData = (short int*) malloc (MAccu[NumM].p*sizeof(short int) ); 
		if (SndData == 0) {PrintCmd("Cannot allocate memory for saving file"); return 1;}
		//converting data
		for (k=0;k<MAccu[NumM].p;k++)SndData[k]=(short int)MAccu[NumM].ptr[k];
		PrintCmd("Step2\n");
		//write chunk
		//char pszText[]="CalcReg Tools : ";
			// 	WriteFile(hFile, pszText, (DWORD)strlen(pszText), &dwWritten, NULL);
				if(WriteFile(hFile, sndMChnk, (DWORD)sizeof(sndMChnk), &dwWritten, NULL))
				{
		//write data
					if(WriteFile(hFile, SndData, (DWORD)sndMChnk[0].length, &dwWritten, NULL))
					{
						PrintCmd("Step3\n");
						bSuccess = TRUE; // It worked!
					}else{PrintCmd("couldn't write mtx data in file.\n"); bSuccess= FALSE;}
				}else{PrintCmd("couldn't write mtx chunk in file.\n"); bSuccess= FALSE;}
			free(SndData);
			CloseHandle(hFile);
	}else{PrintCmd("Couldn't open Mtx File for save!\n"); bSuccess=FALSE;}
	return bSuccess;
}


//------------------------------ Save Matrix ---------------------------
BOOL SaveMFile(Matrix* MAccu, int NumM,char *pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;
	typedef struct mtxChunk{
		char nam[6];
		int n; //mtx n
		int p;// mtx p
		char dat[4];
		int size;
		}mtxChunk;
		
	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;
		char s[50];
		//emplacement pour les donnees concernant le chunk de la matrice a copier
		mtxChunk MChnk[1];

		//prepare chunk 'matrix', 'data' -----------------------------------------		
		strcpy(MChnk[0].nam,"matrix");
		MChnk[0].n=MAccu[NumM].n;
		MChnk[0].p=MAccu[NumM].p;
		strcpy(MChnk[0].dat,"data");
		MChnk[0].size=MAccu[NumM].n*MAccu[NumM].p;
		
		DWORD dwWritten;
		//write chunk
		//char pszText[]="CalcReg Tools : ";
			//	WriteFile(hFile, pszText, (DWORD)strlen(pszText), &dwWritten, NULL);
				if(WriteFile(hFile, MChnk, (DWORD)sizeof(MChnk), &dwWritten, NULL))
				{
		//write data
					if(WriteFile(hFile, MAccu[NumM].ptr, MChnk[0].size*sizeof(float), &dwWritten, NULL))
					{
						bSuccess = TRUE; // It worked!
					}else{PrintCmd("couldn't write mtx data in file.\n"); bSuccess= FALSE;}
				}else{PrintCmd("couldn't write mtx chunk in file.\n"); bSuccess= FALSE;}
		CloseHandle(hFile);
	}else{PrintCmd("Couldn't open Mtx File for save!\n"); bSuccess=FALSE;}
	return bSuccess;
}

//--------------------------- Load Matrix ------------------------------
BOOL LoadMFile(Matrix* MAccu, int NumM,char *pszFileName)
{
	typedef struct mtxChunk{
		char nam[6];
		int n; //mtx n
		int p;// mtx p
		char dat[4];
		int size;
		}mtxChunk;
	HANDLE hFile;
	BOOL bSuccess = FALSE;
	
	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;
		char s[50];
		dwFileSize = GetFileSize(hFile, NULL);
		//sprintf(s,"file .wav size=%d\n",dwFileSize);
		//PrintCmd(s);
		if(dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				DWORD dwRead;

				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Add null terminator
					
					//Here we test if it is an ascii matrix
					//Format of an ascii matrix
					//Remark: %id means ignore the integer data, %is ignore the string data
					if (strstr(pszFileText,"MATRIX ASCII")==pszFileText) {
						PrintCmd("ASCII MATRIX recognised\n");
						LPSTR mtx_format=strstr(pszFileText,"FORMAT"); //on pointe le format derrière "FORMAT"
						if (mtx_format==0) {
							PrintCmd("Error 'FORMAT' chunk not found\n");
							CloseHandle(hFile);
							return FALSE; //error
						}
						if (strstr(pszFileText,"DATA:")==0) {
							PrintCmd("Error 'DATA:' chunk not found\n");
							CloseHandle(hFile);
							return FALSE; //error
						}
						//count the lines and the columns
				char* dataptr=strstr(pszFileText,"DATA:");
				char* fileptr=dataptr+7; //le return fait 2 caractères
				char *count_fmt=mtx_format+6;
				int ascii_mtx_nlines=0;
				int ascii_mtx_pcolumns=0;
				int ignores=0;
				char *p;
				unsigned char retn[]={0x0d,0x0a,0};
				int i=0;char s[100]; 
				char text[100];
				char fmtext[10];
				char nbrtext[10];
						//Il faut déplacer le pointeur en meme temps que de chercher fileptr=strstr(fileptr,retn)+1;
//						while (strstr(fileptr,retn)!=0){fileptr=strstr(fileptr,retn)+2;ascii_mtx_nlines++;}
						while ((p=strstr(fileptr,retn))!=0){if(*fileptr=='/'||(*fileptr==0x0d&&*(fileptr+1)==0x0a)){PrintCmd("-1 com\n");fileptr=p+2;}else{fileptr=p+2;ascii_mtx_nlines++;}}
						while (strstr(count_fmt,"%")!=0&& count_fmt<dataptr){count_fmt=strstr(count_fmt,"%")+1;	if(count_fmt[0]!='i') ascii_mtx_pcolumns++;
																									else ignores++;}
						int *format=(int*)malloc((ascii_mtx_pcolumns+ignores)*sizeof(int)); // 0=ignore,1=ok
						count_fmt=mtx_format+6;
						while (strstr(count_fmt,"%")!=0&& count_fmt<dataptr){count_fmt=strstr(count_fmt,"%")+1;	
																			if(count_fmt[0]=='i') format[i]=0;
																			else {format[i]=(int)(count_fmt-1-mtx_format);
																			//sprintf(s,"def format[%d]=%d\n",i,format[i]);
																			//PrintCmd(s);
																			}
																			i++;}
						
					//On met des zeros à la place des espaces pour utiliser facilement le scanf en pointant sur %
					count_fmt=mtx_format+6;
					while (count_fmt<dataptr && (p=strstr(count_fmt,"%"))!=0){*(p-1)='X';count_fmt++;}
			//sprintf(text,"M%d(%d,%d)\n%d ignores\n",NumM,ascii_mtx_nlines,ascii_mtx_pcolumns,ignores);
			//PrintCmd(text);

			int Mn=ascii_mtx_nlines;
			int Mp=ascii_mtx_pcolumns;
			int datasize=Mn*Mp;
			char * dataend=pszFileText+dwFileSize;
			//matrix handling now
			if (MAccu[NumM].ptr !=0) free(MAccu[NumM].ptr);
			MAccu[NumM].ptr = (float*)malloc(datasize*sizeof(float) ); //datasize/2 because int short and not char
			if (MAccu[NumM].ptr == 0) {PrintCmd("Couldn't allocate memory for loading Matrix file.\n");	free(format);CloseHandle(hFile);return FALSE;}
			MAccu[NumM].n=Mn;MAccu[NumM].p=Mp;
			//filling the matrix
			char *myptr= dataptr+7; //points at start of data (Le return contient deux octets)
			int mtxptr=0;//index  in the matrix
			int i_fmt;
			float nbr;
			while(myptr<dataend){
				char*RtnPos=strstr(myptr,retn); //localiser le return à la fin de la ligne
				//sprintf(s,"[%s]\n",myptr);
				//PrintCmd(s);
				//if (*myptr=='/'){myptr=RtnPos+1;goto LinefmtDone;}
				if (*myptr=='/'){PrintCmd("comment dectected \n");myptr=RtnPos+1;goto LinefmtDone;}
				if (RtnPos==0){goto asciiMtxFillDone;} //no more returns in the ascii file
				i_fmt=0;
				while (myptr<RtnPos && i_fmt <ascii_mtx_pcolumns+ignores){
					while (*myptr==' ' || *myptr=='\t')myptr++; //on cherche le motif (nbr ou string) sur une ligne
					if (format[i_fmt]==0){i_fmt++;while (*myptr!=' ' && *myptr!='\t'&&myptr<RtnPos)myptr++;//PrintCmd("fmt=0\n");
					}
					else {
						if (mtxptr>=Mn*Mp){PrintCmd("matrix overflow\nsoftware problem\n");CloseHandle(hFile);return TRUE;}
				if (i_fmt>=ascii_mtx_pcolumns+ignores)PrintCmd("overflow i_fmt\n");
				//if (format[i_fmt]>100) {PrintCmd("absurde value of format!\nOuting...\n"); goto asciiMtxFillDone;}
				if (i_fmt>=ascii_mtx_pcolumns+ignores){PrintCmd("overflow i_fmt\nOut line\n");goto LinefmtDone;}
					p=mtx_format+format[i_fmt];
					for (i=0;i<5;i++) {if (p[i]=='X')goto fmload_done;else fmtext[i]=p[i];}
				fmload_done:
					fmtext[i]=0;
					
					//PrintCmd("format=");
					//PrintCmd(fmtext);
					//PrintCmd("\n");
					i=0;
					while (*myptr!=' ' && *myptr!='\t'&&myptr<RtnPos&& i<10){nbrtext[i++]=*myptr;myptr++;}
					nbrtext[i]=0;
								//float nbr=0; sscanf(nbrtext,fmtext,&nbr);MAccu[NumM].ptr[mtxptr]=nbr;
				nbr=0; sscanf(nbrtext,"%f",&nbr);MAccu[NumM].ptr[mtxptr]=nbr;
				//sprintf(s,"nbrtext= [%s], nbr = %d\n",nbrtext,(int)nbr);PrintCmd(s);
								mtxptr++;i_fmt++;while (*myptr!=' ' && *myptr!='\t'&&myptr<RtnPos)myptr++;}
					}
					LinefmtDone:
					myptr++;
					}
				asciiMtxFillDone:
					free(format);
					CloseHandle(hFile);
					GlobalFree(pszFileText);
					return TRUE; //success
						}
					//Here the matrix meant to be a  "matrix" of binary data with mtxchunk structure
					mtxChunk *chnk;
					chnk=pszFileText;
					int Mn=chnk[0].n;
					int Mp=chnk[0].p;

					//We fill in the Matrix

					LPSTR  datastrt=0;
					int i;
					int datasize;
					int *dat;
					LPSTR psz = pszFileText;
					for (i=0;i<dwFileSize;i++) {
						if(*psz=='d' && *(psz+1)=='a' &&*(psz+2)=='t' && *(psz+3)=='a' ) 
								{datastrt = psz +8;
								//PrintCmd("Load Mtx found data chunk\n");
								goto datafound;}
								
							psz++;
						}
				datafound:
					//technic to get the int32 == int  with introduction of  int dat
					dat=psz+4;
					datasize = *dat;
					//sprintf(s,"data size=%d\n",datasize);
					//PrintCmd(s);

					if (datastrt == 0) {
						PrintCmd("Failed to find 'data' chunk!\n");
						CloseHandle(hFile);
						return FALSE;
					}
					if (MAccu[NumM].ptr !=0) free(MAccu[NumM].ptr);
					MAccu[NumM].ptr = (float*)malloc(datasize*sizeof(float) ); //datasize/2 because int short and not char
					if (MAccu[NumM].ptr == 0) {PrintCmd("Couldn't allocate memory for loading Matrix file.\n");return FALSE;}
					float *datawav;
					datawav=datastrt;
					//for (i=0;i<datasize;i++) MAccu[NumM].ptr[i]=(float)datastrt[i];
					for (i=0;i<datasize;i++) MAccu[NumM].ptr[i]=(float)datawav[i];
					MAccu[NumM].n=Mn;MAccu[NumM].p=Mp;
					if(Mn*Mp!=datasize)PrintCmd("Error in sizes loadM\n");
					bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}else{PrintCmd("Couldn't load Mtx file!\n"); bSuccess=FALSE;}
	return bSuccess;
}

