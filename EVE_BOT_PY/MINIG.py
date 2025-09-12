import pyautogui
import cv2
import numpy as np
import mss
import time
import random
import json
import os



pyautogui.FAILSAFE = False
pattern_base = []
last_pattern_used = None
prev_gray = None



def load_pattern_base():
    global pattern_base

    pattern_base_path = 'pattern_base.json'
    if not os.path.exists(pattern_base_path):
        print("pattern_base.json не найден!")
        return

    with open(pattern_base_path, "r", encoding="utf-8") as f:
        pattern_base = json.load(f)

def has_screen_changed(threshold=0.5, top_left=(0, 0), bottom_right=(0, 0)):
    #Чем выше threshold, тем сильнее должно быть изменение, чтобы функция вернула True.

    global prev_gray  # Используем глобальную переменную

    with mss.mss() as sct:
        x1, y1 = top_left
        x2, y2 = bottom_right
        region = (x1, y1, x2 - x1, y2 - y1)  # (x, y, width, height)

        # Новый снимок экрана
        current_screen = np.array(sct.grab(region))
        current_gray = cv2.cvtColor(current_screen, cv2.COLOR_BGR2GRAY)

        # Вычисляем разницу
        diff = cv2.absdiff(prev_gray, current_gray)
        diff_value = np.sum(diff) / diff.size  # Среднее изменение пикселей

        # Обновляем предыдущий снимок
        prev_gray = current_gray.copy()

        # Если изменение превышает порог — возвращаем True
        return diff_value > threshold * 255

def element_finder(template_path, threshold=0.8, top_left=None, bottom_right=None):
    global prev_gray

    # Загружаем изображение-образец
    template = cv2.imread(template_path, cv2.IMREAD_UNCHANGED)
    if template is None:
        raise FileNotFoundError(f"Файл {template_path} не найден!")

    template_gray = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)  # Перевод в ч/б

    with mss.mss() as sct:
        if top_left and bottom_right:
            x1, y1 = top_left
            x2, y2 = bottom_right

            # Проверка корректности координат
            if x2 <= x1 or y2 <= y1:
                raise ValueError(f"Некорректные координаты: top_left={top_left}, bottom_right={bottom_right}")

            region = {"top": y1, "left": x1, "width": x2 - x1, "height": y2 - y1}

            try:
                screenshot = sct.grab(region)
            except mss.exception.ScreenShotError:
                print("Ошибка снятия скриншота! Возможно, нужно запустить от имени администратора.")
                return -1, -1
        else:
            screenshot = sct.grab(sct.monitors[1])  # Весь экран

        screen = np.array(screenshot)
        screen_gray = cv2.cvtColor(screen, cv2.COLOR_BGR2GRAY)

        prev_gray = screen_gray.copy()

        # Сохранение скриншота
        cv2.imwrite("screenshot.png", screen)  # Сохранение изображения

    # Поиск шаблона в указанной области
    result = cv2.matchTemplate(screen_gray, template_gray, cv2.TM_CCOEFF_NORMED)
    _, max_val, _, max_loc = cv2.minMaxLoc(result)

    if max_val >= threshold:
        x, y = max_loc

        # Коррекция координат с учётом области
        if top_left:
            x += top_left[0]
            y += top_left[1]

        a = random.randint(6, 15)
        b = random.randint(4, 10)

        return x + a, y + b  # Верхний левый угол найденного элемента

    return -1, -1

def load_mouse_pattern():
    global last_pattern_used

    available_patterns = [p for p in pattern_base if p != last_pattern_used]

    if not available_patterns:
        print("Нет доступных паттернов.")
        return None

    selected_pattern = random.choice(available_patterns)

    last_pattern_used = selected_pattern

    with open(selected_pattern, "r", encoding="utf-8") as f:
        return json.load(f)

def transform_path(pattern, start_pos, end_pos):
    original_start = (pattern[0]["x"], pattern[0]["y"])

    # 1. Вычисляем сдвиг
    shift_x = start_pos[0] - original_start[0]
    shift_y = start_pos[1] - original_start[1]

    # 2. Сдвигаем все точки (делаем так, чтобы первая стала равна A)
    shifted_pattern = []
    for point in pattern:
        shifted_x = point["x"] + shift_x
        shifted_y = point["y"] + shift_y
        shifted_pattern.append({"x": shifted_x, "y": shifted_y})

    # 3. Пересчитываем диапазоны
    shifted_start = (shifted_pattern[0]["x"], shifted_pattern[0]["y"])
    shifted_end = (shifted_pattern[-1]["x"], shifted_pattern[-1]["y"])

    range_x = shifted_end[0] - shifted_start[0]
    range_y = shifted_end[1] - shifted_start[1]

    target_range_x = end_pos[0] - start_pos[0]
    target_range_y = end_pos[1] - start_pos[1]

    # 4. Вычисляем коэффициенты масштабирования
    scale_x = target_range_x / range_x if range_x != 0 else 1
    scale_y = target_range_y / range_y if range_y != 0 else 1

    # 5. Трансформируем путь с учетом масштабирования
    transformed_path = []
    for point in shifted_pattern:
        new_x = (point["x"] - shifted_start[0]) * scale_x + start_pos[0]
        new_y = (point["y"] - shifted_start[1]) * scale_y + start_pos[1]
        transformed_path.append({"x": int(round(new_x)), "y": int(round(new_y))})

    return transformed_path

