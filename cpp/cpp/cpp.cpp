#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

float timeElapsed = 0.0f;
struct RainDrop {
	float x, y;     // Pozicija
	float speedX;   // Horizontalna brzina 
	float speedY;   // Vertikalna brzina
};

// Konstantne vrednosti
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int NUM_DROPS = 150;
std::vector<RainDrop> rainDrops;

// Inicijalizacija kapljica
void initRain() {
	std::srand(static_cast<unsigned>(std::time(0)));
	for (int i = 0; i < NUM_DROPS; ++i) {
		RainDrop drop;
		drop.x = static_cast<float>(std::rand() % WINDOW_WIDTH);//nasumicni odabir pozicije
		drop.y = static_cast<float>(std::rand() % WINDOW_HEIGHT);
		drop.speedX = -0.1f + static_cast<float>(std::rand()) / RAND_MAX; // Horizontalna brzina
		drop.speedY = 0.5f + static_cast<float>(std::rand()) / RAND_MAX; // Vertikalna brzina
		rainDrops.push_back(drop);
	}
}

// Ažuriranje pozicija kapljica
void updateRain() {
	for (auto& drop : rainDrops) {
		drop.x += drop.speedX;
		drop.y -= drop.speedY;

		// Reset kapljice ako izađe van ekrana
		if (drop.y < 0 || drop.x < 0 || drop.x > WINDOW_WIDTH) {
			drop.x = static_cast<float>(std::rand() % WINDOW_WIDTH);
			drop.y = static_cast<float>(WINDOW_HEIGHT + std::rand() % 100);
			drop.speedX = -0.5f + static_cast<float>(std::rand()) / RAND_MAX;
			drop.speedY = 0.6f + static_cast<float>(std::rand()) / RAND_MAX;
		}
	}
}
// Iscrtavanje kapljica
void drawRain() {
	glBegin(GL_LINES);
	glColor3f(0.5f, 0.8f, 1.0f); 
	for (const auto& drop : rainDrops) {
		glVertex2f(drop.x, drop.y);
		glVertex2f(drop.x + drop.speedX * 2, drop.y - drop.speedY * 2); // Pravac kretanja
	}
	glEnd();
}
// Pozicije ptica u jatu i fazni pomaci za sinusoidu
float birdPositions[6][2] = {
	{100.0f, 200.0f}, {200.0f, 350.0f}, {300.0f, 360.0f},
	{300.0f, 300.0f}, {500.0f, 380.0f}, {600.0f, 390.0f}
};
float birdPhases[6] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f }; // Različite faze oscilacija

