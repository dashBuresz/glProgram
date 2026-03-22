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
	//utils
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
			if (fabs(edgePoint.z) > 0.0001f)
			{
				//normalize the point
				edgePoint.x /= edgePoint.z;
				edgePoint.y /= edgePoint.z;
				edgePoint.z /= edgePoint.z;
				//evaulate the validity of this point truly being on the viewports side
				if (edgePoint.x >= -1.0f && edgePoint.x <= 1.0f && edgePoint.y >= -1.0f && edgePoint.y <= 1.0f)
				{
					//evaluate duplicates in case the line goes through the corner of the viewport
					bool dupe = false;
					for (size_t i = 0; i < endpoints.size(); i++)
					{
						if (fabs(endpoints[i].x - edgePoint.x) < 0.0001f
							&& fabs(endpoints[i].y - edgePoint.y) < 0.0001f)
						{
							dupe = true;
							break;
						}
					}
					if (!dupe) endpoints.push_back(edgePoint);
				}
			}
		}
		if (endpoints.size() != 2 && endpoints.size() != 0)
		{
			endpoints.clear();
		}
		return endpoints;
	}
	vec3 intersect(Line& other) 
	{ 
		vec3 intersectionPoint = cross(lineEquation, other.lineEquation); 
		if (fabs(intersectionPoint.z) > 0.0001f) 
		{ 
			intersectionPoint.x /= intersectionPoint.z; 
			intersectionPoint.y /= intersectionPoint.z; 
			intersectionPoint.z = 1.0f; 
		}
		return intersectionPoint; 
	}
	vec3 getP1() { return p1; }
	vec3 getP2() { return p2; }
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
	//utils
	Line& getLine(int idx)
	{
		return lines[idx];
	}
	int pickLine(vec3 mousePosition, float threshold = 0.02f)
	{
		for (size_t i = 0; i < lines.size(); i++)
		{
			if (lines[i].containsPointNear(mousePosition, threshold)) return i;
		}
		return -1;
	}
};


class Circle {
	vec3 p1, p2, p3, center;
	float radius;
public:
	Circle(vec3 point1, vec3 point2, vec3 point3) : p1(point1), p2(point2), p3(point3)
	{
		printf("Circle added\n");
		float A = 2 * (point2.x - point1.x);
		float B = 2 * (point2.y - point1.y);
		float C = point2.x * point2.x + point2.y * point2.y - point1.x * point1.x - point1.y * point1.y;
		float D = 2 * (point3.x - point1.x);
		float E = 2 * (point3.y - point1.y);
		float F = point3.x * point3.x + point3.y * point3.y - point1.x * point1.x - point1.y * point1.y;
		float determinant = A * E - B * D;
		if (fabs(determinant) < 0.0001f)
		{
			printf("No circle can be defined\n");
			return;
		}
		float centerX = (C * E - B * F) / determinant;
		float centerY = (A * F - C * D) / determinant;
		center = vec3(centerX, centerY, 1.0f);
		radius = sqrtf((centerX - point1.x) * (centerX - point1.x) + (centerY - point1.y) * (centerY - point1.y));
		printf("Circle added \n");
		printf("	Center: %f, %f\n", centerX, centerY);
		printf("	Radius: %f\n", radius);
		printf("	Implicit: (x - %f)^2 + (y - %f)^2 = %f^2\n", centerX, centerY, radius);
		printf("	Parametric: <<NOT IMPLEMENTED>>\n");
	}
	std::vector<vec3> intersect(Line& other)
	{
		std::vector<vec3> intersections;

		vec3 a = other.getP1();
		vec3 b = other.getP2();

		float x0 = a.x;
		float y0 = a.y;
		float u = b.x - a.x;
		float v = b.y - a.y;

		float dx = x0 - center.x;
		float dy = y0 - center.y;

		float A = u * u + v * v;
		float B = 2.0f * (u * dx + v * dy);
		float C = dx * dx + dy * dy - radius * radius;

		float D = B * B - 4.0f * A * C;

		if (D < -0.0001f) return intersections;

		if (fabs(D) < 0.0001f)
		{
			float t = -B / (2.0f * A);
			intersections.push_back(vec3(x0 + t * u, y0 + t * v, 1.0f));
			return intersections;
		}

		float sqrtD = sqrtf(D);
		float t1 = (-B + sqrtD) / (2.0f * A);
		float t2 = (-B - sqrtD) / (2.0f * A);

		intersections.push_back(vec3(x0 + t1 * u, y0 + t1 * v, 1.0f));
		intersections.push_back(vec3(x0 + t2 * u, y0 + t2 * v, 1.0f));

		return intersections;
	}
	std::vector<vec3> intersect(Circle& other)
	{
		std::vector<vec3> intersections;

		float x1 = center.x;
		float y1 = center.y;
		float r1 = radius;

		float x2 = other.center.x;
		float y2 = other.center.y;
		float r2 = other.radius;

		float dx = x2 - x1;
		float dy = y2 - y1;
		float d = sqrtf(dx * dx + dy * dy);

		if (d > r1 + r2 + 0.0001f) return intersections;
		if (d < fabs(r1 - r2) - 0.0001f) return intersections;
		if (d < 0.0001f && fabs(r1 - r2) < 0.0001f) return intersections;

		float a = (r1 * r1 - r2 * r2 + d * d) / (2.0f * d);
		float h2 = r1 * r1 - a * a;

		if (h2 < -0.0001f) return intersections;
		if (h2 < 0.0f) h2 = 0.0f;

		float h = sqrtf(h2);

		float xm = x1 + a * dx / d;
		float ym = y1 + a * dy / d;

		float rx = -dy * (h / d);
		float ry = dx * (h / d);

		vec3 p1(xm + rx, ym + ry, 1.0f);
		vec3 p2(xm - rx, ym - ry, 1.0f);

		intersections.push_back(p1);

		if (fabs(p1.x - p2.x) > 0.0001f || fabs(p1.y - p2.y) > 0.0001f)
		{
			intersections.push_back(p2);
		}

		return intersections;
	}
	vec3 getCenter() { return center; }
	float getRadius() { return radius; }
	bool containsPointNear(vec3 point, float threshold = 0.02f)
	{
		float distance = sqrtf((point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y));
		if (fabs(radius - distance) < threshold)
		{
			return true;
		}
		return false;
	}
};
class CircleCollection
{
	Geometry<vec3>* geometry = nullptr;
	std::vector<Circle> circles;
public:
	CircleCollection() {}
	void addCircle(Circle circle)
	{
		circles.push_back(circle);
		int circlePoints = 80;
		for (int i = 0; i < circlePoints; i++)
		{
			float t = 2.0f * M_PI * i / 80.0f;
			float x = circle.getCenter().x + circle.getRadius() * cosf(t);
			float y = circle.getCenter().y + circle.getRadius() * sinf(t);
			geometry->Vtx().push_back(vec3(x, y, 1.0f));
		}
		geometry->updateGPU();
	}
	void initialize()
	{
		geometry = new Geometry<vec3>;
	}
	void draw(GPUProgram* gpuProgram, vec3 color = vec3(0, 1, 0))
	{
		geometry->Draw(gpuProgram, GL_LINE_LOOP, color);
	}
	//utils
	Circle& getCircle(int idx)
	{
		return circles[idx];
	}
	int pickCircle(vec3 mousePosition, float threshold = 0.02f)
	{
		for (size_t i = 0; i < circles.size(); i++)
		{
			if (circles[i].containsPointNear(mousePosition, threshold)) return i;
		}
		return -1;
	}
};

