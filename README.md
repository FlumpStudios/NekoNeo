# Neko Neo

## What is it?
Neko Neo is a 3D level editor used to create HAD files.

## What are HAD files?
HAD files are super small files used to hold level data in as small a package as possible.  
This does limit the flexibility of the levels somewhat (all doors are set on the same level, etc.), but each level comes in at under 4k uncompressed and will compress down to under 500 bytes.

## What can I make levels for?
I created the HAD file format and the Neko Neo editor to make levels for my mini FPS, *Ruyn*.  
Currently, this is the only game available with HAD support, but I plan on making more.

## What was Neko Neo written in?
Neko Neo was written in C using Raylib.

## Why no Interactive UI?
Neko Neo was designed to allow me to create levels as quickly as possible, and I’ve found forcing myself to use keyboard shortcuts makes me a lot more productive. So yeah, no interactive UI—the editor relies 100% on keyboard and console commands.

---

## USER GUIDE  

### Navigation
Use WASD and the mouse to move around. Press `E` to fly up and `Q` to fly down.

### Placing walls and items
To place a wall block, left-click the mouse. To place an item, right-click the mouse. To delete a wall or item, hover over it and press `Delete`.

### Cycling wall textures
You can cycle through the different wall textures by highlighting a block and either using the mouse scroll wheel or pressing `[` or `]` to scroll through the available textures.  
**Note:** The available textures can be changed using other tools such as the 2D editor.

### Cycling through items
This works in the same way as the above, but when highlighting an item rather than a wall.

### Walls and steps
To help keep the super compact file size, the blocks work in a slightly different way from most editors.  
Neko works on the concept of “walls” and “steps.” You set a wall size, which is a single height for all the main walls (each level has a single height), and then you can cycle through 7 different step sizes. To scroll through the steps and walls, highlight the block you want to change and use `,` or `.`. You can set the size of the walls by holding `Shift` while cycling.  
You can also set the global size of each “step” from the console by using the `setstepsize` command.  
This sounds more complicated than it is; it will make sense when you start using it.

### Place doors
To place a door, highlight a wall and press `T`. This will turn the block into a door.  
To add a lock to the door, press `1` (Red), `2` (Green), or `3` (Blue) while highlighting a door. You will see a lock icon appear above the wall. To remove a key, press `0` on the highlighted block.  
To place a key, cycle through the items until you see the corresponding key card.  
You can set the height of the doors by highlighting a door block and cycling through using `,` and `.`. As mentioned before, to keep the file size as small as possible, this can currently only be done on a global level, so all the doors must have the same height throughout the level. This is one of the more egregious limitations of the engine.

### Player start position
To place the player’s start position, highlight the position where you would like the player to start and press `P`.  
You should then see a 3D “P” appear with a direction arrow. You can set the player’s starting direction by highlighting the start position and pressing either `F5` or `F6` to rotate.

### The console
Some features are only accessible via the console. To open the console, press `Tab`.

### Saving and loading
To save a level, type `save [Your Level Name]` into the console.  
To load a level, type `load [Your Level Name]` into the console.

**IMPORTANT:** You can save the file under any name you like, but the game will only recognize files named in the following format: `Level` + a two-digit number (e.g., `Level01`, `Level10`). After a level ends, the game will look for the next level in sequence. If it cannot find the next level, the game will display the ending sequence.  

**NOTE:** The level uploader will only upload files in this format.  

**IMPORTANT NOTE:** The first level must always be named `Level00`. Subsequent levels should follow the format: `Level00`, `Level01`, `Level02`, etc.

---

## Keys and shortcuts
- `[` - Scroll down through wall textures or items  
- `]` - Scroll up through wall textures or items  
- `,` - Scroll down through steps and wall height (scrolls through global door height if highlighting a door block)  
- `.` - Scroll up through steps and wall height (scrolls through global door height if highlighting a door block)  
- `Shift + ,` - Scroll down through global wall height  
- `Shift + .` - Scroll up through global wall height  
- `T` - Convert wall block to door  
- `1, 2, 3` - Adds a key requirement to a door block  
- `0` - Remove a key requirement  
- `Tab` - Opens console  
- `P` - Place the player starting position  
- `F5` - Rotate the player start position anti-clockwise  
- `F6` - Rotate the player start position clockwise  
- `F` or `Centre Mouse Button` - Focus mode (detaches the cursor from the camera so the camera will no longer follow the mouse)  
- `F11` - Toggle full screen  
- `G` - Gameplay simulator  
- `H` - Cycle through help UI  
- `M` - Overhead “Map” view  
- `R` - Cycle through render modes  

---

## Console commands
- `SAVE [Level name]` - Save your level with a name  
- `LOAD [Level name]` - Load level  
- `FILE` - Check the name of the loaded file  
- `SAVE` - Saves the file against the loaded file name  
- `LEVELPACK` - Check which level pack is currently loaded  
- `SETLEVELPACK [Level pack name]` - Set the active level pack  
- `NUKE` - Clear the level  
- `CLS` - Clear the console  
- `CLOSE` - Close the console  
- `CEILCOL` - Check the current ceiling colour number  
- `FLOORCOL` - Check the current floor colour number  
- `SETCEILCOL [Ceiling colour number]` - Set the ceiling colour number  
- `SETFLOORCOL [Floor colour number]` - Set the floor colour number  
- `BACKGROUND` - Check the current background number  
- `SETBACKGROUND [Background number]` - Set the current background number  
- `STEPSIZE` - Check the current global step size  
- `SETSTEPSIZE [Step size]` - Set the global step size  
- `TEST` - Run the level  
- `PREVIEW` - Preview the level in the game engine  
- `QUIT` - Close the editor  

---

Thanks for taking a look at my little project, if you have any problems or suggestion please fire them over to paul.marrable@flumpstudios.co.uk :) 