#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <csignal>
#include <atomic>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

std::ofstream historyFile;
std::atomic<bool> paused(false);     // ✅ Shared pause flag
std::atomic<int> delaySeconds(5);    // ✅ Shared delay (seconds)
std::atomic<bool> delayChanged(false); // Delay changed flag
// Global audio child PID for signal handler
pid_t audioChildPid = -1;



//play mp3 file with SDL
void playMp3File(const std::string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        std::cerr << "Failed to load: " << path << " | Error: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_PlayMusic(music, 1); // Play once

    while (Mix_PlayingMusic()) {
        SDL_Delay(50);
    }

    Mix_FreeMusic(music);
}



// Play a single file, block until done
//void playMp3Blocking(const std::string& path) {
    //pid_t pid = fork();
    //if (pid == 0) {
        //execlp("mpg321", "mpg321", "-q", path.c_str(), NULL);
        //_exit(1); // Only if execlp fails
    //} else if (pid > 0) {
        //audioChildPid = pid;
        //waitpid(pid, NULL, 0); // Wait for it
        //audioChildPid = -1;
    //} else {
        //std::cerr << "Fork failed!" << std::endl;
    //}
//}

// ✅ Cross-platform clear using ANSI
void clearConsole() {
    std::cout << "\033[H\033[J";  // Move cursor home, clear to end of screen
}

