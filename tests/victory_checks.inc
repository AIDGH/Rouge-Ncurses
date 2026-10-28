static void check_victory_rewards(void) {
    Player before=*game->player;
    int old_theme=ui_old_visuals,old_difficulty=difficulty_efficiency;
    mkdir("users",0700);
    const char *weapon_names[]={"Mace","Dagger","MagicWand","NormalArrow","Sword"};
    const char *spell_names[]={"Speed","Power","Healing"};
    for(int i=0;i<5;i++)snprintf(game->player->weapons[i].name,15,"%s",weapon_names[i]);
    for(int i=0;i<3;i++)snprintf(game->player->spells[i].name,10,"%s",spell_names[i]);
    for(int theme=0;theme<2;theme++)for(int choice=0;choice<3;choice++){
        Player *p=game->player;
        ui_old_visuals=theme;difficulty_efficiency=2;
        p->name="qa-victory";p->points=2000;p->gold=100;p->games_count=3;
        victory_results_reset();run_finished=escape_check=0;
        /* Select, claim, try selecting another reward, then exit the final summary. */
        ungetch(10);ungetch('1'+(choice+1)%3);ungetch(10);ungetch('1'+choice);
        win_game_screen(time(NULL)-65);
        long long expected_points=2000+(choice==1?24000:choice==2?12000:0);
        int expected_gold=100+(choice==0?750:choice==2?350:0);
        assert(p->points==expected_points&&p->gold==expected_gold&&p->games_count==4);
        assert(p->play_time>=65&&p->play_time<=66&&run_finished&&escape_check&&victory_results_ready());
        User record={0};assert(load_user_info("qa-victory",&record));
        assert(record.points==expected_points&&record.golds==expected_gold&&record.games_count==4&&record.have_game==0);
        assert(access("users/qa-victory.txt.tmp",F_OK)!=0);
        win_game_screen(time(NULL)); /* No second reward or game-count increment. */
        assert(p->points==expected_points&&p->gold==expected_gold&&p->games_count==4);
    }
    victory_results_reset();game->player->points=7000;game->player->gold=90;
    ungetch(27);win_game_screen(time(NULL));
    assert(game->player->points==7000&&game->player->gold==90);
    User record={0};assert(load_user_info("qa-victory",&record)&&record.points==7000&&record.golds==90);
    *game->player=before;ui_old_visuals=old_theme;difficulty_efficiency=old_difficulty;
    victory_results_reset();run_finished=escape_check=0;
    fprintf(stderr,"PASS all three victory rewards in both themes, explicit claim, no duplicate reward/re-entry, frozen summary time, profile reload and skip bonus\n");
}
