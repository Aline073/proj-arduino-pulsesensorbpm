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

Funcionamento e acionamento dos materiais:

O protótipo desenvolvido integra sensores, atuadores e conectividade para realizar o monitoramento fisiológico remoto de forma eficiente. O processo tem início com a leitura dos batimentos cardíacos, captados pelo dedo do usuário. O sensor de pulso, baseado em tecnologia óptica amplificada, detecta as variações de fluxo sanguíneo e transmite os dados ao microcontrolador ESP32 por meio de um único pino de sinal conectado à entrada analógica D32.
A alimentação dos componentes é fornecida via conexão USB de 5 V, sendo regulada internamente pelo ESP32 para 3.3 V, valor compatível com o sensor de pulso. O buzzer passivo é acionado automaticamente quando os valores medidos ultrapassam os limites pré-definidos, funcionando como um alerta sonoro para situações que exigem atenção imediata — como episódios de taquicardia. Quando os batimentos estão dentro do intervalo saudável de 60 a 120 bpm, o som emitido é intermitente; fora desse intervalo, o buzzer emite um som contínuo.
O microcontrolador opera em modo station, conectando-se a uma rede Wi-Fi local. A transmissão dos dados para monitoramento remoto é realizada via protocolo MQTT, utilizando sockets TCP para comunicação com o broker Mosquitto. Isso permite que os dados sejam publicados em tempo real e acessados por sistemas externos, como dashboards ou aplicativos de saúde.
Por fim, o código responsável pelo funcionamento do sistema é desenvolvido em linguagem C++ e transferido para o ESP32 por meio de cabo USB conectado ao computador, utilizando a IDE Arduino como ambiente de programação. A conexão física via USB entre o microcontrolador e o computador também permite a depuração e o monitoramento em tempo real durante o desenvolvimento.
•	 A conexão da via cabo USB entre o micontrolador e notebook transferirá o código em C+ do software para a execução do projeto.

O código está anexado, antes de carregar, edite as variáveis para inserir o nome da rede Wi‑Fi (SSID), a senha e o endereço IP do computador onde o broker será executado. Para obter o IP da máquina no Windows, abra o Prompt de Comando, execute ipconfig e utilize o valor informado em IPv4.

Faça o download do broker Mosquitto em https://mosquitto.org/download/ compatível com o sistema operacional da máquina e instale-o. Após a instalação, edite o arquivo de configuração mosquitto.conf conforme necessário e ajuste o firewall para permitir a porta MQTT (padrão 1883); se precisar de um passo a passo, consulte o tutorial indicado no link https://www.youtube.com/watch?v=2l14dez0VjI&t=7s. Adicione o executável do Mosquitto ao PATH do sistema para permitir sua execução a partir do prompt de comando.

Em seguida, instale o MQTT Explorer (https://mqtt-explorer.com/) e crie uma nova conexão definindo Name conforme preferir, Host = test.mosquitto.org e Port = 1883; conecte para verificar o funcionamento do cliente.

Com todos os programas instalados, carregue e execute o código no Arduino IDE. Verifique no Monitor Serial do Arduino a conexão Wi‑Fi e as mensagens de status, e confirme no MQTT Explorer que as mensagens publicadas pelo ESP32 estão sendo recebidas corretamente.

