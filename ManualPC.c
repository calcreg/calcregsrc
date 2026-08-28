/* -----------------------------
 *
 * This is the live Documentation
 * of CalcReg
 *
 * BUSSY-SOCRATE REGAN
 *
 * ------------------------------

/* Includes */
#include <windows.h>			// system
#include <Stdio.h>


typedef struct var{//takes the name of the Accu
	int n;			//number of the Accu allocated to it. from 0 to NbrMaxAccu
	int adr;	// Adr of the variable in the WholeMnemoProg list
}var;


 
void SetUpTextProg(int position) ;
void DeleteProg();
void PrintProg(char * s);
void DisplayManual();
void StartApplication(void); 
void decodeCreator();
void decodeCopyRight();

extern void decode(char *txt, int size);
extern void PrintCmd(char * s) ;

extern int StartInfoDone;

int CodedWordConverted = 0;
int CopyRightConverted = 0;



//static char Copyright[] ="Contact me for job or Bug Reports.\nNot to be sold. Read manual Before Use.\nHome made math library. Use at your own risks and enjoy!";
//static char Copyright2[]="\n Copyright 2012\n    CalcReg v1.2\n   by Regan B.S.\n\n";
//static char StartInfo[]="     Click (i) or\n  Menu/Manual.";
//static char CodedWord[]= "The identity of the coder of this program is me: Mr. BUSSY-SOCRATE REGAN born on the thirteen's of June In France. No rights to sell or Use professionnally is given without my permission.";

unsigned char Copyright[]={
0xbc,0x91,0x90,0x8a,0x9f,0x9d,0x8a,0xde,0x93,0x98,0xdd,0x9b,0x92,0x8e,0xdc,0x96,
0x94,0x99,0xda,0x95,0x8b,0xd9,0xba,0x8d,0x90,0xd7,0xa4,0x90,0x85,0x9b,0x81,0x86,
0x81,0xdf,0xfa,0xa1,0x81,0x99,0xcc,0x9f,0x85,0xc9,0x8a,0x82,0xc6,0x96,0x8b,0x8f,
0x86,0xce,0xff,0x8c,0xb8,0xbd,0xbe,0xf9,0xb5,0xb7,0xbb,0xa6,0xb3,0xbd,0xef,0x8c,
0xa9,0xad,0xa6,0xb5,0xa3,0xe4,0x96,0xb2,0xda,0x90,0xb6,0xf2,0xd7,0xda,0xd0,0x93,
0xdc,0xce,0xc9,0xc9,0x8a,0xc5,0xc7,0xd0,0xca,0x80,0xf2,0xf5,0xf8,0xea,0xf7,0xe6,
0xeb,0xbe,0xae,0xde,0xfa,0xe2,0xa5,0xe2,0xf5,0x5e,0x5,0x15,0xd,0x7,0x53,0x1e,0x18,
0x2,0x4a,0x1a,0xc,0x10,0xa,0x2d,0x7c,0x3b,0x39,0x31,0x72,0x35,0x23,0x21,0x26,0x3f,
0x65,0x4b,0x48,0x4b,0x4d,0x19,0x45,0x53,0x5c,0x40,0x59,0x4d,0x41,0xd,0x47,0x6c,
0x32,0x74,0x62,0x14};
unsigned char Copyright2[]={
0xf5,0xde,0xbd,0x91,0x8e,0x87,0x8c,0x97,0x99,0x95,0x89,0xdd,0xcf,0xcc,0xcd,0xce,
0xf1,0xdb,0xda,0xda,0xd9,0xba,0x99,0x94,0x94,0xa5,0x93,0x92,0xd5,0x82,0xc2,0xdc,
0xcb,0xfb,0xd0,0xcf,0xce,0x8f,0x95,0xcb,0xb8,0x8c,0x8f,0x86,0x88,0xc5,0xa6,0xcd,
0xb1,0xce,0xd5,0xd4,0xdd};
/*
0xf5,0xde,0xbd,0x91,0x8e,0x87,0x8c,0x97,0x99,0x95,
0x89,0xdd,0xcf,0xcc,0xcd,0xce,
0xf1,0xdb,0xda,0xda,0xd9,0xba,0x99,0x94,0x94,0xa5,0x93,0x92,0xd5,0x82,0xc2,0xdc,
0xc0,0xfb,0xd0,0xcf,0xce,0x8f,0x95,0xcb,0xb8,0x8c,0x8f,0x86,0x88,0xc5,0xa6,0xcd,
0xb1,0xce,0xd5,0xd4,0xdd};*/

