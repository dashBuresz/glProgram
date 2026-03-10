//=============================================================================================
// Mintaprogram: Zold háromszog. Ervenyes 2025.-tol
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni, ideértve ChatGPT-t és társait is
// - felesleges programsorokat a beadott programban hagyni 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL es GLM fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Szijarto Csaba Istvan
// Neptun : V7UDH6
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================//=============================================================================================
#include "framework.h"

// csúcspont árnyaló
const char * vertSource = R"(
	#version 330				
    precision highp float;

	layout(location = 0) in vec2 cP;	// 0. bemeneti regiszter

	void main() {
		gl_Position = vec4(cP.x, cP.y, 0, 1); 	// bemenet már normalizált eszközkoordinátákban
	}
)";

// pixel árnyaló
const char * fragSource = R"(
	#version 330
    precision highp float;

	uniform vec3 color;			// konstans szín
	out vec4 fragmentColor;		// pixel szín

	void main() {
		fragmentColor = vec4(color, 1); // RGB -> RGBA
	}
)";

const int winWidth = 600, winHeight = 600;
/*
* SKELETON to base the labwork program on. 
class GreenTriangleApp : public glApp {
	Geometry<vec2>* triangle = nullptr;  // geometria
	GPUProgram* gpuProgram = nullptr;	 // csúcspont és pixel árnyalók
public:
	GreenTriangleApp() : glApp("Green triangle") { }

	// Inicializáció, 
	void onInitialization() {
		triangle = new Geometry<vec2>;
		triangle->Vtx() = { vec2(-0.8f, -0.8f), vec2(-0.6f, 1.0f), vec2(0.8f, -0.2f) };
		triangle->updateGPU();
		gpuProgram = new GPUProgram(vertSource, fragSource);
	}

	// Ablak újrarajzolás
	void onDisplay() {
		glClearColor(0, 0, 0, 0);     // háttér szín
		glClear(GL_COLOR_BUFFER_BIT); // rasztertár törlés
		glViewport(0, 0, winWidth, winHeight);
		triangle->Draw(gpuProgram, GL_TRIANGLES, vec3(0.0f, 1.0f, 0.0f));
	}
};

GreenTriangleApp app;*/

class PointCollection
{
	Geometry<vec3> geometry;
public:
	PointCollection() {}
	void addPoint(const vec3& point)		//függévny egy pont hozzáadására, ha konkrét pontot adunk meg paraméterként
	{
		geometry.Vtx().push_back(point);
		geometry.updateGPU();
	}
	void addPoint(float x, float y)			//pont hozzáadása két koordinátával
	{
		geometry.Vtx().push_back(vec3(x, y, 1.0f));
		geometry.updateGPU();
	}
	void draw(GPUProgram* gpuProgram, vec3 color = vec3(1,0,0))	//pontok kirajzolása a megadott színnel
	{
		geometry.Draw(gpuProgram, GL_POINTS, color);
	}
	//utilities
	int size() { return (int)geometry.Vtx().size(); }				//pontok számának lekérdezése
	vec3 getPoint(int index) { return geometry.Vtx()[index]; }	//pont lekérdezése index alapján
	int pickPoint(const vec3& mouse, float threshold = 0.02f) 		//egér pozíció alapján pont kiválasztása egy adott távolságon belül
	{
		int pickedIndex = -1;					//változó a kiválasztott pont indexének tárolására, kezdetben -1 (érvénytelen index)
		float bestDistance = threshold;			//változó a legjobb (legkisebb) távolság tárolására, kezdetben a küszöbérték
		for (int i = 0; i<size(); i++) {
			vec3 point = geometry.Vtx()[i];
			float dx = point.x - mouse.x;		//távolság az egér és a pont x koordinátája között
			float dy = point.y - mouse.y;		//távolság az egér és a pont y koordinátája között
			float distance = sqrt(dx * dx + dy * dy);	//távolság kiszámítása	
			if (distance < bestDistance) {			//ha a távolság kisebb, mint az eddigi legközelebbi pont távolsága, akkor a mostani index lesz a kiválasztott
				pickedIndex = i;
				bestDistance = distance;			
			}
		}
		return pickedIndex;								//ha nincs pont a küszöbértéken belül, akkor a pickedIndex -1 marad és érvénytelen indexet adunk vissza
	}

};

class Line
{
	vec2 p1, p2;
	Geometry<vec2> geometry;

public:
	bool containsPointNear(vec3 point, float threshold) 
	{

	}
	std::vector<vec3> clipToViewPort()
	{

	}
	//we might not need this
	vec3 intersect(Line other)
	{

	}
};
class LineCollection
{
	std::vector<Line> lines;
public:
	void add(Line line) { lines.push_back(line); }

};


class Circle {

};
class CircleCollection
{
	std::vector<Circle> circles;
public:

};

class Scene
{

public:
	void draw() 
	{
	}
};


class LineApp : public glApp {
	Scene scene;
	GPUProgram* gpuProgram = nullptr;
public:
	LineApp() : glApp("Line app") { }
	void onInitialization() {
		glPointSize(10.0f);
		glLineWidth(3.0f);
		gpuProgram = new GPUProgram(vertSource, fragSource);
	}
	void onDisplay() {							//based on the GreenTriangleApp::onDisplay()
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, winWidth, winHeight);
		scene.draw();	//we draw the scene
	}
};
LineApp app;