#pragma once

#include <GLFW/glfw3.h>
#include "main.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    struct key_struct {
        int code;
        vec3 action;
        bool pressed = false;
        key_struct(int code, vec3 action) : code(code), action(action) {}
    };
    static key_struct keys[] = {
    {GLFW_KEY_W, vec3(0, 0, -1)}, 
    {GLFW_KEY_A, vec3(-1, 0, 0)}, 
    {GLFW_KEY_S, vec3(0, 0, 1)}, 
    {GLFW_KEY_D, vec3(1, 0, 0)},
    {GLFW_KEY_SPACE, vec3(0, 1, 0)},
    {GLFW_KEY_Z, vec3(0, -1, 0)},
    };
    vec3 vel_tmp(0);
    for (key_struct &k : keys) {
        if (key == k.code) {
            k.pressed = (action != GLFW_RELEASE);
            
        }
        if (k.pressed) {
            vel_tmp += k.action;
        }
        
    }
    if (mods & GLFW_MOD_CONTROL) {
        vel_tmp *= 0.1;
    }

    vel = -vel_tmp.z * dir + vel_tmp.x * cross(dir, vec3(0, 1, 0)) + vec3(0, vel_tmp.y, 0);
}

void mouse_callback(GLFWwindow *win, double xpos, double ypos) {
    if (cursor_free)
        return;
    float sc = 0.001;
    cam_angle_x += sc*(xpos-win_w/2);
    cam_angle_y -= sc*(ypos-win_h/2);
    glfwSetCursorPos(win, win_w/2, win_h/2);
    dir = vec3(sin(cam_angle_x)*cos(cam_angle_y), sin(cam_angle_y), -cos(cam_angle_x)*cos(cam_angle_y));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cursor_free = !cursor_free;
        glfwSetInputMode(window, GLFW_CURSOR, cursor_free ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    win_w = width;
    win_h = height;
    glViewport(0, 0, width, height);
    update_projection();
}