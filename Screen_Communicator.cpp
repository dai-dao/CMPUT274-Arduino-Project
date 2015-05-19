#include <Arduino.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include <lcd_image.h>    // U of A Utils 

// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

//Potentiometer settings
#define DIAL_PIN 2  //Input pin for Dial

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Sd2Card card;

// Initializing emoticons
lcd_image_t image1 = {"catcupid.lcd",16, 16};
lcd_image_t image2 = {"dogboxer.lcd",16, 16};
lcd_image_t image3 = {"dogrich.lcd",16, 16};
lcd_image_t image4 = {"money.lcd",16, 16};
lcd_image_t image5 = {"smiley.lcd",16, 16};
lcd_image_t image6 = {"suprised.lcd",16, 16};

// Define image structure
typedef struct {
    lcd_image_t img;
    int horizontal;
    int vertical;
    char code;
} image;


image emoticons[6] = {{image1,4,137,1},{image2,24,137,2},
		      {image3,44,137,3},{image4,64,137,4},{image5,84,137,5}
		      ,{image6,104,137,6}};

//Emoticon Selection Box Size
int rect_size = 18;

//Tracks Emoticon Selection Box Location
int rect_horiz,rect_vertic; 
int cur_pos_horiz = 3;
int cur_pos_vertic = 137;

//Joystick Calibration Variables
int hor_cal, vert_cal;

//Joystick Pins
const int VERT = 0;
const int HOR = 1;
const int SEL = 9;

//Cursor Position Tracking
int cur_pos_x = 1;
int cur_pos_y = 1;

//Allows each character to have its own color attached for 
typedef struct{
    char character;
    uint8_t color_code;
}color_char;

//Colors to be used for the variable text coloring
uint16_t color[7] = {ST7735_YELLOW, ST7735_BLUE, ST7735_RED, ST7735_GREEN, ST7735_CYAN, ST7735_MAGENTA, ST7735_WHITE};

//Tracks current text color
uint16_t curr_color = ST7735_WHITE;

//Returns a one-byte code corresponding to the current text color
uint8_t set_color_code(){
    uint8_t code = 0;
    for(int y = 0; y < 7; y++){
        if (color[code] == curr_color){
            return code;
        }

        code++;
    }
}

//2 Line buffer for messages, plus an extra for endline character if needed
color_char buffer[21];

//Save character if more than 2 lines
char overline = 0;

//tracks the current character between loops
int b = 0;

//Add the currently selected emoticon to the message if the joystick button is
//pressed
void select(int sel, image * emoticons){
    if (sel == LOW){
        //Does not add emoticon if there is not enough space in message
        if(b > 18){
            return;
        }
        else{
            for (int i = 0; i < 6; i++){
                if(emoticons[i].horizontal == cur_pos_horiz+1){
                    lcd_image_draw(&(emoticons[i].img), &tft, 0,0,cur_pos_x,
                                   cur_pos_y, 16,16);
                    //Character is an unused character (128-134)
                    buffer[b].character = (emoticons[i].code)+127;
                    b++;
                    //Adds a space, as emoticons are larger than one character
                    buffer[b].character = ' ';
                    b++;
                    //Prints spaces to move the message cursor
                    tft.print("  ");
                    cur_pos_x = 1 + (b%10)*12;
                    cur_pos_y = 1 + (b/10)*16;
                    delay(200);
                }
            }
        }
    }
}

//Moves the emoticon selector and removes the previous if needed
void selection(int x, int y){
    int r = rect_size;
    if(rect_horiz != x && rect_horiz != 0){
        tft.drawRect(rect_horiz, rect_vertic, r, r, ST7735_BLACK);
        tft.drawRect(x,y,r,r,0x07E0);
    }
}

