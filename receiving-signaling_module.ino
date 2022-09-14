// Program obsługujący moduł odbiorczo-sygnalizujący.
// Autor: Przemysław Kusa

// Zdefiniowanie wyprowadzeń, do których podłączone są podzespoły układu
#define Green 7
#define button1 9     
#define button2 5     
#define button3 10    
#define button4 11    
#define button5 6     
#define button6 12    

// Dołączenie bibliotek używanych w programie
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>
#include <DS1302.h>

// Utworzenie obiektów klas
DS1302 rtc(2,3,4);  // Ustawienie wyprowadzeń, do których podłączony został zegar czasu rzeczywistego 2 - RST, 3 - DAT, 4 -  CLK                                                        
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Ustawienie adresu układu na 0x27

Bounce button = Bounce();
Bounce buttonM = Bounce();
Bounce buttonH = Bounce();
Bounce buttonST = Bounce();

// Inicjalizacja zmiennych używanych w programie
int button_state = 0;
int button_state2 = 0;
int button_state3 = 0;
int button_state4 = 0;
int button_state5 = 0;
int state = 0;
int counter = 0;
int period = 1000;
unsigned long time_now = 0;
int h = 0, m = 0, s = 0, d = 1, mo = 1, y = 2000;
int counterh = 0, counterm = 0, counters = 0, counterD = 0, counterMO = 0, counterY = 0;
int stateST = 0;

void setup() {
// Inicjalizacja wyprowadzeń oraz trybu pracy przycisków i diody
  pinMode(Green,OUTPUT);
  digitalWrite(Green, LOW);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);

  button.attach( button3, INPUT );
  buttonM.attach( button4, INPUT );
  buttonH.attach( button5, INPUT );
  buttonST.attach( button6, INPUT );
  
// Ustawienie interwału(w milisekundach) sprawdzania, czy występują drgania styków
  button.interval(20); 
  buttonM.interval(20); 
  buttonH.interval(20); 
  buttonST.interval(20); 
  
   Serial.begin(9600);  // Inicjacja połączenia Bluetooth (9600 baudów)

// Ustawienie liczby kolumn i rzędów wyświetlacza LCD oraz ustawienie kursora i wiadomości startowej
  lcd.begin(16, 2);
  lcd.setCursor(5, 0);
  lcd.print("System");
  lcd.setCursor(2, 1);
  lcd.print("Powiadomien");
  delay(4000);              // Opóźnienie 4000ms
  lcd.clear();              // Wyczyszczenie ekranu

// Ustawianie zegara czasu rzeczywistego w tryb pracy oraz wyłączanie ochrony przed zapisem
  rtc.halt(false);
  rtc.writeProtect(false); 
  rtc.setDOW(WEDNESDAY);     // Ustawianie dnia tygodnia
  rtc.setTime(0, 0, 0);      // Ustawienia godziny
  rtc.setDate(1, 1, 2000);   // Ustawianie daty
}

