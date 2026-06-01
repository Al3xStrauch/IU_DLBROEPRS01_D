#include <iostream>   // Für Ein und Ausgabe in der Konsole
#include <fstream>    // Für die Log-Datei
#include <string>     // Für Textvariablen
#include <fcntl.h>    // Für open()
#include <unistd.h>   // Für read(), write(), close(), sleep()
#include <termios.h>  // Für die serielle Schnittstelle des Macbooks

int main()
{
    //Grundeinstellungen 


    std::string portName = "/dev/cu.usbmodem101";  // Port festlegung Macbook
    std::string input;                             // Eingabe
    std::string answer;                            // Antwort zum Microcontroller

    std::cout << "Taschenrechner Elegoo" << std::endl;
    std::cout << "Port: " << portName << std::endl;

  //Seriellen Port öffnen


    int serialPort = open(portName.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    if (serialPort < 0)
    {
        std::cout << "Fehler: Serieller Port konnte nicht geoeffnet werden." << std::endl;
        std::cout << "Pruefen: Arduino angeschlossen? Serial Monitor geschlossen?" << std::endl;
        return 1;
    }

//Serielle Schnittstelle einstellen


    termios serialSettings{};

    if (tcgetattr(serialPort, &serialSettings) != 0)
    {
        std::cout << "Fehler: Port Einstellungen konnten nicht geladen werden." << std::endl;
        close(serialPort);
        return 1;
    }

    // Baudrate festlegen (senden und empfangen)
    cfsetospeed(&serialSettings, B9600);
    cfsetispeed(&serialSettings, B9600);

    // Einstellungen: 8 Datenbits, keine Paritaet, 1 Stopbit
    serialSettings.c_cflag = (serialSettings.c_cflag & ~CSIZE) | CS8;
    serialSettings.c_cflag |= (CLOCAL | CREAD);
    serialSettings.c_cflag &= ~PARENB;
    serialSettings.c_cflag &= ~CSTOPB;
    serialSettings.c_cflag &= ~CRTSCTS;

    // Flusskontrolle und Sonderverarbeitung deaktivieren
    serialSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    serialSettings.c_lflag = 0;
    serialSettings.c_oflag = 0;

    // Leseverhalten einstellen
    serialSettings.c_cc[VMIN] = 0;
    serialSettings.c_cc[VTIME] = 20;

    if (tcsetattr(serialPort, TCSANOW, &serialSettings) != 0)
    {
        std::cout << "Fehler: Port Einstellungen konnten nicht gesetzt werden." << std::endl;
        close(serialPort);
        return 1;
    }

//Log-Datei öffnen

    std::ofstream logFile("communication_log.txt", std::ios::app);

    if (!logFile.is_open())
    {
        std::cout << "Fehler: Log-Datei konnte nicht geoeffnet werden." << std::endl;
        close(serialPort);
        return 1;
    }

//Programminformationen ausgeben

    std::cout << "Verbindung hergestellt." << std::endl;
    std::cout << "Beispiel: 34 * 72" << std::endl;
    std::cout << "Beenden mit: exit" << std::endl;

    // Pause um Bugs zu vermeiden
    sleep(2);

//Hauptschleife

    while (true)
    {
        std::cout << "\nEingabe: ";
        std::getline(std::cin, input);

        // Programm beenden
        if (input == "exit")
        {
            break;
        }

        // Leere Eingaben ignorieren
        if (input.empty())
        {
            std::cout << "Leere Eingabe ignoriert." << std::endl;
            continue;
        }

        // Eingabe an Arduino senden
        std::string message = input + "\n";
        write(serialPort, message.c_str(), message.length());

        // Antwort vom Arduino lesen
        answer = "";
        char currentChar;

        while (read(serialPort, &currentChar, 1) > 0)
        {
            if (currentChar == '\n')
            {
                break;
            }

            if (currentChar != '\r')
            {
                answer += currentChar;
            }
        }

        // Kommunikation anzeigen
        std::cout << "PC -> Arduino: " << input << std::endl;
        std::cout << "Arduino -> PC: " << answer << std::endl;

        // Kommunikation speichern
        logFile << "PC -> Arduino: " << input << std::endl;
        logFile << "Arduino -> PC: " << answer << std::endl;
        logFile << "-----------------------------" << std::endl;
    }

//Programm beenden

    logFile.close();
    close(serialPort);

    std::cout << "Programm beendet." << std::endl;

    return 0;
}
