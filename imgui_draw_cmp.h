#pragma once

#include <vector>
#include <imgui.h>

struct ImDrawCmpCache {

    std::vector<std::vector<ImDrawVert>> prev_vertices;
    std::vector<std::vector<ImDrawIdx>> prev_indices;

    bool Compare(ImDrawData *const data) {
        const auto draw_list_num = data->CmdListsCount;
        const bool prev_vertices_match = [&]() {
            if (prev_vertices.size() < draw_list_num) return false;
            for (int i = 0; i < prev_vertices.size() && i < data->CmdListsCount; i++) {
                if (prev_vertices[i].size() < data->CmdLists[i]->VtxBuffer.size()) return false;
                if (memcmp(prev_vertices[i].data(), data->CmdLists[i]->VtxBuffer.Data, sizeof(ImDrawVert) * data->CmdLists[i]->VtxBuffer.size()) != 0) return false;
            }
            return true;
        }();
        const bool prev_indices_match = [&]() {
            if (prev_indices.size() < draw_list_num) return false;
            for (int i = 0; i < prev_indices.size() && i < data->CmdListsCount; i++) {
                if (prev_indices[i].size() < data->CmdLists[i]->IdxBuffer.size()) return false;
                if (memcmp(prev_indices[i].data(), data->CmdLists[i]->IdxBuffer.Data, sizeof(ImDrawIdx) * data->CmdLists[i]->IdxBuffer.size()) != 0)  return false;
            }
            return true;
        }();
        if (prev_vertices_match && prev_indices_match) return true;
        if (prev_vertices.size() < draw_list_num) prev_vertices.resize(draw_list_num);
        if (prev_indices.size() < draw_list_num) prev_indices.resize(draw_list_num);
        for (int i = 0; i < draw_list_num; i++) {
            const auto this_draw_list = data->CmdLists[i];
            const auto this_draw_list_vertex_num = this_draw_list->VtxBuffer.size();
            const auto this_draw_list_index_num = this_draw_list->IdxBuffer.size();
            if (prev_vertices[i].size() < this_draw_list_vertex_num) prev_vertices[i].resize(this_draw_list_vertex_num);
            memcpy(prev_vertices[i].data(), this_draw_list->VtxBuffer.Data, sizeof(ImDrawVert) * this_draw_list->VtxBuffer.size());
            if (prev_indices[i].size() < this_draw_list_index_num) prev_indices[i].resize(this_draw_list_index_num);
            memcpy(prev_indices[i].data(), this_draw_list->IdxBuffer.Data, sizeof(ImDrawIdx) * this_draw_list->IdxBuffer.size());
        }
        return false;
    }

};
