#ifndef DS4_H
#define DS4_H


const struct can_frame param222_remote = {
    .can_id = 0x222,
    .can_dlc = 1,
    .data = {0x06} // 0x06 velocidade pelo CAN
};
const struct can_frame param222_local = {
    .can_id = 0x222,
    .can_dlc = 1,
    .data = {0x00} // 0x00 velocidade pelo HMI, 1 para acelerador
};
const struct can_frame param220_local = {
    .can_id = 0x220,
    .can_dlc = 1,
    .data = {0x00} // 0x00 para local, aparentemente todas as fontes
};
const struct can_frame param220_remote = {
    .can_id = 0x220,
    .can_dlc = 1,
    .data = {0x01} // 0x01 para remoto, aparentemente todas as fontes
};
const struct can_frame param226_local = {
    .can_id = 0x226,
    .can_dlc = 1,
    .data = {0x00} // 0x00 parece ser pelo HMI
};
const struct can_frame param226_remote = {
    .can_id = 0x226,
    .can_dlc = 1,
    .data = {0x01} // 0x07 para CAN
};
const struct can_frame param227_local = {
    .can_id = 0x227,
    .can_dlc = 1,
    .data = {0x00} // 0x00 parece ser pelo HMI
};
const struct can_frame param227_remote = {
    .can_id = 0x227,
    .can_dlc = 1,
    .data = {0x03} // 0x03 para CAN
};
const struct can_frame param228_local = {
    .can_id = 0x228,
    .can_dlc = 1,
    .data = {0x01} // 0x01 JOG pela tecla JOG
};
const struct can_frame param228_remote = {
    .can_id = 0x228,
    .can_dlc = 1,
    .data = {0x04} // 0x04 JOG para CAN
};
const struct can_frame param684_remote = {
    .can_id = 0x684,
    .can_dlc = 2,
    .data = {0x00} // só pra deixar zerado
};


#endif // DS4_H
