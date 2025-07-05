import random
import os
import pygame
import time
import platform
import configparser
from pynput import keyboard
from colorama import init, Fore, Style

init()

def clear_console():
    if platform.system() == "Windows":
        os.system("cls")
    else:
        os.system("clear")

def load_settings():
    config = configparser.ConfigParser()
    config_file = 'config.ini'

    if os.path.isfile(config_file):
        use_saved = input("Load saved settings from config.ini? (y/n): ").strip().lower()
        if use_saved == 'y':
            config.read(config_file)
            s = config['Settings']
            total_numbers = int(s.get('total_numbers', 75))
            max_draws = int(s.get('max_draws', total_numbers))
            language = s.get('language', 'bg')
            auto_draw = s.get('auto_draw', 'yes').lower() == 'yes'
            delay = int(s.get('delay', 5))
            play_digits = s.get('play_digits', 'no').lower() == 'yes'
            return total_numbers, max_draws, language, auto_draw, delay, play_digits
        else:
            print("➡️  Okay, let’s set new settings!")

    total_numbers = int(input("Total numbers (75 or 90): "))
    if total_numbers not in (75, 90):
        print("Only 75 or 90 supported.")
        exit()

    max_draws = int(input(f"How many numbers to draw initially? (max {total_numbers}): "))
    language = input("Language code (e.g. bg, en): ").strip()
    auto_draw = input("Auto-draw? (y/n): ").strip().lower() == 'y'
    if auto_draw:
        delay = int(input("Delay between draws (seconds): "))
    else:
        delay = 0
    play_digits = input("Play digits after normal? (y/n): ").strip().lower() == 'y'

    save = input("Save these settings to config.ini? (y/n): ").strip().lower()
    if save == 'y':
        config['Settings'] = {
            'total_numbers': str(total_numbers),
            'max_draws': str(max_draws),
            'language': language,
            'auto_draw': 'yes' if auto_draw else 'no',
            'delay': str(delay),
            'play_digits': 'yes' if play_digits else 'no'
        }
        with open(config_file, 'w') as f:
            config.write(f)
        print(f"✅ Settings saved to {config_file}!")

    return total_numbers, max_draws, language, auto_draw, delay, play_digits

def play_audio(number, language, play_digits=False, total_numbers=75):
    if total_numbers == 75:
        letter = ''
        if 1 <= number <= 15:
            letter = 'B'
        elif 16 <= number <= 30:
            letter = 'I'
        elif 31 <= number <= 45:
            letter = 'N'
        elif 46 <= number <= 60:
            letter = 'G'
        elif 61 <= number <= 75:
            letter = 'O'

        letter_path = f"audio/{language}/{letter}.mp3"
        if os.path.isfile(letter_path):
            pygame.mixer.music.load(letter_path)
            pygame.mixer.music.play()
            while pygame.mixer.music.get_busy():
                time.sleep(0.1)

    normal_path = f"audio/{language}/{number}.mp3"
    if os.path.isfile(normal_path):
        pygame.mixer.music.load(normal_path)
        pygame.mixer.music.play()
        while pygame.mixer.music.get_busy():
            time.sleep(0.1)

    if play_digits:
        if number < 10:
            intro_path = f"audio/{language}_digits/intro.mp3"
            if os.path.isfile(intro_path):
                pygame.mixer.music.load(intro_path)
                pygame.mixer.music.play()
                while pygame.mixer.music.get_busy():
                    time.sleep(0.1)

        digit_path = f"audio/{language}_digits/{number}.mp3"
        if os.path.isfile(digit_path):
            pygame.mixer.music.load(digit_path)
            pygame.mixer.music.play()
            while pygame.mixer.music.get_busy():
                time.sleep(0.1)

def group_bingo_numbers(drawn):
    groups = {'B': [], 'I': [], 'N': [], 'G': [], 'O': []}
    for n in drawn:
        if 1 <= n <= 15:
            groups['B'].append(n)
        elif 16 <= n <= 30:
            groups['I'].append(n)
        elif 31 <= n <= 45:
            groups['N'].append(n)
        elif 46 <= n <= 60:
            groups['G'].append(n)
        elif 61 <= n <= 75:
            groups['O'].append(n)
    for letter in groups:
        groups[letter].sort()
    return groups

def display_90_grid(drawn):
    rows = []
    all_numbers = list(range(1, 91))
    per_row = 18
    for i in range(0, 90, per_row):
        row = []
        for n in all_numbers[i:i+per_row]:
            if n in drawn:
                row.append(f"{Fore.GREEN}[{n:2}]{Style.RESET_ALL}")
            else:
                row.append(f" {n:2} ")
        rows.append(' '.join(row))
    print("\nDrawn numbers grid:\n")
    for row in rows:
        print(row)

