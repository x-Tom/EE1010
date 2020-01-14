#include <windows.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <string>
#include "letters.h"

//const int WIDTH = 120;
//const int HEIGHT = 90;
const int WIDTH = 256;
const int HEIGHT = 144;
bool GAME_MENU = false;
bool GAME_START = false;
bool GAME_OPTIONS = false;
int ROUND = 1;
//std::fstream FHROUND;
//int HROUND;
bool NEW_ROUND = true;

enum PIXEL {
    FULL_SHADE = 0x2588,
    DARK_SHADE = 0x2593,
    MEDIUM_SHADE = 0x2592,
    LIGHT_SHADE = 0x2591
};

template <typename entity1, typename entity2>
bool entityBounds(entity1* e1, entity2* e2);


struct projectile_t {
    float x;
    float y;
    float angle;
    float vx = 1;
    float vy = 1;
    float size = 1;
    bool exists = true;
    void* creator = nullptr;
    wchar_t pixel = PIXEL::FULL_SHADE;
};

//struct entity_t {
//    float x;
//    float y;
//    float angle;
//    float vx;
//    float vy;
//    float angle_differential;
//    float size;
//    //consoleSprite* sprite;
//    //void (*shoot)(std::vector<projectile_t*>*,float,float,float);
//    void shoot(std::vector<projectile_t*>* projList, projectile_t* (*createProjectile)(float x, float y, float angle)) {
//        projList->push_back(createProjectile(this->x + this->size / 2, this->y + this->size / 2, this->angle));
//    };
//};

//struct player_t : entity_t { // /*C++ allows function definitions inside struct (public by defualt) and also struct inheritance
//    float angle_differential = 0.005;
//    float size = 10;
//    
//};
//
//struct enemy_t : entity_t { 
//    float angle_differential = 0.005;
//    float size = 10;
//
//};


struct player_t {
    float x;
    float y;
    float angle;
    float vx;
    float vy;
    float angle_differential = 0.01;
    float size = 10;

    bool alive = true;
    float hp = 100;
    float dmg = 1;

    int kill_count = 0;

    //consoleSprite* sprite;
    //void (*shoot)(std::vector<projectile_t*>*,float,float,float);
    void shoot(std::vector<projectile_t*>* projList, projectile_t* (*createProjectile)(float x, float y, float angle, void* creator)) {
        projList->push_back(createProjectile(this->x + this->size / 2, this->y + this->size / 2, this->angle, this));
    };

    


};

struct enemy_t {
    float x = 50;
    float y = 50;
    float angle = 0;
    float spdx = 0.1;
    float spdy = 0.1;
    float vx = 0.1;
    float vy = 0.1;
    float angle_differential = 0.005;
    float size = 8;
    bool alive = true;
    float hp = 100;
    float dmg = 0.5;
    

    void melee(player_t* target) {

        if (entityBounds<enemy_t, player_t>(this, target)) target->hp -= this->dmg;
        
    };

    void pCollisions(projectile_t* bullet) {
        if (entityBounds<enemy_t, projectile_t>(this, bullet)) {
            bullet->exists = false;
            this->hp -= ((player_t*)bullet->creator)->dmg;
            if (this->hp <= 0) {
                this->alive = false;
                ((player_t*)bullet->creator)->kill_count++;
            }

        };
    };


};



struct menuComponent_t {
    std::wstring text = L" ";
    int x = 50;
    int y = 20;
    bool highlighted = false;
    int pixel_type = PIXEL::MEDIUM_SHADE;
    int pixel_default = PIXEL::MEDIUM_SHADE;
    int pixel_highlighted = PIXEL::DARK_SHADE;
    bool display = true;
    bool selected = false;
};

struct menu_t {
    menuComponent_t title;
    menuComponent_t play;
    menuComponent_t options;
};

//struct consoleSprite {
//    int pos[][] = {
//        {},
//        {},
//        {},
//        {}
//    }
//};

