/* This file is part of the "slavicTales" project.
 * You may use, distribute or modify this code under the terms
 * of the GNU General Public License version 2.
 * See LICENCE.txt in the root directory of the project.
 *
 * Author: Maxim Atanasov
 * E-mail: atanasovmaksim1@gmail.com
 */

#ifndef INPUT_DEF
#define INPUT_DEF

#include <defs.hpp>
#include <input_manager/key_definitions.hpp>
#include <message_bus/message_bus.hpp>
#include <task_manager/task_manager.hpp>
#include <task_manager/task_allocator.hpp>

///This object is responsible for taking input
/**
 *
 * Messages this subsystem listens to: <br>
 *
 * <b>VIRTUAL_SCREEN_COORDINATES</b> - Tell the input manager what the virtual screen resolution is.
 *
 * Message must contain: a pointer to a <b>std::tuple<int, int></b> containing the width and the height of the viewport. <br>
 *
 * <b>REAL_SCREEN_COORDINATES</b> - Tell the input manager what the actual screen resolution is.
 *
 * Message must contain: a pointer to a <b>std::tuple<int, int></b> containing the width and the height of the screen. <br>
 *
 * <b>START_TEXT_INPUT</b> - Starts taking all keyboard input and sends <b>TEXT_STREAM</b> messages.
 *
 * Message must contain: a <b>nullptr</b>.<br>
 *
 * <b>STOP_TEXT_INPUT</b> - Stops taking all keyboard input.
 *
 * Message must contain: a <b>nullptr</b>.<br>
 *
 * <b>CLEAR_TEXT_STREAM</b> - Clears the text buffer if text_input is true.
 *
 * Message must contain: a <b>nullptr</b>.<br>
 *
 * <b>REGISTER_KEY</b> - Registers the given key and sends input events (pressed, held, released) for it.
 *
 * Message must contain: a pointer to a <b>ST::key</b>. <br>
 *
 * <b>UNREGISTER_KEY</b> - Unregisters a previously registered key and stops sending input events for it.
 *
 * Message must contain: a pointer to a <b>ST::key</b>. <br>
 */
class input_manager{
	private:

        struct game_controls{
            const uint16_t keys = 512;
            int8_t mouseClicks[3]{};
			int8_t mouseClicksFramePrev[3]{};
			int32_t mouseX_prev = 0, mouseY_prev = 0;
			int32_t mouseX = 0, mouseY = 0;
			int32_t mouse_scroll = 0;
			uint8_t keyboardFramePrev[512]{};
			const uint8_t* keyboard{};
        };

		int32_t v_width = 1, v_height = 1;
		int32_t r_width = 1, r_height = 1;
		float ratio_w = 1, ratio_h = 1;
		SDL_Event event{};
		game_controls controls{};
		message_bus* gMessage_bus{};
        task_manager* gTask_manager{};
		subscriber msg_sub{};
		std::vector<ST::key> registered_keys;

        //this is where we save the text stream from our input manager
        std::string composition;
        bool text_input = false;

		bool keypress(ST::key) const;
		bool keyheld(ST::key) const;
		bool keyrelease(ST::key) const;
		void handle_messages();
        void take_input();
        static void update_task(void* mngr);

    public:
        input_manager(message_bus* msg_bus, task_manager* tsk_mngr);
        void update();
};

//INLINED METHODS

/**
 * Starts the update_task() method using the task manager.
 */
inline void input_manager::update(){
	gTask_manager->start_task_lockfree(make_task(update_task, this, nullptr, -1));
}

#endif
