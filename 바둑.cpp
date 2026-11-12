#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <math.h> 

#define BOARD_SIZE 15
#define TIME_LIMIT 30

// ==========================================
// 1. 전역 변수 설정
// ==========================================
int board[BOARD_SIZE][BOARD_SIZE] = {0}; 
int cursor_x = BOARD_SIZE / 2; 
int cursor_y = BOARD_SIZE / 2; 
int current_turn = 1;
int game_over = 0;
int winner = 0;
int sound_enabled = 1;
int volume_level = 70;  
int in_volume_menu = 0; 

int player_destroy_count = 3; 
int player_block_count = 3;   
int ai_turn_blocked = 0;      

// ==========================================
// 2. 함수 원형 선언
// ==========================================
void hide_cursor(); // ★ 추가: 깜빡임 방지용 커서 숨기기
void display_board(int remain_time);
void play_game();
int check_win(int y, int x, int color);
void ai_turn();
int evaluate_line_score(int y, int x, int color);
void reset_game();
void move_cursor_top_left();
void set_color(int color_code);
void play_sound(int type);
void display_volume_menu();
int display_coin_toss_ui();

// ==========================================
// 3. 메인 함수
// ==========================================
int main(void)
{
    srand((unsigned int)time(NULL));
    hide_cursor(); // ★ 게임 시작 시 커서를 숨겨 깜빡임을 제거합니다.

    while (1) 
    {
        reset_game();
        system("cls");
        
        // ★ 세로 줄이 넘쳐 스크롤되지 않도록 여백 최적화
        set_color(14);
        printf("\n");
        printf("    ==============================================================\n");
        printf("       ____   ___  __  __  ___  _  __ _   _ \n");
        printf("      / ___| / _ \\|  \\/  |/ _ \\| |/ /| | | |\n");
        printf("     | |  _ | | | | |\\/| | | | | ' / | | | |\n");
        printf("     | |_| || |_| | |  | | |_| | . \\ | |_| |\n");
        printf("      \\____| \\___/|_|  |_|\\___/|_|\\_\\ \\___/ \n\n");
        printf("                   [ 인공지능 오목 대전 ]                   \n");
        printf("    ==============================================================\n\n");
        
        set_color(15);
        printf("      [ ★ 조작 방법 ]\n");
        printf("      ▶ 이  동 : [W] [A] [S] [D] 방향키\n");
        printf("      ▶ 착  수 : [SPACE BAR]\n");
        printf("      ▶ 파  괴 : [X] 상대방 돌 부수기 (게임당 3회)\n"); 
        printf("      ▶ 턴봉쇄 : [C] 코인토스로 AI 턴 뺏기 (50%% 확률, 3회)\n"); 
        printf("      ▶ 소  리 : [M] 켜기/끄기, [V] 음량 조절\n");
        printf("      ▶ 종  료 : [ESC]\n\n");
        
        printf("      [ ※ 게임 규칙 ]\n");
        printf("      ▶ 당신은 하늘색(●), 인공지능은 빨간색(○) 입니다.\n");
        printf("      ▶ 5개의 돌을 먼저 가로, 세로, 대각선으로 이으면 승리!\n\n");
        
        set_color(10);
        printf("    ==============================================================\n");
        printf("             >>> 아무 키나 누르면 게임을 시작합니다 <<<          \n");
        printf("    ==============================================================\n");
        set_color(7);
        
        getch();     
        play_game();

        system("cls");
        set_color(14);
        printf("\n\n\n");
        printf("    ==============================================================\n");
        printf("    ||                                                          ||\n");
        printf("    ||                   [ G A M E  O V E R ]                   ||\n");
        printf("    ||                                                          ||\n");
        
        set_color(15);
        if (winner == 1) {
            printf("    ||             ★ 축하합니다! 당신의 승리입니다! ★         ||\n");
            play_sound(2); play_sound(2);
        } else if (winner == 2) {
            printf("    ||              ※ 아쉽습니다. AI의 승리입니다. ※          ||\n");
            play_sound(3); play_sound(3);
        } else {
            printf("    ||                    무승부 / 강제 종료                    ||\n");
        }
        
        set_color(14);
        printf("    ||                                                          ||\n");
        printf("    ==============================================================\n");
        
        set_color(15);
        printf("\n\n");
        printf("                        다시 한 판 하시겠습니까?\n\n");
        printf("                      [ Y ] 예       [ N ] 아니오\n");

        int is_quit = 0;
        while (1) {
            char choice = getch();
            if (choice == 'y' || choice == 'Y') {
                break;
            } else if (choice == 'n' || choice == 'N') {
                is_quit = 1; 
                break;
            }
        }

        if (is_quit) break;
    } 

    system("cls");
    set_color(15);
    printf("\n\n    게임을 종료합니다. 플레이해주셔서 감사합니다!\n\n");
    set_color(7);
    
    return 0;
}

