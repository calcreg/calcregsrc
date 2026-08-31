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
	}Matrix;

int PlaySoundMatrix(Matrix *, int NumM, float SamplesPerSecond,int mode, int NumM2);
int PlaySoundReg(float , float , float );
int GetAudioMicro(Matrix *MAccu,int NumM,float SamplesPerSecond);
int CloseAudioDevice(void);
int LoadSoundFile(Matrix *MAccu, int NumM, char *);
int SaveSoundFile(Matrix *MAccu, int NumM,int SamplesPerSecond, char*);
int SaveMFile(Matrix *MAccu, int NumM,char*);
int LoadMFile(Matrix *MAccu, int NumM,char*);



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
//------------------------------- Play Matrix ------------------
int PlaySoundMatrix(Matrix *MAccu, int NumM, float SamplesPerSecond,int mode, int NumM2){
//mode is 1=mono, 2=stereo
//        HWAVEOUT     hWaveOut;          // Handle to sound card output
		 
        WAVEFORMATEX WaveFormat;        // The sound format
        WAVEHDR      WaveHeader;        // WAVE header for our sound data
              
 //       char         Data[BUFFERSIZE];  // Sound data buffer
        HANDLE       Done;              // Event Handle that tells us the sound has finished being played.
                // This is a real efficient way to put the program to sleep
                // while the sound card is processing the sound buffer
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
        return FALSE;
     }
	 
int CloseAudioDevice(void){
        if (waveOutClose(hWaveOut) != MMSYSERR_NOERROR)
        {
              Message("Speaker Sound card cannot be closed!");
              return TRUE;
        }
}


//-------------------------- Audio Microphone -------------------------

int GetAudioMicro(Matrix *MAccu,int NumM,float SamplesPerSecond){

        HWAVEIN     hWaveIn;          // Handle to sound card input
		 
        WAVEFORMATEX WaveFormat;        // The sound format
        WAVEHDR      WaveHeader;        // WAVE header for our sound data
         MMRESULT result;      
 //       char         Data[BUFFERSIZE];  // Sound data buffer
        HANDLE       Done;              // Event Handle that tells us the sound has finished being played.
                // This is a real efficient way to put the program to sleep
                // while the sound card is processing the sound buffer
        double x;
        int i;
		unsigned char s[50];
		//do we set no limit for recording memory or is it set by matrix size
		//if (MAccu[NumM].ptr != 0) {free(MAccu[NumM].ptr);}
		int DataSize = MAccu[NumM].n*MAccu[NumM].p;
		if(MAccu[NumM].ptr == 0) {PrintCmd("Matrix not defined!\n Use defM n°,sizen=1,sizep for use with recsndM n°,NbrSamplesPerSec\n");return 0;}
		//MMTIME MMTime = {0};
		//MMTime.wType = TIME_BYTES;

		short int *Data=(short int *)malloc(DataSize*sizeof(short int));
		//unsigned char *Data=(char *)malloc(DataSize*sizeof(char));
		if (Data == 0 ) {
			Message("Error While allocating memory for sound");
			return 0;
			}
			// ** Initialize the sound format we will request from sound card **    
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;     // Uncompressed sound format
        WaveFormat.nChannels = 1;                    // 1=Mono 2=Stereo
        WaveFormat.wBitsPerSample = 16;               // Bits per sample per channel
        WaveFormat.nSamplesPerSec =(int)SamplesPerSecond; //44100;           // Sample Per Second
        WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;    
        WaveFormat.cbSize = 0;
	
        // ** Create our "Sound is Done" event **
    //  Done = CreateEvent (0, FALSE, FALSE, 0);
    /*          
        // ** Open the audio device **
        if (waveInOpen(&hWaveIn,0,&pFormat,(DWORD) Done,0L,CALLBACK_EVENT) != MMSYSERR_NOERROR) 
              {PrintCmd("Micro Sound card\ncannot be opened.\n");return TRUE;}
//		else AudioDeviceState=1; 
  
result = waveInOpen(&hWaveIn, WAVE_MAPPER,&WaveFormat,
            0L, 0L, WAVE_FORMAT_DIRECT);
if (result )
 {
  char fault[256];
  waveInGetErrorText(result, fault, 256);
	PrintCmd(fault);
	PrintCmd( "Failed to open waveform input device.");
	return 0;
 }
*/
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
        WaveHeader.lpData=(LPSTR)Data;
        //WaveHeader.lpData=Data;
        //WaveHeader.dwBufferLength=BUFFERSIZE;
        WaveHeader.dwBufferLength=DataSize*2;//*2 because it is short int
        WaveHeader.dwFlags=0;
        WaveHeader.dwLoops=0;
              
        // ** Prepare the header for Recording with sound card **
        if (waveInPrepareHeader(hWaveIn,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Micro Error preparing Header!");
              return TRUE;
        }
              
        // ** Record sound! **
      //  ResetEvent(Done);  // Reset our Event so it is non-signaled, it will be signaled again with buffer finished
              
        if (waveInAddBuffer(hWaveIn,&WaveHeader,sizeof(WaveHeader)) != MMSYSERR_NOERROR)
        {
              Message("Error reading sound card!");
              return TRUE;
        }
           
// Commence sampling input
	result = waveInStart(hWaveIn);
	if (result){Message( "Failed to start recording");return;}


 // Wait until finished recording
 do {} while (waveInUnprepareHeader(hWaveIn, &WaveHeader, sizeof(WAVEHDR))==WAVERR_STILLPLAYING);

		 /*  
        //Wait until recording sound finishes
        if (WaitForSingleObject(Done,INFINITE) != WAIT_OBJECT_0)
        {
              Message("Error waiting for sound to finish recording");
              return TRUE;
        }  */
		//MAccu[NumM].ptr = (float *) malloc (DataSize*sizeof(float) );
		if (MAccu[NumM].ptr != 0) {
			MAccu[NumM].n=1; MAccu[NumM].p=DataSize;
			for (i=0;i<DataSize;i++){
				MAccu[NumM].ptr[i] = (float) Data[i];
			//	MAccu[NumM].ptr[i] = Data[i];
				}
		}else PrintCmd("Error allocation for micro matrix memory!\nData unavailable\n");
        // close the wav device **
        if (waveInClose(hWaveIn) != MMSYSERR_NOERROR)
        {
              Message("Micro Sound card cannot be closed!");
              return TRUE;
        } 
           
        //Release our event handle **
        CloseHandle(Done);
        free (Data); //release memory
        return FALSE;
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

