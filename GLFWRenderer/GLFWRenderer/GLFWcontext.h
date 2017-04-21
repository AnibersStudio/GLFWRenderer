#include <string>
#include "GLCommon.h"
#include "GLState.h"
#include "CommonTools.h"
class Receiver
{
public:
	virtual bool CursorPosCallback(GLFWwindow * winptr, double x, double y) = 0;
	virtual bool KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode) = 0;
};

class GLFWcontext
{
public:
	GLFWcontext(int w, int h, std::string t, bool isfullscr);
	~GLFWcontext();
	void ShowCursor() const;
	void HideCursor() const;
	bool ChangeResolution(int w, int h);
	bool PollEvents() const;
	void RegisterReceiver(Receiver * r) const; 
private:
	int width;
	int height;
	std::string title;
	bool isfullscreen;
	GLFWwindow * winptr;
	GLState defaultstate;
};
