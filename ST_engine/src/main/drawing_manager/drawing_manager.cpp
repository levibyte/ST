#include <drawing_manager/drawing_manager.hpp>
#include <console/log.hpp>
#include <defs.hpp>

int drawing_manager::initialize(SDL_Window* window, message_bus* msg_bus, task_manager* tsk_mngr){
	if(TTF_Init() < 0){
		fprintf(stderr, "Failed to initialize SDL_TTF: %s\n", TTF_GetError());
		exit(1);
	}

    //set our external dependencies
    this->gMessage_bus = msg_bus;
    this->gTask_manager = tsk_mngr;
	this->window = window;

	//Create a new subscriber object and subscribe to certain messages
	msg_sub = new subscriber();
	gMessage_bus->subscribe(VSYNC_ON, msg_sub);
	gMessage_bus->subscribe(VSYNC_OFF, msg_sub);
	gMessage_bus->subscribe(SHOW_COLLISIONS, msg_sub);
    gMessage_bus->subscribe(SHOW_FPS, msg_sub);
	gMessage_bus->subscribe(SET_DARKNESS, msg_sub);
	gMessage_bus->subscribe(ASSETS, msg_sub);
	gMessage_bus->subscribe(RENDERER_SWITCH, msg_sub);
    gMessage_bus->subscribe(ENABLE_LIGHTING, msg_sub);

	//debug collisions aren't shown by default
	collisions_shown = false;

	//Variables for lights
	darkness_level = 0;
    lights_quality = 1;

	//Create and initialize the rendering object
	gRenderer = new renderer_sdl();
	gRenderer->initialize(window, w_width, w_height);
	gMessage_bus->send_msg(make_msg(VIRTUAL_SCREEN_COORDINATES, make_data<std::tuple<int, int>>(std::make_tuple(w_width, w_height))));
    return 0;
}

void drawing_manager::process_lights_task(void *arg){
    auto self = static_cast<drawing_manager*>(arg);
    self->process_lights(self->lights_arg);
}

void drawing_manager::update(level data, double fps, console* cnsl){
	level_data* temp = data.get_data();

	Camera = temp->Camera;
	handle_messages();
    lights_arg = &temp->lights;

    //start a task to pre-process lighting on a separate thread
    task_id id = gTask_manager->start_task(new task(process_lights_task, this, nullptr, -1));

	ticks = SDL_GetTicks(); //CPU ticks since start
	gRenderer->clear_screen();
	gRenderer->draw_background(temp->background);
	draw_entities(&temp->entities);

	gRenderer->draw_overlay(temp->overlay, ST::pos_mod(ticks, temp->overlay_spriteNum), temp->overlay_spriteNum);
    draw_text_objects(&temp->text_objects);

    //draw the lights when we are sure they are processed
    gTask_manager->wait_for_task(id);
    gRenderer->draw_lights(lightmap);


    #ifdef __DEBUG
	if (collisions_shown) {
		draw_collisions(&temp->entities);
		draw_coordinates(&temp->entities);
	}
	draw_fps(fps);
	draw_console(cnsl);
    #endif
	gRenderer->present();
}


//TODO: Finish it lol
void drawing_manager::draw_text_objects(std::vector<text>* objects) {
    for(auto i : *objects) {
        if (i.is_visible()) {
            gRenderer->draw_text(i.get_font(), i.get_text_string(), i.get_x(), i.get_y() - i.get_font_size(), i.get_color(),
                                 i.get_font_size(), 2);
        }
    }
}

void drawing_manager::draw_fps(double fps){
    if(show_fps) {
        SDL_Color color_font = {255, 0, 255, 255};
        gRenderer->draw_text("OpenSans-Regular.ttf", "fps:" + std::to_string((int) fps), 0, 40, color_font, 40, 1);
    }
}