void draw(wchar_t*, int, int, wchar_t);
void fillRect(wchar_t*, int, int, int, int, wchar_t);
void clearScreen(wchar_t*);
void line(wchar_t*, int, int, int, int, wchar_t);
void drawPlayer(player_t*, wchar_t*);
void noise(wchar_t*, int);
void noise(wchar_t*, clock_t*, clock_t, bool*);
void drawPixelCharacter(wchar_t*, int, int, float, wchar_t, wchar_t);
void drawPixelText(wchar_t*, int, int, float, std::wstring, wchar_t);
void drawPixelCharacterSet(wchar_t*, wchar_t);
projectile_t* createProjectile(float, float, float, void*);
void updateProjectile(projectile_t*);
void updateProjectilesVec(std::vector<projectile_t*>*);
void drawProjectile(wchar_t*, projectile_t*);
void drawProjectilesVec(wchar_t*, std::vector<projectile_t*>*);
void drawMenuComponent(wchar_t*, menuComponent_t*);
void drawMenu(wchar_t*, menu_t*);
void handleInput(player_t*, std::vector<projectile_t*>*);
void menuControls(menu_t*);
void optionsControls(wchar_t*);
void returnControls();
enemy_t* createEnemy(float, float);
void drawEnemy(enemy_t*, wchar_t*);
void drawEnemiesVec(std::vector<enemy_t*>*, wchar_t*);
void updateEnemy(enemy_t*, player_t*, std::vector<projectile_t*>*);
void updateEnemiesVec(std::vector<enemy_t*>*, player_t*, std::vector<projectile_t*>*);
void initEnemiesVec(std::vector<enemy_t*>*);
void drawHUD(wchar_t*, player_t*);
void gameOver(wchar_t*, player_t*);




int main(){
    
    srand(time(NULL));

    //FHROUND.open("highscore/*.*/txt");


    player_t player1 = {20, 20, 0, 0.2, 0.2};
    /*player1.shoot = [](std::vector<projectile_t*>* entityList, float x, float y, float angle) {
        entityList->push_back(createProjectile(x, y, angle));
    };*/
    menu_t menu;
    std::vector<projectile_t*> projectiles;
    std::vector<enemy_t*> enemies;

    enemy_t enemy1 = { 50, 50, 0, 0.1, 0.1, 0.1, 0.1 };

    menu.title.pixel_type = PIXEL::FULL_SHADE;
    menu.title.pixel_default = menu.title.pixel_type;
    menu.title.text = L"CMD> ZOMBIES"; 
    menu.play.text = L"PLAY";
    menu.play.y = 50;
    menu.play.selected = true;
    menu.options.text = L"OPTIONS";
    menu.options.y = 80;



    wchar_t* screen = new wchar_t[WIDTH*HEIGHT];
    clearScreen(screen);
    //memset(screen, (wchar_t)0x00A0, WIDTH * HEIGHT);
    HANDLE console_buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console_buffer);
    DWORD numCharsWritten;

    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 4;
    cfi.dwFontSize.Y = 4;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");

    SetCurrentConsoleFontEx(console_buffer, false, &cfi);
    SetConsoleTitle(L"EE1010 - CMD> ZOMBIES");

    clock_t start = clock();
    int n_param;
    int n_duration = 2;

    wchar_t optCh = PIXEL::DARK_SHADE;

    while (true) {

        
        
       /* player1.x+=0.0001;
        player1.angle += 0.002;*/
        
        (rand() % 2) ? n_param = 5 : n_param = 3;
        if (start + n_duration * CLOCKS_PER_SEC > clock()) noise(screen, n_param); //3
        else GAME_MENU = true;
        
        if (GAME_MENU) {
            clearScreen(screen);
            menuControls(&menu);
            drawMenu(screen, &menu);
        }
        if (GAME_OPTIONS) {
            clearScreen(screen);
            drawPixelCharacterSet(screen, optCh);
            optionsControls(&optCh);
            returnControls();
            //line(screen, 20, 20, 30, 24, PIXEL::FULL_SHADE);

        }
        if (GAME_START) {
            clearScreen(screen);
            
            drawPlayer(&player1, screen);
            if (NEW_ROUND) initEnemiesVec(&enemies);
            /*drawEnemy(&enemy1, screen);
            updateEnemy(&enemy1, &player1);*/
            drawEnemiesVec(&enemies, screen);
            updateEnemiesVec(&enemies, &player1, &projectiles);

            handleInput(&player1, &projectiles);
            updateProjectilesVec(&projectiles);
            drawProjectilesVec(screen, &projectiles);
            returnControls();
            //line(screen, 20, 20, 30, 24, PIXEL::FULL_SHADE);

            drawHUD(screen, &player1);

            if (!player1.alive) {
                //gameOver(screen, &player1);
                return 0;
            }

        }
        
        

        //drawPixelCharacter(screen, 20, 20, 2, 'B', PIXEL::DARK_SHADE);
        /*drawPixelText(screen, 10, 10, 2, L"ABCDEFGHIJ", PIXEL::DARK_SHADE);
        drawPixelText(screen, 10, 30, 2, L"KLMNOPQRSTUVWXYZ", PIXEL::DARK_SHADE);
        drawPixelText(screen, 10, 50, 2, L">", PIXEL::DARK_SHADE);*/
        //KLMNOPQRSTUVWXYZ
        //drawPixelText(screen, 50, 10, 2, L"NIGGER", PIXEL::DARK_SHADE);
        

        screen[WIDTH*HEIGHT -1] = '\0';

        WriteConsoleOutputCharacterW(console_buffer, screen, WIDTH*HEIGHT, {0,0}, &numCharsWritten);
        // SetConsoleCursorPosition(console_buffer, {0,0});            

    }




    return 0;
}

