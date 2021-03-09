

#include <Windows.h>  
#include <iostream> 
#include <math.h>  
#include "HackProcess.h"
#include <vector>
#include <algorithm>    // std::sort
//Create our 'hooking' and process managing object
CHackProcess fProcess;  
using namespace std;  
//We use F6 to exit the hack
#define F6_Key 0x75
//right click
#define RIGHT_MOUSE 0x02
//Here we store the num of players and update it regularly to know how many enemies we are dealing with
//this saves us doing loops unless their necessary e.g. we have 12 players and still loop 32 times wasting our great resources
//This makes our triggerbot MUCH faster in general
int NumOfPlayers = 15;
const DWORD dw_PlayerCountOffs = 0x6F4020;//Engine.dll //Tirar  //////engine.dll+6F4020

//The player base is VERY important so we know where our player info is at
//including current jump status so we can use force jumping making our bHop
const DWORD GameBasePtr = 0xC000;
const DWORD GameBase = 0x00A15F68;

DWORD MeuPlayerBase;
int localizado = 0;
int localizadoang = 0;
int contador = 0;
const DWORD Player_Base = GameBase-0x198;//0x00574560;
//The ATTACK address BELOW, WE WRITE 5 TO SHOOT OR 4 TO 
const DWORD dw_MyTeamOffset = 0xD8;//client // + vai virar -
const DWORD dw_mTeamOffset = 0xD8;//client // -
const DWORD dw_MyHealth = 0x198;//client //+
const DWORD dw_Health = 0x00;//client //+
//FOR the x coord we can use cl_pdump for m_vecOrigin Vector or just move around the map looking for different values
//e.g. to find y coordinate walk up ladder search up, walk down search down etc.
const DWORD dw_Pos = 0x48;//client //0134 //-
const DWORD dw_MyPos = 0x150;//client //+
const CHAR mname = 0x4;//+
//Enemy Vars including the enemy loop
const DWORD EntityPlayer_Base = GameBase;
//How far in memory is each enemy data
const DWORD EntityLoopDistance = 0x1E0;

const DWORD waller = 0x9A8320;
//ViewAngles
//We find these by moving our mouse around constantly looking for changed/unchanged value,
//the alternative is to use cl_pdump 1 and search for the value assigned to m_angRotation vector
                            

//We will use this struct throughout all other tutorials adding more variables every time

DWORD dw_m_angRotationFINAL = 0x09B23760;
//const DWORD dw_m_angRotationFINAL = 0x00;

/*
void EncontraAngulo()
{

float angulo;
float angulocompare;
DWORD compareptr;

for(int i = 0; i < 3300;i++) {
	if(localizadoang == 0) {
ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + 0xA00EB8), &angulo, sizeof(float), 0);
ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + 0xi), &compareptr, sizeof(DWORD), 0);
ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(compareptr), &angulocompare, sizeof(DWORD), 0);
printf("contador = %d \n", i);
printf("Angulo encontrado addr %x \n", compareptr);
printf("Angulo encontrado %f \n", angulocompare);
if(angulo == angulocompare)
{
dw_m_angRotationFINAL = compareptr;
printf("Angulo encontrado addr %x \n", compareptr);
printf("Angulo encontrado %f \n", angulocompare);
localizadoang = 1;
}
	}
}



}*/


struct MyPlayer_t  
{ 
	DWORD CLocalPlayer; 
	DWORD ggGameBase;
	DWORD ggGameBaseB;
	DWORD ggGameBaseC;
	int Team; 
	int Health; 
	float Position[3]; 

	float poscomparer;
	float posfinder;

	DWORD TeamPtr;
	DWORD HealthPtr;
	DWORD PositionPtr;

	DWORD poscomparerptr;

