This is an experimental repo of several hardware we've used with arc em9d development board.  
Feel free to check following dir in your favor.    

### DFPlayer:
* Contains needed lib and example of how to use dfplayer mini with arc em9d development board.  
* Using arduino TX port to communiacte with board.  

### Joystick:
* Contains needed lib and example of how to use Joystick with arc em9d development board.  
* The board itself has only one ADC, thus you can read both up and down in y-dir, but only one direction in x-dir.
* Using gpio to communiacte with board.  

### OLED:
* Contains needed lib and example of how to use OLED with arc em9d development board.  
* Using I2c master port from board to communiacte with board.

### combine:
* Combines DFPlayer, Joystick, OLED with AI text detection application.   