/*
https://docs.microsoft.com/en-us/windows/console/console-screen-buffers
https://docs.microsoft.com/en-us/windows/console/writeconsole
https://docs.microsoft.com/en-us/windows/console/setconsolecursorposition
https://docs.microsoft.com/en-us/windows/console/coord-str
https://docs.microsoft.com/en-us/windows/console/char-info-str
https://docs.microsoft.com/en-us/windows/console/console-screen-buffer-info-str
https://docs.microsoft.com/en-us/windows/console/createconsolescreenbuffer
https://docs.microsoft.com/en-us/windows/console/setconsoleactivescreenbuffer
https://docs.microsoft.com/en-us/windows/console/console-buffer-security-and-access-rights

*/

void draw(wchar_t* buf, int x, int y, wchar_t c){
    if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0) return;
    buf[x + y * WIDTH] = c;
}

void drawPlayer(player_t* p, wchar_t* buf){
    if (!p->alive) return;
    fillRect(buf, p->x, p->y, p->size, p->size, PIXEL::FULL_SHADE);
    float x1 = p->x + p->size / 2;
    float y1 = p->y + p->size / 2;
    float x2 = x1 + (p->size) * 1.5*cos(p->angle);
    float y2 = y1 + (p->size) * 1.5*sin(p->angle);
    line(buf, x1, y1, x2, y2, PIXEL::LIGHT_SHADE);
}

void fillRect(wchar_t* buf, int px, int py, int dx, int dy, wchar_t c) {
    for (int x = px; x < px + dx; x++) {
        for (int y = py; y < py + dy; y++) {
            draw(buf, x, y, c);
        }
    }
}

void clearScreen(wchar_t* buf){
    fillRect(buf, 0, 0, WIDTH, HEIGHT, ' ');
}

void line(wchar_t* buf, int px, int py, int ex, int ey, wchar_t c) { // digital differential analyser algorithm // replace with brehnsens algorithm in future
    float x = px, y = py;
    int step;
    float dx = ex - px;
    float dy = ey - py;
    //int dist = sqrt(pow(dx, 2) + pow(dy, 2));
    (abs(dx) > abs(dy)) ? step = abs(dx) : step = abs(dy);
    
    for (int i = 0; i < step; i++) {
        draw(buf, (int)round(x), (int)round(y), c);
        x = x + ((float)dx / (float)step);
        y = y + ((float)dy / (float)step);
    }

}

void noise(wchar_t* buf, int num = 5){
    int d;
    wchar_t c;


    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        d = rand() % num;
        switch (d) {
        case 0:
            c = PIXEL::FULL_SHADE;
            break;
        case 1:
            c = L' ';
            break;
        case 2:
            c = PIXEL::LIGHT_SHADE;
            break;
        case 3:
            c = PIXEL::MEDIUM_SHADE;
        case 4:
            c = PIXEL::DARK_SHADE;
        }

        buf[i] = c;
    }
}

void noise(wchar_t* buf, clock_t* startTime, clock_t duration, bool* finishedFlag) {
    if (*startTime + duration == clock()) {
        *finishedFlag = true;
        return;
    };
    int d;
    wchar_t c;


    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        d = rand() % 4;
        switch (d) {
        case 0:
            c = PIXEL::FULL_SHADE;
            break;
        case 1:
            c = PIXEL::DARK_SHADE;
            break;
        case 2:
            c = PIXEL::LIGHT_SHADE;
            break;
        case 3:
            c = PIXEL::MEDIUM_SHADE;
        }

        buf[i] = c;
    }
}