// ==========================================
// 4. 게임 진행 및 입력 처리 함수
// ==========================================
void play_game()
{
    system("cls"); 

    time_t turn_start_time = time(NULL); 
    int last_remain_time = TIME_LIMIT;
    
    display_board(TIME_LIMIT); 

    while (!game_over)
    {
        time_t current_time = time(NULL);
        int elapsed = (int)difftime(current_time, turn_start_time);
        int remain_time = TIME_LIMIT - elapsed;

        if (remain_time <= 0) {
            game_over = 1;
            winner = (current_turn == 1) ? 2 : 1; 
            break;
        }

        if (remain_time != last_remain_time) {
            display_board(remain_time);
            last_remain_time = remain_time;
        }

        if (current_turn == 2) {
            ai_turn();
            play_sound(2);
            if (game_over) break;

            current_turn = 1;
            turn_start_time = time(NULL); 
            last_remain_time = TIME_LIMIT;
            display_board(TIME_LIMIT);
            continue; 
        }

        if (kbhit()) {
            char key = getch();
            int is_moved = 0; 

            if (key == 27) break;

            switch (key) {
                case 'c': case 'C':
                    if (player_block_count > 0 && ai_turn_blocked == 0) {
                        player_block_count--;
                        int toss = display_coin_toss_ui();
                        if (toss == 1) ai_turn_blocked = 1; 
                        else current_turn = 2;    
                        
                        turn_start_time = time(NULL); 
                        last_remain_time = TIME_LIMIT;
                        system("cls"); 
                        is_moved = 1;
                    } else {
                        play_sound(3); 
                    }
                    break;

                case 'x': case 'X':
                    if (board[cursor_y][cursor_x] == 2 && player_destroy_count > 0) {
                        board[cursor_y][cursor_x] = 0; 
                        player_destroy_count--;        
                        play_sound(4);                 
                        if (ai_turn_blocked == 1) {
                            ai_turn_blocked = 0; current_turn = 1;    
                        } else {
                            current_turn = 2;    
                        }
                        turn_start_time = time(NULL); 
                        last_remain_time = TIME_LIMIT;
                        is_moved = 1;
                    } else { play_sound(3); }
                    break;

                case 'm': case 'M':
                    sound_enabled = !sound_enabled;
                    is_moved = 1; break;
                    
                case 'v': case 'V':
                {
                    time_t p_start = time(NULL); display_volume_menu();           
                    time_t p_end = time(NULL); turn_start_time += (p_end - p_start); 
                    system("cls"); is_moved = 1; break;
                }
                    
                case 'w': case 'W': 
                    if (cursor_y > 0) { cursor_y--; play_sound(1); } is_moved = 1; break;
                case 's': case 'S': 
                    if (cursor_y < BOARD_SIZE - 1) { cursor_y++; play_sound(1); } is_moved = 1; break;
                case 'a': case 'A': 
                    if (cursor_x > 0) { cursor_x--; play_sound(1); } is_moved = 1; break;
                case 'd': case 'D': 
                    if (cursor_x < BOARD_SIZE - 1) { cursor_x++; play_sound(1); } is_moved = 1; break;
                
                case 32: // SPACE
                    if (board[cursor_y][cursor_x] == 0) {
                        board[cursor_y][cursor_x] = 1; play_sound(2);
                        if (check_win(cursor_y, cursor_x, 1)) { game_over = 1; winner = 1; break; }
                        
                        if (ai_turn_blocked == 1) {
                            ai_turn_blocked = 0; current_turn = 1;    
                        } else {
                            current_turn = 2;    
                        }
                        turn_start_time = time(NULL); last_remain_time = TIME_LIMIT;
                        is_moved = 1;
                    } else { play_sound(3); }
                    break;
            }

            if (is_moved && !game_over) {
                display_board(last_remain_time);
            }
        }
    }
}