//Reads characters from the serial port and adds them to the message buffer, 
//Returns true if message is to be sent
int read_save(){
    //if there is a saved character after an overline situation, add it & print
    if (overline != 0){
        buffer[b].character = overline;
        buffer[b].color_code = set_color_code();
        tft.print(overline);
        overline = 0;
        b++;
    }
    //Checks for activity on the Serial Port
    if(Serial.available()){
        //Save the character read from the serial port, along with a color code,
        //to the message buffer
        char x = Serial.read();
        buffer[b].character = x;
        buffer[b].color_code = set_color_code();

        //Runs if the entered key is Backspace
        if (x == '\b'){
            //Does nothing if the message buffer is empty
            if (b <= 0){
                return 0;
            }
            //
            else{
                //Moves current character one back
                b--;
                cur_pos_x = 1 + (b%10)*12;
                cur_pos_y = 1 + (b/10)*16;
                tft.setCursor(cur_pos_x, cur_pos_y);
                //Fills a black rectangle over the previous character
                tft.fillRect(cur_pos_x, cur_pos_y, 12, 16, ST7735_BLACK);
                return 0;
            }
        }
     
        //If the enter key is pressed or the message buffer is full
        if (x == '\r' || b == 20){
            //If the message buffer is full, saves the last character
            if(b == 20){
                overline = x;
            }
            //Sets the character to be an endline character
            buffer[b].character = '\0';
            buffer[b].color_code = set_color_code();
            //If the message buffer is not full, fill with null characters
            while(b < 20){
                buffer[b].character = '\0';
                b++;
            }
            //Resets message tracker and cursor
            b = 0;
            cur_pos_x = 1 + (b%10)*12;
            cur_pos_y = 1 + (b/10)*16;
            //Returns true
            return 1;
        }

        //Sets cursor to the next line if required
        if ((b%10) == 0 && b != 0){
            tft.setCursor(1, 1+((b/10)*16));
        }
        //Print out the character with the correct color
        tft.setTextColor(curr_color);
        tft.print(buffer[b].character);

        //Adjust character tracker and cursor
        b++;
	cur_pos_x = 1 + (b%10)*12;
	cur_pos_y = 1 + (b/10)*16;
        return 0;
    }
}

//Buffer for Message Reception
color_char saved[21];
int j = 0;

//Receives a character and color code from the connected buffer
//Returns true if received message is finished
int recieve_save(){
    if (Serial3.available()){
        char x = Serial3.read();
        saved[j].character = x;
        delay(50);
        uint8_t c = Serial3.read();
        saved[j].color_code = c;

        //If the received character is the endline character
        if (x == '\0'){
            //clear serial buffer of the endline characters
            while(Serial3.read() == '\0'){
                char l = Serial3.read();
                saved[j].character = '\0';
                j++;
            }
            j = 0;
            return 1;
        }
        j++;
    }
    return 0;
}

//Buffer for message board
color_char board[61];

//Buffer for previous message board
color_char oldboard[61];

//Adds a message to the message board, and prints the board to the screen
void print_message(color_char * message){
    //Clears the board area of the screen and sets the cursor to the board area
    tft.fillRect(1, 35, 126, 96, ST7735_BLACK);
    tft.setCursor(1, 35);

    int v = 0;
    //Adds the message to the board while there is not an endline character
    while(message[v].character != '\0'){ 
       board[v] = message[v]; 
       v++;  
    }

    //Fills the rest of the line with spaces if required, so each message starts
    //on a new line of the board
    while ((v%10) != 0){
        board[v].character = ' ';
        board[v].color_code = 7;
        v++;
    }

    int w = 0;

    //Fills the rest of the board with the characters from the last board
    while (v < 60){
        board[v] = oldboard[w];
        v++;
        w++;
    }
    //Adds a black null character to the end of the message
    board[60].character = '\0';
    board[60].color_code = 7;

    //Prints the board out to the board area of the screen
    int a = 0;
    while(board[a].character != '\0'){

        //Checks if the character is an emoticon character
        uint8_t conver_char = (uint8_t)board[a].character;
        if (conver_char > 127 && conver_char < 134){
            for(uint8_t w = 0; w < 6; w++){
                //Draw the correct emoticon to the board
                uint8_t boardcode = conver_char - 127;
                if ((uint8_t)emoticons[w].code == boardcode){
                    cur_pos_x = 1 + (a%10)*12;
                    cur_pos_y = 35 + (a/10)*16;
                    lcd_image_draw(&(emoticons[w].img), &tft, 0,0,cur_pos_x,
                                   cur_pos_y, 16,16);
                    //adjusts text cursor
                    tft.print(' ');
                    a++;
                    break;
                }
            }
        }
        //Prints the character to the board with the corresponding color
        tft.setTextColor(color[board[a].color_code]);
        tft.print(board[a].character);
        a++;

        //Sets the cursor to the next line if required
        if((a%10) == 0){
            tft.setCursor(1, (35+((a/10)*16)));
        }
    }

    //Saves the board in the oldboard buffer
    for (int k = 0; k < 60; k++){
        oldboard[k] = board[k];
    }

    //Resets the cursor to the message
    cur_pos_x = 1 + (b%10)*12;
    cur_pos_y = 1 + (b/10)*16;
    tft.setCursor(cur_pos_x, cur_pos_y);
}

