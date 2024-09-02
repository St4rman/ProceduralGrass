//=================================
//Author: Idhant Dabral/Starman
//Desc:	* Grass rendering using opengl and compute shaders
//		* currently in progress in a fresh repo
//		* past repo - https://github.com/St4rman/Graphics-Foliage
//
//		* Built on top of NCLGL
//		* showing medium density scene right now, can switch over to high density branch for higher showcase
//		* Ongoing feature -  rotate blades to camera
//==================================


#include "../NCLGL/window.h"
#include "Renderer.h"
#include "gui.h"

// Change this integer for different density scenes. Will have to also switch shader array values. 
// in gpuSceneVertex.glsl, gpuSceneFragment.glsl, and positionCompute.glsl. Change size of positions and yPos arrays 
// I wanted to automate this :[
// density    |  array size
//------------+-------------
//	base	0 | 160000
//	medium	1 | 640000
//	high	2 | 2560000

int density = 0;

int main()	{
	Window w("Grass time!", 1920, 1080, true);
	bool showMouse = false;
	w.LockMouseToWindow(true);
	w.ShowOSPointer(showMouse);
	

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer Renderer(w, density);
	if(!Renderer.HasInitialised()) {
		return -1;
	}
	
	gui* GUI = new gui(1920, 1080);
	if (!GUI->hasGuiInit()) {
		return -1;
	}

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){

		Renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		Renderer.RenderScene();

		GUI->BufferGuiData();
		GUI->RenderGui();
		GUI->UpdateCamPos(Renderer.getCamPos());

		Renderer.SwapBuffers();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F2)) {
			Renderer.updateWindSpeed(GUI->returnWSpeed() / 1000.0f);
			Renderer.updateWindDir(GUI->returnWDir());
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F3)) {
			showMouse != showMouse;
			w.ShowOSPointer(true);
		}
	}

	delete GUI;
	return 0;
}