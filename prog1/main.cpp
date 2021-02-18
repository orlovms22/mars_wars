#include <SFML/Graphics.hpp>
#include "windows.h"
#include "Math.h"

#define KEY_DOWN(key) (GetAsyncKeyState(key) & 0x8000) //обработка нажатий клавиш
#define KEY_PUSH(key) (GetAsyncKeyState(key) & 0x0001)

#define v_max 3 //скорость марсохода
#define angle_max 2 //угловая скорость марсохода
#define bullet_v 30 //скорость пуль марсохода

#define obj_n 3 //количество объектов 

#define width 1900 //размеры окна
#define height 1000

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
	//bool bullet_shot[10];
	int bullet_shot;
	uint8_t reg;
	int16_t energy;
};

object obj[obj_n]; //создание объектов

int main()
{
	RenderWindow window(VideoMode(width, height), "prog1"); //создаем окно

	Texture background; //открываем картинку бэкграунда
	background.loadFromFile("..\\background.png");
	Sprite bg(background);

	RectangleShape energy1; //создаем полоску энергии первого марсохода
	energy1.setSize(Vector2f(200, 20));
	energy1.setPosition(20, 20);
	energy1.setFillColor(Color(0, 255, 0, 128));
	RectangleShape energy1_bar; //создаем фон полоски энергии первого марсохода
	energy1_bar.setSize(Vector2f(200, 20));
	energy1_bar.setPosition(20, 20);
	energy1_bar.setFillColor(Color(0, 0, 0, 128));

	Texture texture1; //текстура первого марсохода
	texture1.loadFromFile("..\\rover_new1.png");
	obj[0].sprite.setTexture(texture1);
	obj[0].sprite.setOrigin(25, 40);

	obj[0].x = 800; //положение в пространстве
	obj[0].y = 800;
	obj[0].angle = 90;
	obj[0].v = 0;

	obj[0].mass = 1; //масса марсохода
	obj[0].energy = 1000; //энергия марсохода

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

	Texture texture1_1; //текстура пуль первого марсохода
	texture1_1.loadFromFile("..\\bull.png");
	for (int i = 0; i < 10; i++)
	{
		obj[0].bullet[i].setTexture(texture1_1);
		obj[0].bullet[i].setPosition(-10, -10);
		obj[0].bullet[i].setOrigin(3, 7);
	}
	obj[0].bullet_shot = 0; //биты состояния пуль 1 - летит / 0 - ожидание
	obj[0].reg = 1; //режим стрельбы первого марсохода
	bool shot0_press = false; //предыдущее состояние кнопки выстрела

	Texture texture2;
	texture2.loadFromFile("..\\rover_new2.png");
	obj[1].sprite.setTexture(texture2);
	obj[1].sprite.setOrigin(25, 40);
	obj[1].x = 800;
	obj[1].y = 400;
	obj[1].angle = 0;
	obj[1].v = 0;
	obj[1].mass = 1;
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


	Texture texture3;
	texture3.loadFromFile("..\\box.png");
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
		//if (shot) bull1.move(bullet_v*sin(angle_b / 57.3), -bullet_v * cos(angle_b / 57.3));
		//if (shot > 80) shot = 0;

		Event event; //обработка закрытия окна
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		if(KEY_PUSH(VK_ESCAPE)) window.close();
		
		if (KEY_DOWN(VK_LCONTROL)) obj[0].energy -= 4; //включение режима ускорения
		if (KEY_DOWN(0x57)) //обработка нажатий клавиш движения
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].v = -v_max;
			else obj[0].v = -v_max * 1.5;
		}
		if (KEY_DOWN(0x53))
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].v = v_max;
			else obj[0].v = v_max * 1.5;
		}
		if (KEY_DOWN(0x41))
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].angle += angle_max;
			else obj[0].angle += angle_max * 1.5;
		}
		if (KEY_DOWN(0x44))
		{
			if (!KEY_DOWN(VK_LCONTROL) || obj[0].energy < 0) obj[0].angle -= angle_max;
			else obj[0].angle -= angle_max * 1.5;
		}

		if (KEY_DOWN(VK_LSHIFT) && shot0_press == 0) //обработка выстрела 
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

				obj[0].x -= 5 * sin(-obj[0].angle / 57.3); //отдача
				obj[0].y += 5 * cos(-obj[0].angle / 57.3);
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

				obj[0].x -= 8 * sin(-obj[0].angle / 57.3); //отдача
				obj[0].y += 8 * cos(-obj[0].angle / 57.3);
			}
			shot0_press = 1;
		}
		energy1.setSize(Vector2f(obj[0].energy/5, 20)); //установка размера полоски энергии
		obj[0].energy+=2; //накопление энергии
		constrain(obj[0].energy, 0, 1000);
		if (!KEY_DOWN(VK_LSHIFT))shot0_press = 0;

		/*if (KEY_PUSH(0x5A) && shot == 0)
		{
			shot = 1;
			bull1.setPosition(sprite.getPosition());
			bull1.setRotation(angle);
			angle_b = angle;
			bull1.move(15 * sin(angle_b / 57.3), -15 * cos(angle_b / 57.3));
		}*/

		if (KEY_DOWN(VK_UP)) obj[1].v = v_max;
		if (KEY_DOWN(VK_DOWN)) obj[1].v = -v_max;
		if (KEY_DOWN(VK_LEFT)) obj[1].angle += angle_max;
		if (KEY_DOWN(VK_RIGHT)) obj[1].angle -= angle_max;
		
		//sprite.move(v*sin(angle / 57.3), -v * cos(angle / 57.3));
		obj[0].x -= obj[0].v * sin(-obj[0].angle / 57.3);
		obj[0].y += obj[0].v * cos(-obj[0].angle / 57.3);

		obj[1].x += obj[1].v * sin(-obj[1].angle / 57.3);
		obj[1].y -= obj[1].v * cos(-obj[1].angle / 57.3);
		/*float dx = 0;
		float dy = 0;
		float da = 0;*/

		for (int i = 0; i < obj_n; i++) //цикл обработки столкновений каждого объекта
		{
			obj[i].dx = 0;
			obj[i].dy = 0;
			obj[i].da = 0;
			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) < 0)
				{
					obj[i].dx -= obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3);
					obj[i].da -= (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3))*cos((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}
			/*if (0 > obj[i].x + v1.l*sin((angle + v1.a) / 57.3))
			{
				dx -= x + v1.l*sin((angle + v1.a) / 57.3);
				da += dx * cos((360 - angle - v1.a) / 57.3) / v1.l;
			}
			if (0 > x + v2.l*sin((angle + v2.a) / 57.3))
			{
				dx -= x + v2.l*sin((angle + v2.a) / 57.3);
				da += dx * cos((360 - angle - v2.a) / 57.3) / v2.l;
			}
			if (0 > x + v3.l*sin((angle + v3.a) / 57.3))
			{
				dx -= x + v3.l*sin((angle + v3.a) / 57.3);
				da += dx * cos((360 - angle - v3.a) / 57.3) / v3.l;
			}
			if (0 > x + v4.l*sin((angle + v4.a) / 57.3))
			{
				dx -= x + v4.l*sin((angle + v4.a) / 57.3);
				da += dx * cos((360 - angle - v4.a) / 57.3) / v4.l;
			}*/

			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3) < 0)
				{
					obj[i].dy -= obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3);
					obj[i].da -= (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3))*sin((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}

			/*if (0 > y + v1.l*cos((angle + v1.a) / 57.3))
			{
				dy -= y + v1.l*cos((angle + v1.a) / 57.3);
				da += dy * sin((360 - angle - v1.a) / 57.3) / v1.l;
			}
			if (0 > y + v2.l*cos((angle + v2.a) / 57.3))
			{
				dy -= y + v2.l*cos((angle + v2.a) / 57.3);
				da += dy * sin((360 - angle - v2.a) / 57.3) / v2.l;
			}
			if (0 > y + v3.l*cos((angle + v3.a) / 57.3))
			{
				dy -= y + v3.l*cos((angle + v3.a) / 57.3);
				da += dy * sin((360 - angle - v3.a) / 57.3) / v3.l;
			}
			if (0 > y + v4.l*cos((angle + v4.a) / 57.3))
			{
				dy -= y + v4.l*cos((angle + v4.a) / 57.3);
				da += dy * sin((360 - angle - v4.a) / 57.3) / v4.l;
			}*/

			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) > width)
				{
					obj[i].dx -= obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) - width;
					obj[i].da -= (obj[i].x + obj[i].c[j].l * sin((obj[i].angle + obj[i].c[j].a) / 57.3) - width)*cos((360 - obj[i].angle - obj[i].c[j].a) / 57.3) / obj[i].c[j].l;
				}
			}

			for (int j = 0; j < obj[i].n; j++) //столкновение со стеной
			{
				if (obj[i].y + obj[i].c[j].l * cos((obj[i].angle + obj[i].c[j].a) / 57.3) > height)
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

			for (int k = 0; k < obj_n; k++) //перебираем другие объекты
			{
				if (k != i && sq(obj[i].x - obj[k].x) + sq(obj[i].y - obj[k].y) < sq(obj[i].max_dist+obj[k].max_dist+10)) //если это не текущий объект и он не далеко
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
								d.x = obj[k].x + obj[k].c[n+1].l * sin((obj[k].angle + obj[k].c[n+1].a) / 57.3); //координата конца стороны другого объекта
								d.y = obj[k].y + obj[k].c[n+1].l * cos((obj[k].angle + obj[k].c[n+1].a) / 57.3);
							}
							else
							{
								d.x = obj[k].x + obj[k].c[0].l * sin((obj[k].angle + obj[k].c[0].a) / 57.3); //координата конца стороны другого объекта
								d.y = obj[k].y + obj[k].c[0].l * cos((obj[k].angle + obj[k].c[0].a) / 57.3);
							}
							if (intersect(a, b, c, d)) //проверка пересечения
							{
								double dl = abs((d.y-c.y)*a.x+(c.x-d.x)*a.y-(d.y-c.y)*c.x-(c.x-d.x)*c.y)/sqrt(sq(d.y-c.y)+sq(d.x-c.x)); //длина вектора выталкивания
								double d_angle = obj[k].angle + obj[k].s[n].a - 90; //напрваление вектора выталкивания
								obj[i].dx += dl * sin(d_angle/57.3)*obj[k].mass / (obj[i].mass + obj[k].mass); //применение к текущему объекту
								obj[i].dy += dl * cos(d_angle/57.3)*obj[k].mass / (obj[i].mass + obj[k].mass);
								obj[i].da -= dl * sin((180 - obj[i].c[j].a - obj[i].angle + d_angle) / 57.3) / obj[i].c[j].l*obj[k].mass / (obj[i].mass + obj[k].mass);
								obj[k].x -= dl * sin(d_angle/57.3)*obj[i].mass / (obj[i].mass + obj[k].mass); //применение к другому объекту
								obj[k].y -= dl * cos(d_angle/57.3)*obj[i].mass / (obj[i].mass + obj[k].mass);
								double e_angle = 57.3*atan((b.x - obj[k].x) / (obj[k].y - b.y));
								double dl_k = sqrt(sq(b.x - obj[k].x) + sq(b.y - obj[k].y));
								obj[k].angle += 57.3*dl*sin((e_angle - d_angle) / 57.3) / dl_k * obj[i].mass / (obj[i].mass + obj[k].mass);
								//A = (y2-y1)
								//B = (x2-x1)
								//C = -A*x1 - B*y1
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
		}

		//CircleShape circle(3);
		//circle.setFillColor(Color(255, 0, 0));
		//CircleShape circle1(3);
		//circle1.setFillColor(Color(0, 255, 0));
		for (int i = 0; i < 10; i++)
		{
			if ((obj[0].bullet_shot >> i) % 2)
			{
				obj[0].bullet[i].setPosition(obj[0].bullet_x[i], obj[0].bullet_y[i]);
				obj[0].bullet[i].setRotation(-obj[0].bullet_angle[i]);
				//obj[0].bullet_x[i] = obj[0].bullet_x[i] - bullet_v * sin(obj[0].bullet_angle[i] / 57.3);
				//obj[0].bullet_y[i] = obj[0].bullet_y[i] - bullet_v * cos(obj[0].bullet_angle[i] / 57.3);
				double min_x = obj[0].bullet_x[i] - bullet_v * sin(obj[0].bullet_angle[i] / 57.3);
				double min_y = obj[0].bullet_y[i] - bullet_v * cos(obj[0].bullet_angle[i] / 57.3);
				int n_object = -1;
				int s_object = -1;
				for (int k = 0; k < obj_n; k++)
				{
					if (k != 0 && sq(obj[0].bullet_x[i] - obj[k].x) + sq(obj[0].bullet_y[i] - obj[k].y) < sq(bullet_v + obj[k].max_dist + 10))
					{
						for (int n = 0; n < obj[k].n; n++)
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
							if (intersect(a, b, c, d))
							{
								double k1 = (a.y - b.y) / (a.x - b.x);
								double p1 = a.y - k1 * a.x;

								double k2 = (c.y - d.y) / (c.x - d.x);
								double p2 = c.y - k2 * c.x;

								double x0 = (p1 - p2) / (k2 - k1);
								double y0 = k1 * x0 + p1;
								//circle.setPosition(x0, y0);
								if (sq(b.x - x0) + sq(b.y - y0) < sq(b.x - min_x) + sq(b.y - min_y))
								{
									min_x = x0;
									min_y = y0;
									n_object = k;
									s_object = n;
								}
							}
						}
					}
				}
				min_x += 3 * sin(obj[0].bullet_angle[i] / 57.3);
				min_y += 3 * cos(obj[0].bullet_angle[i] / 57.3);
				//circle.setPosition(min_x, min_y);
				if (n_object != -1)
				{
					double d_angle = obj[n_object].angle + obj[n_object].s[s_object].a - 90;
					obj[n_object].x -= 5 * sin(d_angle / 57.3)*obj[i].mass / (obj[i].mass + obj[n_object].mass);
					obj[n_object].y -= 5 * cos(d_angle / 57.3)*obj[i].mass / (obj[i].mass + obj[n_object].mass);
					double dl_k = sqrt(sq(min_x - obj[n_object].x) + sq(min_y - obj[n_object].y));
					double e_angle = 57.3*atan((min_x - obj[n_object].x) / (obj[n_object].y - min_y));
					obj[n_object].angle += 57.3*5*sin((e_angle - d_angle) / 57.3) / dl_k * obj[i].mass / (obj[i].mass + obj[n_object].mass);

					//obj[n_object].x -= 5*sin(obj[0].bullet_angle[i] / 57.3);
					//obj[n_object].y -= 5*cos(obj[0].bullet_angle[i] / 57.3);
					obj[0].bullet_shot &= ~(1 << i);
				}
				obj[0].bullet_x[i] = min_x;
				obj[0].bullet_y[i] = min_y;
				obj[0].bullet[i].setPosition(obj[0].bullet_x[i], obj[0].bullet_y[i]);
			}
			else
			{
				obj[0].bullet_x[i] = -30;
				obj[0].bullet_y[i] = -30;
				obj[0].bullet[i].setPosition(-30, -30);
			}
			if (obj[0].bullet_x[i] > width + 30 || obj[0].bullet_x[i] < -30 || obj[0].bullet_y[i] < -30 || obj[0].bullet_y[i] > height + 30)
			{
				obj[0].bullet_shot &= ~(1 << i);
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
		window.draw(energy1_bar); //отрисовка полоски энергии
		window.draw(energy1);
		//window.draw(convex);
		//window.draw(convex1);
		/*if (shot)
		{
			window.draw(bull1);
			shot++;
		}*/
		window.display(); //вывод на экран
		Sleep(1); //ждем 1 мс
	}

	return 0;
}