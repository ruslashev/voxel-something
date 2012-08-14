voxel-something
===============
stop laughing  
  
  
This project is nothing more than just experiment about "Can I do this?". Turns out I sort of can.

##Storing models:
There's own model format, because constantly re-inventing the wheel is the new hip.  
example model:  
```
size [1 2 2]

vox [0 0 0 0 255 0]
vox [0 0 1 0 0 255]
vox [0 1 0 255 0 0]
vox [0 1 1 255 255 0]
```
As you can see, everything is piss simple, parser follows these rules:
* Search **only** for keywords `size` and `vox`
    * After occurence of any of them, search for square brackets (`[`).
	* Prepare to read the data, spaces are used as delimiters (it really makes it easier to parse)
	* If the keyword is `size`, read it's data as follows: `size [`_width_ _height_ _depth_<code>]</code> After that, prepare the model with these specs, assuming every single voxel is empty
	* If the keyword is `vox`: `vox [`_x_ _y_ _z_ _col_<code>]</code>. After fetching data, make the voxel at _x_ _y_ _z_ not empty and color it with base 16 _col_.
  
Notes:
* It does not accept anything else, so you can use it for comments, author signatures or other stuff
* The space placement is not strict, so `vox_[0_0_0_255_255_255]` is the same as `vox____[_0_________0___0__255________255_____255_______]` Imagine yourself that underscores are spaces. Markdown just doesn't let me add as many spaces as I want :(

##Model editor
The model editor is in `\level editor` (I don't know why I called it _level_ editor) directory and is made in [LOVE](love2d.org) game engine. Currently in unusable, broken, messed-up, half-assed state. But hey, it's enough to make a kick-ass skeleton model! _With messed up orientation..._  
Instructions:
* WASD, OL to move
* K to toggle emptiness of currently selected block
* Space to draw selected tile with current color
* Enter to export
  
Exported models are stored in `%AppData%\LOVE\voxel something level editor` on Windows, `$XDG_DATA_HOME/love/` or `~/.local/share/love/` on Linux and `/Users/user/Library/Application Support/LOVE/` on Mac