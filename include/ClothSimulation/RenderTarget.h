#pragma once
#include <learnopengl/camera.h>
//模仿OGRE，RenderTarget设置为渲染的目标，可以是纹理，也可以是一个窗口，一个RenderTarget对应一个FrameBuff（帧缓冲）

class RenderTarget {
	Camera * camera;//一个RenderTarget对应一个Camera

};

