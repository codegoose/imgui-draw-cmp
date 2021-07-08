#include <vector>
#include <utility>
#include <optional>

#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_draw_cmp.h"

std::tuple<int, int> window_center(GLFWwindow *const window) {
    int x, y, w, h;
    glfwGetWindowPos(window, &x, &y);
    glfwGetWindowSize(window, &w, &h);
    return { x + (w / 2), y + (h / 2) };
}

std::optional<int> window_display_hz(GLFWwindow *const window) {
    const auto [ cx, cy ] = window_center(window);
    int num_monitors;
    const auto monitors = glfwGetMonitors(&num_monitors);
    for (int i = 0; i < num_monitors; i++) {
        const auto this_monitor = monitors[i];
        int x, y, w, h;
        glfwGetMonitorWorkarea(this_monitor, &x, &y, &w, &h);
        if (cx >= x && cx < x + w && cy >= y && cy < y + h) {
            const auto this_monitor_vm = glfwGetVideoMode(this_monitor);
            if (this_monitor_vm) return this_monitor_vm->refreshRate;
            break;
        }
    }
    if (const auto primary_monitor = glfwGetPrimaryMonitor(); primary_monitor) {
        if (const auto primary_vm = glfwGetVideoMode(primary_monitor); primary_vm) {
            return primary_vm->refreshRate;
        }
    }
    return std::nullopt;
}

int main() {
    glfwInit();
    const int ifbw = 1024, ifbh = 768;
    const auto window = glfwCreateWindow(ifbw, ifbh, "Bezier Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    const auto imgui_ctx = ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImDrawCmpCache draw_cache;
    int rfbw = ifbw, rfbh = ifbh;
    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window)) {
        const auto hz = window_display_hz(window);
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        const auto [ cfbw, cfbh ] = [&]() -> std::tuple<int, int> {
            int dw, dh;
            glfwGetFramebufferSize(window, &dw, &dh);
            return { dw, dh };
        }();
        const auto draw_data = ImGui::GetDrawData();
        const bool draw_data_changed = !draw_cache.Compare(draw_data);
        const bool fb_size_changed = (cfbw != rfbw || cfbh != rfbh);
        const bool need_redraw = fb_size_changed || draw_data_changed;
        if (!need_redraw) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
            continue;
        }
        spdlog::info("Rendering: {}:{}", draw_data_changed, fb_size_changed);
        if (fb_size_changed) {
            spdlog::info("Framebuffer resized.");
            glViewport(0, 0, cfbw, cfbh);
            rfbw = cfbw;
            rfbh = cfbh;
        }
        glClearColor(.1, .1, .1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);
        glfwSwapBuffers(window);
    }
    return 0;
}
