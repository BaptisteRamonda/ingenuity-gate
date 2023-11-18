# Software
## Components

### 1. [Appel](Appel/main.py)

The "Appel" component handles the recognition of someone ringing the gatebell. It utilizes speech recognition to interpret voice commands, allowing the user to open or close the gate.

### 2. [EtatVersTexte](EtatVersTexte/main.py)

The "EtatVersTexte" component converts the state of the gate into text. It observes changes in the gate's state and updates a graphical user interface accordingly.

### 3. [Interface](Interface/main.py)

The "Interface" component creates a graphical user interface (GUI) for the system. It uses a Whiteboard service to display information about the gate's state and allows users to interact with the system.

### 4. [Maquette](Maquette/main.py)

The "Maquette" component simulates a physical model of the gate using a serial connection. It receives commands from the system and updates the gate's state based on simulated data.

### 5. [ReconnaissanceParole](ReconnaissanceParole/main.py)

The "ReconnaissanceParole" component implements voice recognition to interpret user commands. It listens for a trigger phrase ("ok portail") and then processes subsequent speech to determine actions such as opening or closing the gate.

## Dependencies
- Python 3.10
- [Ingescape](https://ingescape.com/fr/circle/)
- pygame
- speech_recognition
- gTTS
- Other dependencies specified in each component's README
