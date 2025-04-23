// Test code for using Grove Sound Sensor with Arduino NANO 33 BLE SENSE LITE
// Modified from Seeed Studio original test code

// Control whether to use real Edge Impulse model or alternative mockup
#define USE_EDGE_IMPULSE_MOCKUP 1

#if USE_EDGE_IMPULSE_MOCKUP
  // Using a mockup version of Edge Impulse
  // Manually define buffer size, adjust based on actual model
  #define BUFFER_SIZE 1000
  
  // Manually define other necessary constants and types
  #define EI_CLASSIFIER_LABEL_COUNT 2
  #define EI_IMPULSE_OK 0
  
  // Define simple structures to replace Edge Impulse types
  typedef int EI_IMPULSE_ERROR;
  
  struct ei_impulse_result_classification {
    const char* label;
    float value;
  };
  
  struct ei_impulse_result_t {
    ei_impulse_result_classification classification[2]; // Assume 2 categories
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
    // Otherwise consider it neutral
    if (avg > 0.3) { // Threshold can be adjusted
      result->classification[0].label = "positive";
      result->classification[0].value = 0.7;
      result->classification[1].label = "neutral";
      result->classification[1].value = 0.3;
    } else {
      result->classification[0].label = "positive";
      result->classification[0].value = 0.3;
      result->classification[1].label = "neutral";
      result->classification[1].value = 0.7;
    }
    
    return EI_IMPULSE_OK;
  }
#else
  // Use the real Edge Impulse library
  #include "casa0018_inferencing.h"
  #define BUFFER_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE
#endif

// Declare analog input pin, using A0 on Arduino NANO 33 BLE SENSE LITE
const int pinAdc = A0;

// Define LED pins for visualizing sound intensity, using onboard LEDs of NANO 33 BLE SENSE LITE
// Note that RGB LEDs on NANO 33 BLE SENSE LITE are active LOW
const int ledPin = LED_BUILTIN;  // Use built-in LED
const int ledR = LEDR;  // Built-in red LED
const int ledG = LEDG;  // Built-in green LED
const int ledB = LEDB;  // Built-in blue LED

// Fortune telling response database - categorized as positive and neutral
const char* positive_responses[] = {
    "The road ahead is bright, forge ahead!",
    "Your efforts will eventually be rewarded, keep going!",
    "The universe is arranging wonderful things for you, wait patiently!",
    "Good luck is coming, please be prepared!",
    "This is a turning point, seize the opportunity!"
};

const char* neutral_responses[] = {
    "Things are developing, maintain observation and patience",
    "The answer is in your heart, listen carefully to your inner voice",
    "Sometimes letting go of attachment can lead to new possibilities",
    "The wheels of fate are turning, maintain a calm mindset",
    "Everything is possible, depending on how you view it"
};

// Define the number of fortune responses
const int NUM_POSITIVE_RESPONSES = sizeof(positive_responses) / sizeof(positive_responses[0]);
const int NUM_NEUTRAL_RESPONSES = sizeof(neutral_responses) / sizeof(neutral_responses[0]);

// Define status variables
bool is_inference_running = false;
unsigned long last_inference_time = 0;
unsigned long data_collection_start_time = 0;
unsigned long inference_interval = 5000; // Run inference every 5 seconds

// Define inference result variable
int current_mood = 0; // 0=neutral, 1=positive

// Create data buffer
float buffer[BUFFER_SIZE] = {0};
int buffer_index = 0;

void setup()
{
    // Initialize serial communication at 115200 baud rate
    Serial.begin(115200);
    Serial.println("AI Fortune Teller - Fortune telling system based on voice emotion");
    
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
    Serial.println("Device ready! Please speak your fortune telling question into the microphone...");
    Serial.print("Buffer size: ");
    Serial.println(BUFFER_SIZE);
    
    #if USE_EDGE_IMPULSE_MOCKUP
    Serial.println("Note: Using mockup version of emotion recognition instead of actual Edge Impulse model");
    #else
    Serial.println("Note: Using real Edge Impulse model for emotion recognition");
    #endif
    
    last_inference_time = millis();
}

// Callback function to get signal data - required by Edge Impulse
int get_signal_data(size_t offset, float *out_ptr, size_t length) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = buffer[offset + i];
    }
    return 0;
}

void loop()
{
    unsigned long current_time = millis();
    
    // Collect sound data for inference
    if (!is_inference_running) {
        // Collect sound data into buffer
        if (buffer_index < BUFFER_SIZE) {
            // Declare accumulator variable
            long sum = 0;
            
            // Collect multiple samples and average for more stable readings
            for (int i = 0; i < 32; i++) {
                sum += analogRead(pinAdc);
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
                
                // Determine mood state based on inference results
                // Assume the first category is "positive", the second is "neutral"
                if (result.classification[0].value > result.classification[1].value) {
                    current_mood = 1; // Positive
                    digitalWrite(ledG, LOW); // Green light indicates positive
                } else {
                    current_mood = 0; // Neutral
                    digitalWrite(ledB, LOW); // Blue light indicates neutral
                }
                
                // Select a random fortune response based on mood
                if (current_mood == 1) {
                    // Positive emotion -> return positive response
                    int response_index = random(NUM_POSITIVE_RESPONSES);
                    Serial.println("\nFortune result:");
                    Serial.println(positive_responses[response_index]);
                } else {
                    // Neutral emotion -> return neutral response
                    int response_index = random(NUM_NEUTRAL_RESPONSES);
                    Serial.println("\nFortune result:");
                    Serial.println(neutral_responses[response_index]);
                }
            }
            
            // Reset buffer index
            buffer_index = 0;
            is_inference_running = false;
            last_inference_time = current_time;
        }
    }
    
    // Display sound level (helps with debugging)
    if (current_time % 1000 == 0) {
        long sum = 0;
        for (int i = 0; i < 32; i++) {
            sum += analogRead(pinAdc);
        }
        sum >>= 5;
        
        Serial.print("Current sound level: ");
        Serial.println(sum);
    }
    
    // Short delay
    delay(10);
}