// Funkcija za crtanje linije
void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}
// Funkcija za crtanje ptice
void drawBird(float x, float y, float phase) {
	// Parametri za oscilaciju krila
	float wave = 5.0f * sin(timeElapsed + phase); //  oscilacija
	float wingBend = 6.0f * sin(timeElapsed + phase); // Zakrivljenost krila

	// Telo ptice - crtanje malog ovalnog oblika
	glColor3f(0.8f, 0.8f, 0.8f);  // Svetlo siva boja tela
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 20; i++) {
		float angle = 2 * 3.14159 * i / 20;
		float dx = cos(angle) * 6.0f;
		float dy = sin(angle) * 3.0f;
		glVertex2f(x + dx, y + dy);
	}
	glEnd();

	// Levo krilo sa zakrivljenjem
	glColor3f(0.0f, 0.0f, 0.0f);  // Crna boja
	glBegin(GL_LINES);
	glVertex2f(x - 4.0f, y); // Početna tačka
	glVertex2f(x - 4.0f + wave, y + 5.0f + wingBend);  // Završna tačka sa zakrivljenjem
	glEnd();

	// Desno krilo sa zakrivljenjem
	glBegin(GL_LINES);
	glVertex2f(x + 4.0f, y);  // Početna tačka
	glVertex2f(x + 4.0f + wave, y + 5.0f + wingBend);  // Završna tačka sa zakrivljenjem
	glEnd();
}
// Funkcija za crtanje jata ptica
void drawBirdFlock() {
	for (int i = 0; i < 6; i++) {
		// Dodaj sinusoidnu oscilaciju sa faznim pomakom
		float wave = 5.0f * sin((birdPositions[i][0] * 0.01f) + timeElapsed * 0.1 + birdPhases[i]);
		drawBird(birdPositions[i][0], birdPositions[i][1] + wave, birdPhases[i]);
	}
}
// Funkcija za crtanje kruga (koristi se za sunce, oblake, krošnje)
void drawCircle(float cx, float cy, float r, int segments, float rColor, float gColor, float bColor) {
	glColor3f(rColor, gColor, bColor);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= segments; i++) {
		float angle = i * 2.0f * 3.14 / segments;
		float x = r * cos(angle);
		float y = r * sin(angle);
		glVertex2f(cx + x, cy + y);
	}
	glEnd();
}
// Funkcija za crtanje pravougaonika
void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}
// Globalne promenljive za animaciju
float boatX = 0.0f;
float cloudsX = 0.0f;
float windmillAngle = 0.0f; // Dodano za vetrenjaču
void  drawBoatDetails(float x) {
	float scale = 20.0f; // Faktor skaliranja
	float offsetX = x;   // Offset za pomeranje broda

	// crveni deo broda
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(offsetX + 3.5f * scale, 5.5f * scale);
	glVertex2f(offsetX + 3.5f * scale, 8.5f * scale);
	glVertex2f(offsetX + 19.5f * scale, 8.5f * scale);
	glVertex2f(offsetX + 19.5f * scale, 5.5f * scale);
	glEnd();

	glColor3f(0.5f, 0.4f, 0.2f);//vrata
	glBegin(GL_POLYGON);
	glVertex2f(offsetX + 4.0f * scale, 5.5f * scale);
	glVertex2f(offsetX + 4.0f * scale, 8.0f * scale);
	glVertex2f(offsetX + 5.0f * scale, 8.0f * scale);
	glVertex2f(offsetX + 5.0f * scale, 5.5f * scale);
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	for (float k = 1.5f; k <= 19.5f; k += 2.0f) {//crne linije na  brodu
		glBegin(GL_LINE_LOOP);
		glVertex2f(offsetX + k * scale, 5.5f * scale);
		glVertex2f(offsetX + k * scale, 6.5f * scale);
		glVertex2f(offsetX + (k + 0.5f) * scale, 6.2f * scale);
		glVertex2f(offsetX + (k + 1.0f) * scale, 6.2f * scale);
		glVertex2f(offsetX + (k + 1.5f) * scale, 6.5f * scale);
		glVertex2f(offsetX + (k + 1.5f) * scale, 5.5f * scale);
		glEnd();
	}
	glColor3f(0.9f, 0.9f, 0.9f);//prozori
	for (float i = 7.0f; i <= 19.0f; i += 4.5f) {
		glBegin(GL_POLYGON);
		glVertex2f(offsetX + i * scale, 7.5f * scale);
		glVertex2f(offsetX + i * scale, 8.0f * scale);
		glVertex2f(offsetX + (i + 3.0f) * scale, 8.0f * scale);
		glVertex2f(offsetX + (i + 3.0f) * scale, 7.5f * scale);
		glEnd();
	}
	// Krov broda
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_POLYGON);
	glVertex2f(offsetX + 4.5f * scale, 8.5f * scale);
	glVertex2f(offsetX + 4.5f * scale, 10.0f * scale);
	glVertex2f(offsetX + 18.5f * scale, 10.0f * scale);
	glVertex2f(offsetX + 18.5f * scale, 8.5f * scale);
	glEnd();

	// dimnjaci na brodu
	for (float j = 5.5f; j <= 16.0f; j += 2.5f) {
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex2f(offsetX + j * scale, 10.0f * scale);
		glVertex2f(offsetX + j * scale, 12.9f * scale);
		glVertex2f(offsetX + (j + 1.5f) * scale, 12.9f * scale);
		glVertex2f(offsetX + (j + 1.5f) * scale, 10.0f * scale);
		glEnd();

		glColor3f(0.9f, 0.5f, 0.2f);
		glBegin(GL_POLYGON);
		glVertex2f(offsetX + j * scale, 12.9f * scale);
		glVertex2f(offsetX + j * scale, 13.5f * scale);
		glVertex2f(offsetX + (j + 1.5f) * scale, 13.5f * scale);
		glVertex2f(offsetX + (j + 1.5f) * scale, 12.9f * scale);
		glEnd();
	}

}
void drawBoat(float x) {
	float scale = 20.0f; // Faktor skaliranja
	float offsetX = x;
	// dno broda
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex2f(offsetX + 1.0f * scale, 5.5f * scale);
	glVertex2f(offsetX + 4.0f * scale, 1.0f * scale);
	glVertex2f(offsetX + 19.0f * scale, 1.0f * scale);
	glVertex2f(offsetX + 21.5f * scale, 5.5f * scale);
	glEnd();
}
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}
// Funkcija za crtanje vetrenjače
void drawWindmill(float x, float y, float angle) {
	// Trup vetrenjače (pravougaonik)
	drawRectangle(x - 5.0f, y, x + 5.0f, y + 60.0f, 0.5f, 0.3f, 0.1f);

	// Parametri lopatica
	float bladeLength = 40.0f;   // Dužina lopatica
	float baseWidth = 10.0f;     // Širina na osnovi lopatice
	float tipWidth = 2.0f;       // Širina na vrhu lopatice
	float angleRad = angle * 3.14159265f / 180.0f; // Konvertovanje ugla u radijane

	// Funkcija za crtanje jedne lopatice 
	auto drawBlade = [&](float centerX, float centerY, float angle) {
		float cosAngle = cos(angle);
		float sinAngle = sin(angle);

		// Koordinate vrha lopatice
		float tipX = centerX + cosAngle * bladeLength;
		float tipY = centerY + sinAngle * bladeLength;

		// Koordinate leve i desne strane baze lopatice
		float leftBaseX = centerX - sinAngle * (baseWidth / 2.0f);
		float leftBaseY = centerY + cosAngle * (baseWidth / 2.0f);
		float rightBaseX = centerX + sinAngle * (baseWidth / 2.0f);
		float rightBaseY = centerY - cosAngle * (baseWidth / 2.0f);

		// Koordinate leve i desne strane vrha lopatice (sužene)
		float leftTipX = tipX - sinAngle * (tipWidth / 2.0f);
		float leftTipY = tipY + cosAngle * (tipWidth / 2.0f);
		float rightTipX = tipX + sinAngle * (tipWidth / 2.0f);
		float rightTipY = tipY - cosAngle * (tipWidth / 2.0f);

		// Crtanje lopatice (koristeći dva trougla)
		drawTriangle(leftBaseX, leftBaseY, rightBaseX, rightBaseY, leftTipX, leftTipY, 1.0f, 1.0f, 1.0f); // Leva strana
		drawTriangle(rightBaseX, rightBaseY, leftTipX, leftTipY, rightTipX, rightTipY, 1.0f, 1.0f, 1.0f); // Desna strana
	};
	// Pozicija centra rotacije lopatica
	float bladeCenterX = x;
	float bladeCenterY = y + 60.0f;

	// Crtanje 3 lopatice (rotirane za 120 stepeni)
	for (int i = 0; i < 3; ++i) {
		float currentAngle = angleRad + i * 2.0f * 3.14159265f / 3.0f; // Svaka lopatica rotirana za 120°
		drawBlade(bladeCenterX, bladeCenterY, currentAngle);
	}
}
// Funkcija za crtanje oblaka
void drawCloud(float x) {
	drawCircle(100.0f + x, 400.0f, 30.0f, 50, 0.8f, 0.8f, 0.8f);
	drawCircle(130.0f + x, 400.0f, 40.0f, 50, 0.8f, 0.8f, 0.8f);
	drawCircle(160.0f + x, 400.0f, 30.0f, 50, 0.8f, 0.8f, 0.8f);
}

