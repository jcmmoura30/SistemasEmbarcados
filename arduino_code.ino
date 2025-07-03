const int ledPins[4] = {7, 8, 9, 10};  // LEDs para indicar tentativas erradas
const int buzzerPin = 11;               // Buzzer
const int senhaCorreta[] = {1, 2, 3, 4}; // Exemplo de senha
const int tamanhoSenha = 4;

int tentativasErradas = 0;
bool cofreDesbloqueado = false;

String entradaSerial = "";

void setup() {
  Serial.begin(9600);    // Comunicação com monitor serial (entrada senha)
  Serial1.begin(9600);   // Comunicação UART com PIC

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  Serial.println("Sistema Cofre Inicializado.");
  Serial.println("Digite a senha:");
}

void loop() {
  // Verifica se recebeu comando do PIC para resetar tentativas
  if (Serial1.available()) {
    char comando = Serial1.read();
    if (comando == 'R') {  // R = Reset enviado pelo PIC
      resetTentativas();
      Serial.println("Tentativas resetadas via PIC.");
    }
  }

  // Lê dados do monitor serial para receber a senha
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (entradaSerial.length() > 0) {
        verificaSenha(entradaSerial);
        entradaSerial = "";
      }
    } else {
      entradaSerial += c;
    }
  }
}

void verificaSenha(String senhaInput) {
  if (cofreDesbloqueado) {
    Serial.println("Cofre já desbloqueado.");
    return;
  }

  // Converte entrada para array int
  if (senhaInput.length() != tamanhoSenha) {
    Serial.println("Senha incorreta (tamanho diferente).");
    registraTentativaErrada();
    return;
  }

  int senhaInputInt[tamanhoSenha];
  for (int i = 0; i < tamanhoSenha; i++) {
    if (!isDigit(senhaInput[i])) {
      Serial.println("Senha deve conter apenas dígitos.");
      registraTentativaErrada();
      return;
    }
    senhaInputInt[i] = senhaInput[i] - '0';
  }

  // Compara com senha correta
  bool correta = true;
  for (int i = 0; i < tamanhoSenha; i++) {
    if (senhaInputInt[i] != senhaCorreta[i]) {
      correta = false;
      break;
    }
  }

  if (correta) {
    cofreDesbloqueado = true;
    Serial.println("Senha correta! Cofre desbloqueado.");
    // Envia sinal para PIC mostrar mensagem
    Serial1.write('U');  // U = Unlock
    apagaTudo();
  } else {
    Serial.println("Senha incorreta.");
    registraTentativaErrada();
  }
}

void registraTentativaErrada() {
  tentativasErradas++;
  atualizaIndicadores();

  // Envia para PIC o número de tentativas
  Serial1.write('E'); // E = Error / tentativa errada
  Serial1.write(tentativasErradas);

  if (tentativasErradas >= 4) {
    Serial.println("MÁXIMO DE TENTATIVAS ERRADAS! Buzzer acionado.");
    digitalWrite(buzzerPin, HIGH);
    piscaLed(tentativasErradas - 1);
  }
}

void atualizaIndicadores() {
  // Liga LEDs progressivamente conforme tentativas erradas
  for (int i = 0; i < 4; i++) {
    if (tentativasErradas > i) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
  if (tentativasErradas < 4) {
    digitalWrite(buzzerPin, LOW);
  }
}

void piscaLed(int index) {
  // Pisca LED específico (exemplo: quarto LED)
  if (index < 0 || index >= 4) return;
  digitalWrite(ledPins[index], !digitalRead(ledPins[index]));
}

void apagaTudo() {
  // Apaga todos LEDs e buzzer
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], LOW);
  digitalWrite(buzzerPin, LOW);
}

void resetTentativas() {
  tentativasErradas = 0;
  cofreDesbloqueado = false;
  apagaTudo();
  Serial.println("Sistema resetado. Digite a senha:");
}
