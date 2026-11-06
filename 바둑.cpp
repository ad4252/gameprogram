#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define BOARD_SIZE 15   // 바둑판 크기 (15x15)
#define TIME_LIMIT 30   // 턴당 제한 시간 (30초)

// 게임판 배열 (0: 빈칸, 1: 플레이어(흑), 2: AI(백))
int board[BOARD_SIZE][BOARD_SIZE] = {0}; 
int cursor_x = BOARD_SIZE / 2; 
int cursor_y = BOARD_SIZE / 2; 
int current_turn = 1; // 1: 흑(플레이어), 2: 백(컴퓨터 AI)
int game_over = 0;
int winner = 0;

// 함수 원형 선언
void display_board(int remain_time);
void play_game();
int check_win(int y, int x, int color);
void ai_turn();
int evaluate_line_score(int y, int x, int color);
// 화면 깜빡임 방지를 위해 커서를 좌측 상단(0,0)으로 이동시키는 함수
void move_cursor_top_left() {
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int main(void)
{
    // 난수 초기화 (AI의 무작위성을 위함)
    srand((unsigned int)time(NULL));

    system("cls");
    printf("=== 인공지능 오목 대전 (제한시간 %d초) ===\n\n", TIME_LIMIT);
    printf("조작 방법:\n");
    printf(" - 이동 : W, A, S, D 키\n");
    printf(" - 착수 : SPACE (스페이스바)\n");
    printf(" - 종료 : ESC\n\n");
    printf("규칙:\n");
    printf(" - 플레이어는 흑(●), 컴퓨터 AI는 백(○)입니다.\n");
    printf(" - 제한시간 내에 돌을 두지 않으면 패배합니다.\n\n");
    printf("아무 키나 누르면 게임이 시작됩니다...");
    
    getch(); 
    play_game(); 

    // 게임 종료 결과 출력
    system("cls");
    if (winner == 1) {
        printf("\n\n축하합니다! 플레이어(흑)의 승리입니다! ??\n");
    } else if (winner == 2) {
        printf("\n\n아쉽습니다. 컴퓨터 AI(백)의 승리입니다. ??\n");
    } else {
        printf("\n\n게임이 강제 종료되었습니다.\n");
    }
    printf("아무 키나 누르면 프로그램을 종료합니다...\n");
    getch();

    return 0;
}

void play_game()
{
    time_t turn_start_time = time(NULL); 
    int last_remain_time = TIME_LIMIT;
    
    display_board(TIME_LIMIT); 

    while (!game_over)
    {
        // 1. 시간 초과 체크 및 타이머 갱신
        time_t current_time = time(NULL);
        int elapsed = (int)difftime(current_time, turn_start_time);
        int remain_time = TIME_LIMIT - elapsed;

        // 시간 초과 시 패배 처리
        if (remain_time <= 0) {
            game_over = 1;
            winner = (current_turn == 1) ? 2 : 1; 
            break;
        }

        // 1초가 지날 때마다 화면 갱신
        if (remain_time != last_remain_time) {
            display_board(remain_time);
            last_remain_time = remain_time;
        }

        // 2. 컴퓨터(AI) 턴 처리
        if (current_turn == 2) {
            ai_turn();
            if (game_over) break;

            current_turn = 1;
            turn_start_time = time(NULL); // 시간 초기화
            last_remain_time = TIME_LIMIT;
            display_board(TIME_LIMIT);
            continue; // AI가 두면 바로 다음 루프로
        }

        // 3. 플레이어(흑) 키보드 입력 처리
        if (kbhit()) {
            char key = getch();
            int is_moved = 0; 

            if (key == 27) break; // ESC 종료

            switch (key) {
                case 'w': case 'W': 
                    if (cursor_y > 0) cursor_y--; is_moved = 1; break;
                case 's': case 'S': 
                    if (cursor_y < BOARD_SIZE - 1) cursor_y++; is_moved = 1; break;
                case 'a': case 'A': 
                    if (cursor_x > 0) cursor_x--; is_moved = 1; break;
                case 'd': case 'D': 
                    if (cursor_x < BOARD_SIZE - 1) cursor_x++; is_moved = 1; break;
                
                case 32: // 스페이스바 (착수)
                    if (board[cursor_y][cursor_x] == 0) {
                        board[cursor_y][cursor_x] = 1; // 흑돌 놓기
                        
                        // 승리 체크
                        if (check_win(cursor_y, cursor_x, 1)) {
                            game_over = 1;
                            winner = 1;
                            break;
                        }
                        
                        current_turn = 2; // AI로 턴 넘기기
                        turn_start_time = time(NULL); 
                        last_remain_time = TIME_LIMIT;
                        is_moved = 1;
                    }
                    break;
            }

            if (is_moved && !game_over) {
                display_board(last_remain_time);
            }
        }
    }
}

// 5목 승리 판정 함수 (가로, 세로, 대각선 검사)
int check_win(int y, int x, int color)
{
    // 탐색할 4방향 (가로, 세로, 우하대각선, 우상대각선)
    int dy[] = {0, 1, 1, -1};
    int dx[] = {1, 0, 1, 1};

    for (int i = 0; i < 4; i++) {
        int count = 1; // 방금 놓은 돌 1개 포함
        
        // 정방향 탐색
        int ny = y + dy[i], nx = x + dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) {
            count++; ny += dy[i]; nx += dx[i];
        }
        
        // 역방향 탐색
        ny = y - dy[i], nx = x - dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) {
            count++; ny -= dy[i]; nx -= dx[i];
        }

        // 5개가 연속되면 승리
        if (count >= 5) return 1;
    }
    return 0;
}

