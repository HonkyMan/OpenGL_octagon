#include "GL/glut.h" 
#include "GL/glaux.h"
#include <vector>

constexpr auto SIZE_OF_POLYGON = 8;
constexpr auto PI = 3.14;
constexpr auto INITIAL_ANGLE = PI/6; // started angle from OY

using namespace std;

#pragma region Vars declaration

void Display(void);
void Reshape(GLint, GLint);
void Keyboard(unsigned char, int, int);
void Mouse(int, int, int,int);
void createMenu(void);
void menu(int value);


GLint	Width = 512,
Height = 512;

GLubyte ColorR = 170,
ColorG = 170,
ColorB = 170;

GLenum mode = GL_MODELVIEW;
GLdouble splash[3] = {.0, .0, .0};//new GLdouble[3];
GLdouble rotates[7] = { 0, 0, 0, 1, 0, 0, 0 };//new GLdouble[3];
GLdouble zoom[3] = { 1, 1, 1 };

FILE* file;
GLubyte data54[54];
int width = *(data54 + 18);
int height = *(data54 + 22);
GLubyte* pixels;

struct type_point
{
	GLfloat x, y;
	type_point(GLfloat _x, GLfloat _y)
	{
		x = _x;
		y = _y;
	}
	type_point()
	{
		x = 0;
		y = 0;
	}
};

struct polygon
{
	type_point Center;
	vector <type_point> Points;

	double PHI = INITIAL_ANGLE;
	double radius = NULL;
	GLubyte Red, Green, Blue;

	polygon(GLubyte r = ColorR, GLubyte g = ColorG, GLubyte b = ColorB)
	{
		Red = r;
		Green = g;
		Blue = b;
	}
};

vector <polygon> octagons;
int pointCounter = octagons.size();

double count_points_space(type_point a, type_point b);
vector <type_point> count_octo_points(polygon figure, int n);

vector<type_point> normolize(vector <type_point> Points, int n);

#pragma endregion

/* Головная программа */
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	file = fopen("space.bmp", "rb");
	fread(data54, 54, 1, file);
	pixels = new GLubyte[width * height * 3];
	fread(pixels, width * height * 3, 1, file);
	fclose(file);

	polygon buf(ColorR, ColorG, ColorB);
	octagons.push_back(buf);

	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Рисуем линии");
	createMenu();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();

	return 1;
}

