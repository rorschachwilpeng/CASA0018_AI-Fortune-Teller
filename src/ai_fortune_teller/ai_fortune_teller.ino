// AI Fortune Teller - Fortune telling system based on voice emotion
// Using Arduino NANO 33 BLE SENSE LITE with a Grove Sound Sensor and Touch Sensor

// Control whether to use real Edge Impulse model or alternative mockup
#define USE_EDGE_IMPULSE_MOCKUP 0

#if USE_EDGE_IMPULSE_MOCKUP
  // Using a mockup version of Edge Impulse
  // Manually define buffer size, adjust based on actual model
  #define BUFFER_SIZE 1000
  
  // Manually define other necessary constants and types
  #define EI_CLASSIFIER_LABEL_COUNT 3
  #define EI_IMPULSE_OK 0
  
  // Define simple structures to replace Edge Impulse types
  typedef int EI_IMPULSE_ERROR;
  
  struct ei_impulse_result_classification {
    const char* label;
    float value;
  };
  
  struct ei_impulse_result_t {
    ei_impulse_result_classification classification[3]; // Assume 3 categories: positive, neutral, negative
  };
  
  struct signal_t {
    size_t total_length;
    int (*get_data)(size_t, float*, size_t);
  };
  
  // Define a simple mockup emotion analysis function
  EI_IMPULSE_ERROR run_classifier(signal_t* signal, ei_impulse_result_t* result, bool debug) {
    // This is a mockup implementation that recognizes emotions based on sound intensity
    // Calculate average amplitude
    float sum = 0;
    float *buffer = (float*)malloc(signal->total_length * sizeof(float));
    if (!buffer) return -1; // Memory allocation failed
    
    signal->get_data(0, buffer, signal->total_length);
    
    // Calculate average sound intensity
    for (size_t i = 0; i < signal->total_length; i++) {
      sum += buffer[i];
    }
    float avg = sum / signal->total_length;
    
    // Free memory
    free(buffer);
    
    // Set results
    // If sound amplitude is high, consider it a positive emotion
    // If sound is moderate, consider it neutral
    // If sound is low but detectable, consider it negative
    if (avg > 0.4) { 
      result->classification[0].label = "positive";
      result->classification[0].value = 0.7;
      result->classification[1].label = "neutral";
      result->classification[1].value = 0.2;
      result->classification[2].label = "negative";
      result->classification[2].value = 0.1;
    } else if (avg > 0.2) {
      result->classification[0].label = "positive";
      result->classification[0].value = 0.3;
      result->classification[1].label = "neutral";
      result->classification[1].value = 0.6;
      result->classification[2].label = "negative";
      result->classification[2].value = 0.1;
    } else {
      result->classification[0].label = "positive";
      result->classification[0].value = 0.3;
      result->classification[1].label = "neutral";
      result->classification[1].value = 0.1;
      result->classification[2].label = "negative";
      result->classification[2].value = 0.6;
    }
    
    return EI_IMPULSE_OK;
  }
#else
  // Use the real Edge Impulse library
  #include "slimrorschach-project-1_inferencing.h"
  #define BUFFER_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE
#endif

// Define sensor pins
const int SOUND_PIN = A0;  // Sound sensor connected to A0
const int TOUCH_PIN = 2;   // Touch sensor connected to D2

// Define LED pins for visualizing sound intensity, using onboard LEDs of NANO 33 BLE SENSE LITE
// Note that RGB LEDs on NANO 33 BLE SENSE LITE are active LOW
const int ledPin = LED_BUILTIN;  // Use built-in LED
const int ledR = LEDR;  // Built-in red LED
const int ledG = LEDG;  // Built-in green LED
const int ledB = LEDB;  // Built-in blue LED

// Minimum duration in milliseconds for a valid question (5 seconds)
const unsigned long MIN_QUESTION_DURATION = 3000;

// =================== INTERNAL ANSWER DATABASE ===================
// Replacing external SD card-based answers with built-in arrays

