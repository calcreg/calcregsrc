/*------------------------------------------
 *
 *  Windows Sound Program
 *  plays a simple sound wave
 *  with your default sound card.
 *
 *  BUSSY-SOCRATE REGAN
 *
 *-----------------------------------------/
  
     #include <windows.h>
     #include <math.h>
          
     #define PI            3.14159265358979
          
     void Message(LPCSTR message)
     {
        MessageBox(NULL, message, "Sound Error", MB_OK | MB_ICONSTOP);
     }
          
int PlaySoundReg(float SoundFrequency, float SoundAmplitude, float SoundDuration){
              
        HWAVEOUT     hWaveOut;          // Handle to sound card output
        WAVEFORMATEX WaveFormat;        // The sound format
        WAVEHDR      WaveHeader;        // WAVE header for our sound data
              
        HANDLE       Done;              // Event Handle that tells us the sound has finished being played.
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
          x = SoundAmplitude * sin(i*2.0*PI*((double)SoundFrequency)/(double)WaveFormat.nSamplesPerSec); 
              
        Data[i] = (char)(127*x+128);
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

