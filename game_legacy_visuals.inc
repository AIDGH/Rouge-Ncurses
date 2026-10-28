/* Preserved 2025 presentation. Safety changes apply to both themes. */
static void legacy_draw_non_flashlight_room(WINDOW *game_win, Room *room, int room_number) {
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(!m_check){
        wattroff(game_win, COLOR_PAIR(3));
    }
    if(room->type == 's' && room->isVisible == true && room->doorCount == 1){
        wattron(game_win, COLOR_PAIR(15));
    }
    else if(room->type == 't' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(22));
    }
    else if(room->type == 'b' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(14));
    }
    else if(room->type == 'p' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_spell = 1;
        }
        wattron(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'r' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_reinforce = 1;
        }
        wattron(game_win, COLOR_PAIR(25));
    }
    else if(room->type == 's' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_healing = 1;
        }
        wattron(game_win, COLOR_PAIR(15));
    }
    if(room->type == 'm' && room->isVisible == true && all_print == 0){
        in_nightmare = 1;
        return;
    }
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
    }
    all_print = 1;
    in_nightmare = 0;
    room->lock_attemps = 0;
    // if(room->type == 'b'){
    //     for(int i = 0; i < room->pillarCount; i++){
    //         wattron(game_win, COLOR_PAIR(16));
    //         mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
    //         wattroff(game_win, COLOR_PAIR(16));
    //     }
    //     wattron(game_win, COLOR_PAIR(14));
    //     for (int y = room->y; y < room->y + room->height; y++) {
    //         for (int x = room->x; x < room->x + room->width; x++) {
    //             if(y == room->y && x == room->x){
    //                 mvwprintw(game_win, y, x, "┌");
    //             }
    //             else if(y == room->y + room->height - 1 && x == room->x){
    //                 mvwprintw(game_win, y, x, "└");
    //             }
    //             else if(y == room->y && x == room->x + room->width - 1){
    //                 mvwprintw(game_win, y, x, "┐");
    //             }
    //             else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
    //                 mvwprintw(game_win, y, x, "┘");
    //             }
    //             else if (y == room->y) {
    //                 mvwprintw(game_win, y, x, "─");
    //             } else if (y == room->y + room->height - 1) {
    //                 mvwprintw(game_win, y, x, "─");
    //             } else if (x == room->x || x == room->x + room->width - 1) {
    //                 mvwprintw(game_win, y, x, "│");
    //             } else {
    //                 mvwprintw(game_win, y, x, ".");
    //             }
    //         }
    //     }
    //     wattroff(game_win, COLOR_PAIR(14));
    //     wattron(game_win, COLOR_PAIR(6));
    //     for(int i = 0; i < room->enemyCount; i++){
    //         if(room->enemies[i].isVisible == true){
    //             mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
    //         }
    //     }
    //     wattroff(game_win, COLOR_PAIR(6));
    //     wattron(game_win, COLOR_PAIR(color_pair_num));
    //     draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    //     wattroff(game_win, COLOR_PAIR(color_pair_num));
    //     return;
    // }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                // mvwprintw(game_win, y, x, "┌");
                mvwprintw(game_win, y, x, "╔");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                // mvwprintw(game_win, y, x, "└");
                mvwprintw(game_win, y, x, "╚");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "┐");
                mvwprintw(game_win, y, x, "╗");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "┘");
                mvwprintw(game_win, y, x, "╝");
            }
            else if (y == room->y) {
                // mvwprintw(game_win, y, x, "─");
                mvwprintw(game_win, y, x, "═");
            } else if (y == room->y + room->height - 1) {
                // mvwprintw(game_win, y, x, "─");
                mvwprintw(game_win, y, x, "═");
            } else if (x == room->x || x == room->x + room->width - 1) {
                // mvwprintw(game_win, y, x, "│");
                mvwprintw(game_win, y, x, "║");
            } else {
                mvwprintw(game_win, y, x, ".");
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        if(room->type == 'b'){
            wattron(game_win, COLOR_PAIR(20));
        }
        mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        if(room->type == 'b'){
            wattroff(game_win, COLOR_PAIR(20));
        }
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "🛡");
            wattroff(game_win, COLOR_PAIR(19));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if(room->golds[i].type == 'n'){
                wattron(game_win, COLOR_PAIR(12));
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                wattroff(game_win, COLOR_PAIR(12));
            }
            else if(room->golds[i].type == 'b'){
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "🌑");
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if(room->spells[i].type == 's'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍾");
            }
            else if(room->spells[i].type == 'p'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍷");
            }
            else if(room->spells[i].type == 'h'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍸");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍓");
            }
            else if(room->specialfoods[i].type == 'p'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍘");
            }
            else if(room->specialfoods[i].type == 's'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍟");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false && room->weapons[i].count > 0){
            if(room->weapons[i].type == 'm'){
                wattron(game_win, COLOR_PAIR(16));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                wattroff(game_win, COLOR_PAIR(16));
            }
            else if(room->weapons[i].type == 'd'){
                wattron(game_win, COLOR_PAIR(10));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                wattroff(game_win, COLOR_PAIR(10));
            }
            else if(room->weapons[i].type == 'w'){
                wattron(game_win, COLOR_PAIR(17));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                wattroff(game_win, COLOR_PAIR(17));
                //  L'\U0001FA84'
            }
            else if(room->weapons[i].type == 'a'){
                wattron(game_win, COLOR_PAIR(18));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                wattroff(game_win, COLOR_PAIR(18));
            }
            else if(room->weapons[i].type == 's'){
                wattron(game_win, COLOR_PAIR(8));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                wattroff(game_win, COLOR_PAIR(8));
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true || t_check){
            wattron(game_win, COLOR_PAIR(6));
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->isVisible){
            game->whole_map[game->currentLevel][room->doors[i].cord.y][room->doors[i].cord.x].isVisible = true;
            if(room->type == 'p'){
                wattron(game_win, COLOR_PAIR(17));
            }
            else if(room->type == 's'){
                wattron(game_win, COLOR_PAIR(15));
            }
            else if(room->type == 'r'){
                wattron(game_win, COLOR_PAIR(25));
            }
        }
        if(room->doors[i].type == 'n'){
            // mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╬");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(color_pair_num));
                mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        wattroff(game_win, COLOR_PAIR(25));
        wattroff(game_win, COLOR_PAIR(15));
        wattroff(game_win, COLOR_PAIR(17));
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                wattron(game_win, COLOR_PAIR(6));
            }
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                wattroff(game_win, COLOR_PAIR(6));
            }
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->type == 'p'){
            wattron(game_win, COLOR_PAIR(17));
        }
        else if(room->type == 's'){
            wattron(game_win, COLOR_PAIR(15));
        }
        else if(room->type == 'r'){
            wattron(game_win, COLOR_PAIR(25));
        }
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "╣");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "╩");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╠");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╦");
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        if(first_stair == 0 && room->isVisible){
            first_stair = 1;
        }
        // if(room->type == 'p'){
        //     wattron(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattron(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        // if(room->type == 'p'){
        //     wattroff(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattroff(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
    }
    if(room->type == 's' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(15));
    }
    else if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
        wattroff(game_win, COLOR_PAIR(22));
        in_emoji_check();
        if(in_emoji == 0){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    else if(room->type == 'p' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){

        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}

static void legacy_draw_room(WINDOW *game_win, Room *room, int room_number) {
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(!m_check){
        wattroff(game_win, COLOR_PAIR(3));
    }
    if(room->type == 's' && room->isVisible == true && room->doorCount == 1){
        wattron(game_win, COLOR_PAIR(15));
    }
    else if(room->type == 't' && room->isVisible == true){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'b' && room->isVisible == true){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'p' && room->isVisible == true){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'r' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_reinforce = 1;
        }
        wattron(game_win, COLOR_PAIR(25));
    }
    else if(room->type == 's' && room->isVisible == true){
        if(room_number == game->player->room_num){
            in_healing = 1;
        }
        wattron(game_win, COLOR_PAIR(15));
    }
    if(room->type == 'm' && room->isVisible == true && all_print == 0){
        in_nightmare = 1;
        return;
    }
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
    }
    all_print = 1;
    in_nightmare = 0;
    room->lock_attemps = 0;
    if(m_check){
        draw_non_flashlight_room(game_win, room, room_number);
        return;
    }
    else if(room->type == 'm'){
        return;
    }
    else if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#' && room->isVisible == true){
        for (int y = room->y; y < room->y + room->height; y++) {
            for (int x = room->x; x < room->x + room->width; x++) {
                if(y == room->y && x == room->x){
                    mvwprintw(game_win, y, x, "╔");
                }
                else if(y == room->y + room->height - 1 && x == room->x){
                    mvwprintw(game_win, y, x, "╚");
                }
                else if(y == room->y && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "╗");
                }
                else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "╝");
                }
                else if (y == room->y) {
                    mvwprintw(game_win, y, x, "═");
                } else if (y == room->y + room->height - 1) {
                    mvwprintw(game_win, y, x, "═");
                } else if (x == room->x || x == room->x + room->width - 1) {
                    mvwprintw(game_win, y, x, "║");
                }
                else{
                    mvwprintw(game_win, y, x, " ");
                }
            }
        }
        for(int i = 0; i < room->doorCount; i++){
            if(room->isVisible){
                game->whole_map[game->currentLevel][room->doors[i].cord.y][room->doors[i].cord.x].isVisible = true;
                if(room->type == 'p'){
                    wattron(game_win, COLOR_PAIR(17));
                }
                else if(room->type == 's'){
                    wattron(game_win, COLOR_PAIR(15));
                }
                else if(room->type == 'r'){
                    wattron(game_win, COLOR_PAIR(25));
                }
            }
            if(room->doors[i].type == 'n'){
                // mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
                mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╬");
            }
            else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
                if(room->doors[i].is_open == false){
                    wattron(game_win, COLOR_PAIR(6));
                }
                else{
                    wattron(game_win, COLOR_PAIR(7));
                }
                mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
                if(room->doors[i].is_open == false){
                    wattroff(game_win, COLOR_PAIR(6));
                }
                else{
                    wattroff(game_win, COLOR_PAIR(7));
                }
                wattron(game_win, COLOR_PAIR(color_pair_num));
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
                wattroff(game_win, COLOR_PAIR(color_pair_num));
                if(room->doors[i].type == 'h'){
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                }
            }
            else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
                mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
            }
            wattroff(game_win, COLOR_PAIR(25));
            wattroff(game_win, COLOR_PAIR(15));
            wattroff(game_win, COLOR_PAIR(17));
        }
        if(room->window.x != 0 && room->window.x != 0){
            if(room->type == 'p'){
                wattron(game_win, COLOR_PAIR(17));
            }
            else if(room->type == 's'){
                wattron(game_win, COLOR_PAIR(15));
            }
            else if(room->type == 'r'){
                wattron(game_win, COLOR_PAIR(25));
            }
            if(room->window.x == room->x){
                mvwprintw(game_win, room->window.y, room->window.x, "╣");
            }
            else if(room->window.y == room->y){
                mvwprintw(game_win, room->window.y, room->window.x, "╩");
            }
            else if(room->window.x == room->x + room->width - 1){
                mvwprintw(game_win, room->window.y, room->window.x, "╠");
            }
            else if(room->window.y == room->y + room->height - 1){
                mvwprintw(game_win, room->window.y, room->window.x, "╦");
            }
        }
        return;
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            int dx1 = 0;
            int dy1 = 0;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker && room_number == game->player->room_num){
                if(y == room->y && x == room->x){
                    mvwprintw(game_win, y, x, "╔");
                }
                else if(y == room->y + room->height - 1 && x == room->x){
                    mvwprintw(game_win, y, x, "╚");
                }
                else if(y == room->y && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "╗");
                }
                else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "╝");
                }
                else if (y == room->y) {
                    mvwprintw(game_win, y, x, "═");
                } else if (y == room->y + room->height - 1) {
                    mvwprintw(game_win, y, x, "═");
                } else if (x == room->x || x == room->x + room->width - 1) {
                    mvwprintw(game_win, y, x, "║");
                } else {
                    mvwprintw(game_win, y, x, ".");
                }
            }
            else{
                if(y == room->y && x == room->x){
                    mvwprintw(game_win, y, x, "╔");
                }
                else if(y == room->y + room->height - 1 && x == room->x){
                    mvwprintw(game_win, y, x, "╚");
                }
                else if(y == room->y && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "╗");
                }
                else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                    mvwprintw(game_win, y, x, "╝");
                }
                else if (y == room->y) {
                    mvwprintw(game_win, y, x, "═");
                } else if (y == room->y + room->height - 1) {
                    mvwprintw(game_win, y, x, "═");
                } else if (x == room->x || x == room->x + room->width - 1) {
                    mvwprintw(game_win, y, x, "║");
                }
                else{
                    wattron(game_win, COLOR_PAIR(20));
                    if(room_number == game->player->room_num){
                        mvwprintw(game_win, y, x, ".");
                    }
                    else{
                        mvwprintw(game_win, y, x, " ");
                    }
                    wattroff(game_win, COLOR_PAIR(20));
                    if(room->type == 'r' && room->isVisible == true){
                        wattron(game_win, COLOR_PAIR(25));
                    }
                    else if(room->type == 's' && room->isVisible == true){
                        wattron(game_win, COLOR_PAIR(15));
                    }
                    else if(room->type == 'm' && room->isVisible == true){
                        wattron(game_win, COLOR_PAIR(6));
                    }
                }
            }
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->type == 'p'){
            wattron(game_win, COLOR_PAIR(17));
        }
        else if(room->type == 's'){
            wattron(game_win, COLOR_PAIR(15));
        }
        else if(room->type == 'r'){
            wattron(game_win, COLOR_PAIR(25));
        }
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "╣");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "╩");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╠");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╦");
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->isVisible){
            game->whole_map[game->currentLevel][room->doors[i].cord.y][room->doors[i].cord.x].isVisible = true;
            if(room->type == 'p'){
                wattron(game_win, COLOR_PAIR(17));
            }
            else if(room->type == 's'){
                wattron(game_win, COLOR_PAIR(15));
            }
            else if(room->type == 'r'){
                wattron(game_win, COLOR_PAIR(25));
            }
        }
        if(room->doors[i].type == 'n'){
            // mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╬");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            if(game->player->room_num == room_number){
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            }
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(color_pair_num));
                if(game->player->room_num == room_number){
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                }
                wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        wattroff(game_win, COLOR_PAIR(25));
        wattroff(game_win, COLOR_PAIR(15));
        wattroff(game_win, COLOR_PAIR(17));
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        if(room->type == 'b'){
            wattron(game_win, COLOR_PAIR(20));
        }
        int dx1 = 0, dy1 = 0;
        int x = room->pillars[i].x, y = room->pillars[i].y;
        if(x > game->player->cord.x){
            dx1 = 1;
        }
        else if(x < game->player->cord.y){
            dx1 = -1;
        }
        if(y > game->player->cord.y){
            dy1 = 1;
        }
        else if(y < game->player->cord.y){
            dy1 = -1;
        }
        int valid_checker = 0;
        if(side && (dx != 0 || dy != 0)){
            if(side == 1){
                if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 3){
                if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 2){
                if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
            else if(side == 4){
                if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
        }
        else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
            if(dx == 0 && dy == 0){
                valid_checker = 0;
            }
            else{
                valid_checker = 1;
            }
        }
        if(game->player->room_num == room_number){
            mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        }
        if(room->type == 'b'){
            wattroff(game_win, COLOR_PAIR(20));
        }
        wattroff(game_win, COLOR_PAIR(16));
    }
    if(room_number != game->player->room_num){
        if(room->stairCount == 1){
            if(first_stair == 0 && room->isVisible){
                first_stair = 1;
            }
            wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
            mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
            wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        }
        return;
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            int dx1 = 0, dy1 = 0;
            int x = room->foods[i].cord.x, y = room->foods[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            }
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            int dx1 = 0, dy1 = 0;
            int x = room->shields[i].cord.x, y = room->shields[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "🛡");
            }
            wattroff(game_win, COLOR_PAIR(19));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            int dx1 = 0, dy1 = 0;
            int x = room->golds[i].cord.x, y = room->golds[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "🌑");
                }
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            int dx1 = 0, dy1 = 0;
            int x = room->spells[i].cord.x, y = room->spells[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->spells[i].type == 's'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍾");
                }
                else if(room->spells[i].type == 'p'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍷");
                }
                else if(room->spells[i].type == 'h'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍸");
                }
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            int dx1 = 0, dy1 = 0;
            int x = room->specialfoods[i].cord.x, y = room->specialfoods[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍓");
                }
                else if(room->specialfoods[i].type == 'p'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍘");
                }
                else if(room->specialfoods[i].type == 's'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍟");
                }
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false && room->weapons[i].count > 0){
            int dx1 = 0, dy1 = 0;
            int x = room->weapons[i].cord.x, y = room->weapons[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                if(room->weapons[i].type == 'm'){
                    wattron(game_win, COLOR_PAIR(16));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                    wattroff(game_win, COLOR_PAIR(16));
                }
                else if(room->weapons[i].type == 'd'){
                    wattron(game_win, COLOR_PAIR(10));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                    wattroff(game_win, COLOR_PAIR(10));
                }
                else if(room->weapons[i].type == 'w'){
                    wattron(game_win, COLOR_PAIR(17));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                    wattroff(game_win, COLOR_PAIR(17));
                    //  L'\U0001FA84'
                }
                else if(room->weapons[i].type == 'a'){
                    wattron(game_win, COLOR_PAIR(18));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                    wattroff(game_win, COLOR_PAIR(18));
                }
                else if(room->weapons[i].type == 's'){
                    wattron(game_win, COLOR_PAIR(8));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                    wattroff(game_win, COLOR_PAIR(8));
                }
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true || t_check){
            int dx1 = 0, dy1 = 0;
            int x = room->traps[i].cord.x, y = room->traps[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                wattron(game_win, COLOR_PAIR(6));
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
                wattroff(game_win, COLOR_PAIR(6));
            }
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            int dx1 = 0, dy1 = 0;
            int x = room->enemies[i].cord.x, y = room->enemies[i].cord.y;
            if(x > game->player->cord.x){
                dx1 = 1;
            }
            else if(x < game->player->cord.y){
                dx1 = -1;
            }
            if(y > game->player->cord.y){
                dy1 = 1;
            }
            else if(y < game->player->cord.y){
                dy1 = -1;
            }
            int valid_checker = 0;
            if(side && (dx != 0 || dy != 0)){
                if(side == 1){
                    if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 3){
                    if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 2){
                    if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
                else if(side == 4){
                    if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                        valid_checker = 1;
                    }
                }
            }
            else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
                if(dx == 0 && dy == 0){
                    valid_checker = 0;
                }
                else{
                    valid_checker = 1;
                }
            }
            if(valid_checker){
                wattron(game_win, COLOR_PAIR(14));
                if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                    wattron(game_win, COLOR_PAIR(6));
                }
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
                if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
                    wattroff(game_win, COLOR_PAIR(6));
                }
                wattroff(game_win, COLOR_PAIR(14));
            }
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        if(first_stair == 0 && room->isVisible){
            first_stair = 1;
        }
        // if(room->type == 'p'){
        //     wattron(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattron(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        // if(room->type == 'p'){
        //     wattroff(game_win, COLOR_PAIR(17) | A_REVERSE);
        // }
        // else if(room->type == 'r'){
        //     wattroff(game_win, COLOR_PAIR(25) | A_REVERSE);
        // }
        // else{
            wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        // }
    }
    if(room->type == 's' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(15));
    }
    else if((room->type == 't' && room->isVisible == true) || room->type == 'b'){
        wattroff(game_win, COLOR_PAIR(22));
        in_emoji_check();
        if(in_emoji == 0){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    else if(room->type == 'p' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
        int dx1 = 0, dy1 = 0;
        int x = room->ancientkey.x, y = room->ancientkey.y;
        if(x > game->player->cord.x){
            dx1 = 1;
        }
        else if(x < game->player->cord.y){
            dx1 = -1;
        }
        if(y > game->player->cord.y){
            dy1 = 1;
        }
        else if(y < game->player->cord.y){
            dy1 = -1;
        }
        int valid_checker = 0;
        if(side && (dx != 0 || dy != 0)){
            if(side == 1){
                if(game->player->cord.y - y >= 0 && (game->player->cord.y - y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 3){
                if(game->player->cord.y - y <= 0 && (y - game->player->cord.y >= (x - game->player->cord.x)*dx1)){
                    valid_checker = 1;
                }
            }
            else if(side == 2){
                if(game->player->cord.x - x >= 0 && (game->player->cord.x - x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
            else if(side == 4){
                if(game->player->cord.x - x <= 0 && (x - game->player->cord.x >= (y - game->player->cord.y)*dy1)){
                    valid_checker = 1;
                }
            }
        }
        else if((x - game->player->cord.x)*dx >= 0 && (y - game->player->cord.y)*dy >= 0){
            if(dx == 0 && dy == 0){
                valid_checker = 0;
            }
            else{
                valid_checker = 1;
            }
        }
        if(valid_checker){
            wattron(game_win, COLOR_PAIR(12));
            mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
            wattroff(game_win, COLOR_PAIR(12));
        }
    }
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}

static void legacy_draw_last_room(WINDOW *game_win, Room *room, int room_number) {
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(3));
    wattron(game_win, COLOR_PAIR(14));
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                // mvwprintw(game_win, y, x, "╭");
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                // mvwprintw(game_win, y, x, "╰");
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "╮");
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                // mvwprintw(game_win, y, x, "╯");
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "│");
            } else {
                game->whole_map[game->currentLevel][y][x].item = '.';
                mvwprintw(game_win, y, x, ".");
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        wattron(game_win, COLOR_PAIR(6));
        if(room->traps[i].isVisible){
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
        }
        else{
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⟂");
        }
        wattroff(game_win, COLOR_PAIR(6));
        game->whole_map[game->currentLevel][room->traps[i].cord.y][room->traps[i].cord.x].item = '^';
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "+");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        // if(room->enemies[i].type == 'I'){
        //     game->whole_map[game->currentLevel][room->enemies[i].cord.y][room->enemies[i].cord.x].item = 'I';
        //     wattron(game_win, COLOR_PAIR(14));
        //     if(room->type == 't' && room->isVisible == true){
        //         wattron(game_win, COLOR_PAIR(6));
        //     }
        //     mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
        //     if(room->type == 't' && room->isVisible == true){
        //         wattroff(game_win, COLOR_PAIR(6));
        //     }
        //     wattroff(game_win, COLOR_PAIR(14));
        // }
        if(room->enemies[i].isAlive == true){
            wattron(game_win, COLOR_PAIR(17));
            game->whole_map[game->currentLevel][room->enemies[i].cord.y][room->enemies[i].cord.x].item = 'I';
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            if(room->enemies[i].health > 150){
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            else if(room->enemies[i].health > 100){
                wattron(game_win, COLOR_PAIR(14));
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            else if(room->enemies[i].health > 50){
                wattron(game_win, COLOR_PAIR(6));
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            else if(room->enemies[i].health <= 50){
                last_one = 1;
                wattron(game_win, COLOR_PAIR(21));
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x, "─");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x + 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x - 1, "│");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x + 1, "╯");
                mvwprintw(game_win, room->enemies[i].cord.y + 1, room->enemies[i].cord.x - 1, "╰");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x + 1, "╮");
                mvwprintw(game_win, room->enemies[i].cord.y - 1, room->enemies[i].cord.x - 1, "╭");
            }
            wattroff(game_win, COLOR_PAIR(17));
            wattroff(game_win, COLOR_PAIR(14));
            wattroff(game_win, COLOR_PAIR(21));
            wattroff(game_win, COLOR_PAIR(6));
        }
        else{
            room->enemies[i].canMove = true;
            wattron(game_win, COLOR_PAIR(20));
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            wattroff(game_win, COLOR_PAIR(20));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        mvwprintw(game_win, room->window.y, room->window.x, "=");
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "<");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, ">");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->type == 't' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(12));
    }
    if(room->keyCount == 1){
        wattron(game_win, COLOR_PAIR(12));
        game->whole_map[game->currentLevel][room->ancientkey.y][room->ancientkey.x].item = 'A';
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    // for (int y = room->y; y < room->y + room->height; y++) {
    //     for (int x = room->x; x < room->x + room->width; x++) {
    //         mvwprintw(game_win, y, x, "%c", game->whole_map[game->currentLevel][y][x].item);
    //     }
    // }
    wattron(game_win, COLOR_PAIR(color_pair_num));
    draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}

