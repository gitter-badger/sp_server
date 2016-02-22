Want to force a game or application into windowed mode that
doesn't support it through its own interface? Now you can,
to an extent. Results will vary depending on how your DirectX
application was written.

Instructions:

1) Launch dxwnd.exe
2) Right click the application's window pane
3) Select "Add"
4) Use the "..." button to browse for your fullscreened executable
5) Select the DirectX version of your executable if it's known, if
   not, leave it set to "Automatic"
6) If your executable runs in 256 (8-bit) colored mode, check
   the "Emulate 256 Color Palette" check box
7) Click "OK"
8) Now that you have an entry for your executable, simply double click it
   or run the game/application itself. What's neat is as long as dwwnd is
   running, it will intercept the launching of the application itself, so
   it's not necessary to use dxwnd itself to start your game/application

Good luck.

-----------------

Retro ISO Gaming Guild - http://rigg.servegame.com

Credits:

NightWolve
 
  Recompiled with VC++ 7.0 and translated menus with amikai.

Deuce 

  Special thanks to him for finding this application from
  some Japanese site.

Download Space provided by AppAholic.co.uk - A Freeware review website