// ==========================================
// 5. 화면 출력 (★ 화면 깨짐 방지용 완벽 규격화)
// ==========================================
void display_board(int remain_time)
{
    move_cursor_top_left(); 
    
    // 콘솔 기본 25줄 높이를 넘지 않게 딱 24줄로 맞췄습니다!
    set_color(14);
    printf("    ==============================================================\n");
    printf("                   [ 실시간 인공지능 오목 대전 ]                  \n");
    printf("    ==============================================================\n");
    
    set_color(15); 
    printf("    ▶ 제한 시간: %2d초  ", remain_time);
    if (ai_turn_blocked) {
        printf("[ AI 턴 봉쇄! 연속 차례! ]             \n");
    } else {
        if (current_turn == 1) printf("[ 당신(●)의 차례 ]                     \n");
        else                   printf("[ AI(○) 생각중... ]                    \n");
    }
    printf("    ▶ 스킬 현황: [파괴: %d] [봉쇄: %d] [볼륨: %3d%%]                \n", 
           player_destroy_count, player_block_count, volume_level);
    
    printf("    --------------------------------------------------------------\n");
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        printf("       "); // 7칸 여백
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (current_turn == 1 && x == cursor_x && y == cursor_y) {
                set_color(10); printf(" ▣ "); 
            } else if (board[y][x] == 1) {
                set_color(11); printf(" ● ");
            } else if (board[y][x] == 2) {
                set_color(12); printf(" ○ ");
            } else {
                set_color(8);  printf(" ┼ ");                       
            }
        }
        printf("        \n"); // 찌꺼기 방지 공백
    }
    
    set_color(15);
    printf("    --------------------------------------------------------------\n");
    printf("    [조작] W/A/S/D:이동 SPACE:착수 X:파괴 C:봉쇄 M/V:소리 ESC:종료\n");
    printf("    ==============================================================\n");
    set_color(7); 
}

// ==========================================
// 6. 특수 UI (볼륨 조절 및 코인 토스)
// ==========================================
void display_volume_menu()
{
    in_volume_menu = 1;
    system("cls"); 
    
    while (in_volume_menu) {
        move_cursor_top_left(); 
        
        set_color(14);
        printf("\n\n\n");
        printf("    ==============================================================\n");
        printf("                     [ 음  량  조  절  메  뉴 ]                   \n");
        printf("    ==============================================================\n\n");
        
        set_color(11); 
        printf("                       설정된 볼륨: [ %3d%% ]                     \n\n", volume_level);
        
        set_color(15);
        printf("          [W] 또는 [↑]: 음량 증가 (+5%%)                        \n");
        printf("          [S] 또는 [↓]: 음량 감소 (-5%%)                        \n");
        printf("          [M]: 소리 켜기/끄기 (현재: %s)                          \n", sound_enabled ? "ON " : "OFF");
        printf("          [ENTER] 또는 [ESC]: 메뉴 나가기                         \n");
        
        set_color(14);
        printf("\n    ==============================================================\n");
        set_color(7);
        
        if (kbhit()) {
            int key = getch();
            if (key == 224 || key == 0) {
                key = getch(); 
                if (key == 72) { volume_level += 5; if (volume_level > 100) volume_level = 100; play_sound(1); }
                else if (key == 80) { volume_level -= 5; if (volume_level < 0) volume_level = 0; play_sound(1); }
            } else { 
                if (key == 27 || key == 13) { in_volume_menu = 0; }
                else if (key == 'w' || key == 'W') { volume_level += 5; if(volume_level>100) volume_level=100; play_sound(1); }
                else if (key == 's' || key == 'S') { volume_level -= 5; if(volume_level<0) volume_level=0; play_sound(1); }
                else if (key == 'm' || key == 'M') { sound_enabled = !sound_enabled; play_sound(1); }
            }
        }
    }
}