static void legacy_draw_nightmare_room(WINDOW *game_win, Room *room, int room_number) {
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
        in_nightmare = 1;
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                mvwprintw(game_win, y, x, "┌");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                mvwprintw(game_win, y, x, "└");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┐");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "┘");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "─");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "─");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "│");
            } else {
                if((x - game->player->cord.x <= 2 && game->player->cord.x - x <= 2) && (y - game->player->cord.y <= 2 && game->player->cord.y - y <= 2)){
                    mvwprintw(game_win, y, x, ".");
                }
                else{
                    mvwprintw(game_win, y, x, " ");
                }
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        if((room->pillars[i].x - game->player->cord.x <= 2 && game->player->cord.x - room->pillars[i].x <= 2)
        && (room->pillars[i].y - game->player->cord.y <= 2 && game->player->cord.y - room->pillars[i].y <= 2)){
            mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        }
        else{
            mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, " ");
        }
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            if((room->foods[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->foods[i].cord.x<= 2)
            && (room->foods[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->foods[i].cord.y <= 2)){
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            }
            else{
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            if((room->shields[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->shields[i].cord.x <= 2)
            && (room->shields[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->shields[i].cord.y <= 2)){
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "🛡");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
            }
            else{
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(19));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if((room->golds[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->golds[i].cord.x <= 2)
            && (room->golds[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->golds[i].cord.y <= 2)){
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "🌑");
                }
            }
            else{
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "  ");
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if((room->spells[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->spells[i].cord.x <= 2)
            && (room->spells[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->spells[i].cord.y <= 2)){
                if(room->spells[i].type == 's'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍾");
                }
                else if(room->spells[i].type == 'p'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍷");
                }
                else if(room->spells[i].type == 'h'){
                    mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍸");
                }
            }
            else{
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, " ");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if((room->specialfoods[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->specialfoods[i].cord.x <= 2)
            && (room->specialfoods[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->specialfoods[i].cord.y <= 2)){
                if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍓");
                }
                else if(room->specialfoods[i].type == 'p'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍘");
                }
                else if(room->specialfoods[i].type == 's'){
                    mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍟");
                }
            }
            else{
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, " ");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false){
            if((room->weapons[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->weapons[i].cord.x <= 2)
            && (room->weapons[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->weapons[i].cord.y <= 2)){
                if(room->weapons[i].type == 'm'){
                    wattron(game_win, COLOR_PAIR(16));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                    wattroff(game_win, COLOR_PAIR(16));
                }
                else if(room->weapons[i].type == 'd'){
                    wattron(game_win, COLOR_PAIR(10));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                    wattroff(game_win, COLOR_PAIR(10));
                }
                else if(room->weapons[i].type == 'w'){
                    wattron(game_win, COLOR_PAIR(17));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                    wattroff(game_win, COLOR_PAIR(17));
                    //  L'\U0001FA84'
                }
                else if(room->weapons[i].type == 'a'){
                    wattron(game_win, COLOR_PAIR(18));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                    wattroff(game_win, COLOR_PAIR(18));
                }
                else if(room->weapons[i].type == 's'){
                    wattron(game_win, COLOR_PAIR(8));
                    mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                    wattroff(game_win, COLOR_PAIR(8));
                }
            }
            else{
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, " ");
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(6));
            if((room->traps[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->traps[i].cord.x <= 2)
            && (room->traps[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->traps[i].cord.y <= 2)){
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
            }
            else{
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            if((room->enemies[i].cord.x - game->player->cord.x <= 2 && game->player->cord.x - room->enemies[i].cord.x <= 2)
            && (room->enemies[i].cord.y - game->player->cord.y <= 2 && game->player->cord.y - room->enemies[i].cord.y <= 2)){
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            }
            else{
                mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, " ");
            }
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            wattron(game_win, COLOR_PAIR(5));
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╂");
            wattroff(game_win, COLOR_PAIR(5));
        }
        else if(room->doors[i].type == 's' && room->doors[i].password == 1){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(color_pair_num));
            if((room->password_generator.x - game->player->cord.x <= 2 && game->player->cord.x - room->password_generator.x <= 2)
            && (room->password_generator.y - game->player->cord.y <= 2 && game->player->cord.y - room->password_generator.y <= 2)){
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            }
            else{
                mvwprintw(game_win, room->password_generator.y, room->password_generator.x, " ");
            }
            if(room->doors[i].type == 'h'){
            if((room->password_generator2.x - game->player->cord.x <= 2 && game->player->cord.x - room->password_generator2.x <= 2)
                && (room->password_generator2.y - game->player->cord.y <= 2 && game->player->cord.y - room->password_generator2.y <= 2)){
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                }
                else{
                    mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, " ");
                }
            }
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        wattron(game_win, COLOR_PAIR(5));
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "╡");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "╨");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╞");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╥");
        }
        wattroff(game_win, COLOR_PAIR(5));
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num));
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num));
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
    }
    if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
        wattron(game_win, COLOR_PAIR(12));
        if((room->ancientkey.x - game->player->cord.x <= 2 && game->player->cord.x - room->ancientkey.x <= 2)
        && (room->ancientkey.y - game->player->cord.y <= 2 && game->player->cord.y - room->ancientkey.y <= 2)){
            mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        }
        else{
            mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, " ");
        }
        wattroff(game_win, COLOR_PAIR(12));
    }
}