void drawPixelCharacter(wchar_t* buf, int x, int y, float scale, wchar_t c, wchar_t pix) { // x and y are top left corner of character
    int l[10][10]; //BCDEGIMNORSZ
    switch (c) {
    case '0':
        std::copy(&CHARACTER::ZERO[0][0], &CHARACTER::ZERO[0][0] + 100, &l[0][0]);
        break;
    case '1':
        std::copy(&CHARACTER::ONE[0][0], &CHARACTER::ONE[0][0] + 100, &l[0][0]);
        break;
    case '2':
        std::copy(&CHARACTER::TWO[0][0], &CHARACTER::TWO[0][0] + 100, &l[0][0]);
        break;
    case '3':
        std::copy(&CHARACTER::THREE[0][0], &CHARACTER::THREE[0][0] + 100, &l[0][0]);
        break;
    case '4':
        std::copy(&CHARACTER::FOUR[0][0], &CHARACTER::FOUR[0][0] + 100, &l[0][0]);
        break;
    case '5':
        std::copy(&CHARACTER::FIVE[0][0], &CHARACTER::FIVE[0][0] + 100, &l[0][0]);
        break;
    case '6':
        std::copy(&CHARACTER::SIX[0][0], &CHARACTER::SIX[0][0] + 100, &l[0][0]);
        break;
    case '7':
        std::copy(&CHARACTER::SEVEN[0][0], &CHARACTER::SEVEN[0][0] + 100, &l[0][0]);
        break;
    case '8':
        std::copy(&CHARACTER::EIGHT[0][0], &CHARACTER::EIGHT[0][0] + 100, &l[0][0]);
        break;
    case '9':
        std::copy(&CHARACTER::NINE[0][0], &CHARACTER::NINE[0][0] + 100, &l[0][0]);
        break;
    case 'A':
        std::copy(&CHARACTER::A[0][0], &CHARACTER::A[0][0] + 100, &l[0][0]);
        break;
    case 'B':
        std::copy(&CHARACTER::B[0][0], &CHARACTER::B[0][0] + 100, &l[0][0]);
        break;
    case 'C':
        std::copy(&CHARACTER::C[0][0], &CHARACTER::C[0][0] + 100, &l[0][0]);
        break;
    case 'D':
        std::copy(&CHARACTER::D[0][0], &CHARACTER::D[0][0] + 100, &l[0][0]);
        break;
    case 'E':
        std::copy(&CHARACTER::E[0][0], &CHARACTER::E[0][0] + 100, &l[0][0]);
        break;
    case 'F':
        std::copy(&CHARACTER::F[0][0], &CHARACTER::F[0][0] + 100, &l[0][0]);
        break;
    case 'G':
        std::copy(&CHARACTER::G[0][0], &CHARACTER::G[0][0] + 100, &l[0][0]);
        break;
    case 'H':
        std::copy(&CHARACTER::H[0][0], &CHARACTER::H[0][0] + 100, &l[0][0]);
        break;
    case 'I':
        std::copy(&CHARACTER::I[0][0], &CHARACTER::I[0][0] + 100, &l[0][0]);
        break;
    case 'J':
        std::copy(&CHARACTER::J[0][0], &CHARACTER::J[0][0] + 100, &l[0][0]);
        break;
    case 'K':
        std::copy(&CHARACTER::K[0][0], &CHARACTER::K[0][0] + 100, &l[0][0]);
        break;
    case 'L':
        std::copy(&CHARACTER::L[0][0], &CHARACTER::L[0][0] + 100, &l[0][0]);
        break;
    case 'M':
        std::copy(&CHARACTER::M[0][0], &CHARACTER::M[0][0] + 100, &l[0][0]);
        break;
    case 'N':
        std::copy(&CHARACTER::N[0][0], &CHARACTER::N[0][0] + 100, &l[0][0]);
        break;
    case 'O':
        std::copy(&CHARACTER::O[0][0], &CHARACTER::O[0][0] + 100, &l[0][0]);
        break;
    case 'P':
        std::copy(&CHARACTER::P[0][0], &CHARACTER::P[0][0] + 100, &l[0][0]);
        break;
    case 'Q':
        std::copy(&CHARACTER::Q[0][0], &CHARACTER::Q[0][0] + 100, &l[0][0]);
        break;
    case 'R':
        std::copy(&CHARACTER::R[0][0], &CHARACTER::R[0][0] + 100, &l[0][0]);
        break;
    case 'S':
        std::copy(&CHARACTER::S[0][0], &CHARACTER::S[0][0] + 100, &l[0][0]);
        break;
    case 'T':
        std::copy(&CHARACTER::T[0][0], &CHARACTER::T[0][0] + 100, &l[0][0]);
        break;
    case 'U':
        std::copy(&CHARACTER::U[0][0], &CHARACTER::U[0][0] + 100, &l[0][0]);
        break;
    case 'V':
        std::copy(&CHARACTER::V[0][0], &CHARACTER::V[0][0] + 100, &l[0][0]);
        break;
    case 'W':
        std::copy(&CHARACTER::W[0][0], &CHARACTER::W[0][0] + 100, &l[0][0]);
        break;
    case 'X':
        std::copy(&CHARACTER::X[0][0], &CHARACTER::X[0][0] + 100, &l[0][0]);
        break;
    case 'Y':
        std::copy(&CHARACTER::Y[0][0], &CHARACTER::Y[0][0] + 100, &l[0][0]);
        break;
    case 'Z':
        std::copy(&CHARACTER::Z[0][0], &CHARACTER::Z[0][0] + 100, &l[0][0]);
        break;
    case '>':
        std::copy(&CHARACTER::RIGHT_ARROW[0][0], &CHARACTER::RIGHT_ARROW[0][0] + 100, &l[0][0]);
        break;
    default:
        std::copy(&CHARACTER::BLANK[0][0], &CHARACTER::BLANK[0][0] + 100, &l[0][0]);
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (l[i][j]) draw(buf, x + j, y + i, pix);
        }
    }



}

