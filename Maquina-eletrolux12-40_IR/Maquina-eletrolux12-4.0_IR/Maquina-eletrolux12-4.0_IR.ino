#include <IRremote.h>

int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);  // (baud rate)
  irrecv.enableIRIn(); // Inicia o receptor IR

  pinMode(2, INPUT);  // Pressostato
  pinMode(3, OUTPUT); // Motor D
  pinMode(4, OUTPUT); // Motor E
  pinMode(5, OUTPUT); // Bomba Drenagem
  pinMode(6, OUTPUT); // Solenoide Sabao
  pinMode(7, OUTPUT); // Solenoide Amaciante
  pinMode(8, INPUT);  // Botão Turbo
  //pinMode(9);
  //pinMode(10);
  //pinMode(11);
  //pinMode(12);
  //pinMode(13);     
  pinMode(A0, INPUT_PULLUP); // Receptor IR

  //zera todas saidas
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  
  delay(7000); //aguarda 3 segundos para inicio do programa, evitando repiques da chave seletora
}

// Encher a Maquina
// Pin2 - Pressostato
// pin6 - Solenoide Sabao
void encherSabao() {

  //mantem ativo ate o pressostato gritar
  while (digitalRead(2)) {
    digitalWrite(6, LOW); // Liga Solenoide Sabão
    delay(1000);
  }

  digitalWrite(6, HIGH); // Desliga Solenoide Sabão
}

// Encher a Maquina
// Pin7 - Solenoide Amaciante
void encherAmaciante() {
  for (int i = 0; i < 30; i++) {
    digitalWrite(7, HIGH); // Liga Solenoide Amaciante
    delay(1000);
  }
  digitalWrite(7, LOW); // desliga valvula
}

//Modo Lavagem Normal
void lavagem(int batidas) {

  for (int i = 0; i < batidas; i++) {
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
    delay(400);
    
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    delay(150); //repouso motor
    
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
    delay(300);
    
    digitalWrite(4, HIGH);
    digitalWrite(3, HIGH);
    delay(200);
  } //1550 ms ou 1,55 segundos

  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
}

//Modo Lavagem Turbo
void lavagemTurbo(int batidas) {

  for (int i = 0; i < batidas; i++) {
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    delay(500);
    
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    delay(100); //repouso motor
    
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    delay(400);
    
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    delay(180);
  }  //1400 ms ou 1,4 segundos
}

// Centrifugacao Intermediaria
// Pin3 - Motor Direita
// Pin5 - Bomba de Drenagem
// Pin6 - Solenoide Sabão
void centrifugacaoInter() {
  digitalWrite(3, LOW); //liga motor
  digitalWrite(5, LOW); //liga bomba

  for (int i = 0; i < 120; i++) { // Centerifuga por 2 min
    delay(1000);
  }

  digitalWrite(3, HIGH);  // Desliga Motor D
  digitalWrite(6, LOW);   // Liga Solenoide Sabão

  for (int i = 0; i < 30  ; i++) { // Delay de 30 segundos, Repouso do Motor
    delay(1000);
  }
  
  digitalWrite(3, LOW); // Liga Motor D

  for (int i = 0; i < 120; i++) {  // Centerifuga por mais 2 min
    delay(1000);
  }

  digitalWrite(3, HIGH);   // Desliga Motor
  digitalWrite(6, HIGH);   // Desliga Solenoide Sabão
  for (int i = 0; i < 30  ; i++) { // Repouso de 30s, para evitar tranco na frenagem do Motor
    delay(1000);
  }
  digitalWrite(7, HIGH);   // Desliga Bomba de Drenagem
}

// Centrifugação Final
// Pin3 - Motor Direita
// Pin5 - Bomba de Drenagem
void centrifugacao() {
  digitalWrite(3, LOW); //liga motor
  digitalWrite(5, LOW); //liga bomba

  for (int i = 0; i < 200; i++) {
    delay(1000);
  }

  //desliga 30s
  digitalWrite(3, HIGH);

  for (int i = 0; i < 30; i++) {
    delay(1000);
  }

  digitalWrite(3, LOW); // Liga Motor
  for (int i = 0; i < 280; i++) { // Centrifuga por mais 4,6 min
    delay(1000);

    digitalWrite(3, HIGH);  // Desliga Motor
    for (int i = 0; i < 30; i++) {
      delay(1000);
    }
    digitalWrite(5, HIGH);   // Desliga Bomba de Drenagem
  }
}

