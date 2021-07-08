# ImGui Draw Compare

This repo has code that's used to compare ImGui draw data for changes in order to minimize unnecessary re-rendering.

```ImDrawCmpCache::Compare()``` compares cached ```ImDrawVert```/```ImDrawIdx``` data from an ImGui context's ```ImDrawData``` to determine whether the GUI has actually changed. It returns ```true``` if no changes occurred. Essentially, ```Compare()``` is running ```memcmp```'s between the old and new vertex/index data for the UI.

The purpose is to reduce reduce idle GPU load that occurs otherwise.

Here's a simple reference:

``` c++
ImDrawCmpCache draw_cache;
while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    if (draw_cache.Compare(ImGui::GetDrawData())) continue; // Skip rendering if no changes.
    glClearColor(.1, .1, .1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(draw_data); // GUI is literally drawn here.
    glfwSwapBuffers(window);
}
```

There's a full implementation [**example**](https://github.com/codegoose/imgui_draw_cmp/blob/main/example.cpp) in this repo.