void drawPixelText(wchar_t* buf, int x, int y, float scale, std::wstring pixelString, wchar_t pix){
    int spacing = 12;
    int space = 0;
    for (int i = 0; i < pixelString.size(); i++, space += spacing) drawPixelCharacter(buf, x + space, y, scale, pixelString[i], pix);
}

void drawPixelCharacterSet(wchar_t* buf, wchar_t c) {
    drawPixelText(buf, 10, 10, 2, L"ABCDEFGHIJ", c);
    drawPixelText(buf, 10, 30, 2, L"KLMNOPQRSTUVWXYZ", c);
    drawPixelText(buf, 10, 50, 2, L">", c);
}

projectile_t* createProjectile(float x, float y, float angle, void* creator = nullptr)
{
    projectile_t* projectile = new projectile_t;
    projectile->x = x;
    projectile->y = y;
    projectile->angle = angle;
    projectile->creator = creator;
    return projectile;
}

void updateProjectile(projectile_t* p){
    p->x += p->vx * cos(p->angle);
    p->y += p->vy * sin(p->angle);
    if (p->x > WIDTH || p->x < 0 || p->y > HEIGHT || p->y < 0) p->exists = false;
}

void updateProjectilesVec(std::vector<projectile_t*>* p_list) {
    /*for (std::vector<projectile_t*>::iterator i = p_list->begin(); i != p_list->end(); i++) {
        if ((*i)->exists) updateProjectile(*i);
        else {
            delete (*i);
            p_list->erase(i);
        }

    }*/

    for (int i = 0; i < p_list->size(); i++) {
        if ((*p_list)[i]->exists) updateProjectile((*p_list)[i]);
        else {
            delete ((*p_list)[i]);
            p_list->erase(p_list->begin() + i);
        }

    }
}

void drawProjectile(wchar_t* buf, projectile_t* projectile){
    if(projectile->exists) draw(buf, projectile->x, projectile->y, projectile->pixel);
}

void drawProjectilesVec(wchar_t* buf, std::vector<projectile_t*>* p_list){
    for (std::vector<projectile_t*>::iterator i = p_list->begin(); i != p_list->end(); ++i) drawProjectile(buf, *i);
}

