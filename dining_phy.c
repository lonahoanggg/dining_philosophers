// #include "raylib.h"

// #include "rlgl.h"
// #include "raymath.h"


#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stddef.h>


#define EASY 1
#define MEDIUM 2
#define HARD 3

// Put in raylib/examples/core

const int screenWidth = 1920;
const int screenHeight = 1080;

typedef struct {
    float Lifetime;
} Timer;

void StartTimer(Timer * timer, float lifetime){
    if (timer != NULL){
        timer->Lifetime = lifetime;
    }
}

void UpdateTimer(Timer* timer){
    if (timer != NULL && timer->Lifetime > 0){
        timer->Lifetime  -= GetFrameTime();
    } 
}

bool TimerDone(Timer* timer){
    if (timer != NULL){
        return (timer->Lifetime <= 0);
    }
}

typedef struct{
    Rectangle body;
    Rectangle r_arm;
    Rectangle l_arm;
} Philosopher;

typedef struct{
    Rectangle fork;
    Timer fork_timer;
    bool in_use; //
    bool draw_on_table;
    bool on_left_hand; // false: not on left, aka right; true: on left
    bool started;
}Fork;


int phil_maker(Philosopher *p, int b_corner_x, int b_corner_y, int b_dim){

    p->body = (Rectangle){b_corner_x, b_corner_y, b_dim, b_dim};
    p->r_arm = (Rectangle){b_corner_x-30, b_corner_y+30, 30, b_dim};  //w, h
    p->l_arm = (Rectangle){b_corner_x+60, b_corner_y+30, 30, b_dim};

    return 0;
}

int fork_maker(Fork *f, int table_x, int table_y, int dim){
    f->fork = (Rectangle){table_x, table_y, dim, 60};
    f->in_use = false;
    f->draw_on_table = true;
    f->on_left_hand = false;
    f->started = false;
}


void draw_phil(Philosopher phil1, float angle, float c_x, float c_y){
    rlPushMatrix();

    rlTranslatef(c_x, c_y, 0);

    rlRotatef(angle, 0, 0, 1);
    rlTranslatef(-c_x, -c_y, 0);

    DrawRectangle(phil1.body.x, phil1.body.y, phil1.body.width, phil1.body.height, RED);
    DrawRectangle(phil1.r_arm.x, phil1.r_arm.y, phil1.r_arm.width, phil1.r_arm.height, BLACK);
    DrawRectangle(phil1.l_arm.x, phil1.l_arm.y, phil1.l_arm.width, phil1.l_arm.height, BLACK);

    rlPopMatrix();
}

void draw_fork(Fork f, float angle, float c_x, float c_y){
    rlPushMatrix();

    rlTranslatef(c_x, c_y, 0);

    rlRotatef(angle, 0, 0, 1);
    rlTranslatef(-c_x, -c_y, 0);

    DrawRectangle(f.fork.x, f.fork.y, f.fork.width, f.fork.height, BROWN);
    rlPopMatrix();
}

void draw_fork_on_hand(Fork f, float angle, float c_x, float c_y){
    rlPushMatrix();

    rlTranslatef(c_x, c_y, 0);

    rlRotatef(angle, 0, 0, 1);
    rlTranslatef(-c_x, -c_y, 0);

    DrawRectangle(f.fork.x, f.fork.y+50, f.fork.width, f.fork.height, BROWN);
    rlPopMatrix();
}

void draw_times(Timer * phil_timers, int num_items){
    float angle = -20.0f;
    int min, sec;
    for(int i = 0; i < num_items; i ++){
        min = (int)phil_timers[i].Lifetime/60;
        sec = (int)phil_timers[i].Lifetime%60;
        float x = ((screenWidth/2)) + 400 * cos(DEG2RAD * angle);
        float y = (screenHeight/2) + 400 * sin(DEG2RAD * angle);
        DrawText(TextFormat("%02i:%02i", min, sec),x, y, 30, BLACK);
        angle += 360.0f / num_items;
    }
}

Vector2 RotatePoint(Vector2 point, Vector2 center, float angleDeg) {
    float s = sinf(DEG2RAD * angleDeg);
    float c = cosf(DEG2RAD * angleDeg);

    point.x -= center.x;
    point.y -= center.y;

    float xnew = point.x * c - point.y * s;
    float ynew = point.x * s + point.y * c;

    point.x = xnew + center.x;
    point.y = ynew + center.y;

    return point;
}

