# superloop

rascunho pra comunicação can, rs485 e protocolos de segurança para controle de velocidade do inversor.

# TODO
[] Testar como receber várias mensagens de uma vez 
[] Reimplementar pthreads e fazer a comunicação entre elas com funções próprias de real-time
[] Lidar com a situação de escrever na interface na mesma hora de ler (mutexes, semáforos)
[] Interface para enviar velocidade, comandos e ler velocidade