// Positive emotion answers
const char* positive_answers[] = {
    "The stars align in your favor. A path of abundance and joy awaits.",
    "Your spirit radiates with potential. Great fortune approaches on silent wings.",
    "The universe conspires to fulfill your deepest wishes. Remain open to unexpected gifts.",
    "A door you thought closed will soon reveal a golden opportunity.",
    "The cosmic energies surrounding you promise success in your endeavors.",
    "Ancient wisdom whispers that your current path leads to unexpected treasures.",
    "The celestial signs reveal imminent positive changes in your journey.",
    "Fortune smiles upon you. Your patience will soon be rewarded handsomely.",
    "The ethereal forces suggest a season of prosperity is dawning in your life.",
    "Your aura shines with promise. Embrace the good that comes seeking you.",
    "The mystical cards reveal a future bright with possibilities and achievements.",
    "A guardian spirit walks beside you, guiding you toward prosperity.",
    "The cosmic tide turns in your favor, bringing gifts from unexpected shores.",
    "Your inner light draws positive energies and fortunate circumstances.",
    "The ancient runes prophesy success and fulfillment in your immediate future.",
    "A blessing from distant ancestors illuminates your path forward.",
    "The threads of fate are weaving a tapestry of success around your endeavors.",
    "Celestial bodies align to shower you with unexpected blessings.",
    "The mystical veil parts to reveal abundant opportunities approaching.",
    "Your spiritual journey is about to be rewarded with material abundance.",
    "The oracle foresees a period of joy and achievement in your immediate future.",
    "Invisible hands guide you toward a destiny more magnificent than imagined.",
    "The cosmic scales tip in your favor, bringing balance and reward.",
    "Ancient spirits whisper of treasures awaiting your discovery.",
    "Your soul's vibration attracts positive outcomes and fortunate meetings.",
    "The sacred geometry of your life is aligning toward perfect harmony.",
    "Forgotten wisdom returns to guide you toward unexpected victory.",
    "The stars have written your name in their book of good fortune.",
    "Distant realms send energy to support your dreams and aspirations.",
    "The mystical phoenix rises in your chart, promising glorious transformation."
};

// Neutral emotion answers
const char* neutral_answers[] = {
    "The mists of time conceal both challenges and opportunities on your path.",
    "Balance is your key. Neither seek nor avoid what comes naturally to you.",
    "The pendulum swings between fortune and challenge. Your response determines the outcome.",
    "Ancient wisdom suggests watching and waiting before making your next move.",
    "The cosmic streams flow neither for nor against you. Navigate with awareness.",
    "Your journey stands at a crossroads. Patience will reveal the optimal direction.",
    "The oracle sees a path of neutrality - neither blessed nor cursed, but rich with potential.",
    "As the wheel turns, observe carefully before choosing your direction.",
    "The mystical signs suggest a period of preparation rather than action.",
    "Your destiny remains unwritten. The quill now rests in your hand.",
    "The cosmic mirror reflects both light and shadow. Acknowledge both to find your way.",
    "The spirits whisper of stillness. In quiet observation lies your answer.",
    "Neither fortune nor misfortune dominates your horizon. Your choices will tip the scales.",
    "The ancient scrolls speak of a time for gathering wisdom rather than seeking treasure.",
    "The veils between worlds thin, offering glimpses of many possible futures.",
    "Your spiritual journey requires balance between acceptance and ambition.",
    "The cosmic tapestry weaves neutrality into your immediate path. Discernment is required.",
    "The mystic waters run neither hot nor cold. Test before immersion.",
    "Your aura shows equilibrium. Maintain this balance as you proceed.",
    "The celestial bodies neither hinder nor hasten your progress. The pace is yours to set.",
    "Ancient runes reveal a time of evaluation rather than conclusion.",
    "The ethereal guides suggest reflection on both past and future before proceeding.",
    "Your current position offers a vantage point for observation. Use it wisely.",
    "The mystical pendulum swings in perfect balance. Your intention will direct its course.",
    "The cosmic library holds many possible chapters for your story. Choose with care.",
    "Spirit guardians stand neutral, watching how you navigate the upcoming passage.",
    "The sacred geometry of your situation forms a perfect square - stable but requiring energy to change.",
    "Ancestral whispers suggest a time of gathering strength rather than exerting it.",
    "The oracle sees clouds neither dark nor light on your horizon. Prepare for all weather.",
    "The cosmic tide rests at equilibrium. Your movement will create the waves."
};