int gameplay(Vector2 cent_table, int count, float phil_start_time, float recharge_rate){
    
    float table_life = 45.0f;
    Timer table_timer = {0};

           // Initialize audio device
    Music music = LoadMusicStream("../examples/audio/resources/slow-cinematic-clock-ticking-405471.mp3");
    printf("%s", GetWorkingDirectory());
    
    //timer for each philosher
    float phil_life = phil_start_time;
    Timer phil_timers[count];
    for(int i = 0; i < count; i ++){
        Timer phil_timer = {0};
        phil_timers[i] = phil_timer;
    }

    //making each philosopher
    Philosopher philosophers[count];
    for(int i = 0; i < count; i ++){
        phil_maker(&philosophers[i],930,210,60);
    }

    //making each fork
    Fork forks[count];
    for(int i = 0; i < count; i ++){
        fork_maker(&forks[i], 940, (int)((float)screenHeight/2)-200, 20);
    }

    float fork_life = 2.0f;
    Timer fork_timers[count];
    for(int i = 0; i < count; i ++){
        Timer fork_timer = {0};
        fork_timers[i] = fork_timer;
    }

    
    int min;
    int sec;
    bool started = false;
    float volume = 1.0f;
    SetMusicVolume(music, volume);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        Rectangle buttonBounds = { 100, 100, 100, 30 };
        UpdateMusicStream(music);   // Update music buffer with new stream data

        if (!started){
            if (GuiButton(buttonBounds, "Start Round")){
                started = true;
                StartTimer(&table_timer, table_life);
                for(int i = 0; i < count; i ++){
                    StartTimer(&phil_timers[i], phil_life);
                }
            }
        }

        //this is for next iteration's drawing
        for (int i = 0; i < count; i++){
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mouse_pos = GetMousePosition();
                float angle = i * (360.0f / count); 
                //since we used a rotate thingy we have to do some weird stuff to get the new pos
                Vector2 new_mouse_pos = RotatePoint(mouse_pos, cent_table, -angle);

                if(CheckCollisionPointRec(new_mouse_pos, philosophers[i].l_arm)){
                    if (!forks[i].in_use) {
                        forks[i].draw_on_table = false;
                        forks[i].in_use = true;
                        forks[i].on_left_hand = true;
                        // forks[i].started = true;
                        printf("clicked phil %d's left arm\n", i);
                        printf("fork %d is registered\n", i);
                        PlayMusicStream(music);
                    }
                }

                if(CheckCollisionPointRec(new_mouse_pos, philosophers[i].r_arm)){
                    if (!forks[(i-1+count)%count].in_use) {
                        forks[(i-1+count)%count].draw_on_table = false;
                        forks[(i-1+count)%count].in_use = true;
                        forks[(i-1+count)%count].on_left_hand = false;
                        // forks[(i-1+count)%count].started = true;
                        printf("clicked phil %d's right arm arm\n", i);
                        printf("fork %d is registered\n", (i-1+count)%count);
                        PlayMusicStream(music);
                    }
                }
            };
        };

        BeginDrawing();
            UpdateTimer(&table_timer);
            for(int i = 0; i < count; i ++){
                UpdateTimer(&phil_timers[i]);
            }

            //make the times array to pass into our draw time function

            ClearBackground(RAYWHITE);
            DrawCircleV(cent_table, 250, DARKGRAY);
            
            float circ = 360/count;
            
            float angle = 0;
            for (int i = 0; i < count; i++){
                //need some sort of check to see whether or not the arm was clicked before drawing
                draw_phil(philosophers[i], angle, (float)screenWidth/2, (float)screenHeight/2);
                if(forks[i].draw_on_table){ 
                    //draw fork on the table
                    draw_fork(forks[i], (angle+circ/2),(float)screenWidth/2, (float)screenHeight/2);

                }
                else{

                    if (forks[i].on_left_hand){
                        forks[i].fork.x = philosophers[i].l_arm.x; //making it so that the arm is onthe philospoher arm x and y
                        forks[i].fork.y = philosophers[i].l_arm.y;
                        draw_fork_on_hand(forks[i], (angle), (float)screenWidth/2, (float)screenHeight/2);
                        
                    }
                    else{
                        forks[i].fork.x = philosophers[(i+1)%count].r_arm.x; //making it so that the arm is onthe philospoher arm x and y
                        forks[i].fork.y = philosophers[(i+1)%count].r_arm.y;
                        draw_fork_on_hand(forks[i], (angle+circ), (float)screenWidth/2, (float)screenHeight/2);
                    }

                    if (!forks[i].started){
                        StartTimer(&fork_timers[i], fork_life);
                        forks[i].started = true;   
                    } else{
                        UpdateTimer(&fork_timers[i]);
                    }

                    if (TimerDone(&fork_timers[i])) {
                        forks[i].in_use = false;
                        forks[i].draw_on_table = true;
                        fork_timers[i].Lifetime = 2.0f;
                        
                        forks[i].fork.x = 940; 
                        forks[i].fork.y = (int)((float)screenHeight/2)-200;
                        
                        forks[i].started = false;   
                    }     
                    
                }
                if (forks[(i-1+count)%count].in_use && forks[i].in_use){
                    phil_timers[(i-1+count)%count].Lifetime += recharge_rate;
                }
                angle += circ;
            };
            
            // Table Timer
            min = (int)table_timer.Lifetime/60;
            sec = (int)table_timer.Lifetime%60;
            DrawText(TextFormat("%02i:%02i", min, sec),(float)screenWidth/2-30, (float)screenHeight/2-15, 30, BLACK);

            draw_times(phil_timers, count);

            //check if the game should be done / if the user loses

            if(started && TimerDone(&table_timer)){
                return 0;
            }

            for (int i = 0; i < count; i++){
                if(started && TimerDone(&phil_timers[i])){
                   return 1;
                }
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


int winning_screen(){
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
            DrawText(TextFormat("CONGRATS"),((float)screenWidth/2) - 400, ((float)screenHeight/2) - 200, 100, BLACK);
            Rectangle returnToHome = { (float)screenWidth/2 + 50, (float)screenHeight/2, 100, 30 };
            if (GuiButton(returnToHome, "Return to Home")){
                EndDrawing();
                return 0;
            }
            
            ClearBackground(GREEN);
            
            EndDrawing();
        //----------------------------------------------------------------------------------
    }
}

