# 🔮 AI Fortune Teller
MSc Connected Environments | CASA0018 | University College London  
Author: Youtian Peng

<div style="text-align: center;">
  <img src="img/cover.png" alt="Flowchart" width="2000">
</div>

---

## ✨ Introduction
This project explores the development of a lightweight, real-time emotion polarity classifier deployable on ultra-low-power microcontrollers. By classifying user speech into Positive, Neutral, or Negative emotional categories, the system simulates a fortune-telling experience through an embedded device.  
It demonstrates how emotionally aware AI interaction can be achieved within tight memory, latency, and computational constraints.

---

## 🌟 Background
The growing demand for casual digital entertainment highlights a strong user preference for lightweight, emotionally engaging AI experiences.  
Research in affective computing emphasizes the importance of emotion-aware systems in creating richer human-device interactions.  
Furthermore, the principle of micro-interactions—brief, focused feedback loops—supports the development of compact, instant-response AI systems.

To address these emerging needs, this project focuses on enabling an AI-driven fortune-teller system that leverages real-time voice-based emotion recognition, all while operating fully offline on a resource-constrained device (Arduino Nano 33 BLE).

---

## 🎥 Video Presentation

[🔗 Click here to watch the project video](https://www.youtube.com/watch?v=Q2YnmSCwm00)

The video presentation outlines the full journey of the project:

- **Application Design**:  
  Simulating a generative experience through a multi-class classification model.

- **Data Collection and Processing**:  
  1,440 audio samples sourced from the RAVDESS dataset (Kaggle), categorized into Positive, Neutral, and Negative.  
  Dataset exploration revealed imbalance, which was corrected through stratified resampling.

- **Feature Extraction**:  
  Mel-filterbank energy (MFE) features were extracted using a 4000ms window size with a 2000ms stride, providing optimal temporal resolution.

- **Model Development**:  
  A lightweight 1D CNN-based architecture was chosen for efficient feature extraction and low inference latency.  
  Model design went through single-variable configuration experiments, architecture combination trials, and hyperparameter tuning.

- **Results**:  
  The best-performing configuration achieved a testing F1-score >75% with inference latency <500ms, fulfilling edge deployment requirements.

- **Critical Reflection**:  
  Limitations were identified in feature diversity, real-world robustness, and deployment latency optimization, informing future improvement directions.

---

## 🔧 Technical Architecture

This section outlines the key components and design considerations of the AI Fortune Teller system.


### 🔌 Hardware Components

The project employs the Arduino Nano 33 BLE Sense Lite as its computational core, leveraging its built-in machine learning capabilities and low power requirements. Two primary sensors facilitate interaction: a Grove Sound Sensor connected to analog pin A0 captures the user's speech, while a Touch Sensor connected to digital pin D2 detects user engagement. The onboard RGB LED provides intuitive visual feedback through color-coded responses that correspond to detected emotional states.


### 🧠 Code Logic

The system operates through a structured interaction flow where the touch sensor acts as the primary input trigger. When users press and hold the touch sensor, the device begins collecting audio data through the sound sensor. This raw audio data is normalized and stored in a buffer until either the buffer is full or the user releases the touch sensor.

The core emotion analysis relies on a lightweight CNN model trained on the RAVDESS dataset, which classifies speech into three emotional polarities: Positive, Neutral, and Negative. These classifications, rather than specific emotion categories, were chosen to optimize accuracy within the model's size constraints.

Upon classification, the system selects a thematically appropriate fortune response from its internal database, with visual feedback provided through color-coded LEDs: green for positive, blue for neutral, and red for negative predictions. This creates a seamless experience from question to fortune revelation.



### 📦 Enclosure Design
<div style="text-align: center;">
  <img src="img/enclosure.png" alt="AI Fortune Teller Enclosure" width="600">
</div>
The enclosure employs a rapid prototyping approach using sustainable, repurposed materials. This design choice reflects both the iterative nature of the development process and environmental consciousness. The lightweight, portable enclosure houses all components securely while providing access to the interaction surfaces. Strategic openings allow for optimal audio capture while protecting the internal circuitry. This approach to physical design emphasizes functionality and accessibility while enabling quick iterations during the development cycle.

---