// Funkcija za crtanje drveta
void drawTree(float x, float y) {
	// Stablo
	drawRectangle(x - 10.0f, y, x + 10.0f, y + 50.0f, 0.5f, 0.2f, 0.1f);
	// Krošnja
	drawCircle(x, y + 60.0f, 30.0f, 50, 0.0f, 0.5f, 0.0f);
	drawCircle(x - 20.0f, y + 50.0f, 20.0f, 50, 0.0f, 0.5f, 0.0f);
	drawCircle(x + 20.0f, y + 50.0f, 20.0f, 50, 0.0f, 0.5f, 0.0f);
}

// Funkcija za crtanje kuće
void drawHouse() {
	// Osnova kuće 
	drawRectangle(50.0f, 150.0f, 150.0f, 250.0f, 0.8f, 0.5f, 0.2f);
	// Krov 
	glColor3f(0.6f, 0.3f, 0.1f);
	glBegin(GL_TRIANGLES);
	glVertex2f(40.0f, 250.0f);
	glVertex2f(160.0f, 250.0f);
	glVertex2f(100.0f, 300.0f);
	glEnd();
	// Vrata 
	drawRectangle(85.0f, 150.0f, 115.0f, 200.0f, 0.3f, 0.2f, 0.1f);
	// Dodavanje prozora 
	drawRectangle(60.0f, 220.0f, 80.0f, 240.0f, 0.9f, 1.0f, 1.0f); // Levo okno
	drawRectangle(120.0f, 220.0f, 140.0f, 240.0f, 0.9f, 1.0f, 1.0f); // Desno okno
	// Dodavanje dimnjaka 
	drawRectangle(110.0f, 270.0f, 130.0f, 300.0f, 0.5f, 0.5f, 0.5f);
}
// Funkcija za crtanje scene
void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT);
	// Nebo
	void drawSkyGradient();
	{
		glBegin(GL_QUADS);

		// Donji levi ugao (0.0f, 0.0f)
		glColor3f(0.5f, 0.8f, 1.0f);  // Svetlo plava
		glVertex2f(0.0f, 0.0f);

		// Donji desni ugao (800.0f, 0.0f)
		glColor3f(0.6f, 0.9f, 1.0f);  // Svetlo plava (malo svetlija)
		glVertex2f(800.0f, 0.0f);

		// Gornji desni ugao (800.0f, 600.0f)
		glColor3f(0.2f, 0.4f, 0.7f);  // Tamna plava
		glVertex2f(800.0f, 600.0f);

		// Gornji levi ugao (0.0f, 600.0f)
		glColor3f(0.1f, 0.2f, 0.5f);  // Još tamnija plava
		glVertex2f(0.0f, 600.0f);

		glEnd();
	}
	// Trava
	drawRectangle(0.0f, 0.0f, 800.0f, 150.0f, 0.2f, 0.6f, 0.1f);
	// Sunce
	drawCircle(700.0f, 500.0f, 50.0f, 50, 1.0f, 0.7f, 0.0f);
	// Kuća
	drawHouse();
	// Drveće
	drawTree(350.0f, 130.0f);
	drawTree(220.0f, 130.0f);
	// Oblaci
	drawCloud(cloudsX);
	drawCloud(cloudsX + 300);
	// Vetrenjača
	drawWindmill(600.0f, 150.0f, windmillAngle);
	drawBoat(boatX); // donji deo broda

	void drawWaterGradient(); {
		glBegin(GL_QUADS);

		glColor3f(0.05f, 0.2f, 0.35f);  // Tamnija plava
		glVertex2f(0.0f, 0.0f);

		// Donji desni ugao (900.0f, 0.0f)
		glColor3f(0.05f, 0.2f, 0.35f);  // Tamnija plava
		glVertex2f(900.0f, 0.0f);

		// Gornji desni ugao (900.0f, 75.0f)
		glColor3f(0.2f, 0.5f, 0.8f);  // Svetlija plava
		glVertex2f(900.0f, 75.0f);

		// Gornji levi ugao (0.0f, 75.0f)
		glColor3f(0.2f, 0.5f, 0.8f);  // Svetlija plava
		glVertex2f(0.0f, 75.0f);

		glEnd();
	}
	drawBoatDetails(boatX);//gornji deo broda

	// Jato ptica
	drawBirdFlock();

	drawRain();
	updateRain();		
}
// Funkcija za animaciju
void updateAnimation() {
	boatX -= 0.1f; 
	if (boatX < -400.0f) 
		boatX = 800.0f;

	cloudsX += 0.1f; // brzina oblaka
	if (cloudsX > 800.0f)
		cloudsX = -300.0f;

	// Pomeraj ptice horizontalno
	for (int i = 0; i < 6; i++) {
		timeElapsed += 0.01f;
		birdPositions[i][0] += 0.3f;
		if (birdPositions[i][0] > 800.0f)
			birdPositions[i][0] = -50.0f;
	}
	// Rotacija vetrenjače
	windmillAngle += 0.3f;
	if (windmillAngle >= 360.0f)
		windmillAngle = 0.0f;
}
// Glavna funkcija
int main()
{
	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed!" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "Village Scenery", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewInit();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	initRain();
	while (!glfwWindowShouldClose(window)) {
		renderScene();
		glfwSwapBuffers(window);
		glfwPollEvents();
		updateAnimation(); // Ažuriranje animacije
		glFlush(); // Osvežavanje OpenGL poziva
	}
	glfwTerminate();
	return 0;
}