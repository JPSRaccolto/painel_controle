![image](https://github.com/user-attachments/assets/f2a5c9b8-6208-4723-8f46-1d74be421827)

# 👥 Projeto: Sistema Contador de Pessoas com Raspberry Pi Pico

## 📑 Sumário
- [🎯 Objetivos](#-objetivos)
- [📋 Descrição do Projeto](#-descrição-do-projeto)
- [⚙️ Funcionalidades Implementadas](#️-funcionalidades-implementadas)
- [🛠️ Requisitos do Projeto](#️-requisitos-do-projeto)
- [📂 Estrutura do Código](#-estrutura-do-código)
- [🖥️ Como Compilar](#️-como-compilar)
- [🤝 Contribuições](#-contribuições)
- [📽️ Demonstração em Vídeo](#️-demonstração-em-vídeo)
- [💡 Considerações Finais](#-considerações-finais)

## 🎯 Objetivos
- Desenvolver um sistema contador de pessoas utilizando o Raspberry Pi Pico
- Implementar controle de entrada e saída com capacidade máxima
- Apresentar dados em tempo real através de display OLED
- Criar um sistema de alerta visual e sonoro para controle de ocupação

## 📋 Descrição do Projeto
Este projeto utiliza o Raspberry Pi Pico com os seguintes componentes:

- Display OLED SSD1306 via I2C (GPIO 14 e 15)
- LEDs RGB (GPIO 11, 12, 13)
- Botões de entrada, saída e reset (GPIO 5, 6, 22)
- Buzzer duplo (GPIO 10 e 21)
- Sistema de multitarefa utilizando FreeRTOS

## ⚙️ Funcionalidades Implementadas
1. **Controle de Acesso:**
- Contagem de entrada de pessoas através do botão A (GPIO 5)
- Contagem de saída de pessoas através do botão B (GPIO 6)
- Reset administrativo através do botão SELECT (GPIO 22)
- Capacidade máxima configurada para 8 pessoas


2. **Sistema de Alertas Visuais:**
- LED Azul para ambiente vazio (0 pessoas)
- LED Verde para capacidade normal (1-6 pessoas)
- LED Amarelo para quase lotado (7 pessoas)
- LED Vermelho para capacidade máxima (8 pessoas)


3. **Display OLED:**
- Exibição em tempo real do número de pessoas no ambiente
- Mensagens de feedback para entrada, saída e reset
- Tela de aguardo com atualização automática
- Indicação de limites mínimos e máximos atingidos


4. **Sistema de Alertas Sonoros:**
- Alerta sonoro quando capacidade máxima é atingida
- Sinal sonoro duplo para confirmação de reset
- Bloqueio de entrada quando limite é alcançado


5. **Controle Multitarefa:**
- Implementação de sistema multitarefa utilizando FreeRTOS
- Tarefas independentes para entrada, saída, display e reset
- Sincronização através de semáforos e mutex

## 🛠️ Requisitos do Projeto
- **FreeRTOS:** Gerenciamento de tarefas concorrentes
- **Comunicação I2C:** Integração com display OLED SSD1306
- **Manipulação de PWM:** Controle do buzzer duplo
- **Interrupções GPIO:** Detecção de eventos dos botões
- **Semáforos:** Sincronização entre tarefas
- **Mutex:** Proteção de recursos compartilhados (display)
- **Gerenciamento de tarefas:** Criação e escalonamento de 4 tarefas independentes


## 📂 Estrutura do Repositório
```
├── Painel_de_controle.c # Código principal do projeto
└── FreeRTOSConfig       # Configura as tasks
└── ssd1306.c            # Configuração do controle do display
└── ssd1306.h            # Configura a .c como biblioteca
└── font.h               # Configura as letras e números para o dispaly ssd1306
└── ...                  # Demais arquivos necessários
```

## 🖥️ Como Compilar
1. Clone o repositório:
   ```bash
   https://github.com/JPSRaccolto/painel_controle.git
   ```
2. Navegue até o diretório do projeto:
   ```bash
   cd painel_controle
   ```
3. Compile o projeto com seu ambiente de desenvolvimento configurado para o RP2040.
4. Carregue o código na placa BitDogLab.

## 🖥️ Metodo alternativo:
1. Baixe o repositório com arquivo zip.
2. Extraia para uma pasta de fácil acesso
3. Utilize a extensão raspberry pi pico dentro do VS Code para importar o projeto.
4. Aguarde ate criar o arquivo build
5. Utilize o ícone "_compile_" para compilar.
6. Utilize o "_RUN_" com a BitDogLab em modo boot seel para enviar o programa para a sua RP2040.
7. Agora, interaja com os botões e o teclado para mergulhar nas funcionalidades do projeto.

## 🧑‍💻 Autor
**João Pedro Soares Raccolto**

## 📝 Descrição
Sistema contador de pessoas utilizando Raspberry Pi Pico que monitora entrada e saída de pessoas em um ambiente com capacidade controlada. O sistema inclui exibição de dados em um display OLED, alertas visuais através de LEDs RGB com diferentes cores para cada estado de ocupação, e alertas sonoros via buzzer duplo quando condições críticas são detectadas. A arquitetura multitarefa com FreeRTOS permite o controle simultâneo de todas as funcionalidades com sincronização adequada entre tarefas.

## 🤝 Contribuições
Este projeto foi desenvolvido por **João Pedro Soares Raccolto**.
Contribuições são bem-vindas! Siga os passos abaixo para contribuir:

1. Fork este repositório.
2. Crie uma nova branch:
   ```bash
   git checkout -b minha-feature
   ```
3. Faça suas modificações e commit:
   ```bash
   git commit -m 'Minha nova feature'
   ```
4. Envie suas alterações:
   ```bash
   git push origin minha-feature
   ```
5. Abra um Pull Request.

## 📽️ Demonstração em Vídeo
- O vídeo de demonstração do projeto pode ser visualizado aqui: [Drive](https://drive.google.com/file/d/1LcwTsg2AsiAJPlvQJ6R4kndHqS9_Jhja/view?usp=sharing)

## 💡 Considerações Finais
Este projeto demonstra a implementação de um sistema de controle de ocupação utilizando o Raspberry Pi Pico com FreeRTOS. O sistema é capaz de monitorar entrada e saída de pessoas, exibindo essas informações em um display OLED e acionando alertas visuais e sonoros quando limites são atingidos.
A arquitetura baseada em tarefas do FreeRTOS permite que o sistema execute múltiplas funções simultaneamente, garantindo que o controle de acesso, exibição e alertas funcionem de maneira independente e eficiente. O projeto pode ser expandido para incluir sensores automáticos de presença, conectividade wireless para monitoramento remoto e integração com sistemas de segurança mais complexos.
