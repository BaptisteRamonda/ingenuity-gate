# Ingenuity Gate
## Presentation

The Ingenuity Gate project is part of the Robotic and Interactive Systems (SRI) program at the UPSSITECH engineering school, and more specifically of the “Distributed Interaction” course. The main objective of this project was to design a project using Ingescape, a software suite dedicated to Systems Engineering, Software Interoperability and Human-System Integration. The only guideline imposed was the use of a Whiteboard, employed here as a display screen, as one of the central element of the project.

Our group, made up of Peter PIRIOU--DEZY, Baptiste RAMONDA and Constant ROUX, opted to create a model of a gate, whose opening and closing is voice-controlled. The whiteboard screen would then be used as visual feedback indicating the system's current state of opening and closing. It would also be used to display the various voice prompts issued to the system.

## Functionalities

The final system offers a variety of functions, as detailed below:

- Voice-controlled gate closing after activating the system with the defined keyword.
- Voice-controlled gate opening after activating the system with the defined keyword.
- The possibility of requesting gate opening by means of a bell placed at the gate entrance. This request can be confirmed or denied by voice, according to the user's wishes.

Graphical feedback (gate opening/closing in real time) and textual feedback (voice command prompts) are displayed on the whiteboard, serving as a user feedback interface.

## Getting Started

### Dependencies
- Python 3.10
- [Ingescape](https://ingescape.com/fr/circle/)
- pygame
- speech_recognition
- gTTS
- Other dependencies specified in each component's README

### Installation

```bash
python3 -m pip install ingescape
```
```bash
python3 -m pip install SpeechRecognition
```
```bash
python3 -m pip install gtts
```
```bash
python3 -m pip install pygame
```

### Usage

To begin, make sure you start Ingescape by launching the Circle software, then opening the ![platform](software/whiteboard) on it (using the WhiteboardDemo.igsplatform file).

Using the doorbell :

- Press the doorbell button at the gate entrance and wait for the system's voice response
- Answer "oui" or "non" to the system's request to open the gate
- If the answer is "oui", the gate opens, otherwise it remains closed.

Monitor gate opening/closing:

- Say the keyword "ok portail" and wait for the system's voice response indicating gate detection.
- Request the gate to open or close and wait for the system's voice response.

If you take too long to answer, or if the system doesn't understand you. You may need to repeat the activation keyword

## Demonstration
![Opening/Closing demo](demo/OuvertureFermeture.mp4)

![Gatebell demo](demo/AppelEntrant.mp4)
