#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>

// Функция для настройки GLFW
void SetupGLFW() {
    if (!glfwInit()) {
        MessageBoxA(NULL, "Не удалось инициализировать GLFW", "Ошибка", MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
) {
    // Инициализация GLFW
    SetupGLFW();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Oscilloscope", NULL, NULL);
    if (!window) {
        MessageBoxA(NULL, "Не удалось создать окно GLFW", "Ошибка", MB_OK | MB_ICONERROR);
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Включение V-Sync

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float frequency = 1.0f;
    float horizontalScale = 1.0f;
    float verticalScale = 1.0f;
    float horizontalOffset = 0.0f;
    float verticalOffset = 0.0f;

    int selectedWave = 0; // 0 - синус, 1 - прямоугольные импульсы

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Обработка событий

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Oscilloscope");

        ImGui::Text("Select Wave Type:");
        ImGui::RadioButton("Sine Wave", &selectedWave, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Square Wave", &selectedWave, 1);

        // Рисуем график
        if (ImPlot::BeginPlot("Wave Plot", ImVec2(600, 400))) {
            const int numPoints = 1000;
            float x[numPoints], y[numPoints];
            float maxX = 2.0f * horizontalScale;

            for (int i = 0; i < numPoints; ++i) {
                x[i] = (i / (float)(numPoints - 1) * maxX - maxX / 2.0f) * horizontalScale + horizontalOffset;

                if (selectedWave == 0) {
                    // Синусоида
                    y[i] = sin(x[i] * frequency * 3.14159f) * verticalScale + verticalOffset;
                    ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Зеленый цвет
                }
                else {
                    // Прямоугольные импульсы
                    y[i] = (i % 50 < 25) ? verticalScale : -verticalScale;
                    y[i] += verticalOffset;
                    ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Красный цвет
                }
            }

            ImPlot::PlotLine("Wave", x, y, numPoints);
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }

        // Размещение кнопок справа от графика
        ImGui::SameLine();
        ImGui::BeginGroup();

        ImGui::BeginTable("Controls", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Horizontal Scale:");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("+ ##1", ImVec2(100, 50))) horizontalScale *= 1.1f;
        ImGui::SameLine();
        if (ImGui::Button("- ##2", ImVec2(50, 50))) horizontalScale /= 1.1f;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Horizontal Offset:");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("+ ##5", ImVec2(50, 50))) horizontalOffset += 0.1f;
        ImGui::SameLine();
        if (ImGui::Button("- ##6", ImVec2(50, 50))) horizontalOffset -= 0.1f;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Vertical Scale:");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("+ ##3", ImVec2(50, 50))) verticalScale *= 1.1f;
        ImGui::SameLine();
        if (ImGui::Button("- ##4", ImVec2(50, 50))) verticalScale /= 1.1f;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Vertical Offset:");
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("+ ##7", ImVec2(50, 50))) verticalOffset += 0.1f;
        ImGui::SameLine();
        if (ImGui::Button("- ##8", ImVec2(50, 50))) verticalOffset -= 0.1f;

        ImGui::EndTable();

        ImGui::EndGroup();

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}