static void legacy_draw_attacked_room(WINDOW *game_win, Room *room, int room_number) {
    if(is_last_room){
        draw_last_room(game_win, room, room_number);
        return;
    }
    if(room->type == 'm' || room->type == 's' || room->type == 'p' || room->type == 'r' || room->type == 's'){
        return;
    }
    int temp = color_pair_num;
    if(color_pair_num == 0){
        color_pair_num = 14;
    }
    else if(room->type == 't' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(12));
    }
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                mvwprintw(game_win, y, x, "╔");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                mvwprintw(game_win, y, x, "╚");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "╗");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "╝");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "═");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "═");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "║");
            } else {
                if((x - game->player->cord.x <= 1 && game->player->cord.x - x <= 1) && (y - game->player->cord.y <= 1 && game->player->cord.y - y <= 1)){
                    wattron(game_win, COLOR_PAIR(color_pair_num));
                    mvwprintw(game_win, y, x, ".");
                    wattroff(game_win, COLOR_PAIR(color_pair_num));
                }
                else{
                    if(room->type == 't'){
                        wattron(game_win, COLOR_PAIR(12));
                    }
                    mvwprintw(game_win, y, x, ".");
                    if(room->type == 't'){
                        wattroff(game_win, COLOR_PAIR(12));
                    }
                }
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            if((room->foods[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->foods[i].cord.x<= 1)
            && (room->foods[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->foods[i].cord.y <= 1)){
                // wattron(game_win, COLOR_PAIR(color_pair_num));
                wattron(game_win, COLOR_PAIR(7));
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
                wattroff(game_win, COLOR_PAIR(7));
                // wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
            else{
                mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            }
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            if((room->shields[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->shields[i].cord.x<= 1)
            && (room->shields[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->shields[i].cord.y <= 1)){
                // wattron(game_win, COLOR_PAIR(color_pair_num));
                wattron(game_win, COLOR_PAIR(19));
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "🛡");
                wattroff(game_win, COLOR_PAIR(19));
                // wattroff(game_win, COLOR_PAIR(color_pair_num));
            }
            else{
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
                mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "🛡");
            }
            wattroff(game_win, COLOR_PAIR(19));
            wattron(game_win, COLOR_PAIR(color_pair_num));
            // mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, ".");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if((room->golds[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->golds[i].cord.x <= 1)
            && (room->golds[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->golds[i].cord.y <= 1)){
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    // wattron(game_win, COLOR_PAIR(color_pair_num));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    // wattron(game_win, COLOR_PAIR(color_pair_num));
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "🌑");
                }
            }
            else{
                if(room->golds[i].type == 'n'){
                    wattron(game_win, COLOR_PAIR(12));
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                    wattroff(game_win, COLOR_PAIR(12));
                }
                else if(room->golds[i].type == 'b'){
                    mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "🌑");
                }
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if(room->spells[i].type == 's'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍾");
            }
            else if(room->spells[i].type == 'p'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍷");
            }
            else if(room->spells[i].type == 'h'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍸");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍓");
            }
            else if(room->specialfoods[i].type == 'p'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍘");
            }
            else if(room->specialfoods[i].type == 's'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍟");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false){
            if(room->weapons[i].type == 'm'){
                wattron(game_win, COLOR_PAIR(16));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                wattroff(game_win, COLOR_PAIR(16));
            }
            else if(room->weapons[i].type == 'd'){
                wattron(game_win, COLOR_PAIR(10));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                wattroff(game_win, COLOR_PAIR(10));
            }
            else if(room->weapons[i].type == 'w'){
                wattron(game_win, COLOR_PAIR(17));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                wattroff(game_win, COLOR_PAIR(17));
                //  L'\U0001FA84'
            }
            else if(room->weapons[i].type == 'a'){
                wattron(game_win, COLOR_PAIR(18));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                wattroff(game_win, COLOR_PAIR(18));
            }
            else if(room->weapons[i].type == 's'){
                wattron(game_win, COLOR_PAIR(8));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                wattroff(game_win, COLOR_PAIR(8));
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(game->player->currentWeapon.type == 's' && game->player->weapons[4].count >= 2
        &&(room->traps[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->traps[i].cord.x <= 1)
        && (room->traps[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->traps[i].cord.y <= 1)){
            wattron(game_win, COLOR_PAIR(color_pair_num));
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            room->traps[i].isVisible = true;
        }
        if(room->traps[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(6));
            if((room->traps[i].cord.x - game->player->cord.x <= 1 && game->player->cord.x - room->traps[i].cord.x <= 1)
            && (room->traps[i].cord.y - game->player->cord.y <= 1 && game->player->cord.y - room->traps[i].cord.y <= 1)){
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
            }
            else{
                mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
            }
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╬");
        }
        else if(room->doors[i].type == 's' && room->doors[i].password == 1){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(temp));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(temp));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(temp));
                mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                wattroff(game_win, COLOR_PAIR(temp));
            }
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "╣");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "╩");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╠");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╦");
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    color_pair_num = temp;
}

