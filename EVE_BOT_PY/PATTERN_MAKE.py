import pyautogui
import time
import json
import os
from datetime import datetime
import keyboard


def record_mouse_movement(save_dir="mouse_patterns", interval=0.005):

    if not os.path.exists(save_dir):
        os.makedirs(save_dir)

    filename = f"mouse_pattern_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
    filepath = os.path.join(save_dir, filename)

    print("Запись движения мыши... (нажмите ESC для остановки)")

    data = []

    try:
        while True:
            x, y = pyautogui.position()
            data.append({"x": x, "y": y})

            time.sleep(interval)

            if keyboard.is_pressed("esc"):
                print("\nЗапись остановлена.")
                break

    except KeyboardInterrupt:
        print("\nПринудительное завершение.")

    with open(filepath, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4)

    print(f"Данные сохранены в {filepath}")

    pattern_base_path = 'pattern_base.json'

    if not os.path.exists(pattern_base_path):
        with open(pattern_base_path, "w", encoding="utf-8") as f:
            json.dump([f"mouse_patterns/{filename}"], f, indent=4)
    else:
        with open(pattern_base_path, "r", encoding="utf-8") as f:
            pattern_base = json.load(f)

        pattern_base.append(f"mouse_patterns/{filename}")

        with open(pattern_base_path, "w", encoding="utf-8") as f:
            json.dump(pattern_base, f, indent=4)

    print("Имя файла добавлено в pattern_base.json.")

if __name__ == "__main__":
    time.sleep(1.0)

    record_mouse_movement()