/* Функция вывода на экран */
void Display(void)
{
	GLuint tex; 
	glGenTextures(1, &tex); 
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

	glClearColor(0.3, 0.3, 0.3, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(mode);
	glPushMatrix();
	glLoadIdentity();

	glTranslated(splash[0], splash[1], splash[2]);
	// For rotate
	glTranslated(rotates[4], rotates[5], rotates[6]);
	glRotated(rotates[0], rotates[1], rotates[2], rotates[3]);
	glTranslated(-rotates[4], -rotates[5], -rotates[6]);
	//glTranslated(0, 0, 0);
	//gluLookAt(0, 0, -10, 0, 0, 0, 0, 1, 0);
	// glPointSize(1050);


	// Рисование текстуры
	glEnable(GL_TEXTURE_2D);
	glColor3d(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	//glBegin(GL_QUADS); 
	glBegin(GL_QUADS);

	for (int i = 0; i < octagons.size(); i++)
	{
		// Выделение акивного октогона
		//glColor3ub(octagons[i].Red, octagons[i].Green, octagons[i].Blue);
		//// Если активный полигон то делаем жирную линию и красим в белый
		//if ((i + 1) == octagons.size())
		//{
		//	glLineWidth(10);
		//	glColor3ub(GLubyte(255), GLubyte(255), GLubyte(255));
		//}
		//else
		//{
		//	glLineWidth(5);
		//}

		// Рисование октогона
		//glEnable(GL_LINE_SMOOTH);
		//glBegin(GL_LINE_LOOP);

		//for (int j = 0; j < octagons[i].Points.size(); j++)
		//{
		//	glVertex2i(octagons[i].Points[j].x, octagons[i].Points[j].y);
		//}

		//glEnd();


		vector<type_point> norm = normolize(octagons[i].Points, SIZE_OF_POLYGON);

		for (int j = 0; j < octagons[i].Points.size(); j++)
		{
			glTexCoord2d(norm[j].x, norm[j].y); 
			glVertex2d(octagons[i].Points[j].x, octagons[i].Points[j].y);
		}

	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glutSwapBuffers();
	glFinish();
}

#pragma region manipulation

/* Функция изменения размеров окна */
void Reshape(GLint w, GLint h)
{
	Width = w;
	Height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* Функцияобработкисообщенийотклавиатуры */
void Keyboard(unsigned char key, int x, int y)
{
	int i, n = octagons[pointCounter].Points.size(); //Points[pointCounter].size();  /* Изменение RGB-компонент цвета точек */
	if (key == 'r')
		ColorR += 5;

	if (key == 'g')
		ColorG += 5;

	if (key == 'b')
		ColorB += 5;

	// moving and zooming draws on canvas
	if (key == '+')
	{
		octagons[pointCounter].radius += 1;
		octagons[pointCounter].Points = count_octo_points(octagons[pointCounter], SIZE_OF_POLYGON);
		//zoom[0] += 1;
		//zoom[1] += 1;
		//zoom[2] += 1;
	}

	if (key == '-')
	{
		octagons[pointCounter].radius -= 1;
		octagons[pointCounter].Points = count_octo_points(octagons[pointCounter], SIZE_OF_POLYGON);
		//zoom[0] -= 1;
		//zoom[1] -= 1;
		//zoom[2] -= 1;
	}

	if (key == '6')
		splash[0] += 10;

	if (key == '4')
		splash[0] -= 10;

	if (key == '8')
		splash[1] += 10;

	if (key == '2')
		splash[1] -= 10;
	// end moving draws

	// rotating
	if (key == '9')
		rotates[0] += 10;

	if (key == '7')
		rotates[0] -= 10;
	// end rotating

	if (key == ' ')
	{
		polygon buf(ColorR, ColorG, ColorB);
		octagons.push_back(buf);
		++pointCounter;
	}

	if (int(key) == 127)
		//if (key == '') // onpress DELETE button on keyboard
		octagons[pointCounter].Points.pop_back(); //Points[pointCounter].pop_back();


	/* Изменение XY-кординатточек */
	if (key == 'w')
		for (i = 0; i < n; i++)
			octagons[pointCounter].Points[i].y += 9; //Points[pointCounter][i].y += 9;

	if (key == 's')
		for (i = 0; i < n; i++)
			octagons[pointCounter].Points[i].y -= 9;
	//Points[pointCounter][i].y -= 9;

	if (key == 'a')
		for (i = 0; i < n; i++)
			octagons[pointCounter].Points[i].x -= 9;
	//Points[pointCounter][i].x -= 9;

	if (key == 'd')
		for (i = 0; i < n; i++)
			octagons[pointCounter].Points[i].x += 9;
	//Points[pointCounter][i].x += 9;

	glutPostRedisplay();
}

/* Функция обработки сообщения от мыши */
void Mouse(int button, int state, int x, int y)
{  /* клавиша была нажата, но не отпущена */
	if (state != GLUT_DOWN)
		return;  /* новая точка по левому клику */

	if (button == GLUT_LEFT_BUTTON)
	{
		if (!octagons[pointCounter].Center.x && !octagons[pointCounter].Center.y)
		{
			type_point p(x, Height - y);
			octagons[pointCounter].Center = p;
			rotates[4] = octagons[pointCounter].Center.x;
			rotates[5] = octagons[pointCounter].Center.y;
			rotates[6] = 0;
			//rotates[1] = octagons[pointCounter].Center.x;
			//rotates[2] = octagons[pointCounter].Center.y;
		}
		else if( octagons[pointCounter].Center.x && octagons[pointCounter].Center.y)
		{
			type_point p(x, Height - y);
			octagons[pointCounter].radius = count_points_space(octagons[pointCounter].Center, p);
			octagons[pointCounter].Points = count_octo_points(octagons[pointCounter], SIZE_OF_POLYGON);
		}
	}

	glutPostRedisplay();
}
#pragma endregion

#pragma region Menu

// Menu items
enum MENU_TYPE
{
	DELETE_POINT,
	CHANGE_COLOR_WHITE,
	CHANGE_COLOR_RED,
	CHANGE_COLOR_GREEN,
	CHANGE_COLOR_BLUE,
	CHANGE_POSITION_UP,
	CHANGE_POSITION_DOWN,
	CHANGE_POSITION_LEFT,
	CHANGE_POSITION_RIGHT,
};

// Menu handling function declaration
void menu(int);

void createMenu(void)
{
	// create sub menu
	int colorMenu = glutCreateMenu(menu);
	glutAddMenuEntry("White Color", CHANGE_COLOR_WHITE);
	glutAddMenuEntry("Red Color", CHANGE_COLOR_RED);
	glutAddMenuEntry("Green Color", CHANGE_COLOR_GREEN);
	glutAddMenuEntry("Blue Color", CHANGE_COLOR_BLUE);


	int positionMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Move up", CHANGE_POSITION_UP);
	glutAddMenuEntry("Move down", CHANGE_POSITION_DOWN);
	glutAddMenuEntry("Move left", CHANGE_POSITION_LEFT);
	glutAddMenuEntry("Move right", CHANGE_POSITION_RIGHT);

	// Create a menu
	glutCreateMenu(menu);

	// Add menu items
	glutAddMenuEntry("Delete last point", DELETE_POINT);
	glutAddSubMenu("Change color", colorMenu);
	glutAddSubMenu("Change position", positionMenu);

	// Associate a mouse button with menu
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

// Menu handling function definition
void menu(int item)
{
	switch (item)
	{
	case DELETE_POINT:
	{
		octagons[pointCounter].Points.pop_back();
	}
	break;

	case CHANGE_COLOR_WHITE:
	{
		octagons[pointCounter].Blue = 255;
		octagons[pointCounter].Green = 255;
		octagons[pointCounter].Red = 255;
	}
	break;
	case CHANGE_COLOR_RED:
	{
		octagons[pointCounter].Blue = 70;
		octagons[pointCounter].Green = 70;
		octagons[pointCounter].Red = 255;
	}
	break;
	case CHANGE_COLOR_GREEN:
	{
		octagons[pointCounter].Blue = 70;
		octagons[pointCounter].Green = 255;
		octagons[pointCounter].Red = 70;
	}
	break;
	case CHANGE_COLOR_BLUE:
	{
		octagons[pointCounter].Blue = 255;
		octagons[pointCounter].Green = 70;
		octagons[pointCounter].Red = 70;
	}
	break;

	case CHANGE_POSITION_UP:
	{
		int n = octagons[pointCounter].Points.size();
		for (int i = 0; i < n; i++)
			octagons[pointCounter].Points[i].y += 9;
	}
	break;
	case CHANGE_POSITION_DOWN:
	{
		int n = octagons[pointCounter].Points.size();
		for (int i = 0; i < n; i++)
			octagons[pointCounter].Points[i].y -= 9;
	}
	break;
	case CHANGE_POSITION_LEFT:
	{
		int n = octagons[pointCounter].Points.size();
		for (int i = 0; i < n; i++)
			octagons[pointCounter].Points[i].x -= 9;
	}
	break;
	case CHANGE_POSITION_RIGHT:
	{
		int n = octagons[pointCounter].Points.size();
		for (int i = 0; i < n; i++)
			octagons[pointCounter].Points[i].x += 9;
	}
	break;

	default:
	{       /* Nothing */       }
	break;
	}

	glutPostRedisplay();

	return;
}

#pragma endregion

double count_points_space(type_point a, type_point b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

vector <type_point> count_octo_points(polygon figure, int n = SIZE_OF_POLYGON)
{
	vector <type_point> buf;

	for (int i = 0; i < n; i++)
	{
		GLint x = figure.Center.x + figure.radius * cos(figure.PHI + 2 * PI * i / n);
		GLint y = figure.Center.y + figure.radius * sin(figure.PHI + 2 * PI * i / n);
		type_point p(x, y);
		buf.push_back(p);
	}

	return buf;
}

vector<type_point> normolize(vector<type_point> Points, int n = SIZE_OF_POLYGON)
{
	vector<type_point> buf;

	if (Points.size() == 0)
	{
		return buf;
	}

	double max_x = Points[0].x;
	double max_y = Points[0].y;

	// Find max values
	for (int i = 0; i < n; i++)
	{
		if (max_x < Points[i].x)
		{	
			max_x = Points[i].x;
		}

		if (max_y < Points[i].y)	
		{
			max_y = Points[i].y;
		}
	}

	// Normalizing
	for (int i = 0; i < n; i++)
	{
		buf.push_back(type_point(Points[i].x / max_x, Points[i].y / max_y));
	}

	return buf;
}