static void legacy_draw_ranged_attacked_room(WINDOW *game_win, Room *room, int room_number, int dx, int dy) {
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(is_last_room){
        draw_last_room(game_win, room, room_number);
        return;
    }
    if(room->type == 'p'){
       draw_room(game_win, room, room_number);
       return;
    }
    if(room->type == 't'){
       draw_room(game_win, room, room_number);
       return;
    }
    if(room->type == 'r'){
       draw_room(game_win, room, room_number);
       return;
    }
    if(room->type == 's'){
       draw_room(game_win, room, room_number);
       return;
    }
    int temp = color_pair_num;
    if(color_pair_num == 0){
        color_pair_num = 14;
    }
    else if(room->type == 't' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(12));
    }
    if(room->type == 'm' && room->isVisible == true && all_print == 0){
        in_nightmare = 1;
        return;
    }
    if(room->type == 'm' && room->isVisible == true){
        wattron(game_win, COLOR_PAIR(6));
    }
    all_print = 1;
    in_nightmare = 0;
    room->lock_attemps = 0;
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if(y == room->y && x == room->x){
                mvwprintw(game_win, y, x, "╔");
            }
            else if(y == room->y + room->height - 1 && x == room->x){
                mvwprintw(game_win, y, x, "╚");
            }
            else if(y == room->y && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "╗");
            }
            else if(y == room->y + room->height - 1 && x == room->x + room->width - 1){
                mvwprintw(game_win, y, x, "╝");
            }
            else if (y == room->y) {
                mvwprintw(game_win, y, x, "═");
            } else if (y == room->y + room->height - 1) {
                mvwprintw(game_win, y, x, "═");
            } else if (x == room->x || x == room->x + room->width - 1) {
                mvwprintw(game_win, y, x, "║");
            } else {
                if(dx == 1){
                    if(x > game->player->cord.x && y == game->player->cord.y && x - game->player->cord.x <= game->player->currentWeapon.range){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dx == -1 && y == game->player->cord.y && game->player->cord.x - x <= game->player->currentWeapon.range){
                    if(x < game->player->cord.x){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == 1 && x == game->player->cord.x && y - game->player->cord.y <= game->player->currentWeapon.range){
                    if(y > game->player->cord.y){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == -1 && x == game->player->cord.x && game->player->cord.y - y <= game->player->currentWeapon.range){
                    if(y < game->player->cord.y){
                        wattron(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                mvwprintw(game_win, y, x, ".");
                if(dx == 1 && y == game->player->cord.y){
                    if(x > game->player->cord.x && x - game->player->cord.x <= game->player->currentWeapon.range){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dx == -1 && y == game->player->cord.y){
                    if(x < game->player->cord.x && game->player->cord.x - x <= game->player->currentWeapon.range){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == 1 && x == game->player->cord.x){
                    if(y > game->player->cord.y && y - game->player->cord.y <= game->player->currentWeapon.range){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
                else if(dy == -1 && x == game->player->cord.x && game->player->cord.y - y <= game->player->currentWeapon.range){
                    if(y < game->player->cord.y){
                        wattroff(game_win, COLOR_PAIR(color_pair_num));
                    }
                }
            }
        }
    }
    for(int i = 0; i < room->pillarCount; i++){
        wattron(game_win, COLOR_PAIR(16));
        mvwprintw(game_win, room->pillars[i].y, room->pillars[i].x, "▒");
        wattroff(game_win, COLOR_PAIR(16));
    }
    for(int i = 0; i < room->foodCount; i++){
        if(room->foods[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(7));
            mvwprintw(game_win, room->foods[i].cord.y, room->foods[i].cord.x, "♣");
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    for(int i = 0; i < room->shieldCount; i++){
        if(room->shields[i].isUsed == false){
            wattron(game_win, COLOR_PAIR(19));
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, " ");
            mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x, "🛡");
            wattroff(game_win, COLOR_PAIR(19));
        }
        wattron(game_win, COLOR_PAIR(color_pair_num));
        // mvwprintw(game_win, room->shields[i].cord.y, room->shields[i].cord.x + 1, ".");
        wattroff(game_win, COLOR_PAIR(color_pair_num));
    }
    for(int i = 0; i < room->goldCount; i++){
        if(room->golds[i].isUsed == false){
            if(room->golds[i].type == 'n'){
                wattron(game_win, COLOR_PAIR(12));
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "¤");
                wattroff(game_win, COLOR_PAIR(12));
            }
            else if(room->golds[i].type == 'b'){
                mvwprintw(game_win, room->golds[i].cord.y, room->golds[i].cord.x, "🌑");
            }
        }
    }
    for(int i = 0; i < room->spellCount; i++){
        if(room->spells[i].isUsed == false){
            if(room->spells[i].type == 's'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍾");
            }
            else if(room->spells[i].type == 'p'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍷");
            }
            else if(room->spells[i].type == 'h'){
                mvwprintw(game_win, room->spells[i].cord.y, room->spells[i].cord.x, "🍸");
            }
        }
    }
    for(int i = 0; i < room->specialfoodCount; i++){
        if(room->specialfoods[i].isUsed == false){
            if(room->specialfoods[i].type == 'n' || room->specialfoods[i].type == 'r'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍓");
            }
            else if(room->specialfoods[i].type == 'p'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍘");
            }
            else if(room->specialfoods[i].type == 's'){
                mvwprintw(game_win, room->specialfoods[i].cord.y, room->specialfoods[i].cord.x, "🍟");
            }
        }
    }
    for(int i = 0; i < room->weaponCount; i++){
        if(room->weapons[i].isUsed == false){
            if(room->weapons[i].type == 'm'){
                wattron(game_win, COLOR_PAIR(16));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002692');
                wattroff(game_win, COLOR_PAIR(16));
            }
            else if(room->weapons[i].type == 'd'){
                wattron(game_win, COLOR_PAIR(10));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U0001F5E1');
                wattroff(game_win, COLOR_PAIR(10));
            }
            else if(room->weapons[i].type == 'w'){
                wattron(game_win, COLOR_PAIR(17));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "⚚");
                wattroff(game_win, COLOR_PAIR(17));
                //  L'\U0001FA84'
            }
            else if(room->weapons[i].type == 'a'){
                wattron(game_win, COLOR_PAIR(18));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U000027B3');
                wattroff(game_win, COLOR_PAIR(18));
            }
            else if(room->weapons[i].type == 's'){
                wattron(game_win, COLOR_PAIR(8));
                mvwprintw(game_win, room->weapons[i].cord.y, room->weapons[i].cord.x, "%lc", L'\U00002694');
                wattroff(game_win, COLOR_PAIR(8));
            }
        }
    }
    for(int i = 0; i < room->trapCount; i++){
        if(room->traps[i].isVisible == true || t_check){
            wattron(game_win, COLOR_PAIR(6));
            mvwprintw(game_win, room->traps[i].cord.y, room->traps[i].cord.x, "⚺");
            wattroff(game_win, COLOR_PAIR(6));
        }
    }
    for(int i = 0; i < room->doorCount; i++){
        if(room->doors[i].type == 'n'){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "╬");
        }
        else if(room->doors[i].type == 'l' || room->doors[i].type == 'h'){
            if(room->doors[i].is_open == false){
                wattron(game_win, COLOR_PAIR(6));
            }
            else{
                wattron(game_win, COLOR_PAIR(7));
            }
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "☯");
            if(room->doors[i].is_open == false){
                wattroff(game_win, COLOR_PAIR(6));
            }
            else{
                wattroff(game_win, COLOR_PAIR(7));
            }
            wattron(game_win, COLOR_PAIR(temp));
            mvwprintw(game_win, room->password_generator.y, room->password_generator.x, "&");
            wattroff(game_win, COLOR_PAIR(temp));
            if(room->doors[i].type == 'h'){
                wattron(game_win, COLOR_PAIR(temp));
                mvwprintw(game_win, room->password_generator2.y, room->password_generator2.x, "&");
                wattroff(game_win, COLOR_PAIR(temp));
            }
        }
        else if((room->doors[i].type == 's' && room->doors[i].password == 1) || t_check){
            mvwprintw(game_win, room->doors[i].cord.y, room->doors[i].cord.x, "‽");
        }
    }
    for(int i = 0; i < room->enemyCount; i++){
        if(room->enemies[i].isVisible == true){
            wattron(game_win, COLOR_PAIR(14));
            mvwprintw(game_win, room->enemies[i].cord.y, room->enemies[i].cord.x, "%c", room->enemies[i].type - 32);
            wattroff(game_win, COLOR_PAIR(14));
        }
    }
    if(room->window.x != 0 && room->window.x != 0){
        if(room->window.x == room->x){
            mvwprintw(game_win, room->window.y, room->window.x, "╣");
        }
        else if(room->window.y == room->y){
            mvwprintw(game_win, room->window.y, room->window.x, "╩");
        }
        else if(room->window.x == room->x + room->width - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╠");
        }
        else if(room->window.y == room->y + room->height - 1){
            mvwprintw(game_win, room->window.y, room->window.x, "╦");
        }
    }
    if(room_number == 0 && game->currentLevel != 0){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    else if(room->stairCount == 1){
        wattron(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
        mvwprintw(game_win, room->stair.cord.y, room->stair.cord.x, "⛩ ");
        wattroff(game_win, COLOR_PAIR(color_pair_num) | A_REVERSE);
    }
    if(room->type == 's' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(15));
    }
    else if(room->type == 'p' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(17));
    }
    else if(room->type == 'm' && room->isVisible == true){
        wattroff(game_win, COLOR_PAIR(6));
    }
    if(room->keyCount == 1){

        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, room->ancientkey.y, room->ancientkey.x, "Δ");
        wattroff(game_win, COLOR_PAIR(12));
    }
    color_pair_num = temp;
    // wattroff(game_win, COLOR_PAIR(color_pair_num));
}

static void legacy_print_map(WINDOW *game_win){
    int map_width, map_height;
    getmaxyx(game_win, map_height, map_width);
    if(map_height>MAP_ROWS)map_height=MAP_ROWS;
    if(map_width>MAP_COLS)map_width=MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width - BORDER_PADDING;
    int end_y = map_height - BORDER_PADDING;

    // keypad(game_win, TRUE);
    game->levels[game->currentLevel]->rooms[0]->isVisible = true;
    for(int i = start_y; i < map_height; i++){
        for(int j = start_x; j < map_width; j++){
            if(game->whole_map[game->currentLevel][i][j].isVisible == true && game->whole_map[game->currentLevel][i][j].item == '#'){
                wattron(game_win, COLOR_PAIR(3));
                // mvwprintw(game_win, i, j, "%lc", game->whole_map[game->currentLevel][i][j].item);
                mvwprintw(game_win, i, j, "█");
                wattroff(game_win, COLOR_PAIR(3));
            }
            else{
                mvwprintw(game_win, i, j, " ");
            }
        }
    }
    for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
        if(game->levels[game->currentLevel]->rooms[i]->isVisible == true){
            draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
            if(game->levels[game->currentLevel]->rooms[i]->type == 'm'){
                draw_nightmare_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
                in_nightmare = 0;
            }
        }
    }
    int k = 0;
    // for(int i = start_y; i < map_height; i++){
    //     for(int j = start_x; j < map_width; j++){
    //         mvwprintw(game_win, game->whole_map[k]->cord.y, game->whole_map[k]->cord.x, "%lc", game->whole_map[k]->item);
    //         k++;
    //     }
    // }
    // for(int i = start_y; i < map_height; i++){
    //     for(int j = start_x; j < map_width; j++){
    //         mvwprintw(game_win, i, j, "%lc", game->whole_map[game->currentLevel][i][j]);
    //     }
    // }
    wattron(game_win, COLOR_PAIR(color_pair_num));
    draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    box(game_win, 0, 0);
    if(color_pair_num == 0){
        wattron(game_win, COLOR_PAIR(14));
    }
    mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
    mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
    mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(14));
    wrefresh(game_win);
}

