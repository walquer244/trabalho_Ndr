#include <LiquidCrystal.h>

// Configuração do LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int buzzerPin = 8;

const int numCandidatos = 4;

// Vetor de votos
int votos[numCandidatos] = {0, 0, 0, 0};

// Dados dos candidatos
String candidatos[numCandidatos] = {"Savio", "Bolsonaro", "Raphinha", "Nulo"};
int codigos[numCandidatos] = {1, 2, 7, 0}; // 0 é o código para Nulo

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(buzzerPin, OUTPUT);
  
  // Tela de boas-vindas
  lcd.print("  Urna Eletronica");
  lcd.setCursor(0, 1);
  lcd.print("  Bem-vindo!");
  tone(buzzerPin, 2000, 300); // Som de inicialização
  delay(2000);
  
  iniciarVotacao();
}

void loop() {
  if (Serial.available() > 0) {
    String entrada = Serial.readStringUntil('\n');
    entrada.trim();

    // Comando especial para apurar
    if (entrada.equalsIgnoreCase("resultado")) {
      apurar();
      delay(3000); // Tempo para ler o resultado
      iniciarVotacao();
      return;
    }

    // Tenta converter para número
    int codigo = entrada.toInt();
    bool votoValido = false;
    int indiceEncontrado = -1;

    // Verifica se o código digitado existe na lista
    for (int i = 0; i < numCandidatos; i++) {
      if (codigo == codigos[i]) {
        votos[i]++;
        votoValido = true;
        indiceEncontrado = i;
        break;
      }
    }

    lcd.clear();

    if (votoValido) {
      // Lógica específica para voto Nulo vs Candidato
      if (codigo == 0) {
        lcd.print("Voto Nulo!");
        Serial.println("> Voto registrado como NULO.");
      } else {
        lcd.print("Voto Confirmado!");
        lcd.setCursor(0, 1);
        lcd.print("Para: " + candidatos[indiceEncontrado]);
        Serial.println("> Voto registrado para: " + candidatos[indiceEncontrado]);
      }
      
      // Bip de confirmação curto e agradável
      tone(buzzerPin, 1500, 150); 
      delay(2000); // Tempo para usuário ler
      mostrarCandidatos();
      
    } else {
      // Código não existe (ex: digitou 9)
      lcd.print("Codigo Invalido!");
      lcd.setCursor(0, 1);
      lcd.print("Tente novamente.");
      Serial.println("> Erro: Código '" + entrada + "' não reconhecido.");
      
      // Bip de erro (som mais grave)
      tone(buzzerPin, 400, 300);
      delay(2000);
      mostrarCandidatos();
    }
  }
}

void iniciarVotacao() {
  Serial.println("\n--- INICIANDO VOTAÇÃO ---");
  Serial.println("Digite o código do candidato:");
  for (int i = 0; i < numCandidatos; i++) {
    Serial.print(codigos[i]);
    Serial.print(" -> ");
    Serial.println(candidatos[i]);
  }
  Serial.println("Ou digite 'resultado' para encerrar.");
  mostrarCandidatos();
}

void mostrarCandidatos() {
  lcd.clear();
  // Linha 1: Candidatos 1 e 2
  lcd.print(String(codigos[0]) + ":" + candidatos[0]);
  lcd.setCursor(8, 0); // Ajuste para caber melhor
  lcd.print(String(codigos[1]) + ":" + candidatos[1]);
  
  // Linha 2: Candidatos 3 e 4
  lcd.setCursor(0, 1);
  lcd.print(String(codigos[2]) + ":" + candidatos[2]);
  lcd.setCursor(8, 1);
  lcd.print(String(codigos[3]) + ":" + candidatos[3]);
  
  delay(4000); // Tempo para ler a lista
  
  lcd.clear();
  lcd.print("Digite o codigo");
  lcd.setCursor(0, 1);
  lcd.print("no Monitor Serial");
}

void apurar() {
  lcd.clear();
  lcd.print("Apurando votos...");
  tone(buzzerPin, 2500, 500); // Som de processamento
  delay(1000);
  
  Serial.println("\n=== BOLETIM DE URNA ===");
  
  int maiorVotos = -1;
  int vencedorIndex = -1;
  bool houveEmpate = false;

  // Exibe contagem individual
  for (int i = 0; i < numCandidatos; i++) {
    Serial.print(candidatos[i]);
    Serial.print(": ");
    Serial.print(votos[i]);
    Serial.println(" votos");

    if (votos[i] > maiorVotos) {
      maiorVotos = votos[i];
      vencedorIndex = i;
      houveEmpate = false;
    } else if (votos[i] == maiorVotos && i != vencedorIndex) {
      houveEmpate = true;
    }
  }

  // Exibe resultado no LCD
  lcd.clear();
  if (maiorVotos == 0) {
    lcd.print("Nenhum voto!");
    Serial.println("Resultado: Nenhuma votação registrada.");
  } else if (houveEmpate) {
    lcd.print("Houve Empate!");
    lcd.setCursor(0, 1);
    lcd.print("Verifique o Serial");
    Serial.println("Resultado: EMPATE entre os líderes.");
  } else {
    lcd.print("Vencedor:");
    lcd.setCursor(0, 1);
    lcd.print(candidatos[vencedorIndex]);
    Serial.print("Resultado Final: ");
    Serial.print(candidatos[vencedorIndex]);
    Serial.println(" foi eleito!");
  }
}   

