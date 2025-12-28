/*
 input_controller.ino

 Arduino-based input controller for a Nintendo DS automation system.
 This sketch emulates button presses and communicates with a Python
 process over serial to coordinate real-time shiny detection.

 Demonstrated using a Pokémon Black 2 Cresselia encounter.
*/

// --- Pin Assignments ---
const int buttonAPin     = 13;
const int buttonBPin     = 12;
const int buttonStartPin = 2;
const int buttonSelectPin= 3;
const int buttonLBPin    = 5;
const int buttonRBPin    = 6;

// --- shiny flag --- 
bool shinyFound = false;

// --- Function Prototypes ---
void pressA(unsigned int duration);
void pressB(unsigned int duration);
void pressStart(unsigned int duration);
void pressSelect(unsigned int duration);
void pressLB(unsigned int duration);
void pressRB(unsigned int duration);
void pressReset(unsigned int duration = 400);
void performEncounterSequence();

// --- Setup ---
void setup() {
    Serial.begin(9600);
    delay(1000);

    Serial.println("[DEBUG] Initializing button pins...");
    pinMode(buttonAPin, OUTPUT);        digitalWrite(buttonAPin, HIGH);
    pinMode(buttonBPin, OUTPUT);        digitalWrite(buttonBPin, HIGH);
    pinMode(buttonStartPin, OUTPUT);    digitalWrite(buttonStartPin, HIGH);
    pinMode(buttonSelectPin, OUTPUT);   digitalWrite(buttonSelectPin, HIGH);
    pinMode(buttonLBPin, OUTPUT);       digitalWrite(buttonLBPin, HIGH);
    pinMode(buttonRBPin, OUTPUT);       digitalWrite(buttonRBPin, HIGH);

    Serial.println("[DEBUG] Pins initialized. Starting boot sequence...");

    // --- Startup Sequence ---
    delay(7000);
    pressA(200);
    Serial.println("[DEBUG] Passed main screen.");
    delay(2000);
    pressA(200);
    Serial.println("[DEBUG] Opened game.");
}

// --- Main Loop ---
void loop() {
    while (!shinyFound) {
        Serial.println("[DEBUG] Starting new encounter sequence...");
        performEncounterSequence();
        Serial.println("[DEBUG] Exited encounter sequence.");

        if (shinyFound) {
            Serial.println("[DEBUG] Shiny found! Halting loop.");
            while(true) { delay(1000); }
        } else {
            Serial.println("[DEBUG] Resetting game...");
            pressReset(800); // default 400ms soft reset
            delay(1000);
        }
    }

    // Halt program after shiny is found
    while (true);
}

void performEncounterSequence(){
    // Black 2 Title Sequence
    delay(6600); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Nintendo");
    delay(3000); pressA(200); Serial.println("[DEBUG] Pokémon Black2: dates screen");
    delay(2500); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Star");
    delay(1000); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Title Screen");
    delay(2000); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Roar");
    delay(3000); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Continue");
    delay(2200); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Launch C-Gear");

    // Encounter Cresselia
    delay(8000); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Pass Season");
    delay(1000); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Text box");
    delay(800); pressA(200); Serial.println("[DEBUG] Pokémon Black2: Encounter");

    delay(5000);

    // --- Tell Python to start shiny detection ---
    Serial.println("[DEBUG] Sending 'start' signal to Python...");
    Serial.println("start");
    delay(300);

    // Clear serial buffer before reading
    while (Serial.available()) Serial.read();

    unsigned long waitStart = millis();
    bool decisionReceived = false;
    Serial.println("[DEBUG] Waiting up to 15s for Python decision...");

    while (!decisionReceived && millis() - waitStart < 15000) {
        if (Serial.available()) {
            String msg = Serial.readStringUntil('\n');
            msg.trim();
            Serial.print("[DEBUG] Received message: "); Serial.println(msg);

            if (msg == "shiny") {
                shinyFound = true;
                Serial.println("[DEBUG] ✅ Shiny confirmed by Python!");
                decisionReceived = true;
            } else if (msg == "not_shiny") {
                Serial.println("[DEBUG] ❌ Not shiny, continuing hunt.");
                decisionReceived = true;
            }
        }
    }

    if (!decisionReceived) {
        Serial.println("[DEBUG] ⚠️ No response from Python within 15s.");
    }

    delay(1000);
}

// --- Button Press Functions ---
void pressA(unsigned int duration) {
    digitalWrite(buttonAPin, LOW); delay(duration); digitalWrite(buttonAPin, HIGH);
    // Serial.println("[DEBUG] Pressing A");
}

void pressB(unsigned int duration) {
    digitalWrite(buttonBPin, LOW); delay(duration); digitalWrite(buttonBPin, HIGH);
    // Serial.println("[DEBUG] Pressing B");
}

void pressStart(unsigned int duration) {
    digitalWrite(buttonStartPin, LOW); delay(duration); digitalWrite(buttonStartPin, HIGH);
    // Serial.println("[DEBUG] Pressing START");
}

void pressSelect(unsigned int duration) {
    digitalWrite(buttonSelectPin, LOW); delay(duration); digitalWrite(buttonSelectPin, HIGH);
    // Serial.println("[DEBUG] Pressing SELECT");
}

void pressLB(unsigned int duration) {
    digitalWrite(buttonLBPin, LOW); delay(duration); digitalWrite(buttonLBPin, HIGH);
    // Serial.println("[DEBUG] Pressing L");
}

void pressRB(unsigned int duration) {
    digitalWrite(buttonRBPin, LOW); delay(duration); digitalWrite(buttonRBPin, HIGH);
    // Serial.println("[DEBUG] Pressing R");
}

// --- Soft Reset ---
void pressReset(unsigned int duration) { // default 400ms press handled via prototype
    Serial.println("[DEBUG] Performing soft reset (L+R+START+SELECT)...");
    digitalWrite(buttonLBPin, LOW);
    digitalWrite(buttonRBPin, LOW);
    digitalWrite(buttonStartPin, LOW);
    digitalWrite(buttonSelectPin, LOW);

    delay(duration);  // 300-500ms is ideal for DS Lite

    digitalWrite(buttonLBPin, HIGH);
    digitalWrite(buttonRBPin, HIGH);
    digitalWrite(buttonStartPin, HIGH);
    digitalWrite(buttonSelectPin, HIGH);

    // Serial.println("[DEBUG] Reset complete.");
}
