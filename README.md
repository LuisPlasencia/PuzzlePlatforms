# PuzzlePlatforms

Unreal Engine 5.3 multiplayer videogame project. <br>
Platform Game with multiplayer features:
- Server and clients session management
- Use of Unreal Engine Online Subsystem with steam compatibility (disabled by default)
- Emphasis on UI so that clients can host sessions and access a list of sessions available
- Server Travel
- WASD to move
- Space to jump

Instructions to run Multiplayer test:

1. Create server (default IP = 127.0.0.0 port 7777) <br>
CMD Command: "(YourEpicGamesFolderLocation)\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe" "(YourPuzzlePlatformsProjectLocation)\PuzzlePlatforms\PuzzlePlatforms.uproject" /Game/ThirdPerson/Maps/ThirdPersonMap?listen -server -log -nosteam <br> <br>

2. Launch the clients in the 127.0.0.1 IP adress which is opened by the server (enter this command as many clients as you wish) <br>
CMD Command: "(YourEpicGamesFolderLocation)\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe" "(YourPuzzlePlatformsProjectLocation)\PuzzlePlatforms\PuzzlePlatforms.uproject" 127.0.0.1:7777 -game -log -nosteam <br> <br>

3. Press Escape key and Quit, this will take you to the MainMenu, where you can host a session with one client and then join with the others <br> <br>


Playable builds: https://alumnosulpgc-my.sharepoint.com/:f:/g/personal/luis_plasencia101_alu_ulpgc_es/Emx-CZLKiBpFncD25eVipSwBTS-aRXZWjWx2DGaN8S4kng?e=NBX9eQ

![Screenshot (5716)2](https://github.com/LuisPlasencia/PuzzlePlatforms/assets/60783486/bda99dec-551d-4fea-a126-7dc363f8d4d2)

![Screenshot (5719)](https://github.com/LuisPlasencia/PuzzlePlatforms/assets/60783486/d0ff4d8c-b823-44d8-b0d4-2d6315bab46f)

![Screenshot (5721)](https://github.com/LuisPlasencia/PuzzlePlatforms/assets/60783486/1cb232a6-8a10-4d11-83fb-fa96c05f2132)

![Screenshot (5722)](https://github.com/LuisPlasencia/PuzzlePlatforms/assets/60783486/03cde53e-4519-4522-bffe-ce40bb8e8131)

![Screenshot (5724)](https://github.com/LuisPlasencia/PuzzlePlatforms/assets/60783486/beb78516-f92f-444e-a70e-620d4090e554)