// Esvaziar a Maquina
// Pin2 - Pressostato
// Pin5 - Bomba de Drenagem
void esvaziar() {
  digitalWrite(5, LOW); // Liga Bomba de Drenagem até pressostato parar de Gritar
  while (!digitalRead(2)) { 
    delay(1000);
  }
  for (int i = 0; i < 120; i++) {
    delay(1000);
  }
  digitalWrite(5, HIGH); // Desliga Bomba de Drenagem
}
void loop() {
  

  if (irrecv.decode(&results)) {
      Serial.println(results.value, HEX);
      irrecv.resume(); // Lê o Codigo IR do CONTROLE
      delay(100);
      
      //MODO 1 - Lavagem por 3h
      if (results.value == 0xF720DF) {
  
        Serial.println("MODO 1 :: SUPER MOLHO-1");
        Serial.println("### INICIANDO LAVAGEM ###");
        Serial.println(">> Inundando o tanque com agua ate o nivel selecionado...");
        encherSabao(); //inundando o tanque
  
        //bate a roupa durante 5 minutos, deixa de molho 5 minutos, e repete ate completar 3hr
        Serial.println(">> Batendo a roupa por 3hr com repouso de 5 min...");
  
        if (digitalRead(8)) { // Botão Turbo
          // Se precionado Entra lavagem Turbo
          for (int i = 0; i < 18; i++) {
  
            lavagemTurbo(214); //bate a roupa 214x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        } else {

          // Se Não Precionado Entra na Lavagem Normal
          for (int i = 0; i < 18; i++) {
            lavagem(194); //bate a roupa 194x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        }
  
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando durante 4 min...");
        centrifugacaoInter(); //centrifuga a parada
  
        Serial.println(">> Inundando o tanque...");
        encherSabao(); //inundando o tanque
        encherAmaciante(); //inundando o tanque com amaciante
        
        Serial.println(">> Batendo a roupa por mais 5min...");
        lavagem(194); //bate a roupa 194x
        
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por mais 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }

      //MODO 2 - lavagem por 2h
      else if (results.value == 0xF7A05F) {
        Serial.println("MODO 2 :: SUPER MOLHO-2");
  
        Serial.println("### INICIANDO LAVAGEM ###");
        Serial.println(">> Inundando o tanque com agua ate o nivel selecionado...");
        encherSabao(); //inundando o tanque
  
        //bate a roupa durante 5 minutos, deixa de molho 5 minutos, e repete ate completar 2hr
        Serial.println(">> Batendo a roupa por 2hr com repouso de 5 min...");
  
        if (digitalRead(8)) { 
           // Se precionado Entra lavagem Turbo
          for (int i = 0; i < 12; i++) {
  
            lavagemTurbo(214); //bate a roupa 214x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        } else {
          // Se Não Precionado Entra na Lavagem Normal
          for (int i = 0; i < 12; i++) {
            lavagem(194); //bate a roupa 194x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        }
  
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando durante 4 min...");
        centrifugacaoInter(); //centrifuga a parada
  
        Serial.println(">> Inundando o tanque...");
        encherSabao(); //inundando o tanque
        encherAmaciante(); //inundando o tanque com amaciante
  
        Serial.println(">> Batendo a roupa por mais 5min...");
        lavagem(194); //bate a roupa 194x
        
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por mais 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }
        //MODO 3 - lavagem por 1h
      else if (results.value == 0xF7609F) {
  
        Serial.println("MODO 3 :: Rapido");
  
        Serial.println("### INICIANDO LAVAGEM ###");
        Serial.println(">> Inundando o tanque com agua ate o nivel selecionado...");
        encherSabao(); //inundando o tanque
  
        //bate a roupa durante 5 minutos, deixa de molho 5 minutos, e repete ate completar 1hr
        Serial.println(">> Batendo a roupa por 1hr com repouso de 5 min...");
  
        if (digitalRead(8)) {
           // Se precionado Entra lavagem Turbo
          for (int i = 0; i < 6; i++) {
  
            lavagemTurbo(214); //bate a roupa 214x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        } else {
          // Se Não Precionado Entra na Lavagem Normal
          for (int i = 0; i < 6; i++) {
            lavagem(194); //bate a roupa 194x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        }
  
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando durante 4 min...");
        centrifugacaoInter(); //centrifuga a parada
  
        Serial.println(">> Inundando o tanque...");
        encherSabao(); //inundando o tanque
        encherAmaciante(); //inundando o tanque com amaciante
  
        Serial.println(">> Batendo a roupa por mais 5min...");
        lavagem(194); //bate a roupa 194x
     
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por mais 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }
  
      //MODO 4 - lavagem por 30 min
      else if (results.value == 0xF710EF) {
  
        Serial.println("MODO 4 :: Super Rapido");
  
        Serial.println("### INICIANDO LAVAGEM ###");
        Serial.println(">> Inundando o tanque com agua ate o nivel selecionado...");
        encherSabao(); //inundando o tanque
  
        //bate a roupa durante 5 minutos, deixa de molho 5 minutos, e repete ate completar 30min
        Serial.println(">> Batendo a roupa por 30min com repouso de 5 min...");
  
        if (digitalRead(8)) {
           // Se precionado Entra lavagem Turbo
          for (int i = 0; i < 3; i++) {
  
            lavagemTurbo(214); //bate a roupa 214x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        } else {
          // Se Não Precionado Entra na Lavagem Normal
          for (int i = 0; i < 3; i++) {
            lavagem(194); //bate a roupa 194x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
          }
        }
  
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando durante 4 min...");
        centrifugacaoInter(); //centrifuga a parada
  
        Serial.println(">> Inundando o tanque...");
        encherSabao(); //inundando o tanque
        encherAmaciante(); //inundando o tanque com amaciante
  
        Serial.println(">> Batendo a roupa por mais 5min...");
        lavagem(194); //bate a roupa 194x
      
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por mais 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }
  
      //MODO 5 - ENXAGUE + CENTRIFUGAÇÃO INTERMEDIARIA
      else if (results.value == 0xF7906F) {
        Serial.println("MODO 5 :: ENXAGUE + CENTRI.INTER");
  
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando durante 4 min...");
        centrifugacaoInter(); //centrifuga a parada
        
        Serial.println(">> Inundando o tanque...");
        encherSabao(); //inundando o tanque
        encherAmaciante(); //inundando o tanque com amaciante
  
        Serial.println(">> Batendo a roupa por mais 5min...");
        if (digitalRead(8)) {
           // Se precionado Entra lavagem Turbo
          for (int i = 0; i < 3; i++) {
            lavagemTurbo(214); //bate a roupa 214x
          }
        } else {
          // Se Não Precionado Entra na Lavagem Normal
          for (int i = 0; i < 3; i++) {
            lavagem(194); //bate a roupa 194x
          }
        }
         //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por mais 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }
  
      //MODO 5.1 - ENXAGUE 
      else if (results.value == 0xF730CF) {
        Serial.println("MODO 5.1 :: ENXAGUE");
        
        Serial.println(">> Inundando o tanque...");
        encherSabao();     //inundando o tanque
        encherAmaciante(); //inundando o tanque com amaciante
  
        Serial.println(">> Batendo a roupa por mais 5min...");
        if (digitalRead(8)) {
           // Se precionado Entra lavagem Turbo
          for (int i = 0; i < 3; i++) {
  
            lavagemTurbo(214); //bate a roupa 214x
            }
            
        } else {
          // Se Não Precionado Entra na Lavagem Normal
          for (int i = 0; i < 3; i++) {
            lavagem(194); //bate a roupa 194x
          }
        }
        //deixa a roupa de molho por 5 min
            for (int x = 0; x < 300; x++) {
              delay(1000);
            }
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por mais 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }
  
      //MODO 6 - CENTRIFUGACAO
      else if (results.value == 0xF750AF) {
        Serial.println(">> Esvaziando o tanque ate o fim...");
        esvaziar(); //esvazia o tanque
  
        Serial.println(">> Centrifugando por 8 minutos...");
        centrifugacao(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }
  
      //MODO 8 - Teste
      else if (results.value == 0xF7E817) {
        Serial.println("MODO 8 :: Teste");
        Serial.println(">> Inundando o tanque com agua");
        digitalWrite(6, LOW); //inundando o tanque
        delay(2000);
        digitalWrite(6, HIGH);
        digitalWrite(7, HIGH);//inundando o tanque com amaciante
        delay(2000);
        digitalWrite(7, LOW);
  
        //bate a roupa
        Serial.println(">> Batendo a roupa");
  
        if (digitalRead(8)) {
          for (int i = 0; i < 1; i++) {
  
            lavagemTurbo(20); //bate a roupa 214x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 2; x++) {
              delay(1000);
            }
          }
        } else {
          for (int i = 0; i < 1; i++) {
            lavagem(20); //bate a roupa 194x
  
            //deixa a roupa de molho por 5 min
            for (int x = 0; x < 2; x++) {
              delay(1000);
            }
          }
        }
        delay(500);
  
        Serial.println(">> Esvaziando o tanque ate o fim...");
        digitalWrite(5, LOW); //esvazia o tanque
        delay(100);
        digitalWrite(5, HIGH);
  
        Serial.println(">> Centrifugando durante 1 min...");
        centrifugacaoInter(); //centrifuga a parada
  
        Serial.println(">> LAVAGEM COMPLETA! =)");
      }

      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);

    }

}
