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
	Geometry<vec3>* geometry = nullptr;
public:
	PointCollection() {}
	void addPoint(const vec3& point)
	{
		geometry->Vtx().push_back(point);
		geometry->updateGPU();
	}
	void initialize()
	{
		geometry = new Geometry<vec3>;
	}
	void addPoint(float x, float y)			
	{
		printf("Point %f, %f added\n", x, y);
		geometry->Vtx().push_back(vec3(x, y, 1.0f));
		geometry->updateGPU();
	}
	void draw(GPUProgram* gpuProgram, vec3 color = vec3(1,0,0))	
	{
		geometry->Draw(gpuProgram, GL_POINTS, color);
	}
	//utilities
	int size() { return (int)geometry->Vtx().size(); }				
	vec3 getPoint(int index) { return geometry->Vtx()[index]; }	
	int pickPoint(const vec3& mouse, float threshold = 0.02f) 
	{
		int pickedIndex = -1;
		float bestDistance = threshold;
		for (int i = 0; i<size(); i++) {
			vec3 point = geometry->Vtx()[i];
			float dx = point.x - mouse.x;
			float dy = point.y - mouse.y;
			float distance = sqrt(dx * dx + dy * dy);
			if (distance < bestDistance) {
				pickedIndex = i;
				bestDistance = distance;			
			}
		}
		if (pickedIndex != -1)
		{
			printf("Point %f, %f selected\n", geometry->Vtx()[pickedIndex].x, geometry->Vtx()[pickedIndex].y);	//DEBUG REMOVE BEFORE SUBMISSION
		}
		else {
			printf("No point selected\n");	//DEBUG REMOVE BEFORE SUBMISSION
		}
		return pickedIndex;
	}

};

class Line
{
	vec3 p1, p2;
	vec3 lineEquation;

public:
	Line(vec3 point1, vec3 point2) : p1(point1), p2(point2)
	{
		lineEquation = cross(point1, point2);
		printf("Line added\n");
		printf("	Implicit: %f x + %f y + %f\n", point1.y - point2.y, point2.x - point1.x, point1.x * point2.y - point2.x * point1.y);
		float u = point2.x - point1.x;
		float v = point2.y - point1.y;
		printf("	Parametric: r(t) = <%f, %f> + <%f, %f>t\n", point1.x, point1.y, u, v);
	}
	bool containsPointNear(vec3 point, float threshold = 0.02f) 
	{
		float distance = fabs(lineEquation.x * point.x + lineEquation.y * point.y + lineEquation.z) / 
			sqrtf(lineEquation.x * lineEquation.x + lineEquation.y * lineEquation.y);
		return distance < threshold;
	}

	std::vector<vec3> clipToViewPort()
	{
		//TODO finish this function, it should return the clipped line segment as a vector of 2 points, or an empty vector if the line is outside the viewport
		//borders
		
		vec3 left = cross(vec3(-1, -1, 1), vec3(-1, 1, 1));
		vec3 right = cross(vec3(1, -1, 1), vec3(1, 1, 1));
		vec3 bottom = cross(vec3(-1, -1, 1), vec3(1, -1, 1));
		vec3 top = cross(vec3(-1, 1, 1), vec3(1, 1, 1));
		std::vector<vec3> viewportEdges = { left, right, bottom, top };
		std::vector<vec3> endpoints;
		for (vec3 edge : viewportEdges)
		{
			vec3 edgePoint = cross(lineEquation, edge);
			//if z is 0 the two lines are parallel, and they don't intersect each other on this plane
			if (edgePoint.z > 0.0001f)
			{
				//normalize the point
				edgePoint.x /= edgePoint.z;
				edgePoint.y /= edgePoint.z;
				edgePoint.z /= edgePoint.z;
				//evaulate the validity of this point truly being on the viewports side
				if (edgePoint.x >= -1.0f && edgePoint.x <= 1.0f && edgePoint.y >= -1.0f && edgePoint.y <= 1.0f)
				{
					endpoints.push_back(edgePoint);
				}
			}
		}
		if (endpoints.size() > 2) 
		{
			endpoints.clear();
			return endpoints;
		}
		return endpoints;
	}
	//we might not need this
	vec3 intersect(Line other)
	{

	}
};
class LineCollection
{
	Geometry<vec3>*geometry = nullptr;
	std::vector<Line> lines;
public:
	LineCollection() { }
	void addLine(Line line) 
	{
		lines.push_back(line); 
		//TODO implement this line correctly
		for (vec3 endPoint : line.clipToViewPort())
		{
			geometry->Vtx().push_back(endPoint);
		}
		geometry->updateGPU();
	}
	void draw(GPUProgram* gpuProgram, vec3 color = vec3(0, 1, 1))
	{
		geometry->Draw(gpuProgram, GL_LINES, color);
	}
	void initialize()
	{
		geometry = new Geometry<vec3>;
	}
};