def replay_mouse_movement(path, interval):
    for point in path:
        pyautogui.moveTo(point["x"], point["y"], duration=interval)

def mause_move_to(target_x, target_y, step=0.005):
    pattern = load_mouse_pattern()
    if pattern is None:
        print("Не удалось загрузить паттерн!")
        return

    start_x, start_y = pyautogui.position()
    transformed_path = transform_path(pattern, (start_x, start_y), (target_x, target_y))

    replay_mouse_movement(transformed_path, step)

def mause_click(max, type='left', min=0.005): #right
    x, y = pyautogui.position()
    pyautogui.mouseDown(x, y, button=type)

    t = random.uniform(min, max)
    time.sleep(t)

    pyautogui.mouseUp(x, y, button=type)

def key_input(max_delay, symbol, min_delay=0.01):
    t = random.uniform(min_delay, max_delay)
    time.sleep(t)

    keys = symbol.split("+")

    for key in keys:
        pyautogui.keyDown(key)

    time.sleep(t)

    for key in reversed(keys):
        pyautogui.keyUp(key)

def smooth_scroll(amount, min_step=10,max_step=30, min_delay=0.005, max_delay=0.05):# + = вверх | - = вниз

    direction = 1 if amount > 0 else -1
    amount = abs(amount)

    while amount > 0:
        step = random.randint(min_step, max_step)
        scroll_step = min(step, amount)  # Гарантия, что не выйдем за границы
        pyautogui.scroll(scroll_step * direction)
        amount -= scroll_step
        time.sleep(random.uniform(min_delay, max_delay))

def main():
    load_pattern_base()

    time.sleep(3.0)

    x, y = element_finder("visual_patterns/photo1.png")

    mause_move_to(x, y, 0.005)
    mause_click(0.5, "left")

    x = -1
    while x < 0:
        x, y = element_finder("visual_patterns/photo2.png")
        time.sleep(0.5)

    mause_move_to(x, y, 0.005)
    mause_click(0.5, "left")

    x = -1
    while x < 0:
        x, y = element_finder("visual_patterns/photo10.png")
        time.sleep(0.5)

    mause_move_to(x, y, 0.005)
    mause_click(0.5, "left")
    key_input(0.5, 's')

    time.sleep(1.0)
    x = -1
    while x < 0:
        x, _ = element_finder("visual_patterns/photo11.png", top_left=(805, 993), bottom_right=(1149, 1064))
        time.sleep(0.5)

    key_input(0.3, 'shift+f')
    mause_click(0.5, "left")

    x = -1
    while x < 0:
        x, y = element_finder("visual_patterns/photo4.png",0.85,(1397, 179),(1912, 1011))
        key_input(0.5, 'down')
        time.sleep(0.5)

    mause_move_to(x, y, 0.005)
    mause_click(0.5, "left")
    key_input(0.5, 'w')

    x = -1
    while x < 0:
        x, _ = element_finder("visual_patterns/photo14.png",0.95,(1566, 24),(1850, 98))
        time.sleep(0.5)
    key_input(0.5, 'ctrl')

    time.sleep(1.0)

    x = -1
    while x < 0:
        x, _ = element_finder("visual_patterns/photo13.png")
        time.sleep(1.0)

    time.sleep(1.0)

    while x >= 0:
        key_input(0.5, '2')
        time.sleep(3.0)
        x, _ = element_finder("visual_patterns/photo12.png",top_left=(784, 109),bottom_right=(1131, 240))
        time.sleep(3.0)

    key_input(0.5, '1')

    x = -1
    while x < 0:
        x, _ = element_finder("visual_patterns/photo5.png",threshold=0.9 ,top_left=(243, 627), bottom_right=(663, 988))
        time.sleep(3.0)

    x, y = element_finder("visual_patterns/photo6.png")
    mause_move_to(x, y, 0.005)
    mause_click(0.5, "left")
    key_input(0.3, 'shift+r')

    x = -1
    while x < 0:
        x, y = element_finder("visual_patterns/photo7.png")
        time.sleep(0.5)

    mause_move_to(x, y, 0.005)
    mause_click(0.5, "left")
    key_input(0.5, 'd')

    x = -1
    while x < 0:
        x, _ = element_finder("visual_patterns/photo1.png")
        time.sleep(3.0)

    x, y = element_finder("visual_patterns/photo8.png")
    mause_move_to(x, y, 0.005)
    pyautogui.mouseDown(x, y, button='left')

    x, y = element_finder("visual_patterns/photo9.png")
    mause_move_to(x, y, 0.005)
    pyautogui.mouseUp(x, y, button='left')

if __name__ == "__main__":
    main()
