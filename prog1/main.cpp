#include <SFML/Graphics.hpp>
#include "windows.h"
#include "Math.h"
#include "time.h"

#define KEY_DOWN(key) (GetAsyncKeyState(key) & 0x8000) //обработка нажатий клавиш
#define KEY_PUSH(key) (GetAsyncKeyState(key) & 0x0001)

#define v_max 3 //скорость марсохода
#define angle_max 2 //угловая скорость марсохода
#define bullet_v 30 //скорость пуль марсохода
#define bullet_o 10 //отбрасывание от пули
#define bullet_a 5 //отдача от выстрела

#define obj_n 50 //количество объектов
#define bonus_n 5 //максимальное количество бонусов
#define bonus_range 400 //минимальная дистанция от бонуса до марсохода 

#define width 1900 //размеры окна
#define height 950

using namespace sf;

struct pt 
{
	double x, y;
};

void swap(float &a, float &b)
{
	float c = a;
	a = b;
	b = c;
}

float max(float a, float b)
{
	if (a >= b) return a;
	else return b;
}

float min(float a, float b)
{
	if (a <= b) return a;
	else return b;
}

void constrain(int16_t & a, int16_t min, int16_t max)
{
	if (a < min) a = min;
	if (a > max) a = max;
}

double sq(double x)
{
	return x * x;
}

bool intersect(pt a, pt b, pt c, pt d) { //функция определения пересечения отрезков ab и cd
	double v1 = (d.x - c.x)*(a.y - c.y) - (d.y - c.y)*(a.x - c.x);
	double v2 = (d.x - c.x)*(b.y - c.y) - (d.y - c.y)*(b.x - c.x);
	double v3 = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	double v4 = (b.x - a.x)*(d.y - a.y) - (b.y - a.y)*(d.x - a.x);
	if((v1*v2 < 0) && (v3*v4 < 0)) return 1;
	else return 0;
}

struct vector
{
	double a;
	double l;
};

struct object //структура объекта на поле
{
	uint8_t n;
	vector c[10]; //углы
	vector s[10]; //стороны
	bool mv;
	Sprite sprite;
	double x;
	double y;
	double angle;
	float v;
	double dx;
	double dy;
	double da;
	float mass;
	double max_dist;
	Sprite bullet[10];
	double bullet_angle[10];
	double bullet_x[10];
	double bullet_y[10];
	int bullet_shot;
	uint8_t reg;
	int16_t energy;
	int16_t hp;
	bool enable;
};

object obj[obj_n + bonus_n]; //создание объектов