static void legacy_print_all_map(WINDOW *game_win){
    int map_width, map_height;
    getmaxyx(game_win, map_height, map_width);
    if(map_height>MAP_ROWS)map_height=MAP_ROWS;
    if(map_width>MAP_COLS)map_width=MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING;

    for(int i = start_y; i < map_height; i++){
        for(int j = start_x; j < map_width; j++){
            if(game->whole_map[game->currentLevel][i][j].isVisible == true){
                wattron(game_win, COLOR_PAIR(3));
            }
            if(game->whole_map[game->currentLevel][i][j].isVisible == false){
                wattron(game_win, COLOR_PAIR(10));
            }
            if(game->whole_map[game->currentLevel][i][j].item == '#'){
                // mvwprintw(game_win, i, j, "%lc", game->whole_map[game->currentLevel][i][j].item);
                mvwprintw(game_win, i, j, "█");
            }
            wattroff(game_win, COLOR_PAIR(3));
            wattroff(game_win, COLOR_PAIR(10));
        }
    }
    for(int i = 0; i < game->levels[game->currentLevel]->roomsCount; i++){
        if(game->levels[game->currentLevel]->rooms[i]->isVisible == false){
            wattron(game_win, COLOR_PAIR(20));
        }
        all_print = 1;
        draw_room(game_win, game->levels[game->currentLevel]->rooms[i], i);
        if(game->levels[game->currentLevel]->rooms[i]->isVisible == false){
            wattroff(game_win, COLOR_PAIR(20));
        }
    }
    wattron(game_win, COLOR_PAIR(color_pair_num));
    draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
    box(game_win, 0, 0);
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    all_print = 0;
    wrefresh(game_win);
}

static void legacy_guid(WINDOW *game_win){
    int map_width, map_height;
    getmaxyx(game_win, map_height, map_width);
    if(map_height>MAP_ROWS)map_height=MAP_ROWS;
    if(map_width>MAP_COLS)map_width=MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    if(first_ancientkey == 1 || first_attack == 1 || first_damage == 1 || first_enemy_near == 1 || first_food == 1 || first_gold == 1 || first_stair == 1
    || first_password == 1 || first_shield == 1 || first_specialfood == 1 || first_spell == 1 || first_trap == 1 || first_weapon == 1 || first_lock_door == 1){
        mvwprintw(game_win, map_height - 10 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 9 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 8 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 7 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 6 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 5 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 4 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 3 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 2 - 1, map_width - 54 - 23, "                                                                        ");
        mvwprintw(game_win, map_height - 1 - 1, map_width - 54 - 23, "                                                                        ");
        wattron(game_win, COLOR_PAIR(color_pair_num));
        if(color_pair_num == 0){
            wattron(game_win, COLOR_PAIR(14));
        }
        mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
        mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
        mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
        mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
        // wattroff(game_win, COLOR_PAIR(color_pair_num));
        // wattroff(game_win, COLOR_PAIR(14));
    }
    // wattron(game_win, COLOR_PAIR(color_pair_num));
    if(first_ancientkey == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 14;
        int b = -1;
        first_ancientkey = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Ancient Keys are old and magical tools  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│    used to open all kind of doors.      │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│          Use them wisely!              _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ \\_");
    }
    else if(first_attack == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_attack = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Well done! You successfully hit your first │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ shot. You should kill monsters like this   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ You can see the amount of damage that you  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ dealt on combat report!                   _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_damage == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_damage = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Hey, when you get close to a monster it    │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ will damage you with each move.            │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ Some of them also follow you for a few     |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ steps or while you are in the room        _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_enemy_near == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_enemy_near = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Watch your steps! When you see the alert   │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ on the combat box, it means that there is  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ an enemy near you at it will be revealed,  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ but at first they are hidden.             _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_food == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_food = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ You should eat food to keep your hunger    │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ above 30 so you could regenerate you health│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ and also when you are out of food you will |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ loose health. Check it out on bar box!    _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_gold == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_gold = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Golds are kind of points for your XP and   │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ they will be saved forever when you collect│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ them. Look for them!                       |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ You can check them on your bar.           _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_lock_door == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_lock_door = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Some doors are locked and the need password│");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ to be unlocked!                            │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ password generators are shown with \"&\" in  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ the room, look for them and step on them! _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_password == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_password = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ When you step on a password generator its  │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ password will be shown on messages box and │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ it will disapear after a small period of   |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ time. (Some doors need 2 passwords!!!)    _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_shield == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_shield = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Shields are used to defend you against     │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ monsters damage. When you are out of shield│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ the damage will be health damage. It won't |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ work on traps. You can see it on your bar._\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_specialfood == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 19;
        int b = -2;
        first_specialfood = -1;
        mvwprintw(game_win, map_height - 13 - b, map_width - 54 - a, "╭──────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 12 - b, map_width - 54 - a, "│ Special foods are known to be one of the     │");
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "│ most precious kind of food that have magical │");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ effects! They will be rotten after a few time|");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ and loose their magic. You can also combine  |");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ two rotten food for a chance of enchanting   |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ spells. BTW normal ones will give you 20HP. _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰─────────────────────────────────────────────╯ \\_");
    }
    else if(first_trap == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 19;
        int b = -2;
        first_trap = -1;
        mvwprintw(game_win, map_height - 13 - b, map_width - 54 - a, "╭──────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 12 - b, map_width - 54 - a, "│ Traps are hidden in each room and they can   │");
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "│ deal damage to you or move you to battle room│");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ which you have to defeat all of the monsters |");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ in that room to be able to return and        |");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ continue your game. When a trap is revealed  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ It will loose its ability and stays visible._\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰─────────────────────────────────────────────╯ \\_");
    }
    else if(first_stair == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 19;
        int b = -1;
        first_stair = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭──────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ When you see a '<' or '>' sign; it means that│");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ you are in a stair room and you can go on the│");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ stair sign and press the same button of the  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ stair and move to next/previous level. '⛩' _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰─────────────────────────────────────────────╯ \\_");
    }
    else if(first_spell == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_spell = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ Spells are mysterious potions that will    │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ give you temporary effects!                │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ You can use them by pressing \"TAB\" button  |");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ to open your inventory and choose them.   _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    else if(first_weapon == 1){
        time(&quote_time);
        time(&quote_time2);
        int a = 17;
        int b = -1;
        first_weapon = -1;
        mvwprintw(game_win, map_height - 11 - b, map_width - 54 - a, "╭────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "│ RANGED: first press SPACE to aim.          │");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Then press an arrow / WASD key to FIRE.   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ You can use choose by pressing \"TAB\" button|");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ TAB: equipment. E: repeat. Esc: cancel.   _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────────╯ \\_");
    }
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(14));
}