// 오목 AI (가장 점수가 높은 위치를 찾아 돌을 놓음)
void ai_turn()
{
    int best_score = -1;
    int best_y = -1;
    int best_x = -1;

    // 모든 빈칸을 탐색하여 최적의 위치 점수 계산
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == 0) {
                // 공격 점수(자신이 두었을 때) + 방어 점수(상대가 두었을 때)
                int score_attack = evaluate_line_score(y, x, 2);
                int score_defense = evaluate_line_score(y, x, 1);
                
                int total_score = score_attack + score_defense;
                
                // 똑같은 점수일 경우 가끔 다른 위치에 두도록 랜덤성 추가
                total_score += rand() % 5; 

                if (total_score > best_score) {
                    best_score = total_score;
                    best_y = y;
                    best_x = x;
                }
            }
        }
    }

    // 최적의 위치에 백돌(2) 착수
    if (best_y != -1 && best_x != -1) {
        board[best_y][best_x] = 2;
        
        if (check_win(best_y, best_x, 2)) {
            game_over = 1;
            winner = 2;
        }
    }
}

// AI의 두뇌 역할 (특정 위치에 두었을 때의 가치 점수 계산)
int evaluate_line_score(int y, int x, int color)
{
    int score = 0;
    int dy[] = {0, 1, 1, -1};
    int dx[] = {1, 0, 1, 1};

    for (int i = 0; i < 4; i++) {
        int count = 1;
        int open_ends = 0;

        // 정방향
        int ny = y + dy[i], nx = x + dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) {
            count++; ny += dy[i]; nx += dx[i];
        }
        if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == 0) open_ends++;

        // 역방향
        ny = y - dy[i], nx = x - dx[i];
        while (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == color) {
            count++; ny -= dy[i]; nx -= dx[i];
        }
        if (ny >= 0 && ny < BOARD_SIZE && nx >= 0 && nx < BOARD_SIZE && board[ny][nx] == 0) open_ends++;

        // 연속된 돌 개수와 열린 공간에 따른 점수 부여
        if (count >= 5) score += 100000;              // 승리 확정 (5목)
        else if (count == 4 && open_ends == 2) score += 10000; // 양쪽 뚫린 4목 (필승)
        else if (count == 4 && open_ends == 1) score += 1000;  // 한쪽 막힌 4목
        else if (count == 3 && open_ends == 2) score += 1000;  // 양쪽 뚫린 3목
        else if (count == 3 && open_ends == 1) score += 100;   // 한쪽 막힌 3목
        else if (count == 2 && open_ends == 2) score += 100;   // 양쪽 뚫린 2목
    }
    return score;
}

// 화면 출력 함수 (남은 시간 표시)
void display_board(int remain_time)
{
    // system("cls"); // <--- 기존에 화면을 지우던 이 코드를 삭제(또는 주석 처리)합니다.
    move_cursor_top_left(); // <--- 대신 커서를 맨 위로 올립니다!
    
    printf("=== 실시간 인공지능 오목 ===                          \n");
    printf("턴당 제한 시간: %2d초 남음  [ %-15s ]       \n", remain_time, current_turn == 1 ? "당신(●)의 차례" : "AI(○) 생각중...");
    printf("============================                          \n");

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            
            // 플레이어 차례일 때만 커서 표시
            if (current_turn == 1 && x == cursor_x && y == cursor_y) {
                printf("▣"); 
            } 
            else if (board[y][x] == 1) printf("●"); // 플레이어
            else if (board[y][x] == 2) printf("○"); // AI
            else printf("┼");                       // 빈칸
        }
        printf("\n");
    }
    printf("\n(W/A/S/D: 이동, SPACE: 착수, ESC: 종료)               \n");
}