	DWORD BaseOffsA, BaseOffsB,BaseOffsC,BaseOffsD,BaseOffsE,BaseOffsF,BaseOffsG,BaseOffsH;
	DWORD BaseOffsAptr, BaseOffsBptr,BaseOffsCptr,BaseOffsDptr,BaseOffsEptr,BaseOffsFptr,BaseOffsGptr,BaseOffsHptr;
	DWORD TeamGambiarra;
	void ReadInformation() 
	{
		

			ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + GameBase), &ggGameBase, sizeof(DWORD), 0);
		
		BaseOffsA = ggGameBase+0x4FC;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsA), &BaseOffsAptr, sizeof(DWORD), 0);
		BaseOffsB = BaseOffsAptr+0x504;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsB), &BaseOffsBptr, sizeof(DWORD), 0);
		BaseOffsC = BaseOffsBptr+0xD4;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsC), &BaseOffsCptr, sizeof(DWORD), 0);
		BaseOffsD = BaseOffsCptr+0x0;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsD), &BaseOffsDptr, sizeof(DWORD), 0);
		BaseOffsE = BaseOffsDptr+0x90;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsE), &BaseOffsEptr, sizeof(DWORD), 0);
		BaseOffsF = BaseOffsEptr+0x7C;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsF), &BaseOffsFptr, sizeof(DWORD), 0);
		BaseOffsG = BaseOffsFptr+0x120;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsG), &BaseOffsGptr, sizeof(DWORD), 0);
		BaseOffsH = BaseOffsGptr+0x200;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsH), &BaseOffsHptr, sizeof(DWORD), 0);



	//	ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(gggGameBaseB  + (Player * EntityLoopDistance)),&CBaseEntity, sizeof(DWORD), 0);

		
		for(int i = 0; i < 32; i++) {
			if(localizado == 0) {
		CLocalPlayer = BaseOffsH  + (i * EntityLoopDistance);
		poscomparerptr = CLocalPlayer - dw_Pos;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + 0x9B8960), &posfinder, sizeof(float), 0);
        //ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(Nameptr), &Nameptrb, sizeof(DWORD), 0);
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(poscomparerptr), &poscomparer, sizeof(float), 0);
		printf("posfinder: %f \n", posfinder);
		printf("poscomparer: %f \n", poscomparer);
		if(posfinder == poscomparer) {
			CLocalPlayer = BaseOffsH  + (i * EntityLoopDistance);
            localizado = 1;
			contador = i;
			printf("Localizado ;p");
		}else {
        localizado = 0;
		}
		//printf("i = %d", i);
		}
		}
		
		
		
		
		//ggGameBaseB = BaseOffsH;
		//ggGameBaseC = BaseOffsH-0x198;
	//	printf("BaseOffsH = %x", BaseOffsH);

  // ###############CLocalPlayer = ggGameBaseC; ******************************************/

				//////ACHA O MEU PLAYER NO MEIO DOS PLAYERSS!!!!!!!!!!


		
CLocalPlayer = BaseOffsH  + (contador * EntityLoopDistance);
//printf("Localizado %d", localizado);
//CLocalPlayer = MeuPlayerBase-0x4;


	HealthPtr = CLocalPlayer+dw_Health;
	PositionPtr = CLocalPlayer-dw_Pos;
	TeamPtr = CLocalPlayer-dw_mTeamOffset;
	//printf("Team: %x", TeamPtr);
		// Reading CLocalPlayer Pointer to our "CLocalPlayer" DWORD. 
		//ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(ggGameBaseC), &CLocalPlayer, sizeof(DWORD), 0);
		// Reading out our Team to our "Team" Varible. 


		
		
		// Reading out our Health to our "Health" Varible.     
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(HealthPtr), &Health, sizeof(int), 0); 

	
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(TeamPtr), &Team, sizeof(int), 0);//Tirar
	
		// Reading out our Position to our "Position" Varible. 
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(PositionPtr), &Position, sizeof(float[3]), 0);

		//Here we find how many player entities exist in our game, through this we make sure to only loop the amount of times we need
		//when grabbing player data
		//Note that this call could be even better at a regular 15 or so seconds timer but performance shouldn't vary a great deal
		//ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_PlayerCountOffs), &NumOfPlayers, sizeof(int), 0); //Tirar

	//DWORD PosX = 0x9B8960;
	//DWORD PosZ = 0x9B8964;
	//DWORD PosY = 0x9B8968;


	}
}MyPlayer;    




