#!/usr/bin/env -P /usr/bin:/usr/local/bin python3 -B
# coding: utf-8

#
#  main.py
#  ReconnaissanceParole version 1.0
#  Created by Piriou--Dezy, Roux, Ramonda i/o on 2023/11/15
#

import sys
import time

import ingescape as igs
import pygame
import speech_recognition as sr
from gtts import gTTS


def vocal_response(response):
    """
    Synthesizes the provided response into speech and plays it.

    Parameters:
    - response (str): The text to be synthesized and spoken.
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
    Performs speech recognition and interprets the recognized text.

    Returns:
    - prompt (str): The recognized speech converted to text.
    - actionPortail (int): The action to be taken based on the recognized speech
                          (-1 if no relevant action).
    """
    # Initialize the recognizer
    recognizer = sr.Recognizer()
    actionPortail: int = -1
    prompt: str = ""

    with sr.Microphone() as source:
        print("Waiting for the trigger word...")
        audio = recognizer.listen(source)

    try:
        # Recognize speech using the Google Web Speech Recognition API (French)
        prompt = recognizer.recognize_google(audio, language="fr-FR")

        # Verify if the trigger sentence is pronounced
        if "ok portail" in prompt.lower():
            print("Activation of the speech recognition. Please talk now...")
            vocal_response("Oui ?")

            # Listening again for the sentence to transcribe
            with sr.Microphone() as source:
                # Timeout defines how long the system listens after the trigger word is spoken.
                audio = recognizer.listen(source, timeout=10)
            prompt = recognizer.recognize_google(audio, language="fr-FR")
            print("You said:", prompt)
            prompt = prompt.replace("-", " ")
            words = prompt.split()  # Split the recognized text into words

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

    return prompt, actionPortail


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    igs.output_create("commandePortail", igs.INTEGER_T, None)
    igs.output_create("TexteBrut", igs.STRING_T, None)

    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    while (1):
        prompt, commande = recognize_speech()
        if commande != -1:
            igs.output_set_string("TexteBrut", prompt)
            igs.output_set_int("commandePortail", commande)

        time.sleep(0.01)
    input()

    igs.stop()
