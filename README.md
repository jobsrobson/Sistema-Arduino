# Sistema de Monitoramento de Aquaterrário usando Arduino
Este repositório contém o código e a documentação para um sistema de monitoramento de aquaterrário utilizando Arduino, bem como análise dos dados coletados.

### Descrição
O sistema de monitoramento de aquaterrário foi desenvolvido para monitorar e controlar as condições ambientais dentro de um aquaterrário, como temperatura e umidade. O projeto é baseado em um microcontrolador Arduino, sensores específicos e módulos de comunicação.

### Funcionalidades
- Leitura e monitoramento contínuo de sensores de temperatura da água, temperatura e umidade do ar ambiente;
- Armazenamento local dos dados coletados;
- Análise e visualização dos dados coletados durante o período entre 22 de outubro e 16 de novembro de 2023.

### Componentes
- 1x Microcontrolador Arduino Uno
- 1x Shield Data Logger com RTC (Real-Time Clock)
- 1x Cartão de memória SD de até 4GB
- 1x Sensor DS18B20 do tipo Sonda (água)
- 1x Sensor DS18B20 do tipo TO92 (ar)
- 1x Sensor DHT11 (umidade)
- 3x Resistores Cerâmicos de 4,7 kΩ
- 1x Display LCD 4x20 com módulo I²C
- 1x Potenciômetro Analógico de 10kΩ
- 1x Protoboard
- 1x Fonte de Alimentação DC de 9V
- Vários jumpers macho-macho e macho-fêmea

### Estrutura do Repositório
- /Arduino_Code: Contém o código fonte para o microcontrolador Arduino.
- /Data_Analysis: Inclui notebook Python da análise dos dados coletados.
- /Documentation: Documentação adicional, esquemas de conexão e informações sobre os componentes.
- /Images: Imagens utilizadas na documentação.

### Instruções de Uso
- Montagem do circuito conforme esquemas fornecidos na documentação.
- Carregar o código Arduino para o microcontrolador usando Arduino IDE.
- Iniciar a coleta de dados e monitoramento.
