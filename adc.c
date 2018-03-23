
#include <avr/io.h> //TODO: is this needed?



// Diese Beispiel zeigt die Anwendung des ADC eines ATmega169
// unter Verwendung der internen Referenzspannung von nominell 1,1V.
// Zur Anpassung an andere AVR und/oder andere Referenzspannungen
// siehe Erläuterungen in diesem Tutorial und im Datenblatt

/* ADC initialisieren */
void ADC_Init(void)
{
  // die Versorgungsspannung AVcc als Referenz wählen:
  //ADMUX = (1<<REFS0);
  // oder interne Referenzspannung als Referenz für den ADC wählen:
  ADMUX = (1<<REFS1) | (1<<REFS0);

  // Bit ADFR ("free running") in ADCSRA steht beim Einschalten
  // schon auf 0, also single conversion
  ADCSRA = (1<<ADPS1) | (1<<ADPS0);     // Frequenzvorteiler
  ADCSRA |= (1<<ADEN);                  // ADC aktivieren

  /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
     also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */

  ADCSRA |= (1<<ADSC);                  // eine ADC-Wandlung
  while (ADCSRA & (1<<ADSC) ) {         // auf Abschluss der Konvertierung warten
  }
  /* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
     Wandlung nicht übernommen. */
  (void) ADCW;
}

/* ADC Einzelmessung */
uint16_t ADC_Read()
{
  // Kanal waehlen, ohne andere Bits zu beeinflußen
  ADMUX = ADMUX & ~(0x1F);
  ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
  while (ADCSRA & (1<<ADSC) ) {   // auf Abschluss der Konvertierung warten
  }
  return ADCW;                    // ADC auslesen und zurückgeben
}
