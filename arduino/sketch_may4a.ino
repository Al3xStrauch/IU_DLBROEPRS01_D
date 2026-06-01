// Input Variable für die Eingabe
String input = "";

void setup() 
{
  // Serielle Kommunikation mit 9600 Baud
  Serial.begin(9600);

}

void loop() 
{
  // Prüfen, ob Daten über die serielle Schnittstelle empfangen wurden
  if (Serial.available() > 0) 
  {
    // Eingabe bis "Enter" lesen
    input = Serial.readStringUntil('\n');

    // Leerzeichen am Anfang und Ende von Input entfernen
    input.trim();

    // Prüfen, ob die Eingabe leer ist
    if (input.length() == 0) 
    {
      Serial.println("Fehler: Ungueltige Eingabe");
      return;
    }

    // Variablen für die Berechnung festlegen
    // Defintion mit -1 weil dieser Index nicht möglich ist.
    int operatorIndex = -1;
    char operation = '\0';

    // Mathematische Operation wird gesucht mit for Schleife
    for (int i = 0; i < input.length(); i++) 
    {
      char currentChar = input.charAt(i);

      // Plus, Mal und Geteilt soll erkannt werden
      if (currentChar == '+' || currentChar == '*' || currentChar == '/') 
      {
        operatorIndex = i;
        operation = currentChar;
        break;
      }

      // Minus nur dann als Operator erkennen, wenn es nicht am Anfang steht. Damit vorangestellte negative Zahlen möglich sind. 
      if (currentChar == '-' && i > 0) 
      {
        operatorIndex = i;
        operation = currentChar;
        break;
      }
    }

    // Wenn kein Operator wie definiert gefunden wurde, ist die Eingabe ungültig
    if (operatorIndex == -1) 
    {
      Serial.println("Fehler: Kein Operator gefunden");
      return;
    }

    // Eingabe in einen linken und einen rechten Teil teilen
    String leftPart = input.substring(0, operatorIndex);
    String rightPart = input.substring(operatorIndex + 1);

    // Leerzeichen entfernen
    leftPart.trim();
    rightPart.trim();

    // Prüfen, ob beide Teile befüllt sind
    if (leftPart.length() == 0 || rightPart.length() == 0) 
    {
      Serial.println("Fehler: Zweite Variable fehlt");
      return;
    }

    // Text in Zahlen umwandeln
    double leftNumber = leftPart.toFloat();
    double rightNumber = rightPart.toFloat();

    // Variable für das Ergebnis
    double result = 0;

    // Berechnung abhängig vom gefundenen Operator durchführen
    switch (operation) 
    {
      case '+':
        result = leftNumber + rightNumber;
        break;

      case '-':
        result = leftNumber - rightNumber;
        break;

      case '*':
        result = leftNumber * rightNumber;
        break;

      case '/':
        // Ungültige Division durch 0 erkennen
        if (rightNumber == 0) 
        {
          Serial.println("Fehler: Division durch 0 nicht moeglich");
          return;
        }

        result = leftNumber / rightNumber;
        break;

      default:
        Serial.println("Fehler: Unbekannter Operator!");
        return;
    }

    // Ergebnis an den PC zurücksenden
    Serial.print("Ergebniss: ");
    // Rundung auf eine Nachkommastelle festgelegt
    Serial.println(result, 1); 
  }
}
