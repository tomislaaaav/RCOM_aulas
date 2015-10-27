#include "state.h"
#include "utils.h"
#include "alarm.h"

#include <unistd.h>
#include <stdio.h>

static volatile int STOP_UA=FALSE;

static volatile int STOP_SET=FALSE;

static volatile int STOP_DISC=FALSE;

static volatile int STOP_RR=FALSE;

static volatile int STOP_I=FALSE;

int send_SET(int fd, char *SET) {
	
	int res;
	int flag = getFlag();
	
	if (flag){
		alarm(3);
		setFlag(0);
	}

	res = write(fd, SET, sizeof(SET));

	printf("FLAGS SENT FROM SET: %x, %x, %x, %x, %x\n\n", SET[0], SET[1], SET[2], SET[3], SET[4]);
	
	return res;
}

int send_UA(int fd, char *UA) {
  int res;
  
  res = write(fd, UA, sizeof(UA));

  printf("FLAGS SENT FROM UA: %x, %x, %x, %x, %x\n", UA[0], UA[1], UA[2], UA[3], UA[4]);

  return res;
}

int send_DISC(int fd, char *DISC) {
  
  int res;
  
  int flag = getFlag();
	
	if (flag){
		alarm(3);
		setFlag(0);
	}
  
  res = write(fd, DISC, 5);

  printf("FLAGS SENT FROM DISC: %x, %x, %x, %x, %x\n", DISC[0], DISC[1], DISC[2], DISC[3], DISC[4]);

  return res;  
}

int send_RR(int fd, int r){
	char RR[5];

	RR[0] = FLAG;
	RR[1] = A;
	RR[2] = (r << 5) | 0x01;
	RR[3] = RR[1] ^ RR[2];
	RR[4] = FLAG;

	write(fd, RR, 5);
	
	return 0;
}

void receive_UA(int fd, char *UA) {

	int option = START;
	char flag_ST;

	while(!(STOP_UA)) {
				
		read(fd, &flag_ST, 1);
		int flag = getFlag();
		if(flag && flag != -1){
		      alarm(0);
		      setFlag(-1);
		      STOP_UA = TRUE;
		   }

		switch (option){
			case START:
				if (flag_ST == F){
						option = FLAG_RCV;
						UA[0] = flag_ST;
					}
				else
					option = START;
				break;

			case FLAG_RCV:
				if (flag_ST == F){
						option = FLAG_RCV;
						UA[0] = flag_ST;
					}
				else if (flag_ST == A){
						option = A_RCV;
						UA[1] = flag_ST;
					}
				else
					option = START;
				break;

			case A_RCV:
				if (flag_ST == F){
						option = FLAG_RCV;
						UA[0] = flag_ST;
					}
				else if (flag_ST == C_UA){
						option = C_RCV;
						UA[2] = flag_ST;
					}
				else
					option = START;
				break;

			case C_RCV:
				if (flag_ST == F){
						option = FLAG_RCV;
						UA[0] = flag_ST;
					}
				else if (flag_ST == BCC_UA){
						option = BCC_OK;
						UA[3] = flag_ST;
					}
				else
					option = START;
				break;

			case BCC_OK:
				if (flag_ST == F){
						option = STOP_ST;
						UA[4] = flag_ST;
					}
				else
					option = START;
				break;

			case STOP_ST:
				STOP_UA = TRUE;
				break;

			default:
				break;
		} 
	}
}

void receive_SET(int fd, char *SET) {
  
  char flag_ST;
  int option = START;
  
  while(!(STOP_SET)){	
	read(fd, &flag_ST, 1);

	switch (option){
		case START:
			if (flag_ST == F){
					option = FLAG_RCV;
					SET[0] = flag_ST;
				}
			else
				option = START;
			break;

		case FLAG_RCV:
			if (flag_ST == F){
					option = FLAG_RCV;
					SET[0] = flag_ST;
				}
			else if (flag_ST == A){
					option = A_RCV;
					SET[1] = flag_ST;
				}
			else
				option = START;
			break;

		case A_RCV:
			if (flag_ST == F){
					option = FLAG_RCV;
					SET[0] = flag_ST;
				}
			else if (flag_ST == C_SET){
					option = C_RCV;
					SET[2] = flag_ST;
				}
			else
				option = START;
			break;

		case C_RCV:
			if (flag_ST == F){
					option = FLAG_RCV;
					SET[0] = flag_ST;
				}
			else if (flag_ST == BCC_SET){
					option = BCC_OK;
					SET[3] = flag_ST;
				}
			else
				option = START;
			break;

		case BCC_OK:
			if (flag_ST == F){
					option = STOP_ST;
					SET[4] = flag_ST;
				}
			else
				option = START;
			break;

		case STOP_ST:
			STOP_SET = TRUE;
			break;

		default:
			break;
	} 
  }
}