// Negative emotion answers
const char* negative_answers[] = {
    "Shadows gather on your current path. Consider an alternate route to your destination.",
    "The cosmic winds blow against your current direction. Patience and recalibration are needed.",
    "Ancient warnings echo through time. Heed the subtle signs before proceeding further.",
    "The mystical gates appear closed before you. Seek another entrance to your desired realm.",
    "Celestial alignments suggest postponing major decisions until the skies clear.",
    "The oracle foresees obstacles hidden in the mist. Proceed with heightened awareness.",
    "Your spiritual guides counsel caution. Not all opportunities are as they appear.",
    "The ancient runes cast shadows of warning. Reconsider your current approach.",
    "The ethereal tides pull against your vessel. Consider anchoring until they shift.",
    "Distant storm clouds gather in your spiritual forecast. Prepare and protect what you value.",
    "The cosmic mirror reflects approaching challenges designed to strengthen your resolve.",
    "Ancestral whispers urge reconsideration of your current path and intentions.",
    "The mystic pendulum swings toward necessary caution and thoughtful retreat.",
    "Your aura reveals turbulence that requires attention before forward movement.",
    "The cosmic scales tip toward necessary challenge and growth through difficulty.",
    "The veiled future conceals tests of character and endurance on your immediate path.",
    "Spirit guardians raise cautionary hands before the bridge you approach.",
    "The sacred scrolls speak of necessary trials before deserved triumph.",
    "Cosmic energies suggest a period of challenge designed to refine your purpose.",
    "The ancient wisdom suggests that your desired treasure lies beyond unexpected trials.",
    "The mystical waters before you run deep and cold. Prepare before crossing.",
    "Your spiritual journey encounters necessary resistance to strengthen your resolve.",
    "The oracle sees gathering clouds that must be navigated with wisdom and patience.",
    "Forgotten warnings from past cycles emerge to guide your current decisions.",
    "The celestial bodies cast long shadows across your intended path. Proceed with care.",
    "Your current vibration attracts necessary lessons rather than desired outcomes.",
    "The cosmic doorway you approach requires greater preparation than anticipated.",
    "Ancient protectors block an unready passage. Growth is required before proceeding.",
    "The mystical signs point toward a period of challenge before eventual breakthrough.",
    "The ethereal guides suggest retreat and reconsideration of your current approach."
};

// The number of answers in each category
const int NUM_ANSWERS = 30;

// Define status variables
bool is_inference_running = false;
bool is_touch_active = false;
unsigned long touch_start_time = 0;
unsigned long last_inference_time = 0;
unsigned long data_collection_start_time = 0;
unsigned long inference_interval = 5000; // Run inference every 5 seconds

// Create data buffer
float buffer[BUFFER_SIZE] = {0};
int buffer_index = 0;

// Callback function to get signal data - required by Edge Impulse
int get_signal_data(size_t offset, float *out_ptr, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = buffer[offset + i];
    }
    return 0;
}

// Function to get a random answer from the internal arrays based on emotion category
String getRandomAnswer(int emotion_category) {
    // Select random index
    int index = random(NUM_ANSWERS);
    
    // Return answer based on emotion category
    // 0=neutral, 1=positive, 2=negative
    switch(emotion_category) {
        case 1: // Positive
            return positive_answers[index];
        case 2: // Negative
            return negative_answers[index];
        case 0: // Neutral
        default:
            return neutral_answers[index];
    }
}

