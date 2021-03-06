#pragma once

#include "Application.h"
#include "Renderer2D.h"

class Physics_2D_EngineApp : public aie::Application {
public:

	Physics_2D_EngineApp();
	virtual ~Physics_2D_EngineApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
};