void drawing_manager::draw_console(console* cnsl){
    if(cnsl->is_open()){
        gRenderer->draw_rectangle_filled(0, 0, w_width, w_height/2, cnsl->color);
        for(auto i = cnsl->entries.rbegin(); i != cnsl->entries.rend(); ++i) {
            if (cnsl->pos - cnsl->font_size - 50 + cnsl->scroll_offset <= w_height / 2 - cnsl->font_size * 2) {
                gRenderer->draw_text("OpenSans-Regular.ttf", i->text, 0,
                                     cnsl->pos - cnsl->font_size - 20 + cnsl->scroll_offset - 50, i->color,
                                     cnsl->font_size, -1);
            }
            cnsl->pos -= cnsl->font_size + 5;
        }
        gRenderer->draw_rectangle_filled(0, w_height/2 - cnsl->font_size - 12, w_width, 3, cnsl->color_text);
        gRenderer->draw_text("OpenSans-Regular.ttf", "Command: " + cnsl->composition, 0, w_height/2-50, cnsl->color_text,
                             cnsl->font_size, -1);
        cnsl->pos = w_height/2;
    }
}

void drawing_manager::process_lights(std::vector<light>* lights){
    #ifdef __DEBUG
    if(!lighting_enabled){
        return;
    }
    #endif
    for(int i = 0; i < w_width; i++){
        for(int j = 0; j < w_height; j++){
            lightmap[i][j] = darkness_level;
        }
    }
    for(unsigned int q = 0; q < lights->size(); q++){
        int x = lights->at(q).origin_x - Camera.x;
        int y = lights->at(q).origin_y - Camera.y;
        double count = 0;
        double step = darkness_level/(double)lights->at(q).radius;
        count = 0;
        int radius = lights->at(q).radius;
        int intensity = lights->at(q).intensity;
        if(x - radius - intensity > w_width || y - radius - intensity > w_height)
            continue;
        double step2 = 0;
        for(int i = y; i < y + radius + intensity; i++){
            for(int j = x; j < x + radius + intensity; j++){
                if(j > 0 && j < w_width && i > 0 && i < w_height)
                    lightmap[j][i] = (Uint8)lights->at(q).brightness + (Uint8)count;
                if(count + lights->at(q).brightness < darkness_level && j > x + intensity)
                    count += step;
            }
            count = 0;
            count += step2;
            if(step2 + lights->at(q).brightness < darkness_level && i > y + intensity)
                step2 += step;
        }
        count = 0;
        step2 = 0;
        for(int i = y; i > y - radius - intensity; i--){
            for(int j = x; j > x - radius - intensity; j--){
                if(j > 0 && j < w_width && i > 0 && i < w_height)
                    lightmap[j][i] = lights->at(q).brightness + (Uint8)count;
                if(count + lights->at(q).brightness < darkness_level && j < x - intensity)
                    count += step;
            }
            count = 0;
            count += step2;
            if(step2 + lights->at(q).brightness < darkness_level && i < y - intensity)
                step2 += step;
        }
        count = 0;
        step2 = 0;
        for(int i = y; i > y - radius - intensity; i--){
            for(int j = x; j < x + radius + intensity; j++){
                if(j > 0 && j < w_width && i > 0 && i < w_height)
                    lightmap[j][i] = lights->at(q).brightness + (Uint8)count;
                if(count + lights->at(q).brightness < darkness_level && j > x + intensity)
                    count += step;
            }
            count = 0;
            count += step2;
            if(step2 + lights->at(q).brightness < darkness_level && i < y - intensity)
                step2 += step;
        }
        count = 0;
        step2 = 0;
        for(int i = y; i < y + radius + intensity; i++){
            for(int j = x; j > x - radius - intensity; j--){
                if(j > 0 && j < w_width && i > 0 && i < w_height)
                    lightmap[j][i] = lights->at(q).brightness + (Uint8)count;
                if(count + lights->at(q).brightness< darkness_level && j < x - intensity)
                    count += step;
            }
            count = 0;
            count += step2;
            if(step2 + lights->at(q).brightness < darkness_level && i > y + intensity) {
                step2 += step;
            }
        }
    }
}