int display_coin_toss_ui()
{
    system("cls");
    
    set_color(14);
    printf("\n\n\n");
    printf("    ==============================================================\n");
    printf("                       [ 운명의 코인 토스 ]                       \n");
    printf("    ==============================================================\n\n");
    
    set_color(15);
    printf("               앞면이 나오면 AI의 턴을 봉쇄합니다!\n");
    printf("               뒷면이 나오면 당신의 턴이 날아갑니다...\n\n");
    
    set_color(11);
    printf("               (아무 키나 누르면 동전을 던집니다)                 \n");
    
    set_color(14);
    printf("\n    ==============================================================\n");
    set_color(7);
    
    getch(); 

    const char* frames[] = { " ( - ) ", " ( \\ ) ", " ( | ) ", " ( / ) " };
    for (int i = 0; i < 15; i++) {
        move_cursor_top_left();
        set_color(14);
        printf("\n\n\n    ==============================================================\n");
        printf("                       [ 운명의 코인 토스 ]                       \n");
        printf("    ==============================================================\n\n");
        
        set_color(15);
        printf("                          동전 던지는 중...                       \n");
        printf("                                                                  \n");
        set_color(10);
        printf("                             %s                                   \n", frames[i % 4]);
        printf("                                                                  \n");
        
        set_color(14);
        printf("\n    ==============================================================\n");
        
        play_sound(1); 
    }

    int toss = rand() % 2;
    
    move_cursor_top_left();
    set_color(14);
    printf("\n\n\n    ==============================================================\n");
    printf("                       [ 운명의 코인 토스 ]                       \n");
    printf("    ==============================================================\n\n");
    
    if (toss == 1) {
        set_color(10);
        printf("                         결과 : [ 앞 면 ]                         \n");
        printf("                  대성공! AI의 턴이 봉쇄되었습니다!!              \n\n");
        play_sound(5); 
    } else {
        set_color(12);
        printf("                         결과 : [ 뒷 면 ]                         \n");
        printf("                   대실패... 턴이 강제로 넘어갑니다.              \n\n");
        play_sound(6); 
    }
    
    set_color(15);
    printf("               (아무 키나 누르면 게임으로 돌아갑니다)             \n");
    
    set_color(14);
    printf("\n    ==============================================================\n");
    set_color(7);
    
    getch(); 
    return toss; 
}

// ==========================================
// 7. 사운드 시스템
// ==========================================
void play_sound(int type) {
    if (sound_enabled == 0 || volume_level <= 0) return;
    
    int frequency = 800; int duration = 50;
    switch (type) {
        case 1: frequency = 800; duration = 50; break;     
        case 2: frequency = 1200; duration = 100; break;   
        case 3: frequency = 500; duration = 150; break;    
        case 4: frequency = 150; duration = 250; break;    
        case 5: frequency = 1800; duration = 300; break;   
        case 6: frequency = 250; duration = 400; break;    
    }
    
    typedef UINT (WINAPI *FN_waveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);
    typedef UINT (WINAPI *FN_waveOutPrepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);
    typedef UINT (WINAPI *FN_waveOutWrite)(HWAVEOUT, LPWAVEHDR, UINT);
    typedef UINT (WINAPI *FN_waveOutUnprepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);
    typedef UINT (WINAPI *FN_waveOutClose)(HWAVEOUT);

    static HMODULE hWinMM = NULL;
    static FN_waveOutOpen p_waveOutOpen = NULL;
    static FN_waveOutPrepareHeader p_waveOutPrepareHeader = NULL;
    static FN_waveOutWrite p_waveOutWrite = NULL;
    static FN_waveOutUnprepareHeader p_waveOutUnprepareHeader = NULL;
    static FN_waveOutClose p_waveOutClose = NULL;

    if (!hWinMM) {
        hWinMM = LoadLibraryA("winmm.dll");
        if (hWinMM) {
            p_waveOutOpen = (FN_waveOutOpen)GetProcAddress(hWinMM, "waveOutOpen");
            p_waveOutPrepareHeader = (FN_waveOutPrepareHeader)GetProcAddress(hWinMM, "waveOutPrepareHeader");
            p_waveOutWrite = (FN_waveOutWrite)GetProcAddress(hWinMM, "waveOutWrite");
            p_waveOutUnprepareHeader = (FN_waveOutUnprepareHeader)GetProcAddress(hWinMM, "waveOutUnprepareHeader");
            p_waveOutClose = (FN_waveOutClose)GetProcAddress(hWinMM, "waveOutClose");
        }
    }
    if (!hWinMM || !p_waveOutWrite) { Beep(frequency, duration); return; }

    int sampleRate = 44100;
    int amplitude = (32767 * volume_level) / 100;
    int samples = (sampleRate * duration) / 1000;

    WAVEFORMATEX wfx = {1, 1, (DWORD)sampleRate, (DWORD)(sampleRate * 2), 2, 16, 0};
    HWAVEOUT hWaveOut;
    if (p_waveOutOpen(&hWaveOut, (UINT)-1, &wfx, 0, 0, 0) != 0) return;

    short* buffer = (short*)malloc(samples * sizeof(short));
    for (int i = 0; i < samples; i++) {
        double t = (double)i / sampleRate;
        buffer[i] = (short)(amplitude * sin(2.0 * 3.14159265358979323846 * frequency * t));
    }

    WAVEHDR header = { (LPSTR)buffer, (DWORD)(samples * sizeof(short)), 0, 0, 0, 0, 0, 0 };
    p_waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    p_waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

    Sleep(duration + 10); 

    p_waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    p_waveOutClose(hWaveOut); free(buffer);
}

