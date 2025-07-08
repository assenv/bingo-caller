# bingo-caller
bingo caller
This a little experiment of mine to see how chatgpt could help with building a bingo caller program in python.
I wanted a small console bingo caller app, for 75 and 90 ball bingo, that will play audio mp3 files that I had to get from ttsfree.com. I needed bulgarian audio for my local village community, playing for fun.
On the ttsfree.com I did a list for all digits 1 to 90 every digit on new line. After saving the AI voice recording I cut it in Audacity each digit in separate file 1.mp3, 2.mp3....90.mp3
Similar for the separate digits. There I put the digits from 10 to 90 in a list but each digit separated by a space.
1 0
1 1
...
Save all digits and select each digit in audacity and export it in the folder audio/bg_digits/

The audio files have to be in folder audio/ same directory as the python source file.
Folder structure is as follows:
..
bingo-caller.py
audio/
audio/bg/ < here are the normal audio files 1 to 90 and the spoken letters(B, I, N, G, O).mp3 files
audio/bg_digits/ < here go the digits recordings 1 to 90 and intro.mp3 file, saying "Number" that gets played only for digits 1 to 9
Please check the python source for needed python libraries that need to be installed so it can work.
Feel free to copy and expand it. It's free and for fun.
