// Memoory Leak Detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "GameWindow.h"


Blit3D *blit3D = NULL;
GameWindow* game = NULL;

void Init()
{
	game->init();
}

void DeInit(void)
{
	game->dispose();
	if(game) delete game;
}

void Update(double deltaTime)
{
	game->update(deltaTime);
}

void Draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	game->draw();
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	game->doInput(key, scancode, action, mods);
}




void DoResize(int width, int height)
{
	blit3D->trueScreenWidth = blit3D->screenWidth = static_cast<float>(width);
	blit3D->trueScreenHeight = blit3D->screenHeight = static_cast<float>(height);
	blit3D->Reshape(blit3D->shader2d);
}

void DoCursor(double x, double y)
{
	game->doCursor(x, y);
}

void DoMouseButton(int button, int action, int mods)
{
	game->doMouseButton(button, action, mods);
}


int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1280, 704);
	game = new GameWindow();

	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);
	blit3D->SetDoResize(DoResize);
	blit3D->SetDoCursor(DoCursor);
	blit3D->SetDoMouseButton(DoMouseButton);

	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}