//Here is where we narrow the enemy data to the enemies that we want to shoot, 
//these players are added once we validated them e.g. do they have enough health?, or maybe they are on the enemy team
struct TargetList_t 
{
	float Distance;
	float AimbotAngle[3];

	TargetList_t()
	{
		//our default constructor :S
	}

	TargetList_t(float aimbotAngle[], float myCoords[], float enemyCoords[])
	{
		//Send our coordinates and the enemy's to find out how close they are to us!
		Distance = Get3dDistance(myCoords[0], myCoords[1], myCoords[2],
								enemyCoords[0], enemyCoords[1], enemyCoords[2] );

		//Define our aimbot angles and set them for later use when shooting
		AimbotAngle[0] = aimbotAngle[0];
		AimbotAngle[1] = aimbotAngle[1];
		AimbotAngle[2] = aimbotAngle[2];
	}

	//Get our 3d Distance between 2 sets of coordinates(ours and enemies) and find out how close an enemy is to us
	//when it comes to shooting we aim at the closest enemy
	//Simple but effective

	//Here we will AVOID vectors for simplicity but feel free to create to use them
	//if you prefer float Get3dDistance(std::vector<float> myCoords, std::vector<float> enemyCoords)
	
	//See how far away our enemy is to us
	float Get3dDistance(float myCoordsX, float myCoordsZ, float myCoordsY, 
						float eNx, float eNz, float eNy)
	{
		//Use the 3d distance formula to get a resulting Floating distance value POW!!
		return sqrt(
			pow(double(eNx - myCoordsX), 2.0) + 
			pow(double(eNy - myCoordsY), 2.0) + 
			pow(double(eNz - myCoordsZ), 2.0));
	}
};



//ENemy struct
struct PlayerList_t 
{
	DWORD gggGameBase;
	DWORD gggGameBaseB;
	DWORD gggGameBaseC;
	DWORD CBaseEntity; 
	int Team; 
	int Health; 
	float Position[3]; 
	float AimbotAngle[3]; 
	char Name[39]; 

	DWORD Healthptr;
	DWORD Teamptr;
	DWORD Positionptr;
	DWORD Nameptr;


	DWORD BaseOffsA, BaseOffsB,BaseOffsC,BaseOffsD,BaseOffsE,BaseOffsF,BaseOffsG,BaseOffsH;
	DWORD BaseOffsAptr, BaseOffsBptr,BaseOffsCptr,BaseOffsDptr,BaseOffsEptr,BaseOffsFptr,BaseOffsGptr,BaseOffsHptr;
	void ReadInformation(int Player) 
	{
		// Reading CBaseEntity Pointer to our "CBaseEntity" DWORD + Current Player in the loop. 0x10 is the CBaseEntity List Size 
		//"client.dll"+00545204 //0x571A5204







		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + GameBase), &gggGameBase, sizeof(DWORD), 0);
		
		BaseOffsA = gggGameBase+0x4FC;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsA), &BaseOffsAptr, sizeof(DWORD), 0);
		BaseOffsB = BaseOffsAptr+0x504;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsB), &BaseOffsBptr, sizeof(DWORD), 0);
		BaseOffsC = BaseOffsBptr+0xD4;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsC), &BaseOffsCptr, sizeof(DWORD), 0);
		BaseOffsD = BaseOffsCptr+0x0;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsD), &BaseOffsDptr, sizeof(DWORD), 0);
		BaseOffsE = BaseOffsDptr+0x90;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsE), &BaseOffsEptr, sizeof(DWORD), 0);
		BaseOffsF = BaseOffsEptr+0x7C;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsF), &BaseOffsFptr, sizeof(DWORD), 0);
		BaseOffsG = BaseOffsFptr+0x120;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsG), &BaseOffsGptr, sizeof(DWORD), 0);
		BaseOffsH = BaseOffsGptr+0x200;
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(BaseOffsH), &BaseOffsHptr, sizeof(DWORD), 0);



	//	ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(gggGameBaseB  + (Player * EntityLoopDistance)),&CBaseEntity, sizeof(DWORD), 0);

		CBaseEntity = BaseOffsH  + (Player * EntityLoopDistance);

		//printf("gggGameBaseB = %x", CBaseEntity);


     Teamptr = CBaseEntity - dw_mTeamOffset;
	 Healthptr = CBaseEntity + dw_Health;
     Positionptr =  CBaseEntity - dw_Pos;
	 Nameptr = CBaseEntity + mname;
	 

	 		// Reading out our Team to our "Team" Varible. 
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(Teamptr), &Team, sizeof(int), 0); //Tirar
		//printf("TeamId = %d \n", Team);
		// Reading out our Health to our "Health" Varible.     
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(Healthptr), &Health, sizeof(int), 0); 
		// Reading out our Position to our "Position" Varible. 
		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(Positionptr), &Position, sizeof(float[3]), 0); 

		ReadProcessMemory (fProcess.__HandleProcess, (PBYTE*)(Nameptr), &Name, sizeof(char), 0);


 	}
}PlayerList[32];  



