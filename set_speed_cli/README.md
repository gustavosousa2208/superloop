# superloop

rascunho pra comunicação can, rs485 e protocolos de segurança para controle de velocidade do inversor.

## TODO

- [ ] Testar como receber várias mensagens de uma vez
- [ ] Reimplementar pthreads e fazer a comunicação entre elas com funções próprias de real-time
- [ ] Lidar com a situação de escrever na interface na mesma hora de ler (mutexes, semáforos)
- [ ] Interface para enviar velocidade, comandos e ler velocidade
- [ ] Ao invés de mandar a voltagem pra ui como float, melhor mandar como inteiro e colocar um ponto de mentira no ui?
- [ ] No WSL a interface fica resposiva e com relativa folga na latência, só que seja na RPI ou OPI a interface fica menos responsiva, só que com menor latência 🤔
- [ ] Implementar timestamps
- [ ] Implementar um sistema de log
- [ ] Implementar um sistema de configuração
- [ ] Usar memcpy ao invés de for para copiar structs
- [ ] Juntar mensagens no inversor numa ID só

## Dúvidas

- [ ] Se o BMS já mostra corrente e tensão, é preciso coletar esses dados do inversor também?

## Formato de mensagens

É possível exigir do inversor que ele envie múltiplos parâmetros numa mesma mensagem CAN, então aqui tem o formato de mensagens que o inversor envia e quais envia.

Obs.: No WEG WLP coloquei os id's como FF(XX), mas o FF truncou para 07(XX), devido ao máximo do CAN 1.0 que estamos usando.

ID : 0x701 -> 685, 680, 38, 39
16 bits - Setpoint RPM can : 685
16 bits - Estado lógico do inversor : 680
16 bits - Leitura de RPM do encoder : 38
16 bits - Contador de pulsos do encoder : 39

ID : 0x702 -> 30, 33, 34, 1
16 bits - Temperatura do MOSFET 1: 30
16 bits - Temperatura do MOSFET 2: 33
16 bits - Temperatura do ar interno : 34
16 bits - referência de rotação, valor do display superior direito : 1

ID : 0x703 -> 3, 4, 7, 8
16 bits - Corrente do motor : 3
16 bits - Tensão da bateria : 4
16 bits - Tensão de saída (motor) : 7
16 bits - Velocidade do veículo (km/h) : 8
