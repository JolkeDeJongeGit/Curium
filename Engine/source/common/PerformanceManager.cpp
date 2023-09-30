#include "precomp.h"
#include "common/PerformanceManager.h"
#include <unordered_map>
#include <deque>
#include <chrono>
#include <vector>
#include <include/imgui.h>
#include <include/implot.h>

namespace Performance
{
    using time_t = std::chrono::time_point<std::chrono::steady_clock>;
    using span_t = std::chrono::nanoseconds;

    struct entry
    {
        time_t m_start{};
        time_t m_end{};
        span_t m_accum{};
        float m_avg = 0.0f;
        std::deque<float> m_history;
    };
    std::unordered_map<std::string, entry> m_times;

}

Performance::Profiler::Profiler(const std::string& inName) : m_name(inName)
{
    StartTimer(m_name);
}

Performance::Profiler::~Profiler()
{
    EndTimer(m_name);
}

void Performance::StartTimer(const std::string& inName)
{
    m_times[inName].m_start = std::chrono::high_resolution_clock::now();
}

void Performance::EndTimer(const std::string& inName)
{
    auto& e = m_times[inName];
    e.m_end = std::chrono::high_resolution_clock::now();
    auto elapsed = e.m_end - e.m_start;
    e.m_accum += elapsed;
}

void Performance::DebugImgui(bool& inShow)
{
    ImGui::Begin("Profiler", &inShow, ImGuiWindowFlags_NoCollapse);

    for (auto& itr : m_times)
    {
        auto& e = itr.second;
        float duration = (float)((double)e.m_accum.count() / 1000000.0);
        if (e.m_history.size() > 100)
            e.m_history.pop_front();
        e.m_history.push_back(duration);

        e.m_avg = 0.0f;
        for (float f : e.m_history)
            e.m_avg += f;

        e.m_avg /= (float)e.m_history.size();
    }

    if (ImPlot::BeginPlot("Profiler", ImVec2(-1, 200)))
    {
        ImPlot::SetupAxes("Sample", "Time");
        ImPlot::SetupAxesLimits(0, 50, 0, 20);
        for (auto& itr : m_times)
        {
            auto& e = itr.second;

            std::vector<float> vals(
                e.m_history.begin(),
                e.m_history.end());

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded(itr.first.c_str(), vals.data(), (int)vals.size());
            ImPlot::PopStyleVar();
            ImPlot::PlotLine(itr.first.c_str(), vals.data(), (int)vals.size());
        }
        ImPlot::EndPlot();
    }

    for (auto& itr : m_times)
        ImGui::LabelText(itr.first.c_str(), "%f ms", itr.second.m_avg);

    ImGui::End();

    for (auto& itr : m_times)
        itr.second.m_accum = {};
}


