from pynput import mouse

def on_click(x, y, button, pressed):
    if pressed:
        print(f"({x}, {y})")

# Запускаем слушатель мыши
with mouse.Listener(on_click=on_click) as listener:
    listener.join()

#(561, 717)
#(1504, 27)