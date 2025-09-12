import pyautogui
import cv2
import numpy as np
import mss
import time
import random
import json
import os
import MINIG



pyautogui.FAILSAFE = False
pattern_base = []
last_pattern_used = None
prev_gray = None



def element_finder_col(template_path, threshold=0.8, top_left=None, bottom_right=None):
    global prev_gray

    # Загружаем изображение-образец
    template = cv2.imread(template_path, cv2.IMREAD_COLOR)  # Цветное изображение (BGR)
    if template is None:
        raise FileNotFoundError(f"Файл {template_path} не найден!")

    with mss.mss() as sct:
        if top_left and bottom_right:
            x1, y1 = top_left
            x2, y2 = bottom_right

            if x2 <= x1 or y2 <= y1:
                raise ValueError(f"Некорректные координаты: top_left={top_left}, bottom_right={bottom_right}")

            region = {"top": y1, "left": x1, "width": x2 - x1, "height": y2 - y1}

            try:
                screenshot = sct.grab(region)
            except mss.exception.ScreenShotError:
                print("Ошибка снятия скриншота! Возможно, нужно запустить от имени администратора.")
                return -1, -1, 0
        else:
            screenshot = sct.grab(sct.monitors[1])  # Весь экран

        screen = np.array(screenshot)

        # Удаляем альфа-канал (если есть), чтобы у скриншота и шаблона было одинаковое количество каналов
        if screen.shape[-1] == 4:
            screen = cv2.cvtColor(screen, cv2.COLOR_BGRA2BGR)

        prev_gray = screen.copy()

        # Сохранение скриншота
        cv2.imwrite("screenshot.png", screen)

    # Приводим изображения к одинаковому формату (BGR -> Grayscale, если нужно)
    if template.shape[-1] == 3 and screen.shape[-1] == 3:
        # Оба изображения цветные → используем TM_CCOEFF_NORMED по 3 каналам
        result = cv2.matchTemplate(screen, template, cv2.TM_CCOEFF_NORMED)
    else:
        # Если одно из изображений черно-белое, переводим оба в grayscale
        screen_gray = cv2.cvtColor(screen, cv2.COLOR_BGR2GRAY)
        template_gray = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)
        result = cv2.matchTemplate(screen_gray, template_gray, cv2.TM_CCOEFF_NORMED)

    _, max_val, _, max_loc = cv2.minMaxLoc(result)

    if max_val >= threshold:
        x, y = max_loc

        if top_left:
            x += top_left[0]
            y += top_left[1]

        a = random.randint(4, 13)
        b = random.randint(4, 10)

        return x + a, y + b, max_val  # Возвращаем координаты и точность

    return -1, -1, 0

def main():
    MINIG.load_pattern_base()

    time.sleep(3.0)

    while True:
        x1 = -1
        x2 = -1

        x1 ,y1 ,cor1 = element_finder_col("visual_patterns/2_photo2.png")
        x2 ,y2 ,cor2 = element_finder_col("visual_patterns/2_photo1.png")

        if cor1 > 0 or cor2 > 0:

            if cor1 > cor2:
                MINIG.mause_move_to(x1, y1, 0.005)
                MINIG.mause_click(0.5, 'left')
                MINIG.key_input(0.5, 'd')
                return

            else:
                MINIG.mause_move_to(x2,y2, 0.005)
                MINIG.mause_click(0.5, 'left')
                MINIG.key_input(0.5, 'd')

                MINIG.mause_move_to(x2-100,y2+50, 0.005)

                x = -1
                while x < 0:
                    x, _ = MINIG.element_finder("visual_patterns/photo11.png", top_left=(805, 993), bottom_right=(1149, 1064))
                    time.sleep(0.5)

                time.sleep(4.0)

if __name__ == '__main__':
    main()