// ✅ Safe exit handler
void signalHandler(int signum) {
    std::cout << "\n\n🚦 Interrupt signal (" << signum << ") received. Cleaning up...\n";
    if (historyFile.is_open()) {
        historyFile.flush();
        historyFile.close();
        std::cout << "✅ History file flushed and closed.\n";
            //clean up SDL
			Mix_CloseAudio();
			SDL_Quit();
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    exit(signum);
}
void playAudio(int number, int totalNumbers, const std::string& language, bool playDigits) {
    // 75-ball letter
    if (totalNumbers == 75) {
        std::string letter;
        if (number >= 1 && number <= 15) letter = "B";
        else if (number >= 16 && number <= 30) letter = "I";
        else if (number >= 31 && number <= 45) letter = "N";
        else if (number >= 46 && number <= 60) letter = "G";
        else if (number >= 61 && number <= 75) letter = "O";

        playMp3File("./audio/" + language + "/" + letter + ".mp3");
    }

    // Main number
    playMp3File("./audio/" + language + "/" + std::to_string(number) + ".mp3");

    // Digits
    if (playDigits) {
        if (number >= 1 && number <= 9) {
            playMp3File("./audio/" + language + "_digits/intro.mp3");
            playMp3File("./audio/" + language + "_digits/" + std::to_string(number) + ".mp3");
        } else {
            playMp3File("./audio/" + language + "_digits/" + std::to_string(number) + ".mp3");
        }
    }
}

//void playAudio(int number, int totalNumbers, const std::string& language) {
    //// 🔵 1. B/I/N/G/O if 75-ball
    //if (totalNumbers == 75) {
        //char letter;
        //if (number >= 1 && number <= 15) letter = 'B';
        //else if (number >= 16 && number <= 30) letter = 'I';
        //else if (number >= 31 && number <= 45) letter = 'N';
        //else if (number >= 46 && number <= 60) letter = 'G';
        //else if (number >= 61 && number <= 75) letter = 'O';

        //std::string letterPath = "./audio/" + language + "/" + letter + ".mp3";
        //playMp3Blocking(letterPath);
    //}

    //if (number >= 1 && number <= 9) {
        //// 🔵 2. Small digit: normal + intro + digit version
        //std::string normalPath = "./audio/" + language + "/" + std::to_string(number) + ".mp3";
        //std::string introPath  = "./audio/" + language + "_digits/intro.mp3";
        //std::string digitPath  = "./audio/" + language + "_digits/" + std::to_string(number) + ".mp3";

        //playMp3Blocking(normalPath);
        //playMp3Blocking(introPath);
        //playMp3Blocking(digitPath);

    //} else {
        //// 🔵 3. Big number: normal + digits version
        //std::string normalPath = "./audio/" + language + "/" + std::to_string(number) + ".mp3";
        //std::string digitPath  = "./audio/" + language + "_digits/" + std::to_string(number) + ".mp3";

        //playMp3Blocking(normalPath);
        //playMp3Blocking(digitPath);
    //}
//}




void display75Ball(const std::vector<int>& drawn) {
    std::vector<int> B, I, N, G, O;

    for (int n : drawn) {
        if (n >= 1 && n <= 15) B.push_back(n);
        else if (n >= 16 && n <= 30) I.push_back(n);
        else if (n >= 31 && n <= 45) N.push_back(n);
        else if (n >= 46 && n <= 60) G.push_back(n);
        else if (n >= 61 && n <= 75) O.push_back(n);
    }

    std::sort(B.begin(), B.end());
    std::sort(I.begin(), I.end());
    std::sort(N.begin(), N.end());
    std::sort(G.begin(), G.end());
    std::sort(O.begin(), O.end());

    auto printGroup = [](char letter, const std::vector<int>& nums) {
        std::cout << letter << ": ";
        if (nums.empty()) {
        std::cout << "-";
	} else {
        for (size_t i = 0; i < nums.size(); ++i) {
            std::cout << nums[i];
            if (i != nums.size() - 1) std::cout << ", ";
		}
	}
        std::cout << "\n";
    };

    std::cout << "\nDrawn Numbers (Grouped):\n";
    std::cout << "-------------------------\n";
    printGroup('B', B);
    printGroup('I', I);
    printGroup('N', N);
    printGroup('G', G);
    printGroup('O', O);
}


void display90Ball(const std::vector<int>& drawn) {
    std::cout << "\nNumbers:\n";
    for (int i = 1; i <= 90; ++i) {
        bool isDrawn = std::find(drawn.begin(), drawn.end(), i) != drawn.end();
        if (isDrawn) {
            printf("\033[32m[%2d]\033[0m", i);  // highlight drawn
        } else {
            printf(" %2d ", i);
        }

        if (i % 18 == 0) std::cout << "\n";  // 18 per row => 5 rows
    }
}

// ✅ Display status
void displayStatus(const std::vector<int>& drawn, int totalNumbers, int currentNumber) {
    // ✅ THEN play audio
    
    clearConsole();
    std::cout << "==============================" << std::endl;
    std::cout << "🎉 Bingo Caller 🎉" << std::endl;
    std::cout << "Numbers drawn: " << drawn.size() << " / " << totalNumbers << std::endl;
    std::cout << "Current number: \033[32m" << currentNumber << "\033[0m" << std::endl;

    std::cout << "Last numbers: ";
    int count = 0;
    for (auto it = drawn.rbegin(); it != drawn.rend() && count < 5; ++it, ++count) {
        std::cout << *it << " ";
    }
    
    if (totalNumbers == 75) {
        display75Ball(drawn);
    } else if (totalNumbers == 90) {
        display90Ball(drawn);
    }
        
    
    std::cout << std::endl;

    std::cout << (paused ? "⏸️  PAUSED - Press 'p' to resume"
                         : "▶️  RUNNING - Press 'p' to pause") << std::endl;

    std::cout << "Delay: " << delaySeconds.load() << " sec "
              << "( [ = faster, ] = slower )" << std::endl;

    std::cout << "==============================" << std::endl;
    
}



// ✅ Linux: non-blocking getchar
#ifndef _WIN32
int kbhit() {
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
#endif

// ✅ Keyboard listener thread
void keyListener() {
    while (true) {
#ifdef _WIN32
        if (_kbhit()) {
            char ch = _getch();
#else
        if (kbhit()) {
            char ch = getchar();
#endif
            if (ch == 'p' || ch == 'P') {
                paused = !paused;  // Toggle pause flag
            } else if (ch == '[') {
                // Decrease delay, minimum 1 second
                if (delaySeconds > 1) delaySeconds--;
                delayChanged = true;
            } else if (ch == ']') {
                // Increase delay, no upper limit
                delaySeconds++;
                delayChanged = true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    signal(SIGINT, signalHandler);

// SDL init

if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
}

if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
    return 1;
}




    int totalNumbers = 75;

    std::cout << "Total numbers (75 or 90): ";
    std::cin >> totalNumbers;
    if (totalNumbers != 75 && totalNumbers != 90) {
        std::cout << "Only 75 or 90 supported.\n";
        return 1;
    }

    int startDelay = 5;
    std::cout << "Delay between draws (seconds): ";
    std::cin >> startDelay;

    delaySeconds = startDelay;  // ✅ Set initial delay

    std::vector<int> numbers;
    for (int i = 1; i <= totalNumbers; ++i) {
        numbers.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g);

    std::vector<int> drawn;
    historyFile.open("bingo_history.txt");
    

    // ✅ Start keyboard listener
    std::thread listenerThread(keyListener);
	bool shownPaused = false;
	
    for (int n : numbers) {
        while (paused) {
            if (!shownPaused) {
        displayStatus(drawn, totalNumbers, n);
        shownPaused = true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
// when unpaused:
shownPaused = false;

        drawn.push_back(n);
        displayStatus(drawn, totalNumbers, n);
        historyFile << n << "\n";
        historyFile.flush();
		playAudio(n, totalNumbers, "bg", true);
			
		// ✅ Responsive sleep
		int slept = 0;
		int totalDelay = delaySeconds.load() * 1000;  // ms
		while (slept < totalDelay) {
			if (paused) break;  // break sleep early if paused
			if (delayChanged) {
					delayChanged = false;  // reset flag
					break;  // cut sleep, next loop will re-calc delay
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			slept += 100;
			
		}
    }

    historyFile.close();
    std::cout << "✅ All numbers drawn! Game over.\n";

    listenerThread.detach();
    //clean up SDL
    Mix_CloseAudio();
	SDL_Quit();

    return 0;
}
