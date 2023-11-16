#!/usr/bin/env -P /usr/bin:/usr/local/bin python3 -B
# coding: utf-8

#
#  main.py
#  ReconnaissanceParole version 1.0
#  Created by Ingenuity i/o on 2023/11/15
#

import sys
import time
import ingescape as igs
import pygame
import speech_recognition as sr
from gtts import gTTS


def vocal_response(response):
    """
    Synthesizes and vocalizes the provided response using text-to-speech (TTS) technology.

    Parameters:
    - response (str): The text response to be vocalized.

    This function uses the gTTS (Google Text-to-Speech) library to convert the input text
    into speech in French and saves it as an MP3 file named "response.mp3". The function
    then uses the Pygame library to play the generated audio file, ensuring that the program
    waits for the audio playback to complete before exiting.

    Note:
    - Make sure to have the required libraries installed (gTTS and Pygame) before using
      this function.
    - The generated MP3 file ("response.mp3") is overwritten with each function call,
      so be cautious if you need to preserve previous vocalizations.
    """
    # Synthesize the response from the input string
    tts = gTTS(text=response, lang="fr")
    tts.save("response.mp3")

    # Initialize audio pygame
    pygame.init()
    pygame.mixer.init()

    # Read the mp3 file containing the response
    pygame.mixer.music.load("response.mp3")
    pygame.mixer.music.play()

    # Wait for the end of the audio
    while pygame.mixer.music.get_busy():
        continue

    pygame.quit()


def recognize_speech():
    """
    Utilizes speech recognition to transcribe spoken words and identify specific commands.

    Returns a tuple containing the transcribed text and an action indicator for controlling
    a system (e.g., opening or closing a gate).

    Returns:
    tuple (str, int): A tuple containing the transcribed text and an action indicator.
                     - The transcribed text represents the speech recognized from the audio.
                     - The action indicator is an integer:
                       - 1 for an opening request (e.g., "ouvre").
                       - 0 for a closing request (e.g., "ferme").
                       - -1 if no relevant action is detected.
                       Note: Action indicator is specific to the context of controlling a gate.

    This function initializes a speech recognizer and listens for a trigger word ("ok portail").
    Upon detecting the trigger word, it prompts the user to speak and transcribes the spoken words.
    It then interprets the transcribed text to identify specific commands related to opening or
    closing a gate. The recognized text and the action indicator are returned as a tuple.

    Note:
    - The function uses the Google Web Speech Recognition API for speech recognition.
    - Ensure the required library (SpeechRecognition) is installed before using this function.
    - The function uses the vocal_response function to provide feedback during the speech interaction.
    """
    # Initialize the recognizer
    recognizer = sr.Recognizer()
    actionPortail: int = -1
    texte: str = ""

    with sr.Microphone() as source:
        print("Waiting for the trigger word...")
        audio = recognizer.listen(source)

    try:
        # Recognize speech using the Google Web Speech Recognition API (French)
        texte = recognizer.recognize_google(audio, language="fr-FR")

        # Verify if the trigger sentence is pronounced
        if "ok portail" in texte.lower():
            print("Activation of the speech recognition. Please talk now...")
            vocal_response("Oui ?")

            # Listening again for the sentence to transcribe
            with sr.Microphone() as source:
                # Timeout defines how long the system listens after the trigger word is spoken.
                audio = recognizer.listen(source, timeout=10)
            texte = recognizer.recognize_google(audio, language="fr-FR")
            print("You said:", texte)
            texte = texte.replace("-", " ")
            words = texte.split()  # Split the recognized text into words

            # Inspect the output text for seeking an action
            for word in words:
                # Opening detection
                if (word == "ferme") or (word == "verrouille"):
                    actionPortail = 1
                    vocal_response("D'accord, je ferme le portail.")
                    break
                # Closing detection
                if (word == "ouvre") or (word == "deverrouille"):
                    actionPortail = 0
                    vocal_response("D'accord, j'ouvre le portail.")
                    break

    except sr.UnknownValueError:
        print("Sorry, I could not understand.")

    except sr.RequestError as e:
        print(f"Error from API Google: {e}")

    return texte, actionPortail


if __name__ == "__main__":
    # Check if the correct number of command-line arguments is provided
    if len(sys.argv) < 4:
        # Print the correct usage and available network devices if arguments are insufficient
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as the network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    # Set up the IGs agent with specified parameters
    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    # Create output variables for the IGs agent
    igs.output_create("commandePortail", igs.INTEGER_T, None)
    igs.output_create("TexteBrut", igs.STRING_T, None)

    # Start the IGs agent with the specified network device and port
    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    # Continuously listen for speech input and update IGs outputs
    while 1:
        prompt, commande = recognize_speech()
        if commande != -1:
            # Update IGs outputs with recognized speech information
            igs.output_set_string("TexteBrut", prompt)
            igs.output_set_int("commandePortail", commande)

        # Pause execution for a short interval to avoid excessive resource usage
        time.sleep(0.01)