class Scene
{
	PointCollection points;
	LineCollection lines;
	CircleCollection circles;
public:
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

	int selectedLine1 = -1;
	int selectedLine2 = -1;
	int selectedCircle1 = -1;
	int selectedCircle2 = -1;
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
			}
			if (mode == CircleMode)
			{
				if (selected1 == -1 && selected2 == -1 && selected3 == -1)
				{
					selected1 = scene.getPoints().pickPoint(cPoint);
				}
				else if (selected1 != -1 && selected2 == -1 && selected3 == -1)
				{
					selected2 = scene.getPoints().pickPoint(cPoint);
				}
				else if (selected1 != -1 && selected2 != -1 && selected3 == -1)
				{
					selected3 = scene.getPoints().pickPoint(cPoint);
					if (selected3 != -1)
					{
						scene.getCircles().addCircle(Circle(
							scene.getPoints().getPoint(selected1), 
							scene.getPoints().getPoint(selected2), 
							scene.getPoints().getPoint(selected3))
						);
						selected1 = -1;
						selected2 = -1;
						selected3 = -1;
					}
					refreshScreen();
				}
			}
			//TODO implement
			if (mode == IntersectMode)
			{
				int selectedLine = scene.getLines().pickLine(cPoint);
				int selectedCircle = scene.getCircles().pickCircle(cPoint);

				if (selectedLine1 == -1 && selectedLine2 == -1 &&
					selectedCircle1 == -1 && selectedCircle2 == -1)
				{
					if (selectedLine != -1)
					{
						selectedLine1 = selectedLine;
					}
					else if (selectedCircle != -1)
					{
						selectedCircle1 = selectedCircle;
					}
				}
				else if (selectedLine2 == -1 && selectedCircle2 == -1)
				{
					if (selectedLine != -1)
					{
						selectedLine2 = selectedLine;
					}
					else if (selectedCircle != -1)
					{
						selectedCircle2 = selectedCircle;
					}

					if (selectedLine1 != -1 && selectedLine2 != -1)
					{
						Line& l1 = scene.getLines().getLine(selectedLine1);
						Line& l2 = scene.getLines().getLine(selectedLine2);
						vec3 intersection = l1.intersect(l2);
						if (fabs(intersection.z) > 0.0001f)
						{
							scene.getPoints().addPoint(intersection);
							refreshScreen();
						}
					}
					else if (selectedLine1 != -1 && selectedCircle2 != -1)
					{
						Line& l = scene.getLines().getLine(selectedLine1);
						Circle& c = scene.getCircles().getCircle(selectedCircle2);

						std::vector<vec3> intersections = c.intersect(l);
						for (vec3 point : intersections)
						{
							scene.getPoints().addPoint(point);
						}
						refreshScreen();
					}
					else if (selectedCircle1 != -1 && selectedLine2 != -1)
					{
						Circle& c = scene.getCircles().getCircle(selectedCircle1);
						Line& l = scene.getLines().getLine(selectedLine2);

						std::vector<vec3> intersections = c.intersect(l);
						for (vec3 point : intersections)
						{
							scene.getPoints().addPoint(point);
						}
						refreshScreen();
					}
					else if (selectedCircle1 != -1 && selectedCircle2 != -1)
					{
						Circle& c1 = scene.getCircles().getCircle(selectedCircle1);
						Circle& c2 = scene.getCircles().getCircle(selectedCircle2);
						std::vector<vec3> intersections = c1.intersect(c2);
						for (vec3 point : intersections)
						{
							scene.getPoints().addPoint(point);
						}
						refreshScreen();
					}

					selectedLine1 = -1;
					selectedLine2 = -1;
					selectedCircle1 = -1;
					selectedCircle2 = -1;
				}
			}
		}
	}
};
LineApp app;