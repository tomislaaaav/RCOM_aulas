#ifndef ___LINKLAYER
#define ___LINKLAYER

#include "utils.h"
#include "state.h"
#include "alarm.h"
#include "bstuffing.h"

#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <strings.h>

typedef struct {
	char * port; 						//Dispositivo /dev/ttySx, x = 0, 1
	int fd;
	int baudRate; 						//Velocidade de transmissão
	unsigned int timeout; 				//Valor do temporizador: 1 s
	unsigned int maxTries; 				//Número de tentativas em caso de falha
	unsigned int maxFrameSize;
	unsigned int maxPacketSize;
	int status;
	struct termios * oldtio;
	char * dataPacket;
} LinkLayer;

void ll_open(LinkLayer *link_layer);
void ll_close(LinkLayer *link_layer);

void ll_open_receiver(LinkLayer *link_layer);
void ll_open_transmitter(LinkLayer *link_layer);

void ll_close_receiver(LinkLayer *link_layer);
void ll_close_transmitter(LinkLayer *link_layer);

int ll_write(LinkLayer *link_layer, int size);

int ll_read(LinkLayer *link_layer);

void ll_init(LinkLayer * newLinkLayer, char port[20], int baudRate, unsigned int sequenceNumber, unsigned int timeout, unsigned int maxTries, unsigned int maxFrameSize, int status);

void ll_end(LinkLayer * linkLayer);

#endif /* __LINKLAYER */


