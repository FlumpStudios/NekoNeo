
# Neko Neo

### What is it?

Neko Neo is a 3D level editor used to create HAD files.

### What are HAD files?
HAD files are super small files used to hold level data in as small a package as possible.
This does limit the flexibility of the levels somewhat (All doors are set on the same level etc...), but each level comes in at under 4k uncompressed and will compress down to under 500 bytes.

### What can I make levels for?
I created the HAD file format and the Neko Neo editor to make levels for my mini FPS, Ruyn.
Currently this is the only game available with HAD support, but I plan on making more.

#### What was Neko Neo written in?
Neko Neo was written in C using Raylib.

#### Why no Interactive UI?
Neko Neo was deisgned to allow me to create levels as quickly as possible, and I've found forcing myself to use keyboard shortcuts makes me a lot more productive. So yeah, no interactive UI and the editor relies 100% on keyboard and console commands.

## USER GUIDE
#### Navigation
Use WASD and mouse to move around. E to fly up and Q to fly down.

#### Placing walls and items
To place a wall block left click the mouse and to place an item right click the mouse. To delete a wall or item just hover over it and press delete

#### Cycling wall textures
You can cycle through the different wall textures by highlighting a block and either using the mouse scroll wheel or pressing either , or . to scroll through the available textures. 
Note: The available textures can be changed using other tools such as the 2D editor.

#### Cycling through item
This works in the same way as the above, but when highlighting an item rather than a wall.




