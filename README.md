# superloop

rascunho pra comunicação can, rs485 e protocolos de segurança para controle de velocidade do inversor.

## TODO

- [ ] Testar como receber várias mensagens de uma vez 
- [ ] Reimplementar pthreads e fazer a comunicação entre elas com funções próprias de real-time
- [ ] Lidar com a situação de escrever na interface na mesma hora de ler (mutexes, semáforos)
- [ ] Interface para enviar velocidade, comandos e ler velocidade
- [ ] Ao invés de mandar a voltagem pra ui como float, melhor mandar como inteiro e colocar um ponto de mentira no ui?
- [ ] No WSL a interface fica resposiva e com relativa folga na latência, só que seja na RPI ou OPI a interface fica menos responsiva, só que com menor latência 🤔

## Dúvidas

- [ ] Se o BMS já mostra corrente e tensão, é preciso coletar esses dados do inversor também?