void receive_DISC(int fd, char *DISC_rec) {

  char flag_ST;
  int option = START;
  
  while(!(STOP_DISC)){	

	read(fd, &flag_ST, 1);
	
	int flag = getFlag();
	    if(flag && flag != -1){
		  alarm(0);            
		  setFlag(-1);
		  STOP_DISC = TRUE;
		}
	switch (option){
		case START:
			if (flag_ST == F){
				option = FLAG_RCV;
				DISC_rec[0] = flag_ST;
				}
			else
				option = START;
			break;

		case FLAG_RCV:
			if (flag_ST == F) {
				option = FLAG_RCV;
				DISC_rec[0] = flag_ST;
				}
			else if (flag_ST == A){
				option = A_RCV;
				DISC_rec[1] = flag_ST;
				}
			else
				option = START;
			break;

		case A_RCV:
			if (flag_ST == F){
					option = FLAG_RCV;
					DISC_rec[0] = flag_ST;
				}
			else if (flag_ST == C_DISC){
					option = C_RCV;
					DISC_rec[2] = flag_ST;
				}
			else
				option = START;
			break;

		case C_RCV:
			if (flag_ST == F){
					option = FLAG_RCV;
					DISC_rec[0] = flag_ST;
				}
			else if (flag_ST == BCC_DISC){
					option = BCC_OK;
					DISC_rec[3] = flag_ST;
				}
			else
				option = START;
			break;

		case BCC_OK:
			if (flag_ST == F){
					option = STOP_ST;
					DISC_rec[4] = flag_ST;
				}
			else
				option = START;
			break;

		case STOP_ST:
			STOP_DISC = TRUE;
			break;

		default:
			break;
	} 
  }
}

int receive_RR(int fd, char *RR, int s) {

	char flag_ST;
	int option = START;
	int r = s ? 0 : 1;
	int c_rr = 1 | (r << 5); 

	while(!(STOP_RR)){
		read = (fd, &flag_ST, 1);

		int flag = getFlag();
		if(flag && flag != -1){
		    alarm(0);
		    setFlag(-1);
		    STOP_RR = TRUE;
		    return -1;
		}


		switch (option) {
			case START:
				if (flag_ST == F){
						option = FLAG_RCV;
						RR[0] = flag_ST;
					}
				else
					option = START;
				break;
			case FLAG_RCV:
				if (flag_ST == F){
						option = FLAG_RCV;
						RR[0] = flag_ST;
					}
				else if (flag_ST == A) {
						option = A_RCV;
						RR[1] = flag_ST;
					}
				else
					option = START;
				break;
			case A_RCV:
				if (flag_ST == F){
						option = FLAG_RCV;
						RR[0] = flag_ST;
					}
				else if (flag_ST == c_rr) {
						option = C_RCV;
						RR[2] = flag_ST;
					}
				else
					option = START;
				break;
			case C_RCV:
				if (flag_ST == F) {
						option = FLAG_RCV;
						RR[0] = flag_ST;
					}
				else if (flag_ST == c_rr^A){
						option = BCC_OK;
						RR[3] = flag_ST;
					}
				else
					option = START;
				break;
			case BCC_OK:
				if (flag_ST == F){
						option = STOP_ST;
						RR[4] = flag_ST;
					}
				else
					option = START;
				break;

			case STOP_ST:
				STOP_RR = TRUE;
				break;
			default:
				break;
		}
	}
	return r;
}

int receive_FRAME(int fd, char *I) {

	char flag_ST;
	int data = 0;
	int option = START;

	while(!(STOP_I)){
		read = (fd, &flag_ST, 1);

		switch (option) {
			case START:
				data = 0;
				if (flag_ST == F){
					option = FLAG_RCV;
					I[0] = flag_ST;
					data++;
				}
				else
					option = START;
				break;
			case FLAG_RCV:
				data = 1;
				if (flag_ST == F) {
					option = FLAG_RCV;
					I[0] = flag_ST;
				}
				else{
					I[data] = flag_ST;
					data++;
					option = A_RCV;
				}
				break;
			case A_RCV:
				if (data >= 6 && flag_ST == F){
					I[data] = flag_ST;
					option = STOP_ST;
				}
				else if (data > MAX_FRAME_SIZE){
					option = START;
				}
				else if (data < 6 && flag_ST == F){
					option = A_RCV;
				}
				else{
					I[data] = flag_ST;
					data++;
					option = A_RCV;
				}
				break;
			case STOP_ST:
				data++;
				STOP_I = TRUE;
				break;
			default:
				break;
		}
	}
	return data;
}


int check_UA(char *sent) {
	int error = 0;
	if (sent[0] != F || sent[1] != A || sent[2] != C_UA || sent[3] != (sent[1]^sent[2]) || sent[4] != F)
		error = 1;
	return error;
}

int check_DISC(char *DISC_rec) {
	int error = 0;
	if (DISC_rec[0] != F || DISC_rec[1] != A || DISC_rec[2] != C_DISC || DISC_rec[3] != (DISC_rec[1]^DISC_rec[2]) || DISC_rec[4] != F)
		error = 1;
	return error;
}

int getStopUA(){
  return STOP_UA;
}

void setStopUA(int st) {
  STOP_UA = st;
}

int getStopSET() {
  return STOP_SET;
}

void setStopSET(int st) {
  STOP_SET = st;
}

int getStopDISC() {
  return STOP_DISC;
}

void setStopDISC(int st) {
  STOP_DISC = st;
}

void setStopRR(int st) {
	STOP_RR = st;
}

int getStopRR() {
	return STOP_RR;
}
