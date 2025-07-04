
import pyautogui
import cv2
import numpy as np
import mss
import time
import random
import json
import os

def key_input(max_delay, symbol, min_delay=0.01):
    t = random.uniform(min_delay, max_delay)
    time.sleep(t)

    keys = symbol.split("+")

    for key in keys:
        pyautogui.keyDown(key)

    time.sleep(t)

    for key in reversed(keys):
        pyautogui.keyUp(key)

def main():
    time.sleep(2.0)
    key_input(0.3,'s')
    time.sleep(2.0)
    key_input(0.3,'ctrl+z')

if __name__ == '__main__':
    main()