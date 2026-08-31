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

typedef struct Matrix{
	int n;	//n line
	int p;	//p column
	float * ptr; //pointer on the matrix
	}Matrix;

int PlaySoundMatrix(Matrix *, int NumM, float SamplesPerSecond, float bits);
int PlaySoundReg(float , float , float );
int CloseAudioDevice(void);
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
int PlaySoundMatrix(Matrix *MAccu, int NumM, float SamplesPerSecond,float bits){
         
//        HWAVEOUT     hWaveOut;          // Handle to sound card output
		 
        WAVEFORMATEX WaveFormat;        // The sound format
        WAVEHDR      WaveHeader;        // WAVE header for our sound data
              
 //       char         Data[BUFFERSIZE];  // Sound data buffer
        HANDLE       Done;              // Event Handle that tells us the sound has finished being played.
                // This is a real efficient way to put the program to sleep
                // while the sound card is processing the sound buffer
        double x;
        int i,DataSize;
		
		 DataSize = MAccu[NumM].n * MAccu[NumM].p; //Matrix Size
			// ** Initialize the sound format we will request from sound card **    
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;     // Uncompressed sound format
        WaveFormat.nChannels = 1;                    // 1=Mono 2=Stereo
        WaveFormat.wBitsPerSample = 8;               // Bits per sample per channel
        WaveFormat.nSamplesPerSec = (int)SamplesPerSecond; //44100;           // Sample Per Second
        WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;    
        WaveFormat.cbSize = 0;

		//int BUFFERSIZE = SoundDuration * WaveFormat.nSamplesPerSec;
		//WORD *Data=(WORD *)malloc(BUFFERSIZE*sizeof(WORD));
		char *Data=(char *)malloc(DataSize*sizeof(char));
		if (Data == 0 ) {
			Message("Error While allocating memory for sound");
			return 0;
			}
		if (MAccu[NumM].n !=1) Message("Warning Matrix size nxp, n should be 1\n");
		for (i=0; i<DataSize; i++) Data[i]=(char)(128+127*MAccu[NumM].ptr[i]);
		
        // ** Create our "Sound is Done" event **
        Done = CreateEvent (0, FALSE, FALSE, 0);
              
        // ** Open the audio device **
        if (waveOutOpen(&hWaveOut,0,&WaveFormat,(DWORD) Done,0,CALLBACK_EVENT) != MMSYSERR_NOERROR) 
              {Message("Sound card cannot be opened.");return TRUE;}
		else AudioDeviceState=1; 
       
              
        // ** Create the wave header for our sound buffer **
        WaveHeader.lpData=Data;
        //WaveHeader.dwBufferLength=BUFFERSIZE;
        WaveHeader.dwBufferLength=DataSize;
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
              Message("Sound card cannot be closed!");
              return TRUE;
        }
}
