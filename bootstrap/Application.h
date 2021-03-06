#pragma once

// forward declared structure for access to GLFW window
struct GLFWwindow;

namespace aie {

// this is the pure-virtual base class that wraps up an application for us.
// we derive our own applications from this class
class Application {
public:

	Application();
	virtual ~Application();

	// creates a window and begins the game loop which calls Update() and Draw() repeatedly
	// it first calls Startup() and if that succeeds it then starts the loop,
	// ending with Shutdown() if m_gameOver is true
	void run(const char* title, int width, int height, bool fullscreen);

	// these functions must be implemented by a derived class
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(float DeltaTime) = 0;
	virtual void Draw() = 0;

	// wipes the screen clear to begin a frame of drawing
	void ClearScreen();

	// sets the colour that the screen is cleared to
	void SetBackgroundColour(float r, float g, float b, float a = 1.0f);

	// show or hide the OS cursor
	void SetShowCursor(bool visible);

	// enable or disable v-sync
	void SetVSync(bool enabled);

	// sets m_gameOver to true which will close the application safely when the frame ends
	void Quit() { m_gameOver = true; }

	// access to the GLFW window
	GLFWwindow* GetWindowPtr() const { return m_window; }

	// query if the window has been closed somehow
	bool HasWindowClosed();

	// returns the frames-per-second that the loop is running at
	unsigned int GetFPS() const { return m_fps; }

	// returns the width / height of the game window
	unsigned int GetWindowWidth() const;
	unsigned int GetWindowHeight() const;
	
	// returns time since application started
	float GetTime() const;

protected:

	virtual bool createWindow(const char* title, int width, int height, bool fullscreen);
	virtual void destroyWindow();

	GLFWwindow*		m_window;

	// if set to false, the main game loop will exit
	bool			m_gameOver;
	
	unsigned int	m_fps;

};

} // namespace aie