void loop() {
// Odczyt z wyprowadzeń, do których podłączone są przyciski
  button_state = digitalRead(button1);
  button_state2 = digitalRead(button2);
  
// Wywołanie metody aktualizującej stan przycisków
  button.update();
  buttonM.update();
  buttonH.update();
  buttonST.update();

    clockanddate();   // Wywołanie funkcji clockanddate
 
 if(button_state == HIGH) // Jeżeli przycisk 1 zostanie wciśnięty, wywołana zostanie funkcja reset
  {
    reset();
  }

 if(button_state2 == HIGH) // Jeżeli przycisk 2 zostanie wciśnięty, wywołana zostanie funkcja displaymsg
 {
   displaymsg();
 }
//--------------------------------------------------------------
if(buttonST.fell()) // Jeżeli przycisk zostanie naciśnięty(zostanie wykryta zmiana stanu wysokiego na niski), 
{
  stateST++;        // Inkrementuj zmienną stateST
  if(stateST > 1)   // Jeżeli wartość jest większa od 1, wtedy stateST = 0
  {
    stateST = 0;
  }
}

if(button.fell()) // Jeżeli przycisk zostanie naciśnięty(zostanie wykryta zmiana stanu wysokiego na niski), 
 {
  if(stateST == 0)  // Jeżeli stateST wynosi 0
  {
    counters++;     // Inkrementuj zmienną counters, a następnie przypisz jej wartość zmiennej s
    s = counters;
    if(s>=60)       // Jeżeli zmienna s jest większa od 60, wyzeruj zmienne s i counters
    {
      s = 0;
      counters = 0;
    }
    rtc.setTime(h,m,s); // Przypisz zmienną s(sekundy) do ustawienia czasu
   }
   else if(stateST == 1)  // Jeżeli stateST wynosi 1
   {
    y += 1;               // Inkrementuj zmienną y o 1
    if(y>2030)            // Jeżeli zmienna jest większa od 2030
    {
      y = 2000;           // Ustaw wartość zmiennej y na 2000
    }
    rtc.setDate(d,mo,y);  // Przypisz zmienną y(rok) do ustawienia daty
   }
 }

if(buttonM.fell())  // Jeżeli przycisk zostanie naciśnięty(zostanie wykryta zmiana stanu wysokiego na niski), 
 {
  if(stateST == 0)  // Jeżeli stateST wynosi 0
   {
    counterm++;     // Inkrementuj zmienną counterm, a następnie przypisz jej wartość zmiennej m
    m = counterm;
    if(m>=60)       // Jeżeli zmienna m jest większa od 60, wyzeruj zmienne m i counterm
    {
      m = 0;
      counterm = 0;
    }
    rtc.setTime(h,m,s); // Przypisz zmienną m(minuty) do ustawienia czasu
   }
   else if(stateST == 1)  // Jeżeli stateST wynosi 1
   {
    mo += 1;              // Inkrementuj zmienną mo o 1
    if(mo>12)             // Jeżeli zmienna jest większa od 12
    { 
      mo = 1;             // Ustaw wartość zmiennej y na 1
    } 
    rtc.setDate(d,mo,y);  // Przypisz zmienną mo(miesiąc) do ustawienia daty
   }   
 }

if(buttonH.fell())    // Jeżeli przycisk zostanie naciśnięty(zostanie wykryta zmiana stanu wysokiego na niski), 
 {
  if(stateST == 0)    // Jeżeli stateST wynosi 0
   {
    counterh++;       // Inkrementuj zmienną counterh, a następnie przypisz jej wartość zmiennej h
    h = counterh;
    if(h>=24)         // Jeżeli zmienna h jest większa od 24, wyzeruj zmienne h i counterh
     {
      h = 0;
      counterh = 0;   
     }
    rtc.setTime(h,m,s); // Przypisz zmienną h(godziny) do ustawienia czasu
   }
   else if(stateST == 1)  // Jeżeli stateST wynosi 1
   {
    d += 1;               // Inkrementuj zmienną d o 1
    if(d>31)              // Jeżeli zmienna jest większa od 31
    {
      d = 1;              // Ustaw wartość zmiennej d na 1
    }
    rtc.setDate(d,mo,y);  // Przypisz zmienną d(dzień) do ustawienia daty
   }   
 }

// Sprawdzenie, czy można odczytać dane z portu szeregowego
if(Serial.available()>0)
  {
    state = Serial.read();    // Odczytaj dane z portu szeregowego i zapisz je do zmiennej state
    Serial.write(state);      // Wyślij zapisane dane w zmiennej state 
  } 
   
   if (state == '0')         // Jeżeli zmienna state jest równa 0
    {
      digitalWrite(Green, HIGH);      // Zaświeć diodę LED
      state = 0;                      // Ustaw state na 0
      message();                      // Wywołaj funkcję message
    }
  else if(state == '1')     // Jeżeli zmienna state jest równa 1
    {
      digitalWrite(Green, LOW);       // Zgaś diodę LED
      state = 0;                      // Ustaw state na 0
    }
}

void clockanddate()
{
  if(millis() >= time_now + period)   // Odmierzanie sekundy
  {
    time_now += period;
    
  lcd.setCursor(4, 0);              // Ustawianie kursora na ekranie LCD oraz wypisanie godziny
  lcd.print(rtc.getTimeStr()); 
  lcd.setCursor(3, 1);
  lcd.print(rtc.getDateStr());      // Ustawianie kursora na ekranie LCD oraz wypisanie daty
  }
}

void message()
{
   lcd.clear();                     // Wyczyszczenie zawartości wyświetlacza LCD
   counter = counter + 1;           // Zwiększenie wartości counter o 1

  lcd.setCursor(0, 0);              // Ustawianie kursora na ekranie LCD oraz wypisanie na nim "Nowy list"
  lcd.print("Nowy list");  
  lcd.setCursor(0, 1);              // Ustawianie kursora na ekranie LCD oraz wypisanie na nim aktualnej liczby wiadomości
  lcd.print((String)"Ilosc listow:" + counter);  

  delay(4000);                      // Opóźnienie 4000ms
  
  lcd.clear();                      // Wyczyszczenie zawartości wyświetlacza LCD
}

void reset()
{
   lcd.clear();                     // Wyczyszczenie zawartości wyświetlacza LCD
   lcd.setCursor(3, 0);             // Ustawianie kursora na ekranie LCD oraz wypisanie na nim wiadomości
   lcd.print("Wyzerowano");  
   lcd.setCursor(1, 1);
   lcd.print("liczbe listow!");  

   counter = 0;                     // Ustawienie zmiennej counter na 0
   
   delay(5000);                     // Opóźnienie 5000ms
   lcd.clear();                     // Wyczyszczenie zawartości wyświetlacza LCD
}

void displaymsg()
{
   lcd.clear();                     // Wyczyszczenie zawartości wyświetlacza LCD
   lcd.setCursor(0, 0);             // Ustawianie kursora na ekranie LCD oraz wypisanie na nim wiadomości o aktualnej liczbie wiadomości
   lcd.print("Liczba listow");  
   lcd.setCursor(0, 1);
   lcd.print((String)"wynosi: " + counter);  
   
   delay(5000);                     // Opóźnienie 5000ms
   lcd.clear();                     // Wyczyszczenie zawartości wyświetlacza LCD
}
