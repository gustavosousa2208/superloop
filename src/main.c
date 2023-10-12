/*
  File: main.c
  Author: Gustavo Sousa
  Date: October 12, 2023
  Description: Ler e enviar mensagens CAN para uso geral.

  TODO : Ler assíncronamente, se é que necessário
  TODO : Criar função para enviar mensagens
  TODO : Formatar leitura de temperatura do inversor
*/

#include "funcs.h"

#define INTERFACE "vcan0"

int main() {
    
    int s = createCANSocket(INTERFACE);

    struct can_frame frame;
    while (1) {
        ssize_t nbytes = read(s, &frame, sizeof(struct can_frame));
        
        if (nbytes < 0) {
            perror("read");
            break;
        }

        if (frame.can_id == 0x123) {
            printf("Received CAN frame ID: 0x%X, Data Length: %d, Data: ", frame.can_id, frame.can_dlc);
            for (int i = 0; i < frame.can_dlc; i++) {
                printf("%02X ", frame.data[i]);
            }
            printf("\n"); 
        }
    }

    close(s);
}