import telebot
import json
import os

API_TOKEN = '7487361106:AAE3Pg6gPeiPH3FOC9ZDZMcyAA6mOcsKtiY'
TEST_DATA_FILE = 'test_basa.json'
BOT_USERNAME = '@PisskinkExamBot'

bot = telebot.TeleBot(API_TOKEN)
user_state = {}
test_results = {}

if os.path.exists(TEST_DATA_FILE):
    with open(TEST_DATA_FILE, 'r') as f:
        test_results = json.load(f)

questions = [
    {
        "question": "?",
        "options": ["папа", "альтер"],
        "correct_id": 1
    }
]
''',
    {
        "question": "Сколько будет 2 + 2?",
        "options": ["3", "4", "5", "22"],
        "correct_id": 2
    },
    {
        "question": "Язык программирования для Telegram-ботов?",
        "options": ["Python", "HTML", "Photoshop", "Excel"],
        "correct_id": 0
    }
]'''

@bot.message_handler(commands=['start'])
def start_quiz(message):
    user_id = str(message.from_user.id)

    if user_id in test_results:
        bot.send_message(message.chat.id, "Ві ужі прійшлі тіст. Півтірное пріхіждіние нівізміжно.")
        return

    if message.chat.id != message.from_user.id:
        bot.send_message(message.chat.id, f"Щіб прійті тіст, пірійдіть в: {BOT_USERNAME}")
        return

    username = message.from_user.username
    name = f"@{username}" if username else "чірка_ібаній"

    user_state[user_id] = {
        "name": name,
        "score": 0,
        "current_q": 0,
        "chat_id": message.chat.id
    }

    bot.send_message(message.chat.id, "Сійчіс ві прійдіте тіст от нішій вілікій Ксари, тіст міжні прійті тількі ідін різ — ністіятільно прішу о пілнім зівіршеніі тіста, підіхніте!")
    send_next_question(user_id)

def send_next_question(user_id):
    state = user_state.get(user_id)
    if not state:
        return

    if state["current_q"] >= len(questions):
        total = len(questions)
        correct = state["score"]
        name = state["name"]
        chat_id = state["chat_id"]

        bot.send_message(chat_id, f'────────▄▀▀▀▀▀▀▀▄───────\n───────█▒▒▒▒▒▒▒▒▒█──────\n──────█▒▒▒▒▒▒▒▒▒▒█──────\n──────█▒▒▀▄▄▒▄▄▀▒█──────\n─▄▄▄──█▒▒─▀─▒─▀─▒█──────\n█░░░▀▄█▒▒▒▒▒▒▒▒▒▒█──────\n▀▄░░░▄▀▀▄▒▀▀▀▀▀▒▒▒▒▀▀▄───\n─▀▄▄▀░░░░▀▄▒▒▒▒▒▒▒▒▒▒▀▄─\n─█▒▀▄░░░░░░▀▄▒▒▒▒▒▒█▒▒█─\n─▀▄▀▒▀▄░░░░░░▀▄▄▒▒▒▒▀▄▀─\n───█▌▌▄▀░░░░░░░░░▀▄▌▌█───\n───▀█▌█░░░░░▄░░░░░██▀───\n─────██▀▄▄▄▄▀▄▄▄▄▀██────') 
        #bot.send_message(chat_id, f"Тіст зівіршен. {name}, ві отвітілі прівільно на {correct} із {total} віпрісов.")

        test_results[user_id] = {
            "name": name,
            "correct": correct,
            "total": total
        }

        with open(TEST_DATA_FILE, 'w') as f:
            json.dump(test_results, f, indent=4)

        user_state.pop(user_id, None)
        return

    q = questions[state["current_q"]]
    chat_id = state["chat_id"]

    bot.send_poll(
        chat_id,
        q["question"],
        q["options"],
        type='regular',
        allows_multiple_answers=False,
        is_anonymous=False
    )

@bot.poll_answer_handler()
def handle_poll_answer(poll_answer):
    user_id = str(poll_answer.user.id)
    state = user_state.get(user_id)

    if not state:
        return

    current_q_index = state["current_q"]
    correct_id = questions[current_q_index]["correct_id"]
    user_answer = poll_answer.option_ids[0]

    if user_answer == correct_id:
        state["score"] += 1

    state["current_q"] += 1
    send_next_question(user_id)

if __name__ == '__main__':
    bot.polling(none_stop=True)
