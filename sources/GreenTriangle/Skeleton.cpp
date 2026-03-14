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


class PointCollection
{
	Geometry<vec3> geometry;
public:
	PointCollection() {}
	void addPoint(const vec3& point)
	{
		geometry.Vtx().push_back(point);
		geometry.updateGPU();
	}
	void addPoint(float x, float y)			
	{
		printf("Point %f, %f added\n", x, y);
		geometry.Vtx().push_back(vec3(x, y, 1.0f));
		geometry.updateGPU();
	}
	void draw(GPUProgram* gpuProgram, vec3 color = vec3(1,0,0))	
	{
		geometry.Draw(gpuProgram, GL_POINTS, color);
	}
	//utilities
	int size() { return (int)geometry.Vtx().size(); }				
	vec3 getPoint(int index) { return geometry.Vtx()[index]; }	
	int pickPoint(const vec3& mouse, float threshold = 0.02f) 
	{
		int pickedIndex = -1;
		float bestDistance = threshold;
		for (int i = 0; i<size(); i++) {
			vec3 point = geometry.Vtx()[i];
			float dx = point.x - mouse.x;
			float dy = point.y - mouse.y;
			float distance = sqrt(dx * dx + dy * dy);
			if (distance < bestDistance) {
				pickedIndex = i;
				bestDistance = distance;			
			}
		}
		return pickedIndex;
	}

};

class Line
{
	vec3 p1, p2;
	Geometry<vec2> geometry;

public:
	Line(vec3 point1, vec3 point2) : p1(point1), p2(point2)
	{
		printf("Line added\n");
		printf("	Implicit: %f x + %f y + %f\n", point1.y-point2.y, point2.x-point1.x, point1.x*point2.x-point1.y*point2.y);

		printf("	Parametric:");
	}
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
		scene.draw();
	}
};
LineApp app;