// Function to run inference and process results
void runInference() {
    // All LEDs flash to indicate inference starting
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
    digitalWrite(ledB, LOW);
    delay(500);
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, HIGH);
    
    // Perform inference
    ei_impulse_result_t result;
    
    // Create signal
    signal_t signal;
    signal.total_length = BUFFER_SIZE;
    signal.get_data = &get_signal_data;
    
    // Run inference
    Serial.println("Running inference...");
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
    
    if (res != EI_IMPULSE_OK) {
        Serial.print("Inference error: ");
        Serial.println(res);
        digitalWrite(ledR, LOW); // Red light indicates error
    }
    else {
        // Print inference results
        Serial.println("Inference results:");
        for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
            Serial.print(result.classification[i].label);
            Serial.print(": ");
            Serial.println(result.classification[i].value);
        }
        
        // Determine emotion category based on inference results
        // 0=neutral, 1=positive, 2=negative
        int emotion_category = 0; // Default to neutral
        
        // Find the emotion with highest score
        if (EI_CLASSIFIER_LABEL_COUNT >= 3) {
            float positive_score = result.classification[0].value;
            float neutral_score = result.classification[1].value;
            float negative_score = result.classification[2].value;
            
            if (positive_score > neutral_score && positive_score > negative_score) {
                emotion_category = 1; // Positive
                digitalWrite(ledG, LOW); // Green light indicates positive
            } else if (negative_score > neutral_score && negative_score > positive_score) {
                emotion_category = 2; // Negative
                digitalWrite(ledR, LOW); // Red light indicates negative
            } else {
                emotion_category = 0; // Neutral
                digitalWrite(ledB, LOW); // Blue light indicates neutral
            }
        } else {
            // If model only outputs positive/neutral (2 categories)
            if (result.classification[0].value > result.classification[1].value) {
                emotion_category = 1; // Positive
                digitalWrite(ledG, LOW); // Green light indicates positive
            } else {
                emotion_category = 0; // Neutral
                digitalWrite(ledB, LOW); // Blue light indicates neutral
            }
        }
        
        // Get a random fortune answer based on emotion category
        String fortuneResponse = getRandomAnswer(emotion_category);
        
        Serial.println("\nFortune result:");
        Serial.println(fortuneResponse);
    }
    
    // Reset buffer index
    buffer_index = 0;
    is_inference_running = false;
    last_inference_time = millis();
}

void setup() {
    // Initialize serial communication at 115200 baud rate
    Serial.begin(115200);
    while (!Serial);  // Wait for serial connection (important for Nano 33 BLE)
    
    Serial.println("AI Fortune Teller - Fortune telling system based on voice emotion");
    
    // Initialize random number generator
    randomSeed(analogRead(A1)); // Use unconnected analog pin as random seed source
    
    // Set up pin modes
    pinMode(SOUND_PIN, INPUT);
    pinMode(TOUCH_PIN, INPUT);
    
    // Set LED pins as output mode
    pinMode(ledPin, OUTPUT);
    pinMode(ledR, OUTPUT);
    pinMode(ledG, OUTPUT);
    pinMode(ledB, OUTPUT);
    
    // Ensure LEDs are initially off
    // Note: RGB LEDs on NANO 33 BLE SENSE LITE are active LOW (LOW=on, HIGH=off)
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, HIGH);

    // Output device ready message
    Serial.println("Device ready! Press and hold the touch sensor for at least 3 seconds while speaking your question...");
    Serial.print("Buffer size: ");
    Serial.println(BUFFER_SIZE);
    
    #if USE_EDGE_IMPULSE_MOCKUP
    Serial.println("Note: Using mockup version of emotion recognition instead of actual Edge Impulse model");
    #else
    Serial.println("Note: Using real Edge Impulse model for emotion recognition");
    #endif
    
    last_inference_time = millis();
}

