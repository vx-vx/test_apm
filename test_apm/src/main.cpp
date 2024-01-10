#include <list>
#include <string>
#include <vector>
#include <thread>
#include "../ui/gui/gui.h"
#include "../ui/imgui/imgui.h"
#include "header.h"

int __stdcall wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR arguments,
    int commandShow)
{

    // create gui
    gui::CreateHWindow("Menu");
    gui::CreateDevice();
    gui::CreateImGui();

    //create and detach threads
    std::thread time(timer);
    time.detach();

    std::thread get_actions(actions);
    get_actions.detach();
    std::thread apm_update(update_apm);
    apm_update.detach();
    std::thread apm_map(check_map_apm);
    apm_map.detach();
    /*
    std::thread cps(get_cps);
    cps.detach();
    std::thread cps_update(update_cps);
    cps_update.detach();
    std::thread cps_map(check_map_cps);
    cps_map.detach();
     */
    while (gui::isRunning)
    {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // destroy gui
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}