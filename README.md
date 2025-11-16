Este projeto propõe a construção de um sistema portátil de monitoramento de sinais vitais — frequência cardíaca (BPM) — utilizando o sensor de pulso, um microcontrolador Arduino com chip Wi-Fi, e um buzzer para alertas. 
A transmissão dos dados para monitoramento remoto é realizada via protocolo MQTT, utilizando sockets TCP para comunicação com o broker Mosquitto.
A condição é criar uma ferramenta acessível, pode ser muito útil para controle de rotina de exercícios, atividade físicas diárias ou mesmo para fins didáticos e a prevenção de problemas cardiovasculares.
A conexão da via cabo USB entre o micontrolador e o notebook transferirá o código em C++ do software para a execução do projeto.

Os componentes empregados na execução do projeto foram os seguintes.
Hardware
- Arduino ESP32
- Sensor de pulso (Pulse Sensor)
- Buzzer passivo BP18
- Cabo de rede (Ethernet)
- Fios jumper
- Protoboard
- Notebook

Software:
- Arduino IDE 2.3.6 — ambiente para editar, compilar e gravar o código no Arduino.
- Linguagem C/C++ — utilizada para desenvolvimento do firmware.
- Broker MQTT (Mosquitto) — servidor intermediário responsável por gerenciar a troca de mensagens entre dispositivos via protocolo MQTT.
Bibliotecas instaladas na Arduino IDE
- PubSubClient — cliente MQTT para publicar/assinar tópicos.
- PulseSensorPlayground — leitura e processamento do sensor de pulso.
- WiFi (incluído no core do ESP32) — gerenciamento da conexão Wi‑Fi.

O código consta no anexo. Deve inserir o Nome da rede Wifi, a senha e o IP da máquina que será executada.
Para obter o IP da máquina: abra o CMD, rode ipconfig e use o campo IPv4.

Funcionamento e acionamento dos materiais:

O protótipo desenvolvido integra sensores, atuadores e conectividade para realizar o monitoramento fisiológico remoto de forma eficiente. O processo tem início com a leitura dos batimentos cardíacos, captados pelo dedo do usuário. O sensor de pulso, baseado em tecnologia óptica amplificada, detecta as variações de fluxo sanguíneo e transmite os dados ao microcontrolador ESP32 por meio de um único pino de sinal conectado à entrada analógica D32.
A alimentação dos componentes é fornecida via conexão USB de 5 V, sendo regulada internamente pelo ESP32 para 3.3 V, valor compatível com o sensor de pulso. O buzzer passivo é acionado automaticamente quando os valores medidos ultrapassam os limites pré-definidos, funcionando como um alerta sonoro para situações que exigem atenção imediata — como episódios de taquicardia. Quando os batimentos estão dentro do intervalo saudável de 60 a 120 bpm, o som emitido é intermitente; fora desse intervalo, o buzzer emite um som contínuo.
O microcontrolador opera em modo station, conectando-se a uma rede Wi-Fi local. A transmissão dos dados para monitoramento remoto é realizada via protocolo MQTT, utilizando sockets TCP para comunicação com o broker Mosquitto. Isso permite que os dados sejam publicados em tempo real e acessados por sistemas externos, como dashboards ou aplicativos de saúde.
Por fim, o código responsável pelo funcionamento do sistema é desenvolvido em linguagem C++ e transferido para o ESP32 por meio de cabo USB conectado ao computador, utilizando a IDE Arduino como ambiente de programação. A conexão física via USB entre o microcontrolador e o computador também permite a depuração e o monitoramento em tempo real durante o desenvolvimento.
•	 A conexão da via cabo USB entre o micontrolador e notebook transferirá o código em C+ do software para a execução do projeto.

Fazer o download do Broker Mosquitto no site https://mosquitto.org/download/ para o sistema operacional da máquima e instalar.
Após instalar, realizar a alteração do arquivo mosquitto.conf e no firewall, conforme link do youtube https://www.youtube.com/watch?v=2l14dez0VjI&t=7s . Incluir o executável do mosquitto no path.
 
