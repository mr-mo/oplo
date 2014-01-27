#include "GlutApp.h"

class InstancingApp : public oplo::GlutApp
{
public:

	virtual void draw() override
	{}

private:

} g_app;


void resizeFunc(int w, int h) { g_app.resize(w, h); }

void mouseFunc(int button, int state, int x, int y) { g_app.mouseFunc(button, state, x, y); }

void passiveMotion(int x, int y) { g_app.passiveMotion(x, y); }

void motionFunc(int x, int y) { g_app.motionFunc(x, y); }

void keyEvent(unsigned char key, int x, int y) { g_app.keyEvent(key, x, y);  }

void drawFunc() { g_app.drawFunc(); }

int main(int argc, char* argv[])
{
	const char my_name[] = "Convex Instancing";

	glutInit(&argc, argv);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(10, 10);
	glutInitWindowSize(800, 600);

	glutCreateWindow(my_name);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stdout, "Error: %s\n", glewGetErrorString(err));

	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	g_app.initialize(true);
	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(drawFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(keyEvent);
	glutMainLoop();

	return 0;
}