void drawMenuComponent(wchar_t* buf, menuComponent_t* component){
    //wchar_t pixel;
    (component->highlighted) ? component->pixel_type = component->pixel_highlighted : component->pixel_type = component->pixel_default;
    drawPixelText(buf, component->x, component->y, 2, component->text, component->pixel_type);
}

void drawMenu(wchar_t* buf, menu_t* menu){
    if (menu->title.display) drawMenuComponent(buf, &menu->title);
    if (menu->options.display) drawMenuComponent(buf, &menu->options);
    if (menu->play.display) drawMenuComponent(buf, &menu->play);
}

void handleInput(player_t* p, std::vector<projectile_t*>* p_list){
    const int KEY_W = 0x57;
    const int KEY_A = 0x41;
    const int KEY_S = 0x53;
    const int KEY_D = 0x44;

    if (GetAsyncKeyState(KEY_W) & 0x8000) p->y-=p->vy;
    if (GetAsyncKeyState(KEY_A) & 0x8000) p->x-=p->vx;
    if (GetAsyncKeyState(KEY_S) & 0x8000) p->y+=p->vy;
    if (GetAsyncKeyState(KEY_D) & 0x8000) p->x+=p->vx;
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) p->angle-=p->angle_differential;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) p->angle+=p->angle_differential;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        p->shoot(p_list, createProjectile);
        p->angle += 0.02*sin(clock());
    }

    if (p->hp <= 0) {
        p->hp = 0;
        p->alive = false;
    };

}

void menuControls(menu_t* menu){

    /*if (GetAsyncKeyState(VK_UP) & 0x8000 && !(GetAsyncKeyState(VK_UP) & 0x0001)) {
        (menu->options.selected) ? menu->options.selected = false : menu->options.selected = true;
        (menu->play.selected) ? menu->play.selected = false : menu->play.selected = true;
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000 && !(GetAsyncKeyState(VK_DOWN) & 0x0001)) {
        (menu->play.selected) ? menu->play.selected = false : menu->play.selected = true;
        (menu->options.selected) ? menu->options.selected = false : menu->options.selected = true;
    }*/

    if (GetAsyncKeyState(VK_UP) & 0x8000 && !(GetAsyncKeyState(VK_UP) & 0x0001)) {
        menu->play.selected = true;
        menu->options.selected = false;
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000 && !(GetAsyncKeyState(VK_DOWN) & 0x0001)) {
        menu->play.selected = false;
        menu->options.selected = true;
    }
   
    (menu->options.selected) ? menu->options.highlighted = true : menu->options.highlighted = false;
    (menu->title.selected) ? menu->title.highlighted = true : menu->title.highlighted = false;
    (menu->play.selected) ? menu->play.highlighted = true : menu->play.highlighted = false;

    if (GetAsyncKeyState(VK_RETURN) & 0x8000 && !(GetAsyncKeyState(VK_RETURN) & 0x0001) && menu->play.selected && !menu->options.selected) {
        GAME_MENU = false;
        GAME_START = true;
    }

    if (GetAsyncKeyState(VK_RETURN) & 0x8000 && !(GetAsyncKeyState(VK_RETURN) & 0x0001) && menu->options.selected && !menu->play.selected) {
        GAME_MENU = false;
        GAME_OPTIONS = true;
    }

}

void optionsControls(wchar_t* opt_ch){
    wchar_t c = L'A';
    int j = 0;
    for (int i = 0x41; i <= 0x5A; i++, j++) {
        if (GetAsyncKeyState(i) & 0x8000) *opt_ch = c + j;
    }
}

void returnControls(){
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000 && !(GetAsyncKeyState(VK_ESCAPE) & 0x0001)) {
        if (GAME_START) {
            GAME_START = false;
            GAME_MENU = true;
        }
        if (GAME_OPTIONS) {
            GAME_OPTIONS = false;
            GAME_MENU = true;
        }
    }

}

enemy_t* createEnemy(float x, float y)
{
    enemy_t* e = new enemy_t;
    e->x = x + (rand() % 30) - 15;
    e->y = y + (rand() % 30) - 15;
    return e;
}

void drawEnemy(enemy_t* p, wchar_t* buf){
    fillRect(buf, p->x, p->y, p->size, p->size, PIXEL::DARK_SHADE);
}

void drawEnemiesVec(std::vector<enemy_t*>* e_list, wchar_t* buf){
    for (std::vector<enemy_t*>::iterator i = e_list->begin(); i != e_list->end(); ++i) drawEnemy(*i, buf);
}


