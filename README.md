# 🎉 Bingo Caller

**Bingo Caller** is a small experiment to see how far ChatGPT could help build a Bingo caller program in **Python** and **C++**.

This is a simple **console-based** Bingo caller app for **75-ball** and **90-ball** Bingo.
It plays pre-recorded MP3 audio files — I needed **Bulgarian audio** for my local village community to play Bingo for fun!

---

## 📢 How I made the audio

I used [ttsfree.com](https://ttsfree.com) to generate AI voice recordings:

* For the normal numbers, I created a list with all numbers from **1 to 90**, each on a new line.
* After generating the audio, I opened it in **Audacity** and cut each number into separate files:
  `1.mp3`, `2.mp3`, …, `90.mp3`.

For the *separate digits* audio:

* I made a list from **10 to 90**, but separated the digits by a space:

  ```
  1 0
  1 1
  1 2
  ...
  ```
* After generating the file, I split each digit in Audacity and exported them to `audio/bg_digits/`.

---

## 📂 Folder structure

The folder structure must look like this:

```
bingo-caller.py
audio/
 ├── bg/         # Normal number audio (1.mp3 to 90.mp3) + B, I, N, G, O.mp3
 └── bg_digits/  # Digits recordings (1.mp3 to 90.mp3) + intro.mp3 ("Number")
```

⚠️ **Important:** The audio folder must be in the **same directory** as the Python source file.

---

## 🚀 How to run

* Check the Python source for required libraries (`pygame`, `colorama`, `pynput`).
* Install them with:

  ```bash
  pip install pygame colorama pynput
  ```
* Run the script:

  ```bash
  python3 bingo-caller.py
  ```

The **C++ version** uses the **same folder structure** for the audio files.

---

## ✅ License

This project is licensed under the **GNU General Public License (GPL)**.
Feel free to copy, expand, or adapt this project. It’s free and just for fun — enjoy it! 🎉

---

## 🙌 Questions

If you have ideas, improvements, or questions — open an issue or drop me a message!

---

## 🔗 Credits

* Audio: [ttsfree.com](https://ttsfree.com)
* Editing: [Audacity](https://www.audacityteam.org/)

**Have fun playing!** 🥳✨
