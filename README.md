# bingo-caller<br />
bingo caller<br />
This a little experiment of mine to see how chatgpt could help with building a bingo caller program in python.<br />
I wanted a small console bingo caller app, for 75 and 90 ball bingo, that will play audio mp3 files that I had to get from ttsfree.com. I needed bulgarian audio for my local village community, playing for fun.<br />
On the ttsfree.com I did a list for all digits 1 to 90 every digit on new line. After saving the AI voice recording I cut it in Audacity each digit in separate file 1.mp3, 2.mp3....90.mp3<br />
Similar for the separate digits. There I put the digits from 10 to 90 in a list but each digit separated by a space.<br />
1 0<br />
1 1<br />
...<br />
Save all digits and select each digit in audacity and export it in the folder audio/bg_digits/<br />
<br />
The audio files have to be in folder audio/ same directory as the python source file.<br />
Folder structure is as follows:<br />
..<br />
bingo-caller.py<br />
audio/<br />
audio/bg/ < here are the normal audio files 1 to 90 and the spoken letters(B, I, N, G, O).mp3 files<br />
audio/bg_digits/ < here go the digits recordings 1 to 90 and intro.mp3 file, saying "Number" that gets played only for <br />digits 1 to 9<br />
Please check the python source for needed python libraries that need to be installed so it can work.<br />
The same is valid for the C++ app. Same structure for the audio files.<br />
Feel free to copy and expand it. It's free and for fun. Enjoy<br />
<br />