void updateEnemy(enemy_t* e, player_t* p, std::vector<projectile_t*>* p_list) {
    //static const float e_vx_intial = e->vx;
    //static const float e_vy_intial = e->vy;


    float dx = p->x - e->x;
    float dy = p->y - e->y;

    e->angle = atan2(dy, dx);

    e->vx = e->spdx * cos(e->angle);
    e->vy = e->spdy * sin(e->angle);

    e->x += e->vx;
    e->y += e->vy;

    e->melee(p);
    
    for (int i = 0; i < p_list->size(); i++) {
        e->pCollisions((*p_list)[i]);
    }
    
    //e->pCollisions();

}



void initEnemiesVec(std::vector<enemy_t*>* e_list){
    for (int i = 0; i < 5 * ROUND; i++) {
        enemy_t* e = createEnemy(50, 50);
        e->spdx = e->spdy = 0.1 + (ROUND / 100);
        e_list->push_back(e);
    }
    NEW_ROUND = false;
}

void drawHUD(wchar_t* buf, player_t* p){
    std::wstring kills = L"KILLS " + std::to_wstring(p->kill_count);
    drawPixelText(buf, WIDTH - 100, 10, 1, kills, PIXEL::FULL_SHADE);
    std::wstring hp = L"HP " + std::to_wstring((int)p->hp);
    drawPixelText(buf, WIDTH - 100, 30, 1, hp, PIXEL::FULL_SHADE);
    std::wstring round = L"ROUND " + std::to_wstring(ROUND);
    drawPixelText(buf, WIDTH - 100, 50, 1, round, PIXEL::FULL_SHADE);

}

void gameOver(wchar_t* buf, player_t* p){
    std::wstring rounds = L"YOU LASTED " + std::to_wstring(ROUND) + L"ROUNDS";
    drawPixelText(buf, WIDTH/2 - 100, 10, 1, rounds, PIXEL::FULL_SHADE);
    std::wstring kills = L"YOU KILLED " + std::to_wstring(ROUND) + L"ZOMBIES";
    drawPixelText(buf, WIDTH / 2 - 100, 30, 1, kills, PIXEL::FULL_SHADE);
    /*std::wstring highest_round = L"YOU LASTED " + std::to_wstring(ROUND) + L"ROUNDS";
    drawPixelText(buf, WIDTH / 2 - 100, 10, 1, rounds, PIXEL::FULL_SHADE);*/
    
    ////if (FHROUND.peek() == std::ifstream::traits_type::eof()) ;
    //std::string str;
    //while (!FHROUND.eof){ // To get you all the lines.
    //   getline(FHROUND, str); // Saves the line in STRING.
    //}
    // 
    //int hround = std::stoi(str);

    //if (hround > ROUND) {
    //    FHROUND 
    //}

}

void updateEnemiesVec(std::vector<enemy_t*>* p_list, player_t* p, std::vector<projectile_t*>* projList){
    
    if (!p_list->size()) {
        ROUND++;
        NEW_ROUND = true;
    }
    
    for (int i = 0; i < p_list->size(); i++) {
        if ((*p_list)[i]->alive) updateEnemy((*p_list)[i], p, projList);
        else {
            delete ((*p_list)[i]);
            p_list->erase(p_list->begin() + i);
        }
    }
}

/*draw(screen, WIDTH - 7, 0, L'E');
        draw(screen, WIDTH - 6, 0, L'E');
        draw(screen, WIDTH - 5, 0, L'1');
        draw(screen, WIDTH - 4, 0, L'0');
        draw(screen, WIDTH - 3, 0, L'1');
        draw(screen, WIDTH - 2, 0, L'0');*/

template<typename entity1, typename entity2>
bool entityBounds(entity1* e1, entity2* e2){
    float pLeft = e1->x, tLeft = e2->x;
    float pRight = e1->x + e1->size, tRight = e2->x + e2->size;
    float pTop = e1->y, tTop = e2->y;
    float pBottom = e1->y + e1->size, tBottom = e2->y + e2->size;

    return (((pLeft <= tRight && pRight >= tLeft) || (tLeft <= pRight && tRight >= pLeft)) && ((pTop <= tBottom && pBottom >= tTop) || (pTop <= tBottom && pBottom >= tTop)));

}