char StartInfo[]={0xdf,0xde,0xde,0xde,0xde,0xbd,0x92,0x97,0x9d,0x96,0xdd,0xd5,0x94,0xd5,0xdc,0x93,
0x89,0xf1,0xda,0xda,0xb4,0x9c,0x96,0x8d,0xd8,0xba,0x97,0x9b,0x80,0x95,0x9f,0xdc,
0xf2};
unsigned char CodedWord[]={0xab,0x96,0x9b,0xde,0x97,0x9a,0x9b,0x90,0x8a,0x94,
0x89,0x84,0xdd,0x93,0x9a,0xdc,
0x8f,0x93,0x9f,0xda,0x9a,0x96,0x9c,0x9d,0x85,0xd7,0x99,0x93,0xd5,0x80,0x9b,0x9b,
0x81,0xd1,0x80,0x9d,0x81,0x8a,0x9e,0x8a,0x87,0xc9,0x81,0x94,0xc6,0x88,0x81,0xd9,
0xc2,0xad,0xad,0xf0,0xfd,0x9e,0x8f,0x8a,0x8b,0x8f,0xf8,0x80,0x9d,0x92,0x9d,0x8f,
0x98,0x8e,0xe9,0x95,0x83,0x83,0x82,0x8f,0x9f,0xdc,0xd3,0xc8,0xd6,0x97,0xda,0xdd,
0x91,0xdb,0xc5,0xc9,0x8a,0xdc,0xce,0xcd,0xd0,0xd4,0xfb,0xf9,0xf4,0xbf,0xe5,0xb4,
0xfd,0xf6,0xae,0xc1,0xfc,0xe9,0xe0,0xa3,0xc8,0x10,0x5c,0x3c,0xa,0x14,0x1d,0x12,0xa,
0x42,0x4a,0x26,0xa,0x43,0x13,0x37,0x3b,0x32,0x23,0x26,0x72,0x24,0x22,0x6b,0x3a,0x23,
0x28,0x2d,0x1f,0x53,0x48,0x17,0x60,0x41,0x55,0xd,0x5b,0x5a,0x49,0x45,0x44,0x6d,
0x6f,0x70,0x78,0x7a,0x7c,0x6e,0x60,0x66,0x7e,0x25,0x6b,0x73,0xde,0x9b,0x90,0x81,
0x91,0x9f,0xcf,0x9b,0x83,0x93,0x8d,0x8d,0x95,0xa9,0xfb,0xb5,0xaf,0xf3,0xa1,0xab,
0xbe,0xa4,0xae,0xb7,0xb1,0xd6,0xd2,0xd4,0x96,0xb5};
										


