#include <GLFW/glfw3.h>

// ** 开启线框模式
bool wireframe = false;

int key_state_left_control = GLFW_RELEASE;
int key_state_f = GLFW_RELEASE;

void processInputWireframe(GLFWwindow * window)
{
    // ** Ctrl+F 开启/关闭 线框模式
    int key_state_left_control_changed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    if (key_state_left_control_changed != key_state_left_control)
    {
        key_state_left_control = key_state_left_control_changed;
    }
    int key_state_f_changed = glfwGetKey(window, GLFW_KEY_F);
    if (key_state_f_changed != key_state_f)
    {
        key_state_f = key_state_f_changed;
        if (key_state_f == GLFW_PRESS && key_state_left_control == GLFW_PRESS)
        {
            wireframe = !wireframe;
        }
    }
}