static void legacy_quoting(WINDOW *game_win){
    int map_width, map_height;
    getmaxyx(game_win, map_height, map_width);
    if(map_height>MAP_ROWS)map_height=MAP_ROWS;
    if(map_width>MAP_COLS)map_width=MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    int message = rand() % 31;
    mvwprintw(game_win, map_height - 10 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 9 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 8 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 7 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 6 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 5 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 4 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 3 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 2 - 1, map_width - 54 - 23, "                                                                        ");
    mvwprintw(game_win, map_height - 1 - 1, map_width - 54 - 23, "                                                                        ");
    // mvwprintw(game_win, map_height - 0 - 1, map_width - 54 - 23, "                                                                        ");
    wattron(game_win, COLOR_PAIR(color_pair_num));
    if(color_pair_num == 0){
        wattron(game_win, COLOR_PAIR(14));
    }
    mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
    mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
    mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
    mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
    wattroff(game_win, COLOR_PAIR(color_pair_num));
    wattroff(game_win, COLOR_PAIR(14));
    wattron(game_win, COLOR_PAIR(3));
    if(message == 0){
        int a = 9;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│You could live a better life, if you⧹");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│had a better mind and a better body _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────┘");
    }
    else if(message == 1){
        int a = 12;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭───────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│You are deeply attached to your friends⧹");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│and acquaintances. Just leave it!      _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰──────────────────────────────────────┘");
    }
    else if(message == 2){
        int a = -1;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭──────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│It was all so different   │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│before everything changed._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰─────────────────────────┘  ¯");
    }
    else if(message == 3){
        int a = 8;
        int b = 1;
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, " /───────────────────────────────────\\");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "< Excellent day to have a rotten day._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, " \\__________________________________/  ¯");
    }
    else if(message == 4){
        int a = 14;
        int b = 0;
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│Q: What's the difference between Bell    │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Labs and the Boy Scouts of America?     │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│A: The Boy Scouts have adult supervision _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────────┘  ¯");
    }
    else if(message == 5){
        int a = 13;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Q: Why did the WASP cross the road?    │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ A: To get to the middle. :>           _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────────┘  ¯");
    }
    else if(message == 6){
        int a = 14;
        int b = 0;
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│Perilous to all of us are the devices of│");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│an art deeper than we ourselves possess.│");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ -- Gandalf the Grey                    │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ [J.R.R. Tolkien,\"Lord of the Rings\"]  _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────────┘  ¯");
    }
    else if(message == 7){
        int a = 14;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ The human race has one really effective │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ weapon, and that is laughter. =D        _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────────┘  ¯");
    }
    else if(message == 8){
        int a = 14;
        int b = 1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Q: How do you shoot a blue elephant?    │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ A: With a blue-elephant gun.            │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ Q: How do you shoot a pink elephant?    │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ A: Twist its trunk until it turns blue, │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│  then shoot it with a blue-elephant gun._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────────┘  ¯");
    }
    else if(message == 9){
        int a = 1;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Beware of a tall black man │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ With one blond shoe!       _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────┘  ¯");
    }
    else if(message == 10){
        int a = 10;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭─────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ Don't tell any big lies today.      │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ Small ones can be just as effective._⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────────┘  ¯");
    }
    else if(message == 11){
        int a = 23;
        int b = 1;
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╭──────────────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│You'll feel much better once you've given up hope.=\\");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰─────────────────────────────────────────────────┘  ¯¯");
    }
    else if(message == 12){
        int a = 9;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│You are sick, twisted and perverted.│");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│I like that in a person.           _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰───────────────────────────────────┘  ¯");
    }
    else if(message == 13){
        int a = 5;
        int b = 1;
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╭────────────────────────────────╮");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ A tall, dark and creep stranger │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│ will have more fun than you O_o _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰────────────────────────────────┘  ¯");
    }
    else if(message == 14){
        int a = 12;
        int b = 1;
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "╭───────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ Anyone who has had a bull by the tail │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ knows five or six more things         │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ than someone who hasn't.              │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "│  'Mark Twain'                        _⧹");
        mvwprintw(game_win, map_height - 4 - b, map_width - 54 - a, "╰─────────────────────────────────────┘  ¯");
    }
    else if(message == 15){
    int a = 13;
    int b = -1;
    mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
    mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"The only limit to our realization of  │");
    mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  tomorrow is our doubts of today.\"     │");
    mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ - Franklin D. Roosevelt                _⧹");
    mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ ¯");
    }
    else if(message == 16){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"You miss 100%% of the shots you don't   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  take.\"                                 │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ - Wayne Gretzky                        _⧹");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ ¯");
    }
    else if(message == 17){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"In the end, it's not the years in      │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  your life that count. It's the life    │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│  in your years.\"                        │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│  \'Abraham Lincoln\'                      _\\_");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰────────────────────────────────────────╯");
    }
    else if(message == 18){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"Do not go where the path may lead,     │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  go instead where there is no path and  │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│  leave a trail.\"                        │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ - Ralph Waldo Emerson                   _\\_");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰────────────────────────────────────────╯");
    }
    else if(message == 19){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"Life is what happens when you're busy  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  making other plans.\"                   │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ - John Lennon                           _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯  \\");
    }
    else if(message == 20){
        int a = 14;
        int b = 0;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Two antennas met on a roof, fell in     │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ love, and got married. The ceremony     │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ wasn't much, but the reception was      │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ excellent. <$_$>                        \\");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯\\_");
    }
    else if(message == 21){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I told my wife she was drawing her      │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ eyebrows too high. She looked surprised._⧹");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯ ¯");
    }
    else if(message == 22){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I'm reading a book about anti-gravity.  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ It's impossible to put down.            _⧹");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯ ¯");
    }
    else if(message == 23){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Did you hear about the guy who          │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ invented Lifesavers? He made a mint!    _\\");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰─────────────────────────────────────────╯ \\");
    }
    else if(message == 24){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Parallel lines have so much in common.  │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ It’s a shame they’ll never meet. =_(    _\\");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰────────────────────────────────────────╯  \\");
    }
    else if(message == 25){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ My wife told me I had to stop acting    │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ like a flamingo. So I had to put my     │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ foot down.                             _\\");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰────────────────────────────────────────╯ \\_");
    }
    else if(message == 26){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Why did the chicken go to the seance?   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ To talk to the other side.              │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰───────────────────────────────────────/\\_\\_");
    }
    else if(message == 27){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I used to be addicted to the hokey      │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ pokey, but then I turned myself around. │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰───────────────────────────────────────~\\_\\_");
    }
    else if(message == 28){
        int a = 13;
        int b = 0;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ \"The greatest glory in living lies not │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│  in never falling, but in rising every │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│  time we fall.\"                        │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "│ - Nelson Mandela                       │");
        mvwprintw(game_win, map_height - 5 - b, map_width - 54 - a, "╰──────────────────────────────────────`\\_\\_");
    }
    else if(message == 29){
        int a = 14;
        int b = -2;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ Did you hear about the claustrophobic   │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ astronaut? He needed a little space.    │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "╰───────────────────────────────────────¯\\_\\_");
    }
    else if(message == 30){
        int a = 14;
        int b = -1;
        mvwprintw(game_win, map_height - 10 - b, map_width - 54 - a, "╭─────────────────────────────────────────╮");
        mvwprintw(game_win, map_height - 9 - b, map_width - 54 - a, "│ I told my computer I needed a break,    │");
        mvwprintw(game_win, map_height - 8 - b, map_width - 54 - a, "│ and now it won't stop sending me        │");
        mvwprintw(game_win, map_height - 7 - b, map_width - 54 - a, "│ Kit Kat ads.                            │");
        mvwprintw(game_win, map_height - 6 - b, map_width - 54 - a, "╰───────────────────────────────────────¯\\_\\_");
    }
    wattroff(game_win, COLOR_PAIR(3));
}