void drawing_manager::draw_lights(){
    #ifdef __DEBUG
    if(!lighting_enabled){
        return;
    }
    #endif
    //Losing a lot of fps here :)
    SDL_Rect tempRect = {0, 0, lights_quality, lights_quality};
    for(int i = 0; i <= w_width-lights_quality; i += lights_quality){
        int a = 1;
        tempRect.x = i;
        for(int j = 0; j <= w_height-lights_quality; j += lights_quality){
            if(lightmap[i][j] == lightmap[i][j+lights_quality] && j+lights_quality == w_height){
                tempRect.y = j - a*lights_quality;
                tempRect.h = (a+1)*lights_quality;
                SDL_Color light_color = {0, 0, 0, lightmap[i][j]};
                gRenderer->draw_rectangle_filled(tempRect.x, tempRect.y, tempRect.w, tempRect.h, light_color);
            }
            else if(lightmap[i][j] == lightmap[i][j+lights_quality]){
                a++;
            }else{
                tempRect.h = a*lights_quality;
                tempRect.y = j - a*lights_quality;
                SDL_Color light_color = {0, 0, 0, lightmap[i][j]};
                gRenderer->draw_rectangle_filled(tempRect.x, tempRect.y, tempRect.w, tempRect.h, light_color);
                a = 1;
            }
        }
    }
}



void drawing_manager::handle_messages(){
    message* temp = msg_sub->get_next_message();
    while(temp != nullptr){
        if(temp->msg_name == VSYNC_ON){
            gRenderer->vsync_on();
            gMessage_bus->send_msg(make_msg(VSYNC_IS_ON, nullptr));
        }
        else if(temp->msg_name == VSYNC_OFF){
            gRenderer->vsync_off();
            gMessage_bus->send_msg(make_msg(VSYNC_IS_OFF, nullptr));
        }
        else if(temp->msg_name == SET_DARKNESS){
            set_darkness(*(Uint8*)temp->get_data());
        }
        else if(temp->msg_name == SHOW_COLLISIONS){
            auto arg = (bool*)temp->get_data();
            if(*arg) {
                show_collisions();
            }
            else if(!(*arg)){
                hide_collisions();
            }
        }
        else if(temp->msg_name == SHOW_FPS){
            auto arg = (bool*)temp->get_data();
            if(*arg) {
                show_fps = true;
            }
            else if(!(*arg)){
                show_fps = false;
            }
        }
        else if(temp->msg_name == ENABLE_LIGHTING){
            auto arg = (bool*)temp->get_data();
            if(*arg) {
                lighting_enabled = true;
            }
            else if(!(*arg)){
                lighting_enabled = false;
            }
        }
        else if(temp->msg_name == ASSETS) {
            auto gAssets = (ST::assets **) temp->get_data();
            ST::assets *temp_ast = *gAssets;
            asset_ptr = temp_ast;
            gRenderer->upload_surfaces(&asset_ptr->surfaces);
            gRenderer->upload_fonts(&asset_ptr->fonts);
        }
        #ifdef __DEBUG
        else if(temp->msg_name == RENDERER_SWITCH){
            /*auto val = *(std::string*)temp->get_data();
			if(val == "sdl"){
				delete gRenderer;
				gRenderer = nullptr;
				gRenderer = new renderer_sdl();
				gRenderer->initialize(window, w_width, w_height);
                log(SUCCESS, "Renderer switched to SDL2");
			}else if(val == "opengl_legacy"){
				delete gRenderer;
				gRenderer = nullptr;
				gRenderer = new renderer_opengl_immediate();
				gRenderer->initialize(window, w_width, w_height);
                log(SUCCESS, "Renderer switched to OpenGL immediate mode");
			}else if(val == "opengl"){
				delete gRenderer;
				gRenderer = new renderer_opengl_retained();
				gRenderer->initialize(window, w_width, w_height);
                log(SUCCESS, "Renderer switched to OpenGL 3.3");
			}else{
                log(ERROR, "Unknown renderer specified, cannot switch!")
			}
			gRenderer->upload_surfaces(&asset_ptr->surfaces);
            gRenderer->upload_fonts(&asset_ptr->fonts);*/
        }
        #endif
        destroy_msg(temp);
        temp = msg_sub->get_next_message();
    }
}