unsigned char Manual[]="          This is the\n     Original Expert Version.\n\nCalcReg is a Powerful "
		"Tool which accompanies you in your scientific"
		" work. It has the usual basic functions of any calculator and handles the special"
		"functions as well.\n"
		"A few bugs have been fixed since the previous launch and new implementations "
		"are present. Bigger program size.\n"
		"The Functions are calculated using a Base algorithm which gives exact results.\n"
		"The Original Expert is given for free. A donation is possible through the website if you want to reward the "
		"work done for this program: www.palmreg.fr.mu\n"
		"This software has got complexe in built calculation, which gives the possibility to "
		"integrate complexe functions (of the real variable). It is possible to handle complexe functions"
		" of the complexe variable. 3D plot is implemented. All "
		"the features with programs are available through the menu for solving, zero findings, etc...\n"

		"You comments are welcome, just send an email at palmreg@gmail.com\n"
		"or if failing address: (rimavie@gmail.com).\n\n"

		"The first step to get this tool into hand is to try a few examples by cliking"
		" <tst>. Go through the different examples by successively cliking on <tst>."
		"Launch these examples by clicking <exec> button.\n"
		"Clear the Prog area with <cl^>. \nClear the Cmd area with <clv>.\n"
		"If you can't get to the top or down your program, just tap the screen in the text field and"
		" keeping pen on the screen go up or down as you would do to select a text. This happens sometimes"
		" and needs to be fixed...You can also use the palm keyboard to write your program.\n"
		"Cut Copy paste are available in the edition of your program using grafittis. (Check your palm manual.)\n" 
		"This software is based on the reinterpretation of the math syntax into a "
		"list of codes. By debugging you may find the long list of conversion and "
		"launching of these codes. Debugging will be useful for interpreting the error message. "
		"Error tracking message is basic giving the line number where the error is found.\n\n"
		"But, let's have a look to the set of instructions now:\n\n"
		"While your program is running, if it is taken into a loop or if you want to quit the operation, "
		"just click on the 'Brk' blicking at the up right corner until it stops.\n\n"
		"Warning: It is important when you use a variable, you need to make sure you have"
		" defined it at least once by var=... for example t=1 is enough. If you use F1(x) without"
		" defining x as said above, then an error will happen. So remember that when building "
		"your programs.\n\n"
		"You can save your program through the menu.\n"
		"---- Set of Instructions ---\n\n"
		"grid a\n*Draws the grid, a=Width graduation.\n\n"
		"gfxdim x1,x2,y1,y2,deltaX\n*Grid dimension with delta X the "
		"step of calculation of the functions.\n\n"
		"box3d wx,wy,wz,Inc3d\n*Box 3d dimension for the 3d plot of f(x,y). dim box is "
		"from -wx to wx, -wy to wy and -wz to wz. Inc3d is the delta x and y for plotting. "
		"For 3d plotting you will need to adjust first the gfxdim to set an overall window "
		"larger than the 3d margins plot, and then setup the box3d for your needs.\n\n"
		"Trf(x)= (..function(x) here..)\n*Draws the function x->f(x), use x or t or any variable. "
		"The drawing of a function of 2 variables Trf(x,y) is supported.\n\n"
		"Int(a,b,h,x) F..(x)\n *Integrates the function F on the variable x. For precision in"
		" the integration you need to put h small: between 0.1 down to 0.001 or less.\n"
		"x<y => goto lbl\n*If x<y then goes to the label "
		"'lbl:'. The label is set by 'name:' \n\n"
		"bsr lbl\n *branch subroutine.\n\n"
		"rts\n *return from subroutine.\n\n"
		"Re\n *Real part. Set Allow Complexe to use it.\n"
		"Im\n *Imaginary part. Set Allow Complexe to use it.\n"
		"mod(z) to get module of complexe z. Set Allow Complexe to use it.\n"
		"wait n\n *Wait for n Ticks (time units)\n\n"
		"getserial Baudrate,mode\n *open serial port and get 1000bytes  serial data available with OSC(x) function\n\n"  
		"putserial Baudrate,mode,bytedata\n *open serial port and put bytesdata \n\n"  
		"print x \n  *Prints the value of var x\n\n"
		"-Math Functions Supported-\n\n"
		"+ - * / cos sin tan ch sh th acos asin atan ach ash ath x^a sqrt ln exp fact abs\n\n"
		"The algorithm searches first the variables x, y,... followed by the parentheses, "
		"the Functions Fn(x) and at last the numerical priorities * /  then + and - at the end." 
		" Enjoy. If you have any question send email to: palmreg@gmail.com\n";

unsigned char Philosophy[]="\n     Technological Philosophy\n                           or\n"
        "              Modern Slavery\n\nIt seems clear that nowadays unlikely to previous years we have to position "
		"ourselves regarding technology. None is untouched.\nSometimes I have a few thoughts about "
		"what is happening in the world. To some extend I realise how much we get more and more "
		"dependant on machines, how attractive new gadgets can be to us... and what difficulty I had to get out "
		"of my computer, while programming this software, just to feed myself, or spend time with my wife. "
		"What fascination do computers operate on us ?\n"
		"We have computers everywhere, in the kitchen for cooking receipes, "
		"in the living room for entertainments, for leisures... Recently in the teeth for preventing tooth "
		"decay. What about mobile phones ! They are now able to check your position through triangulation "
		"with the emitters, most of them have GPS integrated. They have cameras, did you check for Robot "
		"Programs able to make face recognition... no not yet, you will tell me.\n"
		"\n         NANOTECHNOLOGIES\n\n"
		"Have you noticed the arrival of nanotechnologies. Today computers of the size of less than a centimeter "
		"square using nanotechnologies are almost more powerful than any of the computers we are using. "
		"You will understand that they are able to perform tasks of great precision if they are integrated into human "
		"cells. Science fiction?!... No way! It is already on research for creating treatments against stress, for injections "
		"of medicins for diabetic,... to perform fights againts bacteria... this is the view of the upper side of the iceberg. "
		"\nYou will certainely have to position yourselves on the technologies you want to use or refuse.\n"
		"Now under the pretext of Security you will be asked all information, you will be given the possibility to "
		"receive microcomputers within your body... giving up your privacy. You ought to be very confident in "
		"your respective government to rush in this direction.\n"
		"It seems to me that we've entered a time where Big Brother of George Orwell 1984 is just a kid science fiction "
		"story compared to what is likely to happen.\n"
		"And we rush further and further, hearing always the same motivations: \nSecurity, Comfort, Progress, "
		"Medical advancements, Peace... At what price ! Control, localisation, No Privacy, Don't think just keep "
		"on Labour for the sake of the community.\n"
		"For the Sake of What??? \n"
		"What community ? Do you meet your neighbors ? Today we are brought to fear anyone. Just watch "
		"TV programs :Serial killers everywhere, with special agents. This is not innocent. These programs are "
		"chosen for you, to push you to ask for more security, for more technology. You are brought through "
		"the actual media to think and act as to fear the others and not join your friends,... or meet virtual friends only "
		"through chats, where almost all data are recorded."
		"When the television arrived in our house, I thought, this is a good invention but what are they going "
		"to do with that ? The programs got worse and worse.\n"
		"When internet arrived out from the universities ' lab,... I thought, this is a good invention but are we "
		"going to do with that...\n"
		"Now come the nanotechnologies, the modified bacteria as firms to perform tasks,... I am thinking "
		"they are not going to create back all the dinosaures,... with a nice lady on the adverts: Get a ride on "
		"your beautiful Tyranosaurus Rex for just a few quids,... are they ? \n"
		"It certainly is a movement we can't stop. The machine is on. Anyone will have to make the choice.for himself.\nAt the moment, if Television "
		"upsets you, there is just one button to press to make it die or sleep. But if you accept to get in your "
		"body such technologies as those which  are coming,... I wonder where is the ON/OFF button.\n"
		"\n\nJust think it for yourself... Why is privacy so important, after all ?\n"
		"One may ask himself why would "
		"someone want to know everything about you. Why would some organisation want to record all your "
		"sms, your chats conversations, your personal information,... ?\n"
		"To be honest, I do not undersand it myself ! This is such a crazy idea ! But this is what is happening.";
		