def display_status(drawn, total_numbers, current_draw_limit, next_number, last_numbers_to_show, paused, delay):
    clear_console()
    draw_count = len(drawn)
    remaining = total_numbers - draw_count

    print("="*60)
    print(f"🎉 Bingo Caller 🎉")
    print(f"Draw: {draw_count} of {current_draw_limit}")
    print(f"Numbers left: {remaining}")
    print(f"Delay: {delay} sec")
    print("-"*60)
    print(f"Number drawn: {Fore.GREEN}{next_number}{Style.RESET_ALL}")
    print(f"Last {last_numbers_to_show} numbers: {drawn[-last_numbers_to_show:]}")
    print("-"*60)

    if total_numbers == 75:
        grouped = group_bingo_numbers(drawn)
        for letter in ['B', 'I', 'N', 'G', 'O']:
            nums = ', '.join(f"{Fore.YELLOW}{n}{Style.RESET_ALL}" for n in grouped[letter])
            print(f"{letter}: {nums}")
    elif total_numbers == 90:
        display_90_grid(drawn)

    print("-"*60)
    if paused:
        print("⏸️  PAUSED - Press 'p' to resume")
    else:
        print("Hotkeys: p=pause | =/+=slower | -=faster | h=history")
    print("="*60)

def main():
    pygame.mixer.init()
    total_numbers, max_draws, language, auto_draw, delay, play_digits = load_settings()

    numbers = list(range(1, total_numbers + 1))
    random.shuffle(numbers)

    drawn = []
    last_numbers_to_show = 5

    paused_flag = [False]
    just_resumed = [False]
    delay_changed = [False]
    show_history_flag = [False]
    delay_value = [delay]

    with open("bingo_draw_history.txt", "w") as f:
        f.write("Drawn numbers:\n")

    def on_press(key):
        try:
            if key.char == 'p':
                if not show_history_flag[0]:
                    if paused_flag[0]:
                        just_resumed[0] = True
                    paused_flag[0] = not paused_flag[0]
            elif key.char == '=' or key.char == '+':
                delay_value[0] += 1
                delay_changed[0] = True
            elif key.char == '-':
                delay_value[0] = max(1, delay_value[0] - 1)
                delay_changed[0] = True
            elif key.char == 'h':
                show_history_flag[0] = True
        except AttributeError:
            pass

    listener = keyboard.Listener(on_press=on_press)
    listener.start()

    current_draw_limit = max_draws

    while True:
        if show_history_flag[0]:
            print("\n🔎 Drawn so far:\n", drawn)
            input("\nPress Enter to continue...")
            show_history_flag[0] = False
            continue

        if len(drawn) >= current_draw_limit:
            if len(numbers) == 0:
                print("\n✅ All numbers drawn! Game over.")
                break
            answer = input("\nMax draws reached. Continue? (y/n): ").strip().lower()
            if answer == 'y':
                additional = int(input(f"How many more? (remaining: {len(numbers)}): "))
                if additional > len(numbers):
                    additional = len(numbers)
                current_draw_limit += additional
                continue
            else:
                break

        if auto_draw:
            if paused_flag[0]:
                if drawn:
                    display_status(drawn, total_numbers, current_draw_limit, drawn[-1], last_numbers_to_show, paused_flag[0], delay_value[0])
                else:
                    display_status(drawn, total_numbers, current_draw_limit, '-', last_numbers_to_show, paused_flag[0], delay_value[0])
                time.sleep(0.2)
                continue
            elif just_resumed[0]:
                just_resumed[0] = False
            elif delay_changed[0]:
                delay_changed[0] = False
            else:
                sleeped = 0
                while sleeped < delay_value[0]:
                    if paused_flag[0] or delay_changed[0] or show_history_flag[0]:
                        break
                    time.sleep(0.1)
                    sleeped += 0.1
                if paused_flag[0] or delay_changed[0] or show_history_flag[0]:
                    delay_changed[0] = False
                    continue
        else:
            cmd = input("Press Enter to draw next (or 'q' to quit): ")
            if cmd.lower() == 'q':
                break

        if paused_flag[0] or show_history_flag[0]:
            continue

        next_number = numbers.pop()
        drawn.append(next_number)

        with open("bingo_draw_history.txt", "a") as f:
            f.write(f"{next_number}\n")

        display_status(drawn, total_numbers, current_draw_limit, next_number, last_numbers_to_show, paused_flag[0], delay_value[0])
        play_audio(next_number, language, play_digits, total_numbers)

    print("\n✅ Game over.")
    print("Numbers drawn:", drawn)
    listener.stop()

if __name__ == "__main__":
    main()
