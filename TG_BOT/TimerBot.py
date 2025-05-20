import telebot
from datetime import datetime

API_TOKEN = '7487361106:AAE3Pg6gPeiPH3FOC9ZDZMcyAA6mOcsKtiY'
TARGET_DATE = datetime(2007, 1, 4, 1, 30, 0)
#6203370141,695779230

bot = telebot.TeleBot(API_TOKEN)

updating = False  

def time_difference(target_date):
    now = datetime.now()

    elapsed_time = now - target_date

    elapsed_years = elapsed_time.days // 365
    elapsed_months = (elapsed_time.days % 365) // 30
    elapsed_days = elapsed_time.days % 30
    elapsed_hours = elapsed_time.seconds // 3600
    elapsed_minutes = (elapsed_time.seconds % 3600) // 60
    elapsed_seconds = elapsed_time.seconds % 60

    elapsed_string = f"{elapsed_hours}:{elapsed_minutes}:{elapsed_seconds}   {elapsed_days}-{elapsed_months}-{elapsed_years}"

    next_target_date = datetime(now.year, target_date.month, target_date.day, target_date.hour, target_date.minute, target_date.second)

    if now > next_target_date:
        next_target_date = datetime(now.year + 1, target_date.month, target_date.day, target_date.hour, target_date.minute, target_date.second)

    remaining_time = next_target_date - now

    remaining_months = remaining_time.days // 30
    remaining_days = remaining_time.days % 30
    remaining_hours = remaining_time.seconds // 3600
    remaining_minutes = (remaining_time.seconds % 3600) // 60
    remaining_seconds = remaining_time.seconds % 60

    remaining_string = f"{remaining_hours}:{remaining_minutes}:{remaining_seconds}   {remaining_days}-{remaining_months}-_"

    return elapsed_string, remaining_string

@bot.message_handler(commands=['start'])
def start(message):
    global updating

    if updating:
        chat_id = message.chat.id
        bot.send_message(chat_id, "Отображение времени уже активно. Используйте /end, чтобы остановить обновление.")
        return

    chat_id = message.chat.id
    elapsed, remaining = time_difference(TARGET_DATE)

    initial_message = bot.send_message(chat_id, f"С рождения Ксари прошло:\n{elapsed}\n\nДо ближайшего дня рождения Ксари:\n{remaining}")
    
    previous_message = f"С рождения Ксари прошло:\n{elapsed}\n\nДо ближайшего дня рождения Ксари:\n{remaining}"

    updating = True  

    while updating:
        elapsed, remaining = time_difference(TARGET_DATE)
        
        new_message = f"С рождения Ксари прошло:\n{elapsed}\n\nДо ближайшего дня рождения Ксари:\n{remaining}"
        
        if new_message != previous_message:
            bot.edit_message_text(new_message, chat_id, initial_message.message_id)
            previous_message = new_message

@bot.message_handler(commands=['end'])
def end(message):
    global updating

    chat_id = message.chat.id
    bot.send_message(chat_id, "Остановка обновления сообщений. Используйте /start, чтобы снова начать обновление.")
    
    updating = False  

@bot.message_handler(content_types=['new_chat_members'])
def welcome_new_member(message):
    for new_member in message.new_chat_members:
        bot.send_message(message.chat.id, f"Привет, {new_member.first_name}! Я бот, который отслеживает время с момента рождения Ксари.\n\n"
                                         "Вот что я могу:\n"
                                         "/start - начать отслеживание времени и обновлять информацию о времени с рождения Ксари и времени до следующего дня рождения.\n"
                                         "/end - остановить обновление времени.")

if __name__ == '__main__':
    bot.polling(none_stop=True)
