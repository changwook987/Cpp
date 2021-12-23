#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

#define SIZE_X 100
#define SIZE_Y 50

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(COORD pos)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursorVisible(bool visible)
{
    CONSOLE_CURSOR_INFO info = {
        0,
    };

    info.dwSize = 1;
    info.bVisible = visible ? 1 : 0;

    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void reset()
{
    char buff[50] = "";
    sprintf(buff, "mode con cols=%d lines=%d", SIZE_X + 20, SIZE_Y + 1);
    system(buff);
    gotoxy({0, 0});
    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++)
        {
            if (i == 0 || i == SIZE_Y - 1)
            {
                if (j == 0 || j == SIZE_X - 1)
                {
                    putchar('+');
                }
                else
                {
                    putchar('-');
                }
            }
            else if (j == 0 || j == SIZE_X - 1)
            {
                putchar('|');
            }
            else
            {
                putchar(' ');
            }
        }
        putchar(10);
    }
}

class shape
{
private:
    string *pattern;
    int size_x;
    int size_y;
    int color;

public:
    shape() {}
    shape(string *pattern_, int len)
    {
        pattern = pattern_;
        size_y = len;
        int max = 0;
        for (int i = 0; i < size_y; i++)
        {
            max = pattern[i].length() > max ? pattern[i].length() : max;
        }
        size_x = max;
        color = 0;
    }
    shape(string *pattern_, int len, int color_)
    {
        pattern = pattern_;
        size_y = len;
        int max = 0;
        for (int i = 0; i < size_y; i++)
        {
            max = pattern[i].length() > max ? pattern[i].length() : max;
        }
        size_x = max;
        color = color_;
    }
    void show(COORD pos)
    {
        setColor(color);
        for (int i = 0; i < size_y; i++)
        {
            gotoxy(pos);
            cout << pattern[i];
            pos.Y++;
        }
        setColor(0);
    }
    COORD getSize()
    {
        return {size_x, size_y};
    }
};

class object
{
private:
    shape shape1;
    COORD pos;

public:
    object() {}

    object(shape shape_, COORD pos_)
    {
        shape1 = shape_;
        pos = pos_;
    }

    void moveTo(COORD pos_)
    {
        if (pos_.X < 1 || pos_.Y < 1)
            return;
        if (pos_.X + shape1.getSize().X > SIZE_X - 1)
            return;
        if (pos_.Y + shape1.getSize().Y > SIZE_Y - 1)
            return;

        for (int i = 0; i < shape1.getSize().Y; i++)
        {
            gotoxy(pos);
            for (int j = 0; j < shape1.getSize().X; j++)
                putchar(' ');
            pos.Y++;
        }
        pos = pos_;
        shape1.show(pos);
    }

    void move_left()
    {
        COORD pos_ = {pos.X - 1, pos.Y};
        moveTo(pos_);
    }
    void move_right()
    {
        COORD pos_ = {pos.X + 1, pos.Y};
        moveTo(pos_);
    }
    void move_up()
    {
        COORD pos_ = {pos.X, pos.Y - 1};
        moveTo(pos_);
    }
    void move_down()
    {
        COORD pos_ = {pos.X, pos.Y + 1};
        moveTo(pos_);
    }

    COORD getPos()
    {
        return pos;
    }

    COORD getMidPos()
    {
        COORD size = shape1.getSize();
        return {pos.X + size.X / 2, pos.Y + size.Y / 2};
    }

    bool inBound(object other)
    {
        for (int i = other.getPos().Y; i <= other.shape1.getSize().Y + other.getPos().Y; i++)
        {
            if (pos.Y <= i && pos.Y + shape1.getSize().Y >= i)
            {
                for (int j = other.getPos().X; j <= other.shape1.getSize().X + other.getPos().X; j++)
                {
                    if (pos.X <= j && pos.X + shape1.getSize().X >= j)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    shape getShape()
    {
        return shape1;
    }
};

int main()
{
    string player_pattern[] = {"  A  ", "  M  ", "| M |", "AAAAA"};
    string bulit_pattern[] = {"A", "V"};
    string enermy_pattern[] = {"(*&&&*)", "/\\/\\/\\/"};

    shape player_shape = shape(player_pattern, 4, 0xe);
    shape bulit_shape = shape(bulit_pattern, 2, 0xc);
    shape enermy_shape = shape(enermy_pattern, 2, 0x9);

    object player = object(player_shape, {SIZE_X / 2, SIZE_Y * 2 / 3});

    object bulit[10];
    for (int i = 0; i < 10; i++)
    {
        bulit[i] = object(bulit_shape, {1, 1});
    }

    object enermy[10];
    for (int i = 0; i < 10; i++)
    {
        enermy[i] = object(enermy_shape, {1, 1});
    }

    int cooltime = 0;
    int score = 0;

    cursorVisible(false);
    reset();

    srand(time(NULL));

    while (true)
    {
        for (int i = 0; i < 10; i++)
        {
            int seed = (rand() % 4);
            if (seed == 0)
            {
                enermy[i].move_left();
            }
            else if (seed == 1)
            {
                enermy[i].move_right();
            }
            else if (seed == 2)
            {
                enermy[i].move_up();
            }
            else if (seed == 3)
            {
                enermy[i].move_down();
            }
        }

        for (int i = 0; i < 10; i++)
        {
            if (bulit[i].getPos().Y == 1 && cooltime == 0)
            {
                bulit[i].moveTo(player.getMidPos());
                cooltime = 5;
            }
            else
            {
                bulit[i].move_up();
            }

            for (int j = 0; j < 10; j++)
            {
                if (bulit[i].inBound(enermy[j]))
                {
                    enermy[j].moveTo({(short)((rand() % SIZE_X - enermy[j].getShape().getSize().X - 1) + 1), 1});
                    gotoxy({SIZE_X + 2, 3});
                    setColor(15);
                    score += 10;
                    printf("score : %d", score);
                    setColor(0);
                }
            }
        }

        if (GetAsyncKeyState(VK_LEFT))
        {
            player.move_left();
        }
        if (GetAsyncKeyState(VK_RIGHT))
        {
            player.move_right();
        }
        if (GetAsyncKeyState(VK_UP))
        {
            player.move_up();
        }
        if (GetAsyncKeyState(VK_DOWN))
        {
            player.move_down();
        }

        player.moveTo(player.getPos());
        cooltime = cooltime - 1 == -1 ? 0 : cooltime - 1;
        Sleep(5);
    }
}
