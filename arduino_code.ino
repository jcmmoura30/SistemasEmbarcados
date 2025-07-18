const int ledPins[4] = {7, 8, 9, 10};  // LEDs para indicar tentativas erradas
const int buzzerPin = 11;              // Buzzer
const int senhaCorreta[] = {1, 2, 3, 4}; // Senha correta
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

  // Verifica tamanho da senha
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
    Serial1.write('U');  // Envia sinal para PIC
    apagaTudo();
  } else {
    Serial.println("Senha incorreta.");
    registraTentativaErrada();
  }
}

void registraTentativaErrada() {
  tentativasErradas++;
  atualizaIndicadores();

  Serial1.write('E'); // E = tentativa errada
  Serial1.write(tentativasErradas);

  if (tentativasErradas >= 4) {
    Serial.println("MÁXIMO DE TENTATIVAS ERRADAS! Buzzer acionado.");
    digitalWrite(buzzerPin, HIGH);
    piscaTodosLeds(10, 300);  // Pisca todos os LEDs 10 vezes

    // Após piscadas, manter todos LEDs acesos
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
  }
}

void atualizaIndicadores() {
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

void piscaTodosLeds(int vezes, int intervalo) {
  for (int i = 0; i < vezes; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], HIGH);
    }
    delay(intervalo);
    for (int j = 0; j < 4; j++) {
      digitalWrite(ledPins[j], LOW);
    }
    delay(intervalo);
  }
}

void apagaTudo() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  digitalWrite(buzzerPin, LOW);
}

void resetTentativas() {
  tentativasErradas = 0;
  cofreDesbloqueado = false;
  apagaTudo();
  Serial.println("Sistema resetado. Digite a senha:");
}
