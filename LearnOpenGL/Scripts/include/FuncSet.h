#ifndef _FUNCSET_H_
#define _FUNCSET_H_

#include <GLFW/glfw3.h>

class FuncSet
{
public:
    int (*draw)(GLFWwindow *);
    void (*setup)(GLFWwindow *);
    void (*imgui)(GLFWwindow*);
    void (*viewport)(GLFWwindow *);
    FuncSet(void (*in_setup)(GLFWwindow *), int (*in_draw)(GLFWwindow *), void (*in_imgui)(GLFWwindow*) = nullptr, void (*in_viewport)(GLFWwindow *) = nullptr)
    {
        draw = in_draw;
        setup = in_setup;
        imgui = in_imgui;
        viewport = in_viewport;
    }
    FuncSet(){}
};

#endif