void loop() {
    unsigned long current_time = millis();
    
    // Read touch sensor state
    int touchState = digitalRead(TOUCH_PIN);
    
    // Check if touch sensor is pressed
    if (touchState == HIGH) {
        // Touch sensor is pressed
        if (!is_touch_active) {
            // Touch just started
            is_touch_active = true;
            touch_start_time = current_time;
            buffer_index = 0; // Reset buffer when touch starts
            Serial.println("Touch sensor activated. Please speak your question for at least 3 seconds...");
            
            // Flash green LED to indicate touch is detected
            digitalWrite(ledG, LOW);
            delay(100);
            digitalWrite(ledG, HIGH);
        }
        
        // Collect sound data for inference when touch is active
        if (!is_inference_running) {
            // Collect sound data into buffer
            if (buffer_index < BUFFER_SIZE) {
                // Declare accumulator variable
                long sum = 0;
                
                // Collect multiple samples and average for more stable readings
                for (int i = 0; i < 32; i++) {
                    sum += analogRead(SOUND_PIN);
                }
                
                // Calculate average
                sum >>= 5;
                
                // Normalize data (assuming ADC range is 0-1023)
                float normalized_value = (float)sum / 1023.0f;
                
                // Store in buffer
                buffer[buffer_index++] = normalized_value;
                
                // Display collection progress
                if (buffer_index % 100 == 0) {
                    Serial.print("Data collection: ");
                    Serial.print((buffer_index * 100) / BUFFER_SIZE);
                    Serial.println("%");
                    
                    // Blink blue LED to indicate data collection
                    digitalWrite(ledB, !digitalRead(ledB));
                }
            }
            else {
                // Buffer full, prepare for inference
                Serial.println("Data collection complete, analyzing your question...");
                is_inference_running = true;
                
                // Run the inference
                runInference();
            }
        }
    } else {
        // Touch sensor is not pressed
        if (is_touch_active) {
            // Touch just ended
            is_touch_active = false;
            unsigned long touch_duration = current_time - touch_start_time;
            
            Serial.print("Touch sensor released. Duration: ");
            Serial.print(touch_duration);
            Serial.println(" ms");
            
            // Check if touch duration meets minimum requirement (5 seconds)
            if (touch_duration >= MIN_QUESTION_DURATION) {
                // If we have collected some data and touch was long enough
                if (buffer_index > 0) {
                    Serial.println("Question duration sufficient, analyzing...");
                    is_inference_running = true;
                    
                    // If buffer isn't full yet but we have enough data, run inference
                    if (buffer_index < BUFFER_SIZE) {
                        Serial.println("Buffer not full but sufficient data collected.");
                    }
                    
                    // Run the inference
                    runInference();
                }
            } else {
                // Touch was too short
                Serial.println("Question too short (less than 3 seconds). Please try again with a longer question.");
                buffer_index = 0; // Reset buffer
                
                // Flash red LED to indicate question was too short
                for (int i = 0; i < 3; i++) {
                    digitalWrite(ledR, LOW);
                    delay(100);
                    digitalWrite(ledR, HIGH);
                    delay(100);
                }
            }
        }
    }
    
    // Display sound level (helps with debugging)
    if (current_time % 1000 == 0) {
        long sum = 0;
        for (int i = 0; i < 32; i++) {
            sum += analogRead(SOUND_PIN);
        }
        sum >>= 5;
        
        Serial.print("Current sound level: ");
        Serial.print(sum);
        Serial.print(" | Touch: ");
        Serial.println(digitalRead(TOUCH_PIN) == HIGH ? "PRESSED" : "Not pressed");
        
        // If touch is active, show how long it's been pressed
        if (is_touch_active) {
            unsigned long current_duration = current_time - touch_start_time;
            Serial.print("Current touch duration: ");
            Serial.print(current_duration);
            Serial.println(" ms");
            
            // Show remaining time until minimum duration
            if (current_duration < MIN_QUESTION_DURATION) {
                Serial.print("Keep holding for ");
                Serial.print((MIN_QUESTION_DURATION - current_duration) / 1000.0);
                Serial.println(" more seconds...");
            }
        }
    }
    
    // Short delay
    delay(10);
}