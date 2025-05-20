import telebot
import json
import os
import requests

API_TOKEN = '7487361106:AAE3Pg6gPeiPH3FOC9ZDZMcyAA6mOcsKtiY'
XI_API_KEY = 'sk_18414ad2b24d5982dea6221d9cbf1fb9de2c31bd56ba1db7'

USER_DATA_FILE = 'C:\\TG_BOT\\basa.json'

ELEVENLABS_URL = "https://api.elevenlabs.io/v1/voices"
CHUNK_SIZE = 1024
headers = {
    "Accept": "application/json",
    "xi-api-key": XI_API_KEY,
    "Content-Type": "application/json"
}

SIZE = 500

Exclusive_user = {6203370141,695779230,1279390268,1352571953}

bot = telebot.TeleBot(API_TOKEN)

user_data = {}
generated_users = {}

def load_generated_users():
    if os.path.exists(USER_DATA_FILE):
        with open(USER_DATA_FILE, 'r') as file:
            return {int(k): v for k, v in json.load(file).items()}
    return {}

def save_generated_users():
    with open(USER_DATA_FILE, 'w') as file:
        json.dump({str(k): v for k, v in generated_users.items()}, file)

generated_users = load_generated_users()

def show_speakers_list():
    response = requests.get(ELEVENLABS_URL, headers=headers)
    data = response.json()
    voice_list = [voice['name'] for voice in data['voices']]
    return data, voice_list

def find_voice_id_by_name(data, name):
    for voice in data.get('voices', []):
        if voice.get('name') == name:
            return voice.get('voice_id')
    return None

def get_audio(output_path, text, voice_id, chunk_size=CHUNK_SIZE):
    data = {
        "text": text,
        "model_id": "eleven_multilingual_v2",
        "voice_settings": {
            "stability": 0.5,
            "similarity_boost": 0.8,
            "style": 0.0,
            "use_speaker_boost": True
        }
    }

    response = requests.post(f"https://api.elevenlabs.io/v1/text-to-speech/{voice_id}/stream", headers=headers, json=data, stream=True)

    if response.ok:
        with open(output_path, "wb") as f:
            for chunk in response.iter_content(chunk_size=chunk_size):
                f.write(chunk)
        print("Audio stream saved successfully.")
    else:
        raise Exception(response.text)

@bot.message_handler(commands=['start'])
def start(message):
    chat_id = message.chat.id 

    if chat_id in Exclusive_user:
        bot.send_message(chat_id, "Привет! У вас неограниченное количество символов для использования.")
        bot.send_message(chat_id, "Введите текст, который вы хотите преобразовать в речь (англ/рус):")
        user_data[chat_id] = {'stage': 'waiting_for_text'}
        return

    if chat_id in generated_users:
        remaining_characters = generated_users[chat_id]['remaining_characters']
        if remaining_characters <= 0:
            bot.send_message(chat_id, f"У вас закончились {SIZE} токенов. Вы не можете генерировать текст.")
            return
        else:
            bot.send_message(chat_id, f"Привет! У вас осталось {remaining_characters} символов.")
    else:
        generated_users[chat_id] = {'remaining_characters': SIZE}
        bot.send_message(chat_id, f"Привет! У вас {SIZE} символов для использования.")

    bot.send_message(chat_id, f"Введите текст, который вы хотите преобразовать в речь (англ/рус):")
    user_data[chat_id] = {'stage': 'waiting_for_text'}

@bot.message_handler(func=lambda message: message.chat.id in user_data and user_data[message.chat.id]['stage'] == 'waiting_for_text')
def handle_text(message):
    chat_id = message.chat.id 
    user_text = message.text

    max_size = SIZE if chat_id not in generated_users else generated_users[chat_id]['remaining_characters']

    if chat_id not in Exclusive_user and len(user_text) > max_size:
        bot.send_message(chat_id, f"Вы можете использовать только {max_size} символов.")
        return

    if chat_id not in generated_users and len(user_text) > SIZE:
        bot.send_message(chat_id, f"Текст слишком длинный. Пожалуйста, введите текст не длиннее {SIZE} символов.")
        return

    data, voice_list = show_speakers_list()

    bot.send_message(chat_id, "Доступные голоса:\n" + "\n".join(voice_list))

    user_data[chat_id] = {'text': user_text, 'data': data, 'stage': 'waiting_for_voice'}
    bot.send_message(chat_id, "Введите имя голоса для генерации аудио:")

@bot.message_handler(func=lambda message: message.chat.id in user_data and user_data[message.chat.id]['stage'] == 'waiting_for_voice')
def handle_voice_name(message):
    chat_id = message.chat.id 
    voice_name = message.text

    text = user_data[chat_id]['text']
    data = user_data[chat_id]['data']

    voice_id = find_voice_id_by_name(data, voice_name)
    if voice_id is None:
        bot.send_message(chat_id, "Голос не найден. Попробуйте снова.")
        return

    user_folder = f"C:\\TG_BOT\\audio\\{chat_id}"
    os.makedirs(user_folder, exist_ok=True)

    existing_files = os.listdir(user_folder)
    record_number = len(existing_files) + 1 

    output_path = os.path.join(user_folder, f"{voice_name}_{chat_id}_{record_number}.mp3")
    
    try:
        get_audio(output_path, text, voice_id)
        with open(output_path, 'rb') as audio_file:
            bot.send_audio(chat_id, audio_file)
        bot.send_message(chat_id, "Аудио успешно сгенерировано и отправлено!")

        if chat_id not in Exclusive_user:
            generated_users[chat_id]['remaining_characters'] -= len(text)

        save_generated_users()

        user_data.pop(chat_id)
    except Exception as e:
        bot.send_message(chat_id, f"Произошла ошибка при генерации аудио: {str(e)}")

if __name__ == '__main__':
    bot.polling(none_stop=True)
