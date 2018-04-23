/* Copyright (C) 2018 Maxim Atanasov - All Rights Reserved
 * You may not use, distribute or modify this code.
 * This code is proprietary and belongs to the "slavicTales"
 * project. See LICENCE.txt in the root directory of the project.
 *
 * E-mail: atanasovmaksim1@gmail.com
 */

#ifndef DRAWING_DEF
#define DRAWING_DEF

#include <defs.hpp>
#include <renderer/renderer_sdl/font_cache.hpp>
#include <assets_manager/assets.hpp>
#include <game_manager/level/level_data.hpp>
#include <game_manager/level/light.hpp>
#include <console/console.hpp>
#include <message_bus/message_bus.hpp>
#include <task_manager/task_manager.hpp>
#include <game_manager/level/camera.hpp>
#include <renderer/renderer_sdl/renderer_sdl.hpp>

///This object is responsible for issuing drawing commands and drawing the current level.
/**
 * Handles drawing the entities, text objects, lights, the console window and debug information on the screen.
 *
 * Messages this subsystem listens to: <br>
 *
 * <b>VSYNC_ON</b> - Activates VSYNC.
 *
 * Message must contain: a <b>nullptr</b>. <br>
 *
 * <b>VSYNC_OFF</b> - Disables VSYNC.
 *
 * Message must contain: a <b>nullptr</b>. <br>
 *
 * <b>SHOW_COLLISIONS</b> - Enables the drawing of collision boxes and coordinates in a __DEBUG build.
 *
 * Message must contain: a pointer to a <b>bool</b>. <br>
 *
 * <b>SHOW_FPS</b> - Show or hide the current fps in a __DEBUG build.
 *
 * Message must contain: a pointer to a <b>bool</b>. <br>
 *
 * <b>SET_DARKNESS</b> - Sets the current darkness level.
 *
 * Message must contain: a pointer to a <b>uint8_t</b>. <br>
 *
 * <b>ASSETS</b> - Updates the internal pointer to the assets. This messages is recieved from the asset manager
 * whenever new assets are loaded. <br>
 *
 * Message must contain: a pointer to a <b>ST::assets*</b> containing the assets struct. <br>
 *
 * <b>ENABLE_LIGHTING</b> - Enable or disable the lighting.
 *
 * Message must contain: a pointer to a <b>bool</b>. <br>
 */
class drawing_manager{
    private:
        //external dependencies - injected through main or the message bus
        message_bus* gMessage_bus{};
        task_manager* gTask_manager{};
		ST::assets* asset_ptr{};

        //a subscriber object - so we can subscribe to and recieve messages
        subscriber* msg_sub{};

        //rendering object - the interface
        renderer_sdl* gRenderer{};

        //CPU ticks since start - used for animating sprites
        Uint32 ticks = 0;


        //basically the argument passed to process_lights
        std::vector<ST::light>* lights_arg{};

        //Basically the viewport
        ST::camera Camera{};

        //Internal rendering resolution
        const int w_width = 1920;
        const int w_height = 1080;

        //variables for drawing light
        uint8_t lightmap[1920][1080]{};
        uint8_t darkness_level = 0;
        int lights_quality = 0;

        //debug
        bool collisions_shown = false;
        bool show_fps = true;
        bool lighting_enabled = true;

        //Drawing functions
        void draw_entities(std::vector<ST::entity>*);
        void draw_collisions(std::vector<ST::entity>*);
        void draw_coordinates(std::vector<ST::entity>*);
        void draw_lights();
        void draw_fps(double fps);
        void draw_console(console* cnsl);
        void draw_text_objects(std::vector<ST::text>*);

        //Pre-processing
        void process_lights(std::vector<ST::light>* arg);
        bool is_onscreen(ST::entity* i);

        //Other functions
        void handle_messages();
        void show_collisions();
        void hide_collisions();
        void set_darkness(uint8_t arg);
        static void process_lights_task(void* arg);

    public:
        drawing_manager() = default;
        int initialize(SDL_Window* win, message_bus* msg_bus, task_manager* tsk_mngr);
        void update(ST::level_data* temp, double, console* gConsole);
        void close();
};

#endif