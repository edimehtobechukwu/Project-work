#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<windows.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>


#define ABitSleepTime 100
#define RoadLength   100  // Channel length, 100bit
#define ContentionPeriod  512/ / The contention period is 512bit and the shortest frame length 512bit
#define EnhanceConflictSignals 48 // Enhance conflict signal 48bit

(char road[RoadLength];  / / Define the channel )
// int busflag = 0; // Channel idle signal sign, 0 idle

void BitTime(int bit){// Transfer a time definition required for a bit
	while(bit-- != 0){
		Sleep(ABitSleepTime);
	}
}


void ChannelMonitor(int tranStart){ // Channel monitor 96bit
	int idleBit = 96;
	while(idleBit-- != 0){
		printf("Transtart:% D in channel monitor idlebit:% d \ n", tranStart, idleBit);
		if(road[tranStart] != '-'){// If the channel is not idle, continue to listen to 96 until the channel has been idle 96bit
			idleBit = 96;
		}
		BitTime(1);
	}
}



void IndexReg(int retranCount, int tranStart){ / / Index Retreat
	int r = 0;
	srand(time(NULL) - tranStart * 100000);
	r = rand()%4; // Return
// r = rand ()% ((int) POW (2, retrancount); // Return
	printf("Transtart:% D In the index retraction index, the number of times:% d \ n", tranStart, r);
	BitTime(r * ContentionPeriod); //
}

void* Client(void *arg){ / / Communication host * arg is 1 or -1
	int *flag = (int *)arg, tranStart = -1, data = ContentionPeriod + 8 * 8, j = 0, retranCount = 0; // Data is the 8-byte of the transmission minimum frame length + package
	if(*flag == 1) tranStart = 0; / / Determine the direction of channel transmission
	else tranStart = RoadLength - 1;
	while(1){
		ChannelMonitor(tranStart);// Monitor the channel idle for 96bit, ready to perform while
		while((data--) != 0){
			printf("A==>  %s   <==B\n", road);
			if(road[tranStart + j] == '-'){
				if(abs(j) == RoadLength - 1){ / / Signal is transmitted to the other end
					road[tranStart + j] = 'B' - *flag;
					BitTime(1);
					printf("A==>  %s   <==B\n", road);
					if(data != 0){ // Because no conflict is detected, it can be considered that there will be no conflict in the back transmission.
						BitTime(data);
						data = 0;
						j = 0;
						while(abs(j) != RoadLength - 1){/ / The signal gradually passes to the other side
							road[tranStart + j] = '-' ;
							j += (*flag);
							BitTime(1);
						}
						printf("Transtart:% D Transfer is successful, the number of times required to retransmit" D \ N ", tranStart, retranCount);
						memset(road, '-', sizeof(road)); // Auxiliary channel must be emptied
						retranCount = 0;
					}
				}
				else{
					road[tranStart + j] = 'B' - *flag;
					j += (*flag);
				}

			}
			else{// Conflict, send 48bit enhancement conflicts, then enter the random index return algorithm
				printf("Transtart:% D Conflict \ n", tranStart);
				retranCount += 1;
				printf("Transtart:% D senders to enhance conflict signal% D \ N", tranStart, EnhanceConflictSignals);
				BitTime(EnhanceConflictSignals + RoadLength);
				memset(road, '-', sizeof(road));/ / Clear channel, enter the index return algorithm
				printf("Transtart:% D senders to enhance conflict signals" \ n ", tranStart);
				if(retranCount > 10){
					printf("The number of retransmissions exceeds 10 times, no longer continuing to retransmit \ n");
					retranCount = 0;
					break;
				}
				else{// Enter the index return algorithm
					printf("Transtart:% D Enter the return algorithm \ n", tranStart);
					IndexReg(retranCount, tranStart);
					break;
				}
			}
			BitTime(1);
		}
		data = ContentionPeriod + 8 * 8;
		j = 0;
	}

}


int main(){
	int tranStart1 = 1, tranStart2 = -1;
	pthread_t thid1,thid2;
	memset(road, '-', sizeof(road));
	printf("Creating host a \ n");// Create thread 1
//	srand(time(NULL));
	if(pthread_create(&thid1, NULL, Client, &tranStart1)){
		printf("thread1 creation failed\n");
		exit(1);
	}
	printf("Creating host B \ N");// Create thread 2
	if(pthread_create(&thid2, NULL, Client, &tranStart2)){
		printf("thread2 creation failed\n");
		exit(1);
	}
	while(1){
//		printf("A==>  %s   <==B\n", road);
		BitTime(10);
	}
	return 0;
}