void drawing_manager::set_darkness(Uint8 arg){
    darkness_level = arg;
    for(int i = 0; i < w_width; i++){
        for(int j = 0; j < w_height; j++)
            lightmap[i][j] = arg;
    }
}

void drawing_manager::draw_entities(std::vector<entity>* entities){
    for(auto& i : *entities){
        if(is_onscreen(&i)){
            if(i.get_animation_num() == 0){
                if(i.is_static()){
                    gRenderer->draw_texture(i.get_texture(), i.get_x(), i.get_y());
				}
                else{
                    gRenderer->draw_texture(i.get_texture(), i.get_x() - Camera.x, i.get_y() - Camera.y);
				}
            }
            else{
                int time = ticks >> 7; //ticks/128
                if(i.is_static()){
                    gRenderer->draw_sprite(i.get_texture(), i.get_x() , i.get_y(), ST::pos_mod(time, i.get_sprite_num()), i.get_animation(), i.get_animation_num(), i.get_sprite_num());
                }else{
					gRenderer->draw_sprite(i.get_texture(), i.get_x() - Camera.x, i.get_y() - Camera.y , ST::pos_mod(time, i.get_sprite_num()), i.get_animation(), i.get_animation_num(), i.get_sprite_num());
				}
            }
        }
    }
}

bool drawing_manager::is_onscreen(entity* i){
    if(!i->is_visible())
        return false;
    if(i->is_static())
        return true;
    return (*i).get_x() - Camera.x + (*i).get_tex_w() >= 0 && (*i).get_x() - Camera.x <= w_width
           && (*i).get_y()-Camera.y  > 0 && (*i).get_y()-Camera.y - (*i).get_tex_h() <= w_height;
}

void drawing_manager::draw_collisions(std::vector<entity>* entities){
    for(auto& i : *entities)
        if(is_onscreen(&i)){
            int Xoffset, Yoffset;
            if(i.is_static()){
                Xoffset = 0;
                Yoffset = 0;
            }else{
                Xoffset = Camera.x;
                Yoffset = Camera.y;
            }
            if(i.is_affected_by_physics()){
                SDL_Colour colour = {240, 0, 0, 100};
                gRenderer->draw_rectangle_filled(i.get_x() - Xoffset + i.get_col_x_offset(), i.get_y() - Yoffset + i.get_col_y_offset(), i.get_col_x(), i.get_col_y(), colour);
            }
            else{
                SDL_Colour colour2 = {0, 0, 220, 100};
                gRenderer->draw_rectangle_filled(i.get_x() - Xoffset + i.get_col_x_offset(), i.get_y() - Yoffset + i.get_col_y_offset(), i.get_col_x(), i.get_col_y(), colour2);
            }
        }
}

void drawing_manager::draw_coordinates(std::vector<entity>* entities){
    for(auto& i : *entities) {
        if (is_onscreen(&i)) {
            if (i.is_affected_by_physics()) {
                int Xoffset, Yoffset;
                if (i.is_static()) {
                    Xoffset = 0;
                    Yoffset = 0;
                } else {
                    Yoffset = Camera.y;
                    Xoffset = Camera.x;
                }
                std::string tempX = "x: " + std::to_string(i.get_x());
                std::string tempY = "y: " + std::to_string(i.get_y());
                SDL_Colour colour_text = {255, 255, 0, 255};
                gRenderer->draw_text("OpenSans-Regular.ttf", tempX, i.get_x() - Xoffset,
                                     i.get_y() - Yoffset - i.get_tex_h(), colour_text, 25, 1);
                gRenderer->draw_text("OpenSans-Regular.ttf", tempY, i.get_x() - Xoffset,
                                     i.get_y() - Yoffset - i.get_tex_h() + 30, colour_text, 25, 1);
            }
        }
    }
}

void drawing_manager::show_collisions(){
    collisions_shown = true;
}

void drawing_manager::hide_collisions(){
    collisions_shown = false;
}

void drawing_manager::close(){
    delete msg_sub;
	TTF_Quit();
    delete gRenderer;
}
