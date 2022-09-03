#include <iostream>
#include <raylib.h>
#include <vector>
#include <math.h>
#include <fstream>
#include <map>

#include <inipp.h>

// Fast inverse square root, as seen in Quake
float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}



int playspace_x = 1280;
int playspace_y = 720;
float cam_x = 0;
float cam_y = 0;
float cam_zoom = 1;
bool exp_grav = true;

float random(float min, float max){
    return GetRandomValue(min, max);
}

typedef struct Cell{
    Color color;
    float vx, vy, x, y;
};

std::vector<Cell*> generate_group(Color color, int amount){
    std::vector<Cell*> cells = std::vector<Cell*>();

    for(int i = 0; i < amount; i++){
        Cell* cell = new Cell();

        cell->color = color;

        cell->x = random(0, playspace_x);
        cell->y = random(0, playspace_y);

        cells.emplace_back(cell);
    }

    return cells;
}

void draw(std::vector<Cell*> cells, float draw_size){
    for(int i = 0; i < cells.size(); i++){
        Cell* c = cells.at(i);
        DrawCircleV(Vector2{ (c->x - cam_x) * cam_zoom, (c->y - cam_y) * cam_zoom }, draw_size * cam_zoom, c->color);
    }
}

void rule(std::vector<Cell*> a, std::vector<Cell*> b, float g){
    for(int i = 0; i < a.size(); i++){
        Cell* c = a[i];
        for(int j = 0; j < b.size(); j++){
            Cell* d = b[j];

            float dx = c->x - d->x;
            float dy = c->y - d->y;

            float dist = Q_rsqrt(dx * dx + dy * dy);

            if(dist == 0){
                continue;
            }

            float nx = dx * dist;
            float ny = dy * dist;

            if(exp_grav){
                c->vx += nx * g * (dist / dist);
                c->vy += ny * g * (dist / dist);
            } else {
                c->vx += nx * g * dist;
                c->vy += ny * g * dist;
            }
        }
    }
}

void update_pos(std::vector<Cell*> v){
    for(int i = 0; i < v.size(); i++){

        Cell* c = v[i];

        float vel = sqrt(c->vx * c->vx + c->vy * c->vy);

        if(vel > 200) {
            c->vx /= vel;
            c->vy /= vel;

            c->vx *= 200;
            c->vy *= 200;
        }

        c->x += c->vx * GetFrameTime();
        c->y += c->vy * GetFrameTime();
        /*
       if(c->x < 0 )
           c->x = 1280;
       if(c->y < 0 )
           c->y = 720;
       if(c->x > 1280 )
           c->x = 0;
       if(c->y > 720 )
           c->y = 0;
           */
        if(c->x < 0 || c->x > playspace_x)
            c->vx *= -0.9;
        if(c->y < 0 || c->y > playspace_y)
            c->vy *= -0.9;

        if(c->x < 0 )
            c->x = 0;
        if(c->y < 0 )
            c->y = 0;
        if(c->x > playspace_x )
            c->x = playspace_x;
        if(c->y > playspace_y )
            c->y = playspace_y;

    }
}

int main() {

    inipp::Ini<char> ini;
    std::ifstream is("simulation.ini");
    ini.parse(is);
    ini.generate(std::cout);
    ini.strip_trailing_comments();

    auto simsettings = ini.sections["Simulation"];

    if(simsettings.count("seed")){
        SetRandomSeed(std::stoi(simsettings["seed"]));
    }
    if(simsettings.count("x")){
        playspace_x = std::stoi(simsettings["x"]);
    }
    if(simsettings.count("y")){
        playspace_y = std::stoi(simsettings["y"]);
    }
    if(simsettings.count("exp_grav")){
        exp_grav = simsettings["exp_grav"] == "true";
    }

    std::cout << "SIM SETTINGS: \n\twidth: " << playspace_x << "\n\theight:" << playspace_y << "\n\tExponential gravity: " << (exp_grav ? "true" : "false") << std::endl;

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Life");


    if(simsettings.count("target_fps")){
        SetTargetFPS(std::stoi(simsettings["target_fps"]));
    } else {
        SetTargetFPS(60);
    }

    std::map<std::string, std::vector<Cell*>> colors;

    auto csection = ini.sections["Colors"];

    Color c_type_color_list[] = {
            BLACK,
            DARKPURPLE,
            PURPLE,
            GREEN,
            BROWN,
            GRAY,
            LIGHTGRAY,
            WHITE,
            RED,
            ORANGE,
            YELLOW,
            LIME,
            BLUE,
            DARKBLUE,
            PINK,
            BEIGE
    };

    for (auto color : csection){
        std::string color_name = color.first;
        int color_amount = std::stoi(color.second);

        auto colorsec = ini.sections[color_name];

        Color c = c_type_color_list[std::stoi(colorsec["color"])];

        std::pair<std::string, std::vector<Cell*>> pair = std::pair<std::string, std::vector<Cell*>>(color_name, generate_group(c, color_amount));
        colors.insert(pair);
    }

    //std::vector<Cell*>red = generate_group(RED, 800);
    //std::vector<Cell*>green = generate_group(GREEN, 400);

    while(!WindowShouldClose()){
        //rule(green, red, -1);
        //rule(green, green, 100);
        //rule(red, green, 200 );
        //rule(red, red, -10);

        for (auto color : colors){
            for (auto color2 : colors){
                if(ini.sections[color.first].count(color2.first))
                    rule(color.second, color2.second, std::stof(ini.sections[color.first][color2.first]));
            }
        }

        for (auto color : colors){
            update_pos(color.second);
        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Vector2 delta = GetMouseDelta();
            cam_x -= delta.x / cam_zoom;
            cam_y -= delta.y / cam_zoom;
        }

        cam_zoom += GetMouseWheelMove() / 5;
        if(cam_zoom < 0.01)
            cam_zoom = 0.01;

        //update_pos(green);
        //update_pos(red);

        BeginDrawing();
        ClearBackground(WHITE);

        //draw(red);
        //draw(green);

        DrawRectangle(-cam_x * cam_zoom, -cam_y * cam_zoom, playspace_x * cam_zoom, playspace_y * cam_zoom, BLACK);

        for (auto color : colors){
            float draw_size = 2;
            if(ini.sections[color.first].count("size"))
                draw_size = std::stof(ini.sections[color.first]["size"]);
            draw(color.second, draw_size);
        }


        EndDrawing();
    }

    CloseWindow();

}