int losing_screen(){
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
            DrawText(TextFormat(":("),((float)screenWidth/2) - 150, ((float)screenHeight/2) - 50, 100, BLACK);
            Rectangle returnToHome = { (float)screenWidth/2 + 50, (float)screenHeight/2, 100, 30 };
            if (GuiButton(returnToHome, "Return to Home")){
                EndDrawing();
                return 0;
            }
            
            ClearBackground(RED);
            
            EndDrawing();
        //----------------------------------------------------------------------------------
    }
}

int homescreen(){
    int choice;

    Vector2 cent_table = { (float)screenWidth/2, (float)screenHeight/2 };
    Philosopher phil1;
    phil_maker(&phil1,930,210,60);
    Fork f;
    fork_maker(&f, 940, (int)((float)screenHeight/2)-200, 20);


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
            Rectangle eas = { 100, 100, 100, 30 };
            Rectangle med = { 100, 150, 100, 30 };
            Rectangle har = { 100, 200, 100, 30 };
            
            ClearBackground(RAYWHITE);
            DrawCircleV(cent_table, 250, DARKGRAY);

            int count = 5;
            float circ = 360/count;
            
            float angle = 0;
            for (int i = 0; i < count; i++){
                draw_phil(phil1, angle, (float)screenWidth/2, (float)screenHeight/2);
                draw_fork(f, (angle+circ/2),(float)screenWidth/2, (float)screenHeight/2);
                angle += circ;
            };

            if (GuiButton(eas, "Easy")){
                choice = EASY;
                EndDrawing();
                break;
            }
            if (GuiButton(med, "Medium")){
                choice = MEDIUM;
                EndDrawing();
                break;
            }if (GuiButton(har, "Hard")){
                choice = HARD;
                EndDrawing();
                break;
            }
            EndDrawing();
        //----------------------------------------------------------------------------------
    }
    int ret;
    if (choice == 1){
        ret = gameplay(cent_table, 5, 10, 0.15);
    }   
    else if (choice == 2){
        ret = gameplay(cent_table, 6, 10, 0.07);
    }    
    else if (choice == 3){
        ret = gameplay(cent_table, 7, 10, 0.05);
    }       

    return ret;

}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int ret;
    SetTargetFPS(120);               // Set our game to run at 120 frames-per-second
    InitWindow(screenWidth, screenHeight, "Dining Cubes");
    InitAudioDevice();   
    while(!WindowShouldClose()){
        ret = homescreen();

        if (ret == 0){
            winning_screen();
        }
        else{
            losing_screen();
        }
    }
    return 0;
}
