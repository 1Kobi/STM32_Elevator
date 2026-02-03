# STM32 F7 Elevator Control System 🛗

Sterownik windy oparty na mikrokontrolerze **STM32F767ZI** , wykorzystujący system czasu rzeczywistego **FreeRTOS** oraz cyfrowe przetwarzanie sygnałów (**CMSIS-DSP**). Projekt realizuje precyzyjne sterowanie pozycją kabiny z użyciem regulatora PID oraz filtracji FIR.

## 📸 Galeria

![obiekt](https://github.com/user-attachments/assets/e5005106-fcd1-4d4c-807f-223648b16f06)
*Rzeczywisty model windy sterowany przez STM32.*

<img width="1281" height="750" alt="aplikacja" src="https://github.com/user-attachments/assets/e9c0d6bc-80a1-45f4-beba-bf7c36f9d3bb" />
*Aplikacja PC wizualizująca pracę windy w czasie rzeczywistym.*

<img width="1350" height="533" alt="schemat" src="https://github.com/user-attachments/assets/aea0e5c6-a7fe-4454-9f3b-aef7a80764cf" />
*Schemat podłączenia na płytce stykowej*

## 🚀 Kluczowe Funkcjonalności
### 🧠 Firmware (STM32)
* **System Operacyjny:** Oparty na **FreeRTOS** z podziałem na zadania o różnych priorytetach (Sterowanie, Wyświetlacz, Telemetria).
* **Zaawansowane Sterowanie (PID):**
    * Własna implementacja regulatora PID z obsługą *Anti-Windup*.
    * Dwukanałowe wyjście PWM (oddzielne kanały dla ruchu W GÓRĘ i W DÓŁ).
    * Strefa nieczułości (Dead-zone) i kompensacja tarcia statycznego.
* **Cyfrowe Przetwarzanie Sygnału (DSP):**
    * Filtr **FIR (Finite Impulse Response)** 7. rzędu do wygładzania odczytów z czujnika odległości.
    * Współczynniki filtra zostały zaprojektowane i obliczone w środowisku **MATLAB**, a następnie zaimplementowane przy użyciu biblioteki **ARM CMSIS-DSP**.
* **Komunikacja i Bezpieczeństwo:**
    * Sterowanie i telemetria przez **UART (DMA)**.
    * Weryfikacja integralności danych za pomocą sumy kontrolnej **CRC32**.
    * Obsługa enkodera oraz czujnika ultradźwiękowego HC-SR04.

### 🖥️ Software (PC App)
* **Technologia:** C++ / Qt Framework (Qt SerialPort, Qt Charts).
* **Wizualizacja:** Wykresy w czasie rzeczywistym (Pozycja zadana vs aktualna, Wypełnienie PWM).
* **Sterowanie:** Możliwość zadawania wysokości docelowej z poziomu komputera.

---

## 📂 Opis Kluczowych Modułów (Firmware)

Kod sterownika został podzielony na niezależne moduły sprzętowe i logiczne:

* **`wlasny_PID.c`** – Autorska implementacja regulatora PID.
    * Obsługuje logikę **Split-Range**: ujemne wyjście steruje kanałem "Dół", dodatnie kanałem "Góra".
    * Zawiera mechanizm **Anti-Windup** (ograniczenie członu całkującego).
    * Implementuje "strefę nieczułości" (dead-zone) dla uchybu < 0.4 cm, aby wyeliminować drgania w stanie ustalonym.

* **`hcsr04.c`** – Obsługa czujnika odległości.
    * Wykorzystuje timer w trybie **Input Capture** do precyzyjnego pomiaru czasu trwania impulsu Echo.
    * Przelicza czas na odległość w centymetrach.
    * Zawiera zabezpieczenia przed błędnymi odczytami (np. wartości ujemne lub >400cm).

* **`filtr.c`** – Cyfrowe przetwarzanie sygnału.
    * Implementuje filtr **FIR (Finite Impulse Response)** 7. rzędu przy użyciu biblioteki `arm_math.h` (CMSIS-DSP).
    * Współczynniki filtra zostały wygenerowane w środowisku **MATLAB**, aby skutecznie tłumić szum pomiarowy czujnika ultradźwiękowego.

* **`crc.c`** – Weryfikacja danych.
    * Programowa implementacja algorytmu **CRC-32** (wielomian `0xEDB88320`).
    * Służy do weryfikacji poprawności komend przychodzących z PC (ignoruje ramki uszkodzone).

* **`encoder.c`** – Obsługa pozycjonowania.
    * Zlicza impulsy i przelicza je na pozycję absolutną windy.
    * Obsługuje programowe krańcówki (soft-limits) zapobiegające wyjechaniu poza zakres min/max.

---

## 🛠️ Architektura Sprzętowa

Projekt uruchomiony na płytce **NUCLEO-F767ZI**.

| Peryferium | Pin | Timer / Funkcja | Opis |
| :--- | :--- | :--- | :--- |
| **HC-SR04 Trig** | PA6 | TIM3 CH1 | Wyzwalanie pomiaru odległości |
| **HC-SR04 Echo** | PA0 | TIM2 CH1 | Pomiar czasu powrotu echa (Input Capture) |
| **Silnik GÓRA** | PF8 | TIM13 CH1 | Sygnał PWM dla ruchu w górę |
| **Silnik DÓŁ** | PF7 | TIM11 CH1 | Sygnał PWM dla ruchu w dół |
| **Enkoder** | PD12, PD13 | TIM4 CH1/CH2 | Sprzężenie zwrotne pozycji (symulowane/rzeczywiste) |
| **UART (PC)** | PD8, PD9 | USART3 | Komunikacja z komputerem (115200 baud) |
| **LCD I2C** | PB9, PB6 | I2C1 | Wyświetlacz 2x16 (Status, Pozycja) |

## 💻 Architektura Oprogramowania (FreeRTOS)

System składa się z trzech głównych wątków (Tasks) komunikujących się przez kolejki i semafory:

1.  **`ControlTask` (High Priority, 25ms)**
    * Główna pętla sterowania.
    * Odczytuje pozycję z enkodera i HC-SR04.
    * Filtruje sygnał odległości (FIR).
    * Oblicza wyjście regulatora PID i wysterowuje PWM.
    * Wysyła dane do kolejki telemetrii.
2.  **`DisplayTask` (Normal Priority, 300ms)**
    * Obsługuje wyświetlacz LCD po I2C.
    * Prezentuje zadaną i aktualną wysokość.
3.  **`UARTTask` (Low Priority)**
    * Odbiera dane z kolejki `elevatorQueueHandle`.
    * Wysyła sformatowane ramki danych do PC z użyciem **DMA**.

## 📡 Protokół Komunikacyjny

Aplikacja Qt i STM32 komunikują się z prędkością **115200 bps**.

1.  **PC -> STM32 (Komenda):** `H<Wysokość>:<CRC32>`
    * Narzuca nową pozycję zadaną. STM32 sprawdza CRC przed wykonaniem.
2.  **STM32 -> PC (Telemetria):** `$<Dist_Filt>,<Dist_Raw>,<Target>,<PWM_Up>,<PWM_Down>;`
    * Wysyłane cyklicznie co 125ms (co 5 obiegów pętli sterowania 25ms).

---

### Odbiór komend (PC -> STM32)
Format: `H<Wartość>:<CRC32>`
* Przykład: `H150:A1B2C3D4` (Ustaw wysokość na 150 cm).
* Ramka jest akceptowana tylko, jeśli obliczone CRC zgadza się z przesłanym.

### Telemetria (STM32 -> PC)
Format: `$<Dist_Filtered>,<Dist_Raw>,<Target>,<PWM_Up>,<PWM_Down>;`
* Przykład: `$120,121,150,800,0;`