static void legacy_messaging(WINDOW *game_win){
    { //Updating weapon
        if(game->player->currentWeapon.type == 'd'){
            game->player->currentWeapon = game->player->weapons[1];
        }
        else if(game->player->currentWeapon.type == 'w'){
            game->player->currentWeapon = game->player->weapons[2];
        }
        else if(game->player->currentWeapon.type == 'a'){
            game->player->currentWeapon = game->player->weapons[3];
        }
        else if(game->player->currentWeapon.type == 's'){
            game->player->currentWeapon = game->player->weapons[4];
        }
    }
    time(&move_time_2);
    if(difftime(move_time_2, move_time) >= 4){
        time(&move_time);
        mvwprintw(game_win, 3, 5, "                                                                                 ");
        mvwprintw(game_win, 3, 82, "                                                                                 ");
        mvwprintw(game_win, 2, 82, "                                                                                 ");
    }
    if(password_disapear % 20 == 19 || difftime(move_time_2, move_time_lock) >= 15){
        time(&move_time_lock);
        is_pass_lock = 0;
    }
    if(is_pass_lock == 0){
        time_t mid_time;
        time(&mid_time);
        long long pt = difftime(mid_time, start_time) * 5 * difficulty_efficiency;
        mvwprintw(game_win, 2, 5, "                                                                               ");
        wattron(game_win, COLOR_PAIR(7));
        mvwprintw(game_win, 2, 5, "Current level: %d🟢 Completed level: %d✅ Total points: %lldXP", game->currentLevel + 1, game->unlockedLevel, pointss + game->player->gold * 10 + pt);
        wattroff(game_win, COLOR_PAIR(7));
        if(game->unlockedLevel == -1){
            mvwprintw(game_win, 2, 5, "Current level: %d🟢 Completed level: %d✅ Total points: %lld", game->currentLevel + 1, 0, pointss + game->player->gold * 10 + pt);
        }
        else{
            if((game->currentLevel == game->levelCount - 1 && !is_last_room) || game->currentLevel == 6){
                mvwprintw(game_win, 2, 5, "Current level: %d🟢 Completed level: %d✅ Total points: %lldXP Enemy Left: %d❌", game->currentLevel + 1, game->unlockedLevel, pointss + game->player->gold * 10 + pt, game->levels[game->currentLevel]->rooms[0]->enemyLeft);
            }
            else{
                mvwprintw(game_win, 2, 5, "Current level: %d🟢 Completed level: %d✅ Total points: %lld", game->currentLevel + 1, game->unlockedLevel, pointss + game->player->gold * 10 + pt);
            }
        }
    }
    int map_width, map_height;
    getmaxyx(game_win, map_height, map_width);
    if(map_height>MAP_ROWS)map_height=MAP_ROWS;
    if(map_width>MAP_COLS)map_width=MAP_COLS;
    int start_x = BORDER_PADDING;
    int start_y = BORDER_PADDING;
    int end_x = map_width * 3 / 4 - BORDER_PADDING;
    int end_y = map_height * 3 / 4 - BORDER_PADDING + 5;
    int effects_count = 0;
    int a = (game->player->foodCount - 1) / 100;
    int spacing = 1;
    { // Effects
        if(game->player->Power_effect > 0){
            effects_count++;
        }
        if(game->player->Health_effect > 0){
            effects_count++;
        }
        if(game->player->Speed_effect > 0){
            effects_count++;
        }
    }
    { // Cleaning
        mvwprintw(game_win, map_height - 11, 4, "                                                                                       ");
        mvwprintw(game_win, map_height - 10, 4, "                                                                                       ");
        mvwprintw(game_win, map_height - 9, 4, "                                                                                       ");
        mvwprintw(game_win, map_height - 8, 4, "                                                                                       ");
        mvwprintw(game_win, map_height - 7, 4, "                                                                                       ");
        mvwprintw(game_win, map_height - 6, 4, "                                                                                       ");
        for(int j = 0; j < 20; j++){
            mvwprintw(game_win, map_height - 6, 5 + j, " ");
            mvwprintw(game_win, map_height - 5, 5 + j, " ");
            mvwprintw(game_win, map_height - 4, 5 + j, " ");
        }
        mvwprintw(game_win, map_height - 11, 5, "                                                                                       ");
        mvwprintw(game_win, map_height - 10, 5, "                                                                                       ");
        mvwprintw(game_win, map_height - 9, 5, "                                                                                       ");
        mvwprintw(game_win, map_height - 8, 5, "                                                                                       ");
        mvwprintw(game_win, map_height - 7, 5, "                                                                                       ");
        mvwprintw(game_win, map_height - 2, 4, "                                                                                       ");
        mvwprintw(game_win, map_height - 3, 4, "                                                                                       ");
    }
    in_emoji_check();
    if(in_emoji == 0){
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            wattron(game_win, A_REVERSE | COLOR_PAIR(color_pair_num));
            if(character != L'\U00000040' && character != L'\U00002117'
            && character != L'\U0000263A' && character != L'\U0000263B'
            && character != L'\U0000235F' && character != L'\U0000058D'
            && character != L'\U0000272F'
            && valid_move_check(game->player->cord.y, game->player->cord.x + 1)){
                draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            }
            else{
                draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            }
        }
        else{
            wattron(game_win, COLOR_PAIR(color_pair_num));
            draw_player_avatar(game_win,game->player->cord.y,game->player->cord.x);
            wattroff(game_win, COLOR_PAIR(color_pair_num));
        }
        if(game->whole_map[game->currentLevel][game->player->cord.y][game->player->cord.x].item == '#'){
            wattroff(game_win, A_REVERSE | COLOR_PAIR(color_pair_num));
        }
    }
    wattron(game_win, COLOR_PAIR(6));
    { // Spell state
        if(game->player->Speed_effect > 0){
            wattron(game_win, COLOR_PAIR(19));
            mvwprintw(game_win, map_height - 7 - effects_count, 5 + spacing, "You have Speed effect left for %d moves.🏃 ", game->player->Speed_effect);
            wattroff(game_win, COLOR_PAIR(19));
        }
        if(game->player->Power_effect > 0){
            wattron(game_win, COLOR_PAIR(17));
            int a = 0;
            if(game->player->Health_effect == 0){
                a++;
            }
            mvwprintw(game_win, map_height - 9 + a, 5 + spacing, "You have Strength effect left for %d moves.💪", game->player->Power_effect);
            wattroff(game_win, COLOR_PAIR(17));
        }
        if(game->player->Health_effect > 0){
            wattron(game_win, COLOR_PAIR(7));
            mvwprintw(game_win, map_height - 8, 5 + spacing, "You have healing effect left for %d moves.💟", game->player->Health_effect);
            wattroff(game_win, COLOR_PAIR(7));
        }
    }
    { // Weapon state
        wattron(game_win, COLOR_PAIR(14));
        if(game->player->currentWeapon.type == 'm'){
            mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Mace⚒");
        }
        else if(game->player->currentWeapon.type == 'd'){
            mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Dagger %d🗡", game->player->currentWeapon.count);
        }
        else if(game->player->currentWeapon.type == 'w'){
            mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Magic Wand %d⚚", game->player->currentWeapon.count);
        }
        else if(game->player->currentWeapon.type == 'a'){
            mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Bow and Arrow🏹 %d➳", game->player->currentWeapon.count);
        }
        else if(game->player->currentWeapon.type == 's'){
            if(game->player->currentWeapon.count == 1){
                mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Sword⚔ I");
            }
            if(game->player->currentWeapon.count == 2){
                mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Sword⚔ II");
            }
            if(game->player->currentWeapon.count == 3){
                mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Sword⚔ III");
            }
            if(game->player->currentWeapon.count == 4){
                mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Sword⚔ IV");
            }
            if(game->player->currentWeapon.count == 5){
                mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon: Sword⚔ V");
            }
        }
        wattroff(game_win, COLOR_PAIR(14));
        wattron(game_win, COLOR_PAIR(3));
        mvwprintw(game_win, map_height - 7, 5 + spacing, "Current weapon:");
        wattroff(game_win, COLOR_PAIR(3));
    }
    { // Health and Shield state
        int a = 0;
        if(in_spell){
            for(int j = 0; j < game->player->health / 10; j++){
                wchar_t heart = L'❤';
                mvwprintw(game_win, map_height - 6, 5 + spacing + 2*j, "💜");
            }
            if((game->player->health + 5) % 10 == 0){
                a = 1;
                mvwprintw(game_win, map_height - 6, 4 + spacing + 2*game->player->health / 10, "🍆");
            }
        }
        else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 'r'){
            for(int j = 0; j < game->player->health / 10; j++){
                mvwprintw(game_win, map_height - 6, 5 + spacing + 2*j, "💙");
            }
            if((game->player->health + 5) % 10 == 0){
                a = 1;
                mvwprintw(game_win, map_height - 6, 4 + spacing + 2*game->player->health / 10, "💧");
            }
        }
        else if(game->levels[game->currentLevel]->rooms[game->player->room_num]->type == 's'){
            for(int j = 0; j < game->player->health / 10; j++){
                wchar_t heart = L'❤';
                mvwprintw(game_win, map_height - 6, 5 + spacing + 2*j, "💚");
            }
            if((game->player->health + 5) % 10 == 0){
                a = 1;
                mvwprintw(game_win, map_height - 6, 4 + spacing + 2*game->player->health / 10, "🥒");
            }
        }
        else{
            wattron(game_win, COLOR_PAIR(6));
            for(int j = 0; j < game->player->health / 10; j++){
                wchar_t heart = L'❤';
                mvwprintw(game_win, map_height - 6, 5 + spacing + 2*j, "❤");
            }
            if((game->player->health + 5) % 10 == 0){
                a = 1;
                mvwprintw(game_win, map_height - 6, 4 + spacing + 2*game->player->health / 10, "💔");
            }
        }
        wattroff(game_win, COLOR_PAIR(6));
        wattron(game_win, COLOR_PAIR(19));
        for(int j = 0; j < game->player->shield / 10; j++){
            mvwprintw(game_win, map_height - 6, 5 + 2*game->player->health / 10 + spacing + 2*j + a, "🌌");
        }
        if((game->player->shield + 5) % 10 == 0){
            mvwprintw(game_win, map_height - 6, 4 + spacing + 2*game->player->shield / 10 + 2*game->player->health / 10 + a, "🌀");
        }
        wattroff(game_win, COLOR_PAIR(19));
    }
    { // Food and Gold state
        for(int j = 0; j < game->player->foodCount / 10; j++){
            mvwprintw(game_win, map_height - 5 + j / 10, 5 + spacing + 2*j - 2*(j/10)*10, "🍖");
        }
        if((game->player->foodCount + 5) % 10 == 0){
            mvwprintw(game_win,  map_height - 5 + game->player->foodCount / 100, 4 + spacing + 2*game->player->foodCount / 10 - 2*(game->player->foodCount / 100)*10, "🥓");
        }
        if(poisonous_food_check == 1){
            mvwprintw(game_win,  map_height - 5 + game->player->foodCount / 100, 5 + spacing + 2*game->player->foodCount / 10 - 2*(game->player->foodCount / 100)*10, "🥩");
            poisonous_food_check = 0;
        }
        wattron(game_win, COLOR_PAIR(12));
        mvwprintw(game_win, map_height - 4 + a, 4 + spacing, " 💰%d$", game->player->gold);
        wattroff(game_win, COLOR_PAIR(12));
    }
    { //Borders
        wattron(game_win, COLOR_PAIR(15));
        mvwprintw(game_win, 1, 4, "┌───────Messages────────────────────────────────────────────────────────────┐");
        mvwprintw(game_win, 2, 80, "│");
        mvwprintw(game_win, 3, 80, "│");
        mvwprintw(game_win, 2, 4, "│");
        mvwprintw(game_win, 3, 4, "│");
        mvwprintw(game_win, 4, 4, "└───────────────────────────────────────────────────────────────────────────┘");
        mvwprintw(game_win, 1, 81, "┌───────Combat-Report───────────────────────────────────────────────────────┐");
        mvwprintw(game_win, 2, 80 + 77, "│");
        mvwprintw(game_win, 3, 80 + 77, "│");
        mvwprintw(game_win, 2, 81, "│");
        mvwprintw(game_win, 3, 81, "│");
        mvwprintw(game_win, 4, 81, "└───────────────────────────────────────────────────────────────────────────┘");
        mvwprintw(game_win, map_height - 8 - effects_count, 4, "┌───────Bar─────────────────────────────────────────────────────────────────┐");
        mvwprintw(game_win, map_height - 7 - effects_count, 80, "│");
        mvwprintw(game_win, map_height - 6 - effects_count, 80, "│");
        mvwprintw(game_win, map_height - 5 - effects_count, 80, "│");
        mvwprintw(game_win, map_height - 4 - effects_count, 80, "│");
        mvwprintw(game_win, map_height - 7 - effects_count, 4, "│");
        mvwprintw(game_win, map_height - 6 - effects_count, 4, "│");
        mvwprintw(game_win, map_height - 5 - effects_count, 4, "│");
        mvwprintw(game_win, map_height - 4 - effects_count, 4, "│");
        mvwprintw(game_win, map_height - 7, 80, "│");
        mvwprintw(game_win, map_height - 6, 80, "│");
        mvwprintw(game_win, map_height - 5, 80, "│");
        mvwprintw(game_win, map_height - 4, 80, "│");
        mvwprintw(game_win, map_height - 3, 80, "│");
        mvwprintw(game_win, map_height - 7, 4, "│");
        mvwprintw(game_win, map_height - 6, 4, "│");
        mvwprintw(game_win, map_height - 5, 4, "│");
        mvwprintw(game_win, map_height - 4, 4, "│");
        mvwprintw(game_win, map_height - 3, 4, "│");
        mvwprintw(game_win, map_height - 3 + a, 4, "└───────────────────────────────────────────────────────────────────────────┘");
        mvwprintw(game_win, 2, 159, "☰");
        mvwprintw(game_win, 3, 158, "\u2009🎒");
        mvwhline(game_win, start_y - 1, start_x - 1, ACS_HLINE, map_width - 11);
        mvwhline(game_win, end_y, start_x - 1, ACS_HLINE, map_width - 11);
        mvwprintw(game_win, start_y - 1, start_x - 2, "╭");
        mvwprintw(game_win, start_y - 1, map_width - 6, "╮");
        mvwprintw(game_win, end_y, start_x - 2, "╰");
        mvwprintw(game_win, end_y, map_width - 6, "╯");
        for(int i = start_y; i < end_y; i++){
            mvwprintw(game_win, i, start_x - 2, "│");
            mvwprintw(game_win, i, map_width - 6, "│");
        }
        wattroff(game_win, COLOR_PAIR(15));
        time(&last_room2);
        last_room2 = time(NULL);
        drag2 = clock();
        if(game->currentLevel == game->levelCount - 1){
            if((double)(drag2 - drag1) / CLOCKS_PER_SEC >= 0.03){
                which_pos = 1 - which_pos;
                time(&last_room1);
                drag1 = clock();
                int a = map_height - 32;
                int b = map_width - 60;
                mvwprintw(game_win, a, b, "                                            ");
                mvwprintw(game_win, a + 1, b, "                                            ");
                mvwprintw(game_win, a + 2, b, "                                            ");
                mvwprintw(game_win, a + 3, b, "                                            ");
                mvwprintw(game_win, a + 4, b, "                                            ");
                mvwprintw(game_win, a + 5, b, "                                            ");
                mvwprintw(game_win, a + 6, b, "                                            ");
                mvwprintw(game_win, a + 7, b, "                                            ");
                mvwprintw(game_win, a + 8, b, "                                            ");
                mvwprintw(game_win, a + 9, b, "                                            ");
                mvwprintw(game_win, a + 10, b, "                                            ");
                mvwprintw(game_win, a + 11, b, "                                            ");
                mvwprintw(game_win, a + 12, b, "                                            ");
                mvwprintw(game_win, a + 13, b, "                                            ");
                mvwprintw(game_win, a + 14, b, "                                            ");
                b -= 41;
                a -= 5;
                mvwprintw(game_win, a, b, "                                                  ");
                mvwprintw(game_win, a + 1, b, "                                                  ");
                mvwprintw(game_win, a + 2, b, "                                                  ");
                mvwprintw(game_win, a + 3, b, "                                                  ");
                mvwprintw(game_win, a + 4, b, "                                                  ");
                mvwprintw(game_win, a + 5, b, "                                                  ");
                mvwprintw(game_win, a + 6, b, "                                                  ");
                mvwprintw(game_win, a + 7, b, "                                                  ");
                mvwprintw(game_win, a + 8, b, "                                                  ");
                mvwprintw(game_win, a + 9, b, "                                                  ");
                mvwprintw(game_win, a + 10, b, "                                                  ");
                mvwprintw(game_win, a + 11, b, "                                                  ");
                mvwprintw(game_win, a + 12, b, "                                                  ");
                mvwprintw(game_win, a + 13, b, "                                                  ");
                mvwprintw(game_win, a + 14, b, "                                                  ");
                b += 41;
                a += 5;
                wattron(game_win, COLOR_PAIR(14));
                if(last_one){
                    b -= 41;
                    a -= 5;
                    wattron(game_win, COLOR_PAIR(17));
                    mvwprintw(game_win, a + 3, b, "╭────────────────────────────────────────────╮");
                    mvwprintw(game_win, a + 4, b, "│          What is happening?!!!             │");
                    mvwprintw(game_win, a + 5, b, "│   I think I have underestimated you...     │");
                    mvwprintw(game_win, a + 5, b, "│After all... Finally! A mighty HERO appeared│");
                    mvwprintw(game_win, a + 6, b, "│ I always knew I have a glorious death...  _\\");
                    mvwprintw(game_win, a + 7, b, "╰───────────────────────────────────────────╯ \\");
                    mvwprintw(game_win, a + 8, b, "                                               \\");
                    mvwprintw(game_win, a + 9, b, "                                                \\");
                    mvwprintw(game_win, a + 10, b, "                                                \\_____");
                    b += 41;
                    a += 5;
                }
                else if(is_last_room){
                    b -= 40;
                    a -= 5;
                    wattron(game_win, COLOR_PAIR(6));
                    if(game->levels[game->currentLevel]->rooms[0]->enemies[game->levels[game->currentLevel]->rooms[0]->enemyCount - 1].health >= 120){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│                IMMORTAL!!!                │");
                        mvwprintw(game_win, a + 5, b, "│      The GREATEST MoNSteR of all time     │");
                        mvwprintw(game_win, a + 6, b, "│Lets see how you can come up with this one_\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    if(game->levels[game->currentLevel]->rooms[0]->enemies[game->levels[game->currentLevel]->rooms[0]->enemyCount - 1].health <= 100){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│ Well maybe you are not that bad little one│");
                        mvwprintw(game_win, a + 5, b, "│ But still not good ENOUGH!!!              │");
                        mvwprintw(game_win, a + 6, b, "│ Catch it Immortalll...                   _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    b += 40;
                    a += 5;
                }
                else{
                    b -= 40;
                    a -= 5;
                    if(game->levels[game->currentLevel]->rooms[0]->enemyLeft > 15){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│ Welcome to Treasure Level you little hero │");
                        mvwprintw(game_win, a + 5, b, "│ Lets see if you can handle all of my      │");
                        mvwprintw(game_win, a + 6, b, "│ monsters in this room!                   _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    else if(game->levels[game->currentLevel]->rooms[0]->enemyLeft <= 10 && game->levels[game->currentLevel]->rooms[0]->enemyLeft >= 4){
                        mvwprintw(game_win, a + 3, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 4, b, "│ Ha Ha HAAA looking at you on this filed   │");
                        mvwprintw(game_win, a + 5, b, "│ is joyously and also funny how you suffer!│");
                        mvwprintw(game_win, a + 6, b, "│ I still have a BIG suprise for you       _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    else if(game->levels[game->currentLevel]->rooms[0]->enemyLeft <= 1){
                        mvwprintw(game_win, a + 4, b, "╭───────────────────────────────────────────╮");
                        mvwprintw(game_win, a + 5, b, "│ I guess you are ready for my suprise...   │");
                        mvwprintw(game_win, a + 6, b, "│ And now... RISE of the---                _\\");
                        mvwprintw(game_win, a + 7, b, "╰──────────────────────────────────────────╯ \\");
                        mvwprintw(game_win, a + 8, b, "                                              \\");
                        mvwprintw(game_win, a + 9, b, "                                               \\");
                        mvwprintw(game_win, a + 10, b, "                                               \\_____");
                    }
                    b += 40;
                    a += 5;
                }
                if(which_pos){
                    mvwprintw(game_win, a, b, " <>=======() ");
                    mvwprintw(game_win, a + 1, b, "(/\\___   /|\\          ()==========<>_");
                    mvwprintw(game_win, a + 2, b, "      \\_/ | \\        //|\\   ______/ \\)");
                    mvwprintw(game_win, a + 3, b, "        \\_|  \\      // | \\_/");
                    mvwprintw(game_win, a + 4, b, "          \\|\\/|\\_   //  /\\/");
                    mvwprintw(game_win, a + 5, b, "           (oo)\\ \\_//  /");
                    mvwprintw(game_win, a + 6, b, "          //_/\\_\\/ /  |");
                    mvwprintw(game_win, a + 7, b, "         @@/  |=\\  \\  |");
                    mvwprintw(game_win, a + 8, b, "              \\_=\\_ \\ |");
                    mvwprintw(game_win, a + 9, b, "                \\==\\ \\|\\_");
                    mvwprintw(game_win, a + 10, b, "             __(\\===\\(  )\\");
                    mvwprintw(game_win, a + 11, b, "            (((~) __(_/   |");
                    mvwprintw(game_win, a + 12, b, "                 (((~) \\  /");
                    mvwprintw(game_win, a + 13, b, "               ______/ /");
                    mvwprintw(game_win, a + 14, b, "              '------'");
                }
                else{
                    mvwprintw(game_win, a + 3, b, "           _  _");
                    mvwprintw(game_win, a + 4, b, "           |\\/|__    ___");
                    mvwprintw(game_win, a + 5, b, "    ____   (oo)\\ \\_//   \\ ___");
                    mvwprintw(game_win, a + 6, b, "   //  /\\_//_/\\_\\/ /  |\\_|  \\");
                    mvwprintw(game_win, a + 7, b, "  // /\\/ @@/  |=\\  \\  | \\_|  \\_______ ");
                    mvwprintw(game_win, a + 8, b, " // | \\_/  /   \\_=\\_ \\ |  (/\\___   /|\\");
                    mvwprintw(game_win, a + 9, b, "//|\\______/ \\) \\==\\ \\|\\_  ()=========<>");
                    mvwprintw(game_win, a + 10, b, "_<>======()     _(\\===\\(  )\\");
                    mvwprintw(game_win, a + 11, b, "              (((~) __(_/   |");
                    mvwprintw(game_win, a + 12, b, "                  (((~) \\  /");
                    mvwprintw(game_win, a + 13, b, "                  ______/ /");
                    mvwprintw(game_win, a + 14, b, "                  '------'");
                }
            }
            wattroff(game_win, COLOR_PAIR(14));
            wattroff(game_win, COLOR_PAIR(17));
            wattroff(game_win, COLOR_PAIR(6));
        }
        time_t time_to_quote;
        time(&time_to_quote);
        if(difftime(time_to_quote, quote_time) >= 3){
            guid(game_win);
        }
        if(difftime(time_to_quote, quote_time2) >= 10){
            time(&quote_time2);
            mvwprintw(game_win, map_height - 10 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 9 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 8 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 7 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 6 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 5 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 4 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 3 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 2 - 1, map_width - 54 - 23, "                                                                        ");
            mvwprintw(game_win, map_height - 1 - 1, map_width - 54 - 23, "                                                                        ");
            wattron(game_win, COLOR_PAIR(color_pair_num));
            if(color_pair_num == 0){
                wattron(game_win, COLOR_PAIR(14));
            }
            mvwprintw(game_win, map_height - 8, map_width - 22, "█████████████████");
            mvwprintw(game_win, map_height - 7, map_width - 22, "█████████████████");
            mvwprintw(game_win, map_height - 6, map_width - 22, "█████████████████");
            mvwprintw(game_win, map_height - 5, map_width - 22, "███  ████  ██████");
            mvwprintw(game_win, map_height - 4, map_width - 22, "█████    ████████");
            mvwprintw(game_win, map_height - 3, map_width - 22, "█████████████████");
            wattroff(game_win, COLOR_PAIR(color_pair_num));
            wattroff(game_win, COLOR_PAIR(14));
        }
        if(difftime(time_to_quote, quote_time) >= 15){
            time(&quote_time);
            quoting(game_win);
        }
    }
}
