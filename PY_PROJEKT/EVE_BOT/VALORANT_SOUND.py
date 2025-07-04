


from pynput import mouse
import pygame
import threading
import time

pygame.mixer.init()
AUDIO_FILE = "audio.wav"
sound = pygame.mixer.Sound(AUDIO_FILE)

is_playing = False
start_time = 0
MIN_PLAY_DURATION = 0.8 #0.8 # секунд

def play_looped_sound():
    sound.play(loops=-1)

def stop_sound_after_min_delay():
    global is_playing
    elapsed = time.time() - start_time
    remaining = MIN_PLAY_DURATION - elapsed
    if remaining > 0:
        time.sleep(remaining)
    sound.stop()
    is_playing = False

def on_click(x, y, button, pressed):
    global is_playing, start_time
    if button == mouse.Button.left:
        if pressed and not is_playing:
            start_time = time.time()
            is_playing = True
            threading.Thread(target=play_looped_sound, daemon=True).start()
        elif not pressed and is_playing:
            threading.Thread(target=stop_sound_after_min_delay, daemon=True).start()

if __name__ == '__main__':
    with mouse.Listener(on_click=on_click) as listener:
        listener.join()