int main()
{
	Texture background; //открываем картинку бэкграунда
	background.loadFromFile("..\\background.png");

	Texture texture0;
	texture0.loadFromFile("..\\rover_destroyed1.png");

	Texture texture1; //текстура первого марсохода
	texture1.loadFromFile("..\\rover_new1.png");

	Texture texture2;
	texture2.loadFromFile("..\\rover_new2.png");

	Texture texture_bullet; //текстура пуль первого марсохода
	texture_bullet.loadFromFile("..\\bull.png");

	Texture texture3;
	texture3.loadFromFile("..\\box.png");

	Texture texture4;
	texture4.loadFromFile("..\\wall1.png");

	Texture texture_health;
	texture_health.loadFromFile("..\\box_health.png");

	Texture texture_triple;
	texture_triple.loadFromFile("..\\box_triple.png");

	Texture texture_double;
	texture_double.loadFromFile("..\\box_double.png");

	srand(time(NULL) % 32765);
	RenderWindow window(VideoMode(width, height), "mars_wars"); //создаем окно

	Sprite bg(background);

	RectangleShape energy1; //создаем полоску энергии первого марсохода
	energy1.setSize(Vector2f(200, 20));
	energy1.setPosition(20, 20);
	energy1.setFillColor(Color(0, 255, 0, 128));
	RectangleShape energy1_bar; //создаем фон полоски энергии первого марсохода
	energy1_bar.setSize(Vector2f(200, 20));
	energy1_bar.setPosition(20, 20);
	energy1_bar.setFillColor(Color(0, 0, 0, 128));

	RectangleShape hp1; //полоска жизней первого марсохода
	hp1.setSize(Vector2f(200, 20));
	hp1.setPosition(20, 50);
	hp1.setFillColor(Color(255, 0, 0, 128));
	RectangleShape hp1_bar; //создаем фон полоски жизней первого марсохода
	hp1_bar.setSize(Vector2f(200, 20));
	hp1_bar.setPosition(20, 50);
	hp1_bar.setFillColor(Color(0, 0, 0, 128));

	for (int i = 0; i < obj_n + bonus_n; i++)
	{
		obj[i].enable = true;
	}

	obj[0].sprite.setTexture(texture1);
	obj[0].sprite.setOrigin(25, 40);

	obj[0].x = 800; //положение в пространстве
	obj[0].y = 800;
	obj[0].angle = 90;
	obj[0].v = 0;

	obj[0].mass = 1; //масса марсохода
	obj[0].energy = 1000; //энергия марсохода
	obj[0].hp = 1000; //жизни марсохода

	obj[0].n = 8; //задаем углы марсохода
	obj[0].c[0].a = -38.88;
	obj[0].c[0].l = 40;
	obj[0].c[1].a = 38.88;
	obj[0].c[1].l = 40;
	obj[0].c[2].a = 141.12;
	obj[0].c[2].l = 40;
	obj[0].c[3].a = -158;
	obj[0].c[3].l = 34;
	obj[0].c[4].a = -168;
	obj[0].c[4].l = 40;
	obj[0].c[5].a = 168;
	obj[0].c[5].l = 40;
	obj[0].c[6].a = 158;
	obj[0].c[6].l = 34;
	obj[0].c[7].a = -141.12;
	obj[0].c[7].l = 40;
	obj[0].max_dist = 40; //максимальная дистанция обработки столкновений 

	obj[0].s[0].a = 90; //задаем стороны марсохода
	obj[0].s[0].l = 50;
	obj[0].s[1].a = 180;
	obj[0].s[1].l = 62;
	obj[0].s[2].a = -90;
	obj[0].s[2].l = 13;
	obj[0].s[3].a = 26.57;
	obj[0].s[3].l = 9;
	obj[0].s[4].a = -90;
	obj[0].s[4].l = 15;
	obj[0].s[5].a = 153.43;
	obj[0].s[5].l = 9;
	obj[0].s[6].a = -90;
	obj[0].s[6].l = 13;
	obj[0].s[7].a = 0;
	obj[0].s[7].l = 62;

	for (int i = 0; i < 10; i++)
	{
		obj[0].bullet[i].setTexture(texture_bullet);
		obj[0].bullet[i].setPosition(-10, -10);
		obj[0].bullet[i].setOrigin(3, 7);
	}
	obj[0].bullet_shot = 0; //биты состояния пуль 1 - летит / 0 - ожидание
	obj[0].reg = 1; //режим стрельбы первого марсоход
	bool shot0_press = false; //предыдущее состояние кнопки выстрела

	RectangleShape energy2; //создаем полоску энергии второго марсохода
	energy2.setSize(Vector2f(200, 20));
	energy2.setPosition(width - 220, 20);
	energy2.setFillColor(Color(0, 255, 0, 128));
	RectangleShape energy2_bar; //создаем фон полоски энергии второго марсохода
	energy2_bar.setSize(Vector2f(200, 20));
	energy2_bar.setPosition(width - 220, 20);
	energy2_bar.setFillColor(Color(0, 0, 0, 128));

	RectangleShape hp2; //полоска жизней второго марсохода
	hp2.setSize(Vector2f(200, 20));
	hp2.setPosition(width - 220, 50);
	hp2.setFillColor(Color(255, 0, 0, 128));
	RectangleShape hp2_bar; //создаем фон полоски жизней второго марсохода
	hp2_bar.setSize(Vector2f(200, 20));
	hp2_bar.setPosition(width - 220, 50);
	hp2_bar.setFillColor(Color(0, 0, 0, 128));

	obj[1].sprite.setTexture(texture2);
	obj[1].sprite.setOrigin(25, 40);
	obj[1].x = 800;
	obj[1].y = 400;
	obj[1].angle = 0;
	obj[1].v = 0;

	obj[1].mass = 1;
	obj[1].energy = 1000; //энергия марсохода
	obj[1].hp = 1000; //жизни марсохода

	obj[1].n = 8;
	obj[1].c[0].a = -38.88;
	obj[1].c[0].l = 40;
	obj[1].c[1].a = 38.88;
	obj[1].c[1].l = 40;
	obj[1].c[2].a = 141.12;
	obj[1].c[2].l = 40;
	obj[1].c[3].a = -158;
	obj[1].c[3].l = 34;
	obj[1].c[4].a = -168;
	obj[1].c[4].l = 40;
	obj[1].c[5].a = 168;
	obj[1].c[5].l = 40;
	obj[1].c[6].a = 158;
	obj[1].c[6].l = 34;
	obj[1].c[7].a = -141.12;
	obj[1].c[7].l = 40;
	obj[1].max_dist = 40;

	obj[1].s[0].a = 90;
	obj[1].s[0].l = 50;
	obj[1].s[1].a = 180;
	obj[1].s[1].l = 62;
	obj[1].s[2].a = -90;
	obj[1].s[2].l = 13;
	obj[1].s[3].a = 26.57;
	obj[1].s[3].l = 9;
	obj[1].s[4].a = -90;
	obj[1].s[4].l = 15;
	obj[1].s[5].a = 153.43;
	obj[1].s[5].l = 9;
	obj[1].s[6].a = -90;
	obj[1].s[6].l = 13;
	obj[1].s[7].a = 0;
	obj[1].s[7].l = 62;

	for (int i = 0; i < 10; i++)
	{
		obj[1].bullet[i].setTexture(texture_bullet);
		obj[1].bullet[i].setPosition(-10, -10);
		obj[1].bullet[i].setOrigin(3, 7);
	}
	obj[1].bullet_shot = 0; //биты состояния пуль 1 - летит / 0 - ожидание
	obj[1].reg = 1; //режим стрельбы второго марсохода
	obj[1].enable = true;
	bool shot1_press = false; //предыдущее состояние кнопки выстрела

	obj[2].sprite.setTexture(texture3);
	obj[2].sprite.setOrigin(11, 11);
	obj[2].x = 600;
	obj[2].y = 600;
	obj[2].angle = 0;
	obj[2].mass = 100000;
	obj[2].n = 4;
	obj[2].c[0].a = -45;
	obj[2].c[0].l = 15.55;
	obj[2].c[1].a = 45;
	obj[2].c[1].l = 15.55;
	obj[2].c[2].a = 135;
	obj[2].c[2].l = 15.55;
	obj[2].c[3].a = -135;
	obj[2].c[3].l = 15.55;
	obj[2].max_dist = 15.55;

	obj[2].s[0].a = 90;
	obj[2].s[0].l = 21;
	obj[2].s[1].a = 180;
	obj[2].s[1].l = 21;
	obj[2].s[2].a = -90;
	obj[2].s[2].l = 21;
	obj[2].s[3].a = 0;
	obj[2].s[3].l = 21;

	/*Texture texture5;
	texture5.loadFromFile("..\\wall_corner1.png");*/

	/*Texture texture4;
	texture4.loadFromFile("C:\\С++\\infa_project\\prog1\\base1.png");
	obj[3].sprite.setTexture(texture4);
	obj[3].sprite.setOrigin(50, 50);
	obj[3].x = 800;
	obj[3].y = 600;
	obj[3].angle = 0;
	obj[3].mass = 10000000;

	obj[3].n = 10;
	obj[3].c[0].a = -55;
	obj[3].c[0].l = 61;
	obj[3].c[1].a = -35;
	obj[3].c[1].l = 61;
	obj[3].c[2].a = 35;
	obj[3].c[2].l = 61;
	obj[3].c[3].a = 55;
	obj[3].c[3].l = 61;
	obj[3].c[4].a = 125;
	obj[3].c[4].l = 61;
	obj[3].c[5].a = 145;
	obj[3].c[5].l = 61;
	obj[3].c[6].a = 41.2;
	obj[3].c[6].l = 53;
	obj[3].c[7].a = -41.2;
	obj[3].c[7].l = 53;
	obj[3].c[8].a = -145;
	obj[3].c[8].l = 61;
	obj[3].c[9].a = -125;
	obj[3].c[9].l = 61;

	obj[3].s[0].a = 45;
	obj[3].s[0].l = 22;
	obj[3].s[1].a = 90;
	obj[3].s[1].l = 7;
	obj[3].s[2].a = 135;
	obj[3].s[2].l = 22;
	obj[3].s[3].a = 180;
	obj[3].s[3].l = 70;
	obj[3].s[4].a = -135;
	obj[3].s[4].l = 22;
	obj[3].s[5].a = 180;
	obj[3].s[6].a = -90;
	obj[3].s[6].l = 70;
	obj[3].s[7].a = 0;
	obj[3].s[8].a = -45;
	obj[3].s[8].l = 22;
	obj[3].s[9].a = 0;
	obj[3].s[9].l = 70;

	obj[3].max_dist = 61;*/

	/*int n = 3;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			obj[n].sprite.setTexture(texture3);
			obj[n].sprite.setOrigin(11, 11);
			obj[n].x = 13 + i * 22;
			obj[n].y = 13 + j * 22;
			obj[n].angle = 0;
			obj[n].mass = 0.5;
			obj[n].n = 4;
			obj[n].c[0].a = -45;
			obj[n].c[0].l = 15.55;
			obj[n].c[1].a = 45;
			obj[n].c[1].l = 15.55;
			obj[n].c[2].a = 135;
			obj[n].c[2].l = 15.55;
			obj[n].c[3].a = -135;
			obj[n].c[3].l = 15.55;
			obj[n].max_dist = 15.55;

			obj[n].s[0].a = 90;
			obj[n].s[0].l = 21;
			obj[n].s[1].a = 180;
			obj[n].s[1].l = 21;
			obj[n].s[2].a = -90;
			obj[n].s[2].l = 21;
			obj[n].s[3].a = 0;
			obj[n].s[3].l = 21;

			n++;
		}
	}*/

	for (int n = 3; n < obj_n; n++)
	{
		int object_type = 1 + 4 * rand() / RAND_MAX; //тип объекта 1 - ящик, 2 - стена, 3 - структура: три ящика и две стены, 4 - структура: две стены углом
		//int object_type = 1;
		if (object_type == 1)
		{
			obj[n].sprite.setTexture(texture3);
			obj[n].sprite.setOrigin(11, 11);
			obj[n].x = 200 + (width - 400) * rand() / RAND_MAX;
			obj[n].y = 100 + (height - 200) * rand() / RAND_MAX;
			obj[n].angle = 360 * rand() / RAND_MAX;
			obj[n].mass = 0.5;
			obj[n].n = 4;
			obj[n].c[0].a = -45;
			obj[n].c[0].l = 15.55;
			obj[n].c[1].a = 45;
			obj[n].c[1].l = 15.55;
			obj[n].c[2].a = 135;
			obj[n].c[2].l = 15.55;
			obj[n].c[3].a = -135;
			obj[n].c[3].l = 15.55;
			obj[n].max_dist = 15.55;

			obj[n].s[0].a = 90;
			obj[n].s[0].l = 21;
			obj[n].s[1].a = 180;
			obj[n].s[1].l = 21;
			obj[n].s[2].a = -90;
			obj[n].s[2].l = 21;
			obj[n].s[3].a = 0;
			obj[n].s[3].l = 21;
		}
		else if (object_type == 2)
		{
			obj[n].sprite.setTexture(texture4);
			obj[n].sprite.setOrigin(16, 48);
			obj[n].x = 200 + (width - 400) * rand() / RAND_MAX;
			obj[n].y = 100 + (height - 200) * rand() / RAND_MAX;
			obj[n].angle = 360 * rand() / RAND_MAX;
			obj[n].mass = 2.5;
			obj[n].n = 4;
			obj[n].c[0].a = -7.71;
			obj[n].c[0].l = 48.44;
			obj[n].c[1].a = 7.71;
			obj[n].c[1].l = 48.44;
			obj[n].c[2].a = 172.29;
			obj[n].c[2].l = 48.44;
			obj[n].c[3].a = -172.29;
			obj[n].c[3].l = 48.44;
			obj[n].max_dist = 48.44;

			obj[n].s[0].a = 90;
			obj[n].s[0].l = 21;
			obj[n].s[1].a = 180;
			obj[n].s[1].l = 21;
			obj[n].s[2].a = -90;
			obj[n].s[2].l = 21;
			obj[n].s[3].a = 0;
			obj[n].s[3].l = 21;
		}
		else if (object_type == 3)
		{
			int ver = 10 * rand() / RAND_MAX;
			if (obj_n - n >= 5 && ver > 7)
			{
				int corner = 4 * rand() / RAND_MAX;
				//int corner = 1;

				double x_obj = 0;
				double y_obj = 0;
				double angle_obj = 0;

				if (corner == 0)
				{
					x_obj = 200 + 50 * rand() / RAND_MAX;
					y_obj = 100 + (height - 100) * rand() / RAND_MAX;
					angle_obj = 57.3*atan((x_obj - width / 2) / (y_obj - height / 2)) + 90;
					if (height / 2 > y_obj) angle_obj -= 180;
				}
				if (corner == 1)
				{
					x_obj = width - 200 + 50 * rand() / RAND_MAX;
					y_obj = 100 + (height - 100) * rand() / RAND_MAX;
					angle_obj = 57.3*atan((x_obj - width / 2) / (y_obj - height / 2)) + 90;
					if (height / 2 > y_obj) angle_obj -= 180;
				}
				if (corner == 2)
				{
					x_obj = 100 + (width - 200) * rand() / RAND_MAX;
					y_obj = 100 + 50 * rand() / RAND_MAX;
					angle_obj = 57.3*atan((x_obj - width / 2) / (y_obj - height / 2)) - 90;
				}
				if (corner == 3)
				{
					x_obj = 100 + (width - 200) * rand() / RAND_MAX;
					y_obj = height - 100 + 50 * rand() / RAND_MAX;
					angle_obj = 57.3*atan((x_obj - width / 2) / (y_obj - height / 2)) + 90;
				}

				obj[n].sprite.setTexture(texture4);
				obj[n].sprite.setOrigin(16, 48);
				obj[n].x = x_obj;
				obj[n].y = y_obj;
				obj[n].angle = angle_obj;
				obj[n].mass = 2.5;
				obj[n].n = 4;
				obj[n].c[0].a = -7.71;
				obj[n].c[0].l = 48.44;
				obj[n].c[1].a = 7.71;
				obj[n].c[1].l = 48.44;
				obj[n].c[2].a = 172.29;
				obj[n].c[2].l = 48.44;
				obj[n].c[3].a = -172.29;
				obj[n].c[3].l = 48.44;
				obj[n].max_dist = 48.44;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;

				n++;

				obj[n].sprite.setTexture(texture4);
				obj[n].sprite.setOrigin(16, 48);
				obj[n].x = x_obj - 98 * sin(angle_obj / 57.3);
				obj[n].y = y_obj - 98 * cos(angle_obj / 57.3);
				obj[n].angle = angle_obj;
				obj[n].mass = 2.5;
				obj[n].n = 4;
				obj[n].c[0].a = -7.71;
				obj[n].c[0].l = 48.44;
				obj[n].c[1].a = 7.71;
				obj[n].c[1].l = 48.44;
				obj[n].c[2].a = 172.29;
				obj[n].c[2].l = 48.44;
				obj[n].c[3].a = -172.29;
				obj[n].c[3].l = 48.44;
				obj[n].max_dist = 48.44;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;

				n++;

				obj[n].sprite.setTexture(texture3);
				obj[n].sprite.setOrigin(11, 11);
				obj[n].x = x_obj - 22.67 * sin((angle_obj + 48.58) / 57.3);
				obj[n].y = y_obj - 22.67 * cos((angle_obj + 48.58) / 57.3);
				obj[n].angle = angle_obj;
				obj[n].mass = 0.5;
				obj[n].n = 4;
				obj[n].c[0].a = -45;
				obj[n].c[0].l = 15.55;
				obj[n].c[1].a = 45;
				obj[n].c[1].l = 15.55;
				obj[n].c[2].a = 135;
				obj[n].c[2].l = 15.55;
				obj[n].c[3].a = -135;
				obj[n].c[3].l = 15.55;
				obj[n].max_dist = 15.55;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;

				n++;

				obj[n].sprite.setTexture(texture3);
				obj[n].sprite.setOrigin(11, 11);
				obj[n].x = x_obj - 41.63 * sin((angle_obj + 24.1) / 57.3);
				obj[n].y = y_obj - 41.63 * cos((angle_obj + 24.1) / 57.3);
				obj[n].angle = angle_obj;
				obj[n].mass = 0.5;
				obj[n].n = 4;
				obj[n].c[0].a = -45;
				obj[n].c[0].l = 15.55;
				obj[n].c[1].a = 45;
				obj[n].c[1].l = 15.55;
				obj[n].c[2].a = 135;
				obj[n].c[2].l = 15.55;
				obj[n].c[3].a = -135;
				obj[n].c[3].l = 15.55;
				obj[n].max_dist = 15.55;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;

				n++;

				obj[n].sprite.setTexture(texture3);
				obj[n].sprite.setOrigin(11, 11);
				obj[n].x = x_obj - 48.26 * sin((angle_obj + 55.98) / 57.3);
				obj[n].y = y_obj - 48.26 * cos((angle_obj + 55.98) / 57.3);
				obj[n].angle = angle_obj;
				obj[n].mass = 0.5;
				obj[n].n = 4;
				obj[n].c[0].a = -45;
				obj[n].c[0].l = 15.55;
				obj[n].c[1].a = 45;
				obj[n].c[1].l = 15.55;
				obj[n].c[2].a = 135;
				obj[n].c[2].l = 15.55;
				obj[n].c[3].a = -135;
				obj[n].c[3].l = 15.55;
				obj[n].max_dist = 15.55;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;
			}
			else
			{
				n--;
			}
		}
		else if (object_type == 4)
		{
			int ver = 10 * rand() / RAND_MAX;
			if (obj_n - n >= 2 && ver > 6)
			{
				double x_obj = 100 + (width - 200) * rand() / RAND_MAX;
				double y_obj = 50 + (height - 100) * rand() / RAND_MAX;
				//double angle_obj = 360 * rand() / RAND_MAX;
				double angle_obj = 57.3*atan((x_obj - width / 2) / (y_obj - height / 2)) + 45;
				if (height / 2 > y_obj) angle_obj -= 180;


				obj[n].sprite.setTexture(texture4);
				obj[n].sprite.setOrigin(16, 48);
				obj[n].x = x_obj;
				obj[n].y = y_obj;
				obj[n].angle = angle_obj;
				obj[n].mass = 2.5;
				obj[n].n = 4;
				obj[n].c[0].a = -7.71;
				obj[n].c[0].l = 48.44;
				obj[n].c[1].a = 7.71;
				obj[n].c[1].l = 48.44;
				obj[n].c[2].a = 172.29;
				obj[n].c[2].l = 48.44;
				obj[n].c[3].a = -172.29;
				obj[n].c[3].l = 48.44;
				obj[n].max_dist = 48.44;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;

				n++;

				obj[n].sprite.setTexture(texture4);
				obj[n].sprite.setOrigin(16, 48);
				obj[n].x = x_obj - 71 * sin((angle_obj + 45) / 57.3);
				obj[n].y = y_obj - 71 * cos((angle_obj + 45) / 57.3);
				obj[n].angle = angle_obj + 90;
				obj[n].mass = 2.5;
				obj[n].n = 4;
				obj[n].c[0].a = -7.71;
				obj[n].c[0].l = 48.44;
				obj[n].c[1].a = 7.71;
				obj[n].c[1].l = 48.44;
				obj[n].c[2].a = 172.29;
				obj[n].c[2].l = 48.44;
				obj[n].c[3].a = -172.29;
				obj[n].c[3].l = 48.44;
				obj[n].max_dist = 48.44;

				obj[n].s[0].a = 90;
				obj[n].s[0].l = 21;
				obj[n].s[1].a = 180;
				obj[n].s[1].l = 21;
				obj[n].s[2].a = -90;
				obj[n].s[2].l = 21;
				obj[n].s[3].a = 0;
				obj[n].s[3].l = 21;
			}
			else
			{
				n--;
			}
		}
	}

	obj[obj_n].sprite.setTexture(texture_health);
	obj[obj_n+1].sprite.setTexture(texture_health);
	obj[obj_n+2].sprite.setTexture(texture_triple);
	obj[obj_n+3].sprite.setTexture(texture_triple);
	obj[obj_n+4].sprite.setTexture(texture_double);

	for (int n = obj_n; n < obj_n + bonus_n; n++)
	{
		obj[n].sprite.setOrigin(11, 11);
		obj[n].x = -100;
		obj[n].y = -100;
		obj[n].angle = 0;
		obj[n].mass = 0.5;
		obj[n].enable = false;
		obj[n].n = 4;
		obj[n].c[0].a = -45;
		obj[n].c[0].l = 15.55;
		obj[n].c[1].a = 45;
		obj[n].c[1].l = 15.55;
		obj[n].c[2].a = 135;
		obj[n].c[2].l = 15.55;
		obj[n].c[3].a = -135;
		obj[n].c[3].l = 15.55;
		obj[n].max_dist = 15.55;

		obj[n].s[0].a = 90;
		obj[n].s[0].l = 21;
		obj[n].s[1].a = 180;
		obj[n].s[1].l = 21;
		obj[n].s[2].a = -90;
		obj[n].s[2].l = 21;
		obj[n].s[3].a = 0;
		obj[n].s[3].l = 21;
	}

	//Sprite sprite(texture);
	//sprite.setOrigin(25, 40);
	//sprite.setPosition(400, 300);
	/*vector v1, v2, v3, v4;
	v1.a = -31.61;
	v1.l = 39.2;
	v2.a = 31.61;
	v2.l = 39.2;
	v3.a = 148.39;
	v3.l = 39.2;
	v4.a = -148.39;
	v4.l = 39.2;*/
	/*float x = 400;
	float y = 400;
	float v = 0;
	float angle = 0;
	int shot = 0;*/

	/*Texture texture1;
	texture1.loadFromFile("C:\\С++\\infa_project\\prog1\\bull.png");
	Sprite bull1(texture1);
	bull1.setOrigin(2, 3);
	float angle_b = 0;*/

	/*ConvexShape convex;
	convex.setPointCount(4);
	convex.setFillColor(Color::Black);*/

	/*ConvexShape convex1;
	convex1.setPointCount(7);
	convex1.setFillColor(Color::Red);*/

	while (window.isOpen())
	{

		if (KEY_PUSH(VK_SPACE))
		{
			Texture pause;
			pause.loadFromFile("..\\pause1.png");
			Sprite pause_sprite;
			pause_sprite.setTexture(pause);
			pause_sprite.setScale(2, 2);
			pause_sprite.setPosition(width / 2 - 242, height / 2 - 86);
			window.draw(pause_sprite);
			window.display();
			while (1)
			{
				bool exit_flag = false;
				Event event; //обработка закрытия окна
				while (window.pollEvent(event))
				{
					if (event.type == Event::Closed)
					{
						window.close();
						exit_flag = true;
					}
				}
				if (KEY_PUSH(VK_ESCAPE)) window.close();
				if (KEY_PUSH(VK_SPACE)) exit_flag = true;
				if (exit_flag) break;
			}
		}

		Event event; //обработка закрытия окна
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		if (KEY_PUSH(VK_ESCAPE)) window.close();

		int bonus_count = 0;
		for (int i = obj_n; i < obj_n + bonus_n; i++)
		{
			bonus_count += obj[i].enable;
		}
		if (bonus_count < 3)
		{
			if (1.0 * rand() / RAND_MAX > 0.996)
			{
				int i = 0;
				while (1)
				{
					i = obj_n + (bonus_n + 1)*rand() / RAND_MAX;
					if (!obj[i].enable) break;
				}
				obj[i].enable = true;
				bool bonus_flag = false;
				while (!bonus_flag)
				{
					obj[i].x = 100 + (width - 200) * rand() / RAND_MAX;
					obj[i].y = 100 + (height - 200) * rand() / RAND_MAX;
					obj[i].angle = 360 * rand() / RAND_MAX;
					
					bonus_flag = true;
					if (sq(obj[i].x - obj[0].x) + sq(obj[i].y - obj[0].y) < sq(bonus_range)) bonus_flag = false;
					if (sq(obj[i].x - obj[1].x) + sq(obj[i].y - obj[1].y) < sq(bonus_range)) bonus_flag = false;

					/*for (int j = obj_n; j < obj_n + bonus_n; j++)
					{
						if (sq(obj[i].x - obj[j].x) + sq(obj[i].y - obj[j].y) < sq(bonus_range)) bonus_flag = false;
					}*/
				}
			}
		}
		
		if (KEY_DOWN(VK_LCONTROL) && obj[0].hp > 0) obj[0].energy -= 4; //включение режима ускорения
		if (KEY_DOWN(0x57) && obj[0].hp > 0) //обработка нажатий клавиш движения
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].v = -v_max;
			else obj[0].v = -v_max * 1.5;
		}
		if (KEY_DOWN(0x53) && obj[0].hp > 0)
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].v = v_max;
			else obj[0].v = v_max * 1.5;
		}
		if (KEY_DOWN(0x41) && obj[0].hp > 0)
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].angle += angle_max;
			else obj[0].angle += angle_max * 1.5;
		}
		if (KEY_DOWN(0x44) && obj[0].hp > 0)
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].angle -= angle_max;
			else obj[0].angle -= angle_max * 1.5;
		}

		if (KEY_DOWN(VK_LSHIFT) && shot0_press == 0 && obj[0].hp > 0) //обработка выстрела 
		{
			if (obj[0].reg == 1 && obj[0].energy > 300) //проверка режима и энергии | одиночный режим
			{
				int i = 0;
				while ((obj[0].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[0].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[0].bullet_angle[i] = obj[0].angle; //задаем направление и положение
				obj[0].bullet_x[i] = obj[0].x - 15 * sin(obj[0].angle / 57.3);
				obj[0].bullet_y[i] = obj[0].y - 15 * cos(obj[0].angle / 57.3);
				obj[0].energy -= 300;

				obj[0].x -= bullet_a * sin(-obj[0].angle / 57.3); //отдача
				obj[0].y += bullet_a * cos(-obj[0].angle / 57.3);
			}
			if (obj[0].reg == 2 && obj[0].energy > 500) //проверка режима и энергии | режим тройного выстрела
			{
				int i = 0;
				while ((obj[0].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[0].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[0].bullet_angle[i] = obj[0].angle; //задаем направление и положение
				obj[0].bullet_x[i] = obj[0].x - 15 * sin(obj[0].angle / 57.3);
				obj[0].bullet_y[i] = obj[0].y - 15 * cos(obj[0].angle / 57.3);
				i = 0;
				while ((obj[0].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[0].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[0].bullet_angle[i] = obj[0].angle + 10; //задаем направление и положение
				obj[0].bullet_x[i] = obj[0].x - 15 * sin(obj[0].angle / 57.3);
				obj[0].bullet_y[i] = obj[0].y - 15 * cos(obj[0].angle / 57.3);
				i = 0;
				while ((obj[0].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[0].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[0].bullet_angle[i] = obj[0].angle - 10;//задаем направление и положение
				obj[0].bullet_x[i] = obj[0].x - 15 * sin(obj[0].angle / 57.3);
				obj[0].bullet_y[i] = obj[0].y - 15 * cos(obj[0].angle / 57.3);
				obj[0].energy -= 500;

				obj[0].x -= 1.6 * bullet_a * sin(-obj[0].angle / 57.3); //отдача
				obj[0].y += 1.6 * bullet_a * cos(-obj[0].angle / 57.3);
			}
			if (obj[0].reg == 3 && obj[0].energy > 400) //проверка режима и энергии | режим двойного выстрела
			{
				int i = 0;
				while ((obj[0].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[0].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[0].bullet_angle[i] = obj[0].angle + 4 * rand() / RAND_MAX; //задаем направление и положение
				obj[0].bullet_x[i] = obj[0].x - 15 * sin(obj[0].angle / 57.3) - 8 * sin((obj[0].angle + 90) / 57.3);
				obj[0].bullet_y[i] = obj[0].y - 15 * cos(obj[0].angle / 57.3) - 8 * cos((obj[0].angle + 90) / 57.3);
				i = 0;
				while ((obj[0].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[0].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[0].bullet_angle[i] = obj[0].angle - 4 * rand() / RAND_MAX;;//задаем направление и положение
				obj[0].bullet_x[i] = obj[0].x - 15 * sin(obj[0].angle / 57.3) - 8 * sin((obj[0].angle - 90) / 57.3);
				obj[0].bullet_y[i] = obj[0].y - 15 * cos(obj[0].angle / 57.3) - 8 * cos((obj[0].angle - 90) / 57.3);
				obj[0].energy -= 400;

				obj[0].x -= 1.8 * bullet_a * sin(-obj[0].angle / 57.3); //отдача
				obj[0].y += 1.8 * bullet_a * cos(-obj[0].angle / 57.3);
			}
			shot0_press = 1;
		}
		if (!KEY_DOWN(VK_LSHIFT))shot0_press = 0;

		energy1.setSize(Vector2f(obj[0].energy/5, 20)); //установка размера полоски энергии
		obj[0].energy+=2; //накопление энергии
		constrain(obj[0].energy, 0, 1000);

		hp1.setSize(Vector2f(obj[0].hp / 5, 20)); //установка размера полоски жизней
		constrain(obj[0].hp, 0, 1000);

		energy2.setSize(Vector2f(obj[1].energy / 5, 20)); //установка размера полоски энергии
		obj[1].energy += 2; //накопление энергии
		constrain(obj[1].energy, 0, 1000);

		hp2.setSize(Vector2f(obj[1].hp / 5, 20)); //установка размера полоски жизней
		constrain(obj[1].hp, 0, 1000);


		if (KEY_DOWN(VK_RCONTROL) && obj[1].hp > 0) obj[1].energy -= 4; //включение режима ускорения
		if (KEY_DOWN(VK_DOWN) && obj[1].hp > 0) //отработка нажатий клавиш
		{
			if (!KEY_DOWN(VK_RCONTROL) || obj[1].energy < 0) obj[1].v = -v_max;
			else obj[1].v = -v_max * 1.5;
		}
		if (KEY_DOWN(VK_UP) && obj[1].hp > 0)
		{
			if (!KEY_DOWN(VK_RCONTROL) || obj[1].energy < 0) obj[1].v = v_max;
			else obj[1].v = v_max * 1.5;
		}
		if (KEY_DOWN(VK_LEFT) && obj[1].hp > 0)
		{
			if (!KEY_DOWN(VK_RCONTROL) || obj[1].energy < 0) obj[1].angle += angle_max;
			else obj[1].angle += angle_max * 1.5;
		}
		if (KEY_DOWN(VK_RIGHT) && obj[1].hp > 0)
		{
			if (!KEY_DOWN(VK_RCONTROL) || obj[1].energy < 0) obj[1].angle -= angle_max;
			else obj[1].angle -= angle_max * 1.5;
		}

		if (KEY_DOWN(VK_RSHIFT) && shot1_press == 0 && obj[1].hp > 0) //обработка выстрела 
		{
			if (obj[1].reg == 1 && obj[1].energy > 300) //проверка режима и энергии | одиночный режим
			{
				int i = 0;
				while ((obj[1].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[1].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[1].bullet_angle[i] = obj[1].angle; //задаем направление и положение
				obj[1].bullet_x[i] = obj[1].x - 15 * sin(obj[1].angle / 57.3);
				obj[1].bullet_y[i] = obj[1].y - 15 * cos(obj[1].angle / 57.3);
				obj[1].energy -= 300;

				obj[1].x -= bullet_a * sin(-obj[1].angle / 57.3); //отдача
				obj[1].y += bullet_a * cos(-obj[1].angle / 57.3);
			}
			if (obj[1].reg == 2 && obj[1].energy > 500) //проверка режима и энергии | режим тройного выстрела
			{
				int i = 0;
				while ((obj[1].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[1].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[1].bullet_angle[i] = obj[1].angle; //задаем направление и положение
				obj[1].bullet_x[i] = obj[1].x - 15 * sin(obj[1].angle / 57.3);
				obj[1].bullet_y[i] = obj[1].y - 15 * cos(obj[1].angle / 57.3);
				i = 0;
				while ((obj[1].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[1].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[1].bullet_angle[i] = obj[1].angle + 10; //задаем направление и положение
				obj[1].bullet_x[i] = obj[1].x - 15 * sin(obj[1].angle / 57.3);
				obj[1].bullet_y[i] = obj[1].y - 15 * cos(obj[1].angle / 57.3);
				i = 0;
				while ((obj[1].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[1].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[1].bullet_angle[i] = obj[1].angle - 10;//задаем направление и положение
				obj[1].bullet_x[i] = obj[1].x - 15 * sin(obj[1].angle / 57.3);
				obj[1].bullet_y[i] = obj[1].y - 15 * cos(obj[1].angle / 57.3);
				obj[1].energy -= 500;

				obj[1].x -= 1.6 * bullet_a * sin(-obj[1].angle / 57.3); //отдача
				obj[1].y += 1.6 * bullet_a * cos(-obj[1].angle / 57.3);
			}
			if (obj[1].reg == 3 && obj[0].energy > 400) //проверка режима и энергии | режим двойного выстрела
			{
				int i = 0;
				while ((obj[1].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[1].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[1].bullet_angle[i] = obj[1].angle + 4 * rand() / RAND_MAX; //задаем направление и положение
				obj[1].bullet_x[i] = obj[1].x - 15 * sin(obj[1].angle / 57.3) - 8 * sin((obj[1].angle + 90) / 57.3);
				obj[1].bullet_y[i] = obj[1].y - 15 * cos(obj[1].angle / 57.3) - 8 * cos((obj[1].angle + 90) / 57.3);
				i = 0;
				while ((obj[1].bullet_shot >> i) % 2) //поиск свободной пули
				{
					i++;
				}
				obj[1].bullet_shot |= 1 << i; //устанавливаем бит полета пули
				obj[1].bullet_angle[i] = obj[1].angle - 4 * rand() / RAND_MAX;;//задаем направление и положение
				obj[1].bullet_x[i] = obj[1].x - 15 * sin(obj[1].angle / 57.3) - 8 * sin((obj[1].angle - 90) / 57.3);
				obj[1].bullet_y[i] = obj[1].y - 15 * cos(obj[1].angle / 57.3) - 8 * cos((obj[1].angle - 90) / 57.3);
				obj[1].energy -= 400;

				obj[1].x -= 1.8 * bullet_a * sin(-obj[1].angle / 57.3); //отдача
				obj[1].y += 1.8 * bullet_a * cos(-obj[1].angle / 57.3);
			}
			shot1_press = 1;
		}
		if (!KEY_DOWN(VK_RSHIFT))shot1_press = 0;
		
		obj[0].x -= obj[0].v * sin(-obj[0].angle / 57.3); //изменение позициии первого марсохода 
		obj[0].y += obj[0].v * cos(-obj[0].angle / 57.3);

		obj[1].x += obj[1].v * sin(-obj[1].angle / 57.3); //изменение позиции второго марсохода
		obj[1].y -= obj[1].v * cos(-obj[1].angle / 57.3);

		for (int i = 0; i < obj_n + bonus_n; i++) //цикл обработки столкновений каждого объекта
		{
			obj[i].dx = 0;
			obj[i].dy = 0;
			obj[i].da = 0;
			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) < 0 && obj[i].enable)
				{
					obj[i].dx -= obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3);
					obj[i].da -= (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3))*cos((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}

			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3) < 0 && obj[i].enable)
				{
					obj[i].dy -= obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3);
					obj[i].da -= (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3))*sin((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}

			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) > width && obj[i].enable)
				{
					obj[i].dx -= obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) - width;
					obj[i].da -= (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) - width)*cos((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}

			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3) > height && obj[i].enable)
				{
					obj[i].dy -= obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3) - height;
					obj[i].da -= (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3) - height)*sin((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}

			obj[i].angle += obj[i].da * 57.3; //применение поправок к положению
			obj[i].x += obj[i].dx;
			obj[i].y += obj[i].dy;

			obj[i].dx = 0;
			obj[i].dy = 0;
			obj[i].da = 0;

			if (obj[i].enable)
			{
				for (int k = 0; k < obj_n + bonus_n; k++) //перебираем другие объекты
				{
					if (k != i && sq(obj[i].x - obj[k].x) + sq(obj[i].y - obj[k].y) < sq(obj[i].max_dist + obj[k].max_dist + 10) && obj[k].enable) //если это не текущий объект и он не далеко
					{
						for (int j = 0; j < obj[i].n; j++) //перебираем углы текущего объекта
						{
							for (int n = 0; n < obj[k].n; n++) //перебираем стороны другого объекта
							{
								pt a, b, c, d;
								a.x = obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3); //координаты угла текущего объекта
								a.y = obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3);
								b.x = obj[i].x; //координаты центра текущего объекта
								b.y = obj[i].y;
								c.x = obj[k].x + obj[k].c[n].l * sin((obj[k].angle + obj[k].c[n].a) / 57.3); //координата конца стороны другого объекта
								c.y = obj[k].y + obj[k].c[n].l * cos((obj[k].angle + obj[k].c[n].a) / 57.3);
								if (n < obj[k].n - 1)
								{
									d.x = obj[k].x + obj[k].c[n + 1].l * sin((obj[k].angle + obj[k].c[n + 1].a) / 57.3); //координата конца стороны другого объекта
									d.y = obj[k].y + obj[k].c[n + 1].l * cos((obj[k].angle + obj[k].c[n + 1].a) / 57.3);
								}
								else
								{
									d.x = obj[k].x + obj[k].c[0].l * sin((obj[k].angle + obj[k].c[0].a) / 57.3); //координата конца стороны другого объекта
									d.y = obj[k].y + obj[k].c[0].l * cos((obj[k].angle + obj[k].c[0].a) / 57.3);
								}
								if (intersect(a, b, c, d)) //проверка пересечения
								{
									double dl = abs((d.y - c.y)*a.x + (c.x - d.x)*a.y - (d.y - c.y)*c.x - (c.x - d.x)*c.y) / sqrt(sq(d.y - c.y) + sq(d.x - c.x)); //длина вектора выталкивания
									double d_angle = obj[k].angle + obj[k].s[n].a - 90; //напрваление вектора выталкивания
									obj[i].dx += dl * sin(d_angle / 57.3)*obj[k].mass / (obj[i].mass + obj[k].mass); //применение к текущему объекту
									obj[i].dy += dl * cos(d_angle / 57.3)*obj[k].mass / (obj[i].mass + obj[k].mass);
									obj[i].da -= dl * sin((180 - obj[i].c[j].a - obj[i].angle + d_angle) / 57.3) / obj[i].c[j].l*obj[k].mass / (obj[i].mass + obj[k].mass);
									obj[k].x -= dl * sin(d_angle / 57.3)*obj[i].mass / (obj[i].mass + obj[k].mass); //применение к другому объекту
									obj[k].y -= dl * cos(d_angle / 57.3)*obj[i].mass / (obj[i].mass + obj[k].mass);
									double e_angle = 57.3*atan((b.x - obj[k].x) / (b.y - obj[k].y));
									if (b.x - obj[k].x < 0 && b.y - obj[k].y < 0) e_angle += 180;
									if (b.y - obj[k].y < 0 && b.x - obj[k].x > 0) e_angle += 180;
									double dl_k = sqrt(sq(b.x - obj[k].x) + sq(b.y - obj[k].y));
									obj[k].angle += 57.3*dl*sin((e_angle - d_angle) / 57.3) / dl_k * obj[i].mass / (obj[i].mass + obj[k].mass);

									if ((i == 0 || i == 1) && k == obj_n)
									{
										obj[i].hp += 200;
										obj[k].x = -100;
										obj[k].y = -100;
										obj[k].enable = false;
									}
									if ((k == 0 || k == 1) && i == obj_n)
									{
										obj[k].hp += 200;
										obj[i].x = -100;
										obj[i].y = -100;
										obj[i].enable = false;
									}
									if ((i == 0 || i == 1) && k == obj_n + 1)
									{
										obj[i].hp += 200;
										obj[k].x = -100;
										obj[k].y = -100;
										obj[k].enable = false;
									}
									if ((k == 0 || k == 1) && i == obj_n + 1)
									{
										obj[k].hp += 200;
										obj[i].x = -100;
										obj[i].y = -100;
										obj[i].enable = false;
									}
									if ((i == 0 || i == 1) && k == obj_n + 2)
									{
										obj[i].reg = 2;
										obj[k].x = -100;
										obj[k].y = -100;
										obj[k].enable = false;
									}
									if ((k == 0 || k == 1) && i == obj_n +2)
									{
										obj[k].reg = 2;
										obj[i].x = -100;
										obj[i].y = -100;
										obj[i].enable = false;
									}
									if ((i == 0 || i == 1) && k == obj_n + 3)
									{
										obj[i].reg = 2;
										obj[k].x = -100;
										obj[k].y = -100;
										obj[k].enable = false;
									}
									if ((k == 0 || k == 1) && i == obj_n + 3)
									{
										obj[k].reg = 2;
										obj[i].x = -100;
										obj[i].y = -100;
										obj[i].enable = false;
									}
									if ((i == 0 || i == 1) && k == obj_n + 4)
									{
										obj[i].reg = 3;
										obj[k].x = -100;
										obj[k].y = -100;
										obj[k].enable = false;
									}
									if ((k == 0 || k == 1) && i == obj_n + 4)
									{
										obj[k].reg = 3;
										obj[i].x = -100;
										obj[i].y = -100;
										obj[i].enable = false;
									}
									//A = (y2-y1)
									//B = (x2-x1)
									//C = -A*x1 - B*y1
								}
							}
						}

					}
				}
			}

			obj[i].angle += obj[i].da * 57.3;
			obj[i].x += obj[i].dx;
			obj[i].y += obj[i].dy;

			obj[i].sprite.setRotation(-obj[i].angle); //установка спрайта в нужное положение
			obj[i].sprite.setPosition(obj[i].x, obj[i].y);

			obj[i].v = 0;
		}

		window.draw(bg); //отрисовка фона
		for (int i = 0; i < obj_n + bonus_n; i++) //отрисовка спрайтов
		{
			window.draw(obj[i].sprite);
		}
		//window.draw(obj[0].bullet[0]);

		//CircleShape circle(3);
		//circle.setFillColor(Color(255, 0, 0));
		//CircleShape circle1(3);
		//circle1.setFillColor(Color(0, 255, 0));
		for (int i = 0; i < 10; i++) //обработка пуль первого объекта
		{
			if ((obj[0].bullet_shot >> i) % 2) //если пуля летит
			{
				obj[0].bullet[i].setPosition(obj[0].bullet_x[i], obj[0].bullet_y[i]); //задаем позицию спрайта
				obj[0].bullet[i].setRotation(-obj[0].bullet_angle[i]);
				double min_x = obj[0].bullet_x[i] - bullet_v * sin(obj[0].bullet_angle[i] / 57.3); //позиция точки куда пуля прилетит
				double min_y = obj[0].bullet_y[i] - bullet_v * cos(obj[0].bullet_angle[i] / 57.3);
				int n_object = -1; //номер обекта в который попали
				int s_object = -1; //сторона объекта в который попали
				for (int k = 0; k < obj_n + bonus_n; k++) //перебираем другие объекты
				{
					if (k != 0 && sq(obj[0].bullet_x[i] - obj[k].x) + sq(obj[0].bullet_y[i] - obj[k].y) < sq(bullet_v + obj[k].max_dist + 10)) //если они достаточно близко
					{
						for (int n = 0; n < obj[k].n; n++) //перебираем стороны объекта
						{
							pt a, b, c, d;
							a.x = obj[0].bullet_x[i] - bullet_v * sin(obj[0].bullet_angle[i] / 57.3);
							a.y = obj[0].bullet_y[i] - bullet_v * cos(obj[0].bullet_angle[i] / 57.3);
							//circle1.setPosition(a.x, a.y);
							b.x = obj[0].bullet_x[i];
							b.y = obj[0].bullet_y[i];
							c.x = obj[k].x + obj[k].c[n].l * sin((obj[k].angle + obj[k].c[n].a) / 57.3);
							c.y = obj[k].y + obj[k].c[n].l * cos((obj[k].angle + obj[k].c[n].a) / 57.3);
							if (n < obj[k].n - 1)
							{
								d.x = obj[k].x + obj[k].c[n + 1].l * sin((obj[k].angle + obj[k].c[n + 1].a) / 57.3);
								d.y = obj[k].y + obj[k].c[n + 1].l * cos((obj[k].angle + obj[k].c[n + 1].a) / 57.3);
							}
							else
							{
								d.x = obj[k].x + obj[k].c[0].l * sin((obj[k].angle + obj[k].c[0].a) / 57.3);
								d.y = obj[k].y + obj[k].c[0].l * cos((obj[k].angle + obj[k].c[0].a) / 57.3);
							}
							if (intersect(a, b, c, d)) //проверяем пересечение траектории пули с стороной объекта
							{
								double k1 = (a.y - b.y) / (a.x - b.x);
								double p1 = a.y - k1 * a.x;

								double k2 = (c.y - d.y) / (c.x - d.x);
								double p2 = c.y - k2 * c.x;

								double x0 = (p1 - p2) / (k2 - k1); //вычисляем точку пересечения
								double y0 = k1 * x0 + p1;
								//circle.setPosition(x0, y0);
								if (sq(b.x - x0) + sq(b.y - y0) < sq(b.x - min_x) + sq(b.y - min_y)) //если она ближе к текущему положению пули
								{
									min_x = x0; //записываем точку пересечения с объектом
									min_y = y0;
									n_object = k; //номер объекта
									s_object = n; //и его сторону
								}
							}
						}
					}
				}
				min_x += 3 * sin(obj[0].bullet_angle[i] / 57.3); //немного сдвигаем пулю назад
				min_y += 3 * cos(obj[0].bullet_angle[i] / 57.3);
				if (n_object != -1) //если пересечение с объектом было
				{
					double d_angle = obj[n_object].angle + obj[n_object].s[s_object].a - 90; //вычисляем отталкивание объекта
					obj[n_object].x -= bullet_o * sin(obj[0].bullet_angle[i] / 57.3) / obj[n_object].mass;
					obj[n_object].y -= bullet_o * cos(obj[0].bullet_angle[i] / 57.3) / obj[n_object].mass;
					double dl_k = sqrt(sq(min_x - obj[n_object].x) + sq(min_y - obj[n_object].y));
					double e_angle = 57.3*atan((obj[n_object].x - min_x) / (obj[n_object].y - min_y));
					if (obj[n_object].x - min_x < 0 && obj[n_object].y - min_y < 0) e_angle += 180;
					if (obj[n_object].y - min_y < 0 && obj[n_object].x - min_x > 0) e_angle += 180;
					obj[n_object].angle -= 57.3*bullet_o*sin((e_angle - obj[0].bullet_angle[i]) / 57.3) / dl_k / obj[n_object].mass;

					if (obj[n_object].hp != 0)
					{
						obj[n_object].hp -= 200; //вычитаем здоровье
						obj[n_object].reg = 1; //сбиваем модификатор оружия
						if (obj[n_object].hp <= 0) //если здоровье упало ниже нуля
						{
							obj[n_object].hp = 0;
							obj[n_object].sprite.setTexture(texture0); //то заменяем текстуру на уничтоженный объект
						}
					}

					obj[0].bullet_shot &= ~(1 << i); //деактивируем пулю
				}
				obj[0].bullet_x[i] = min_x;
				obj[0].bullet_y[i] = min_y;
				obj[0].bullet[i].setPosition(obj[0].bullet_x[i], obj[0].bullet_y[i]); //двигаем пулю на новое место
			}
			else
			{
				obj[0].bullet_x[i] = -30;
				obj[0].bullet_y[i] = -30;
				obj[0].bullet[i].setPosition(-30, -30); //если пуля неактивна то передвигаем ее за границу экрана
			}
			if (obj[0].bullet_x[i] > width + 30 || obj[0].bullet_x[i] < -30 || obj[0].bullet_y[i] < -30 || obj[0].bullet_y[i] > height + 30) //проверка на выход за границу поля
			{
				obj[0].bullet_shot &= ~(1 << i); //деактивируем пулю
			}
		}

		for (int i = 0; i < 10; i++) //обработка пуль второго объекта
		{
			if ((obj[1].bullet_shot >> i) % 2) //если пуля летит
			{
				obj[1].bullet[i].setPosition(obj[1].bullet_x[i], obj[1].bullet_y[i]); //задаем позицию спрайта
				obj[1].bullet[i].setRotation(-obj[1].bullet_angle[i]);
				double min_x = obj[1].bullet_x[i] - bullet_v * sin(obj[1].bullet_angle[i] / 57.3); //позиция точки куда пуля прилетит
				double min_y = obj[1].bullet_y[i] - bullet_v * cos(obj[1].bullet_angle[i] / 57.3);
				int n_object = -1; // номер обекта в который попали
				int s_object = -1; //сторона объекта в который попали
				for (int k = 0; k < obj_n + bonus_n; k++) //перебираем другие объекты
				{
					if (k != 1 && sq(obj[1].bullet_x[i] - obj[k].x) + sq(obj[1].bullet_y[i] - obj[k].y) < sq(bullet_v + obj[k].max_dist + 10)) //если они достаточно близко
					{
						for (int n = 0; n < obj[k].n; n++) //перебираем стороны объекта
						{
							pt a, b, c, d;
							a.x = obj[1].bullet_x[i] - bullet_v * sin(obj[1].bullet_angle[i] / 57.3);
							a.y = obj[1].bullet_y[i] - bullet_v * cos(obj[1].bullet_angle[i] / 57.3);
							//circle1.setPosition(a.x, a.y);
							b.x = obj[1].bullet_x[i];
							b.y = obj[1].bullet_y[i];
							c.x = obj[k].x + obj[k].c[n].l * sin((obj[k].angle + obj[k].c[n].a) / 57.3);
							c.y = obj[k].y + obj[k].c[n].l * cos((obj[k].angle + obj[k].c[n].a) / 57.3);
							if (n < obj[k].n - 1)
							{
								d.x = obj[k].x + obj[k].c[n + 1].l * sin((obj[k].angle + obj[k].c[n + 1].a) / 57.3);
								d.y = obj[k].y + obj[k].c[n + 1].l * cos((obj[k].angle + obj[k].c[n + 1].a) / 57.3);
							}
							else
							{
								d.x = obj[k].x + obj[k].c[1].l * sin((obj[k].angle + obj[k].c[1].a) / 57.3);
								d.y = obj[k].y + obj[k].c[1].l * cos((obj[k].angle + obj[k].c[1].a) / 57.3);
							}
							if (intersect(a, b, c, d)) // проверяем пересечение траектории пули с стороной объекта
							{
								double k1 = (a.y - b.y) / (a.x - b.x);
								double p1 = a.y - k1 * a.x;

								double k2 = (c.y - d.y) / (c.x - d.x);
								double p2 = c.y - k2 * c.x;

								double x0 = (p1 - p2) / (k2 - k1); //вычисляем точку пересечения
								double y0 = k1 * x0 + p1;
								//circle.setPosition(x0, y0);
								if (sq(b.x - x0) + sq(b.y - y0) < sq(b.x - min_x) + sq(b.y - min_y)) //если она ближе к текущему положению пули
								{
									min_x = x0; //записываем точку пересечения с объектом
									min_y = y0;
									n_object = k; //номер объекта
									s_object = n; //и его сторону
								}
							}
						}
					}
				}
				min_x += 3 * sin(obj[1].bullet_angle[i] / 57.3); //немного сдвигаем пулю назад
				min_y += 3 * cos(obj[1].bullet_angle[i] / 57.3);
				if (n_object != -1) //если пересечение с объектом было
				{
					double d_angle = obj[n_object].angle + obj[n_object].s[s_object].a - 90; //вычисляем отталкивание объекта
					obj[n_object].x -= bullet_o * sin(obj[1].bullet_angle[i] / 57.3) / obj[n_object].mass;
					obj[n_object].y -= bullet_o * cos(obj[1].bullet_angle[i] / 57.3) / obj[n_object].mass;
					double dl_k = sqrt(sq(min_x - obj[n_object].x) + sq(min_y - obj[n_object].y));
					double e_angle = 57.3*atan((obj[n_object].x - min_x) / (obj[n_object].y - min_y));
					if (obj[n_object].x - min_x < 0 && obj[n_object].y - min_y < 0) e_angle += 180;
					if (obj[n_object].y - min_y < 0 && obj[n_object].x - min_x > 0) e_angle += 180;
					obj[n_object].angle -= 57.3 * bullet_o * sin((e_angle - obj[1].bullet_angle[i]) / 57.3) / dl_k / obj[n_object].mass;

					if (obj[n_object].hp != 0)
					{
						obj[n_object].hp -= 200; //вычитаем здоровье
						obj[n_object].reg = 1; //сбиваем модификатор оружия
						if (obj[n_object].hp <= 0)
						{
							obj[n_object].hp = 0;
							obj[n_object].sprite.setTexture(texture0);
						}
					}
				
					obj[1].bullet_shot &= ~(1 << i); //деактивируем пулю
				}
				obj[1].bullet_x[i] = min_x;
				obj[1].bullet_y[i] = min_y;
				obj[1].bullet[i].setPosition(obj[1].bullet_x[i], obj[1].bullet_y[i]); //двигаем пулю на новое место
			}
			else
			{
				obj[1].bullet_x[i] = -30;
				obj[1].bullet_y[i] = -30;
				obj[1].bullet[i].setPosition(-30, -30); //если пуля неактивна то передвигаем ее за границу экрана
			}
			if (obj[1].bullet_x[i] > width + 30 || obj[1].bullet_x[i] < -30 || obj[1].bullet_y[i] < -30 || obj[1].bullet_y[i] > height + 30) //проверка на выход за границу поля
			{
				obj[1].bullet_shot &= ~(1 << i); //деактивируем пулю
			}
		}
		//window.draw(circle);
		//window.draw(circle1);

		/*for (int j = 0; j < 4; j++)
		{
			convex.setPoint(j, Vector2f(obj[0].x + obj[0].c[j].l * sin((obj[0].angle + obj[0].c[j].a) / 57.3), obj[0].y + obj[0].c[j].l * cos((obj[0].angle + obj[0].c[j].a) / 57.3)));
		}*/

		/*for (int j = 0; j < 7; j++)
		{
			convex1.setPoint(j, Vector2f(obj[3].x + obj[3].c[j].l * sin((obj[3].angle + obj[3].c[j].a) / 57.3), obj[3].y + obj[3].c[j].l * cos((obj[3].angle + obj[3].c[j].a) / 57.3)));
		}*/

		/*window.draw(bg); //отрисовка фона
		//window.draw(obj[0].sprite);
		//window.draw(obj[1].sprite);
		//window.draw(obj[2].sprite);
		for (int i = 0; i < obj_n; i++) //отрисовка спрайтов
		{
			window.draw(obj[i].sprite);
		}
		//window.draw(obj[0].bullet[0]);
		for (int i = 0; i < 10; i++)
		{
			window.draw(obj[0].bullet[i]); //отрисовка пуль
		}*/

		for (int i = 0; i < 10; i++)
		{
			window.draw(obj[0].bullet[i]); //отрисовка пуль
		}
		for (int i = 0; i < 10; i++)
		{
			window.draw(obj[1].bullet[i]); //отрисовка пуль
		}
		window.draw(energy1_bar); //отрисовка полоски энергии
		window.draw(energy1);
		window.draw(hp1_bar); //отрисовка полоски жизней
		window.draw(hp1);

		window.draw(energy2_bar); //отрисовка полоски энергии
		window.draw(energy2);
		window.draw(hp2_bar); //отрисовка полоски жизней
		window.draw(hp2);
		//window.draw(convex);
		//window.draw(convex1);
		window.display(); //вывод на экран
		Sleep(1); //ждем 1 мс
	}

	return 0;
}