// ==========================================
// 8. 기타 유틸리티 및 인공지능
// ==========================================
// ★ 깜빡임 방지용 커서 숨기기 함수
void hide_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = 0; // 커서 숨김
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void reset_game() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) { board[y][x] = 0; }
    }
    cursor_x = BOARD_SIZE / 2; cursor_y = BOARD_SIZE / 2;
    current_turn = 1; game_over = 0; winner = 0;                
    player_destroy_count = 3; player_block_count = 3; ai_turn_blocked = 0;
}

void move_cursor_top_left() {
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void set_color(int color_code) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_code);
}

int check_win(int y, int x, int color) {
    int dy[] = {0, 1, 1, -1}, dx[] = {1, 0, 1, 1};
    for (int i = 0; i < 4; i++) {
        int count = 1, ny = y + dy[i], nx = x + dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) {
            count++; ny += dy[i]; nx += dx[i];
        }
        ny = y - dy[i], nx = x - dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) {
            count++; ny -= dy[i]; nx -= dx[i];
        }
        if (count >= 5) return 1;
    }
    return 0;
}

void ai_turn() {
    int best_score = -1, best_y = -1, best_x = -1;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == 0) {
                int total_score = evaluate_line_score(y, x, 2) + evaluate_line_score(y, x, 1) + rand() % 5; 
                if (total_score > best_score) { best_score = total_score; best_y = y; best_x = x; }
            }
        }
    }
    if (best_y != -1 && best_x != -1) {
        board[best_y][best_x] = 2;
        if (check_win(best_y, best_x, 2)) { game_over = 1; winner = 2; }
    }
}

int evaluate_line_score(int y, int x, int color) {
    int score = 0; int dy[] = {0, 1, 1, -1}, dx[] = {1, 0, 1, 1};
    for (int i = 0; i < 4; i++) {
        int count = 1, open_ends = 0;
        int ny = y + dy[i], nx = x + dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) { count++; ny += dy[i]; nx += dx[i]; }
        if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == 0) open_ends++;

        ny = y - dy[i], nx = x - dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) { count++; ny -= dy[i]; nx -= dx[i]; }
        if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == 0) open_ends++;

        if (count >= 5) score += 100000;              
        else if (count == 4 && open_ends == 2) score += 10000; 
        else if (count == 4 && open_ends == 1) score += 1000;  
        else if (count == 3 && open_ends == 2) score += 1000;  
        else if (count == 3 && open_ends == 1) score += 100;   
        else if (count == 2 && open_ends == 2) score += 100;   
    }
    return score;
}