unsigned char NewFunctionalties[]="Welcome abord, there are changes with previous versions.\n"
		"\nYou can set low precision for faster graph plotting.\n"
		"Click M. It is possible to move graph once there are plotted by clicking and dragging in the plot window."
		"If Z appears in the right down corner then your are in Move mode.\n"
		"Click Z to make a Zoom and define your zoom-window by click - drag - release.\n"
		"Click the star * at the up corner of the graph window to select the point coordinates and click the point in the graph window.\n"
		"+ and - to zoom in and Zoom out.\n\n"
		"Click D displays the derivation of the function on the display!\n\n."
		"These new options make this tool more handy for plotting functions.\n";
		


/*
 * StartApplication
 */
void StartApplication(void) {
	char information[]="cmd:";
	char information1[]="Build Math Library";

	decode (Copyright2,sizeof(Copyright2) ); //decodage du text cripté avec un cosinus
	PrintCmd(Copyright2);
	decode (StartInfo,sizeof(StartInfo) );//decodage du text cripté

	PrintCmd(StartInfo);
	StartInfoDone=1;//after that the next printing will first clean the cmd field

	//decode(Copyright,sizeof(Copyright) );//decodage du text cripté
	//decode (CodedWord,sizeof(CodedWord) );//decodage du text cripté
}

void decodeCreator(){
	if (CodedWordConverted == 0) decode (CodedWord,sizeof(CodedWord) );//decodage du text cripté
	CodedWordConverted = 1;
	}
 
void decodeCopyRight(){
	if (CopyRightConverted == 0)	decode(Copyright,sizeof(Copyright) );//decodage du text cripté
	CopyRightConverted = 1;
	}
 
void PrintProg(char * s){
	printf("PrintProg not yet implemented\n----\n");
	printf ("%s",s);
	printf ("----\n");
}

void DeleteProg(){
	printf ("DeletedProg not yet implemented\n");
}


void SetUpTextProg(int position){
/*	FormPtr 	Frm;
	FieldPtr 	FldPtr;
	ScrollBarType *scrollBarPtr;
	UInt16 valueP,minP,maxP,pageSizeP;
	UInt16 pos,textHeight,fieldHeight,maxValue;

	pos =(UInt16) position;
	Frm = FrmGetFormPtr(frmadc16);
	FldPtr = (FieldPtr)(FrmGetObjectPtr(Frm, (FrmGetObjectIndex(Frm, fld_prog))));
	FldSetScrollPosition (FldPtr,pos);
	//change and Set maxValue for the scrollbar
	scrollBarPtr = FrmGetObjectPtr(Frm,FrmGetObjectIndex(Frm, ProgScrollbarId));
	FldGetScrollValues (FldPtr, &pos,&textHeight, &fieldHeight);//from field

	if (textHeight > fieldHeight) maxValue = textHeight - fieldHeight;
	else if (pos) maxValue = pos;
	else maxValue = 0;
	SclSetScrollBar (scrollBarPtr,pos, 0, maxValue,fieldHeight-1);
*/
	printf ("SetUpProg not implemented\n");
}

void DisplayManual(){
		DeleteProg();
		PrintProg(Manual);
		SetUpTextProg(0);//set at start of the text
}