class Circle {
	//TODO implement this class
};
class CircleCollection
{
	Geometry<vec3>* geometry = nullptr;
	//TODO make a circlecollection 
	std::vector<Circle> circles;
public:
	CircleCollection() {}
	//TODO: implement functions 
	void initialize()
	{
		geometry = new Geometry<vec3>;
	}
	void draw(GPUProgram* gpuProgram, vec3 color = vec3(0, 1, 0))
	{
		geometry->Draw(gpuProgram, GL_POINTS, color);
	}

};

class Scene
{
	PointCollection points;
	LineCollection lines;
	CircleCollection circles;
public:
	//TODO: implement the functions below
	void initialize()
	{
		points.initialize();
		lines.initialize();
		circles.initialize();
	}
	void draw(GPUProgram* gpuProgram)
	{
		points.draw(gpuProgram);
		lines.draw(gpuProgram);
		circles.draw(gpuProgram);
	}
	//getters
	PointCollection& getPoints() { return points; }
	LineCollection& getLines() { return lines; }
	CircleCollection& getCircles() { return circles; }
	
};

enum Mode { PointMode, LineMode, CircleMode, IntersectMode };
class LineApp : public glApp {
	Scene scene;
	Mode mode;
	GPUProgram* gpuProgram = nullptr;
	//utilities
	int selected1 = -1;
	int selected2 = -1;
	int selected3 = -1;
public:
	LineApp() : glApp("Line app") {}
	void onInitialization() {
		mode = PointMode;
		glPointSize(10.0f);
		glLineWidth(3.0f);
		gpuProgram = new GPUProgram(vertSource, fragSource);
		scene.initialize();
	}
	void onDisplay() {                            //based on the GreenTriangleApp::onDisplay()
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, winWidth, winHeight);
		scene.draw(gpuProgram);
	}
	vec3 pxlToNDC(int pX, int pY)
	{
		float cX = (2.0f * pX) / winWidth - 1.0f;
		float cY = 1.0f - (2.0f * pY) / winHeight;

		return vec3(cX, cY, 1.0f);
	}
	void onKeyboard(int key) override
	{
		if (key == 'p')
		{
			mode = PointMode;
			printf("Point mode active\n");		//TODO: REMOVE BEFORE SUBMISSION
		}
		if (key == 'l')
		{
			mode = LineMode;
			printf("Line mode active\n");		//TODO: REMOVE BEFORE SUBMISSION
		}
		if (key == 'c')
		{
			mode = CircleMode;
			printf("Circle mode active\n");		//TODO: REMOVE BEFORE SUBMISSION
		}
		if (key == 'i')
		{
			mode = IntersectMode;
			printf("Intersect mode active\n");	//TODO: REMOVE BEFORE SUBMISSION
		}
	}
	void onMousePressed(MouseButton but, int pX, int pY) override
	{
		//TODO implement conversion from operating system pixel coordinates to normalized device coordinates
		if (but == MOUSE_LEFT)
		{
			vec3 cPoint = pxlToNDC(pX, pY);
			if (mode == PointMode)
			{
				scene.getPoints().addPoint(cPoint.x, cPoint.y);
				refreshScreen();
			}
			if (mode == LineMode)
			{
				if (selected1 == -1 && selected2 == -1)
				{
					selected1 = scene.getPoints().pickPoint(cPoint);
				}
				else if (selected1 != -1 && selected2 == -1)
				{ 
					selected2 = scene.getPoints().pickPoint(cPoint);
					if (selected2 != -1) {
						scene.getLines().addLine(Line(scene.getPoints().getPoint(selected1), scene.getPoints().getPoint(selected2)));
						selected1 = -1;
						selected2 = -1;
					}
					refreshScreen();
				}
				else if (selected1 != -1 && selected2 != -1 && selected1 != selected2)
				{
					
				}
			}
			if (mode == CircleMode)
			{

			}
			if (mode == IntersectMode)
			{

			}
		}
	}
};
LineApp app;