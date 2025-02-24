# Rouge-Ncurses
Rouge game with C, using ncurses library with terminal graphic!                           

For running the game, you should be using Linux or WSL on Windows and you need to install ncurses and sdl2.                           
# Install Commands:                           
Ncurses install command: sudo apt-get install libncurses5-dev libncursesw5-dev         
SDL2 install command: sudo apt-get install libsdl2-dev         
SDL2_Mixer install command: sudo apt-get install libsdl2-mixer-dev            

And all of the running commands are written in Makefile so all you have to do is just write this command:                           
make && ./exec                           

Or you can run the run.bat file to compile and run all of the files with this command:                                                      
./run.bat                           

So a window will be opened for you to play and enjoy the game. It also have a tutorial which will be written by Timmy (Cute creature on down and right side of the screen) while you play, I hope you enjoy it :D                           

Don't delete or move any files in "Base_Sounds" and also the folder.                           
You can add your musics in "music" folder and don't change name or move the folder.                           

users.txt file is where you users main informations go in this format:                           
{                           
Username: Sample                           
Email: Sample@sample.sample                           
Password: Sample1234                           
}                           

So don't change or write anything it that.                           
And also "users" folder is where each user game information stays on in this format:                           
{                           
health: 100                           
points: 0 (The formula is: (play time) * difficulty * 5 + golds * 10)                           
games count: 0                           
play time: 0 (By seconds)                           
golds: 0                           
food: 100                           
weapons: 1 Mace1, Dagger0, MagicWand0, NormalArrow0, Sword0                           
spells: 0 Speed0, Power0, Healing0                           
ancientkey: 0                           
broken ancientkey: 0                           
level: 0                           
game: 0 (0 -> new game | 1 -> have a previouse game)                           
(If there is a game it will be saved here)                           
}                           

So don't change or write anything in that.                           