//Used to compare the distance when sorting the array of Target Enemies
//JUST think of this as an assist for a sorting algorithm
//and this isn't necessarily related to hacking but it helps us out
struct CompareTargetEnArray
{
	//USE A COMPARATOR TO SORT OUR ARRAY nicely
	bool operator() (TargetList_t & lhs, TargetList_t & rhs) 
	{
		return lhs.Distance < rhs.Distance; 
	}
};


//Props to Nubtik and a couple of other places on the interwebs for this function
void CalcAngle( float *src, float *dst, float *angles )
{

	double delta[3] = { (src[0]-dst[0]), (src[1]-dst[1]), (src[2]-dst[2]) };
	double hyp = sqrt(delta[0]*delta[0] + delta[1]*delta[1]);
	angles[0] = (float) (asinf(delta[2]/hyp) * 57.295779513082f);
	angles[1] = (float) (atanf(delta[1]/delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;

	if(delta[0] >= 0.0) 
	{ 
		angles[1] += 180.0f; 
	}
}




	float Calculadistancia(float myCoordsX, float myCoordsZ, float myCoordsY, 
						float eNx, float eNz, float eNy)
	{
		//Use the 3d distance formula to get a resulting Floating distance value POW!!
		return sqrt(
			pow(double(eNx - myCoordsX), 2.0) + 
			pow(double(eNy - myCoordsY), 2.0) + 
			pow(double(eNz - myCoordsZ), 2.0));
	}

void Aimbot()
{
	float fNearest = (float)INT_MAX;//Meu cod
	int iAimAt = -1;
	int gambiarrameutime = 12;
	///////
	//Declare our target list to define our victims through a dynamic array
	TargetList_t* TargetList = new TargetList_t[NumOfPlayers];
	//Loop through all our players and retrieve their information
	//we use this mainly here to find out what players are our enemies and what players are on our team
	//loops through players to set the target enemies
	int targetLoop = 0;
	for(int i = 1; i < NumOfPlayers; i ++) //Começa com 2 pq se n mira em mim mesmo e explode a porra toda
	{
		if (GetAsyncKeyState(VK_RBUTTON)) { ////Meu
			i++;
		}
			if (GetAsyncKeyState(VK_F2)) { ////Meu
			gambiarrameutime = 12;
		}
				if (GetAsyncKeyState(VK_F3)) { ////Meu
			gambiarrameutime = 16;
		}

			if (GetAsyncKeyState(VK_F4)) { ////Meu
			gambiarrameutime = 8;
		}


		PlayerList[i].ReadInformation(i);

		// Skip if they're my teammates. 
		if (PlayerList[i].Team == MyPlayer.Team) 
			continue; 

		//Make sure our targets have at keast 2 health
		if (PlayerList[i].Health < 2) 
			continue; //break to next iteration //Tirar

		if(Calculadistancia(MyPlayer.Position[0], MyPlayer.Position[1], (MyPlayer.Position[2]),PlayerList[i].Position[0],PlayerList[i].Position[1], (PlayerList[i].Position[2]) ) < fNearest )//dis :D
				{
	
					//	if(!Parede.noobie && !Parede.aimdelay) {  //Mudar aki...

					iAimAt = i;

					fNearest = Calculadistancia(MyPlayer.Position[0], MyPlayer.Position[1], 
						(MyPlayer.Position[2]),PlayerList[i].Position[0],PlayerList[i].Position[1], (PlayerList[i].Position[2]) );
					//	}
				}


		//PlayerList[i].Position[2] -= 10;
		CalcAngle (MyPlayer.Position, PlayerList[i].Position, PlayerList[i].AimbotAngle);  

		//ONCE WE GET HERE we have already read all the information we want, all enemies have been checked for valid data
		//Therefore players on our team a dont get selected and only enemies with over 1 health are allowed

		//WE NOW DEFINE A LIST OF ENEMIES TO SHOOT AT, from here we will scan that array and choose to aim at the closest one
		//Create our target loop player and store the most important information
		TargetList[targetLoop] = TargetList_t(PlayerList[i].AimbotAngle, MyPlayer.Position, PlayerList[i].Position);

		//Increment so we can add more players onto the array
		targetLoop++;
	}




	//ONLY AIM if we have any enemies 
	if(targetLoop > 0)
	{
		//SORT ENEMIES ARRAY BY DISTANCE before AIMING AT THEM
		//JUST NORMAL std::sort function with our custom comparator, not much to it
		//target loop -1 because it will always go 1 above the count
		std::sort(TargetList, TargetList+targetLoop, CompareTargetEnArray());

		//AIM at the closest guy we found, 
		//by default aim at ALL times, if you right click hold it switches it off
		//This isn't the best example but change the example to whatevery you like E.g. Left click etc...
		if (GetAsyncKeyState(VK_MBUTTON)) 
		{
			if(iAimAt != -1) {
 
				
			/*
					WriteProcessMemory (fProcess.__HandleProcess, 
				(PBYTE*)(fProcess.__csgoexe+dw_m_angRotation), 
					TargetList[0].AimbotAngle, 12, 0);*/
				
				if(MyPlayer.Health > 2) { //Evita um bug louco
		WriteProcessMemory (fProcess.__HandleProcess, 
				(PBYTE*)(dw_m_angRotationFINAL), 
					TargetList[0].AimbotAngle, 14, 0);
				}
/*
				WriteProcessMemory (fProcess.__HandleProcess, 
				(PBYTE*)(dw_m_angRotationX), 
					TargetList[0].AimbotAngle, 12, 0);
					
						WriteProcessMemory (fProcess.__HandleProcess, 
				(PBYTE*)(dw_m_angRotationY), 
					TargetList[0].AimbotAngle, 12, 0);
								WriteProcessMemory (fProcess.__HandleProcess, 
				(PBYTE*)(dw_m_angRotationZ), 
					TargetList[0].AimbotAngle, 12, 0);*/

	

		

			}

			//no recoil above text
		}

	}
	//reset the loop counter
	targetLoop = 0;
	//DELETE OUR ARRAY and clear memory, very important
	delete [] TargetList;
}



int main()
{

	fProcess.RunProcess(); // Waiting for CSS......
	cout << "Game found! Running Aimbot..." << endl; 
 
	//Exit if the F6 key is pressed
	while (!GetAsyncKeyState(F6_Key)) // or for(;;)
	{
		//Read player information into our struct so we know the player 
		//base and the bunny hop addresses

		
		MyPlayer.ReadInformation(); 
	//	EncontraAngulo();


		//Call our Aimbot
		
		Aimbot();

	}
}