//Changes the currect color according to the Potentiometer reading
void change_color(){
    int c = analogRead(DIAL_PIN);
    if(c >=0 && c <150){
        curr_color = ST7735_YELLOW;
    }
    if(c >=150 && c <300){
        curr_color = ST7735_BLUE;
    }
    if(c >=300 && c <450){
        curr_color = ST7735_RED;
    }
    if(c >=450 && c<600){
        curr_color = ST7735_GREEN;
    }
    if(c >=600 && c <750){
        curr_color = ST7735_CYAN;
    }
    if(c >= 750 && c <900){
        curr_color = ST7735_MAGENTA;
    }
    if(c >=900){
        curr_color = ST7735_WHITE;
    }
}

void setup(){

    //Initialize Serial Ports
    Serial.begin(9600);
    Serial3.begin(9600);
   
    // Initialize joystick
    pinMode(SEL, INPUT);
    digitalWrite(SEL,HIGH);

    //Reads resting Joystick Values for Calibration
    vert_cal = analogRead(VERT);
    hor_cal = analogRead(HOR);	

    // If your TFT's plastic wrap has a Red Tab, use the following:
    //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
    // If your TFT's plastic wrap has a Green Tab, use the following:
    //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab
    // If your TFT's plastic wrap has a Black Tab, use the following:
    tft.initR(INITR_BLACKTAB); // initialize a ST7735R chip, black tab

    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("failed!");
        while (1) {};  // Just wait, stuff exploded.
    }
    Serial.println("OK!");

    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
        Serial.println("Raw SD Initialization has failed");
        while (1) {};  // Just wait, stuff exploded.
    }

    // clear screen to black
    tft.fillScreen(0);

    //Draw boxes around the distinct screen areas
    tft.drawRect(0, 0, 128, 34, ST7735_BLUE);
    tft.drawRect(0,34,128,98, ST7735_RED);
    tft.drawRect(0,132, 128,28, ST7735_GREEN);

    //Draws the emoticons to the emoticon area of the screen
    for (int i =0; i<6; i++){
        lcd_image_draw(&(emoticons[i].img), &tft, 0,0,emoticons[i].horizontal,
                       emoticons[i].vertical, 16,16);
    }

    //Initializes Text options
    tft.setTextSize(2);
    tft.setCursor(1, 1);
    tft.setTextWrap(false);

    //Draws a box around the first emoticon
    int r = rect_size;
    tft.drawRect(cur_pos_horiz, cur_pos_vertic,r,r,ST7735_GREEN);

    //Notifies User that Setup is complete
    Serial.println("Ready for User Input");
}

void loop(){
    int hor, ver, sel;
    hor = analogRead(HOR) - hor_cal; // 0-1023
    ver = analogRead(VERT) - vert_cal; // 0-1023
    sel = digitalRead(SEL); // HIGH if not pressed, LOW otherwise
  
    // To keep track of the previous emoticon selector location
    rect_horiz = cur_pos_horiz;
    rect_vertic = cur_pos_vertic;
 
    // Setting conditions to execute the movement of selection
    if ( hor > 8 && cur_pos_horiz <= 84) {
        cur_pos_horiz += 20;
        delay(150);
    }
    else if (hor < -8 && cur_pos_horiz >= 20) {
        cur_pos_horiz -= 20;
        delay(150);
    }

    selection(cur_pos_horiz, cur_pos_vertic);
  
    //Prints the received message to the board if message is completed
    if(recieve_save()){
        print_message(saved);
    }

    change_color();

    select(sel, emoticons);

    //Prints the message to the board and sends it if message is completed
    if (read_save()){
        //Clears the message area of the screen
        tft.fillRect(1, 1, 126, 32, ST7735_BLACK);
        //Add the message to the board
        print_message(buffer);
        //Send message over the Serial3 Port
        for (int h = 0; h <= 20; h++){
            Serial3.print(buffer[h].character);
            Serial3.write(buffer[h].color_code);
        }
    }
}
