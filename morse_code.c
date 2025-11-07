// includes all the files needed for the rest of the code
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#include <math.h>
#include "hardware/pwm.h"
#include "includes/potentiometer.h"
#include <stdlib.h>
#include "includes/buzzer.h"

// defines constants to be used in the code
#define R                   13 
#define G                   12 
#define B                   11 
#define BUZZER_PIN          17
#define BUTTON_PIN          16
#define BUTTON_PIN2         2
#define BRIGHTNESS          25
#define MAX_COLOUR_VALUE    255
#define MAX_PWM_LEVEL       65535
#define E4                  330
#define E5                  659
#define C5                  523
#define G5                  784
#define G4                  392
#define A4                  440
#define B4                  494
#define AS4                 466

// declares global variables 
int lettertime = 0;
int tpressed = 0;
int tnotpressed = 0;
int lettercount = 0;
char Letter[10] = "";
char Word[10] = "";
int numOfAttempts = 0;

// plays the error sound
void playError() {
    buzzer_init();
    buzzer_enable(220);
    sleep_ms(200);
    buzzer_enable(140);
    sleep_ms(300);
    buzzer_disable();
}

// plays the specified frequency
void playNote(unsigned int frequency) {
 buzzer_enable(frequency);
 sleep_ms(150);
 
}

// plays tune if they get 4 letters correct by iterating through array of notes
void playSong() {
    unsigned int song[] = {E5,  E5, E5, C5, E5, G5, G4, C5, G4, E4, A4, B4, AS4, A4};
    for (unsigned int i = 0; i < 14; i++) {
        playNote(song[i]);
        sleep_ms(100);
    }
}

// creates an array of each letter and its morse code equivalent
char grid[26][2][10] = {{".-", "A"}, {"-...", "B"}, {"-.-.", "C"}, {"-..", "D"}, {".", "E"}, {"..-.", "F"}, {"--.", "G"}, {"....", "H"}, {"..", "I"}, {".---", "J"}, {"-.-", "K"},
   {".-..", "L"}, {"--", "M"}, {"-.", "N"}, {"---", "O"}, {".--.", "P"}, {"--.-", "Q"}, {".-.", "R"}, {"...", "S"}, {"-", "T"}, {"..-", "U"}, {"...-", "V"},
   {".--", "W"}, {"-..-", "X"}, {"-.--", "Y"}, {"--..", "Z"}};

// responsible for setting up the three GPIO pins (R, G and B)that are used to control an RGB LED
void setup_rgb() {
    gpio_set_function(R, GPIO_FUNC_PWM);
    gpio_set_function(G, GPIO_FUNC_PWM);
    gpio_set_function(B, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(R);
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);
    slice_num = pwm_gpio_to_slice_num(G);
    pwm_init(slice_num, &config, true);
    slice_num = pwm_gpio_to_slice_num(B);
    pwm_init(slice_num, &config, true);
}

// sets the colour of an RGB LED based on the red, green and blue values
void colour(int red, int green, int blue) {
    pwm_set_gpio_level(R, ~(MAX_PWM_LEVEL * red / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(G, ~(MAX_PWM_LEVEL * green / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(B, ~(MAX_PWM_LEVEL * blue / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
}

// runs the welcome routine
void welcome() {
    // initialise the potentiometer
    potentiometer_init();
    // initialise the seven segment display
    seven_segment_init();
    // turn the seven segment display off when the program starts
    seven_segment_off();
    // displays 8 on the seven-segment display and prints welcome message
    colour(0,0,0);
    seven_segment_show(8);
    printf("Welcome\n");
    sleep_ms(500);
    seven_segment_off();

}

// determines whether the program should reset or be terminated
void tocontinue() {
    printf("%s\n", "Press left button to continue or right button to terminate:\n");

    int set = 0;

    // loops the code until a button is pressed
    while (set == 0) {
        bool right = gpio_get(BUTTON_PIN);
        bool left = gpio_get(BUTTON_PIN2);

    if (right) { // terminates the code when the right button is clicked and displays red light 
        printf("%s\n", "Program terminated.\n");
        colour(250,0,0);
        sleep_ms(500);
        colour(0,0,0);
        exit(1);
        set += 1;
    }
    
    if (left) { // terminates the code when the left button is clicked and displays green light
        printf("%s\n", "Program reset.\n");
        colour(0,250,0);
        sleep_ms(500);
        colour(0,0,0);
        welcome();
        set += 1;
    } 
    }
}

// verifies if the input morse code represented by the Letter string matches any entry in the grid array
void check(char* Letter) {
    int set = 0;
    int rows = sizeof(grid)/sizeof(grid[0]);
    // iterates through the morse code grid array
    for(int i = 0; i < rows; i++) { 
        if(strcmp(Letter, grid[i][0]) == 0) { // if an input matches
            printf("%s\n", grid[i][1]); // prints the letter to console
            strcat(Word, grid[i][1]); // adds the letter to the string of correct letters
            lettercount += 1; // adds to tally of correct letters
            colour(0,250,0); // displays green light
            sleep_ms(500);
            colour(0,0,0);
            lettertime = 0;
            seven_segment_show(i+10); // displays letter on seven segment display
            sleep_ms(500);
            seven_segment_off();
            set = 1;
        }
    }
    // if the inputted morse code doesn't represent a letter then it outputs an error message
    if (set == 0) {
        printf("Error - not a letter.\n");
        playError();
        colour(250,0,0);
        sleep_ms(500);
        colour(0,0,0);
        seven_segment_show(8);
        sleep_ms(500);
        seven_segment_off();
        lettertime = 0;
    }
}

// void playDot() {
//     buzzer_init();
//     buzzer_enable(392);
//     sleep_ms(200);
//     buzzer_disable();
// }

// void playDash() {
//     buzzer_init();
//     buzzer_enable(392);
//     sleep_ms(500);
//     buzzer_disable();
// }


int main() {
    // initialises various components of the code
    stdio_init_all();
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);
    gpio_init(BUTTON_PIN2);
    gpio_set_dir(BUTTON_PIN2, GPIO_IN);
    gpio_pull_down(BUTTON_PIN2);
    setup_rgb();
    timer_hw->dbgpause = 0;

    // calls the welcome subroutine
    welcome();
  
    // creates a variable to be used later in determining the new time limit.
    int value1 = 2;

    while (true) {

        lettertime++; // increments lettertime by 1 every loop
        sleep_ms(50); // waits 50 milliseconds to ensure that the timings are correct

        unsigned int value = potentiometer_read(2); // reads the current value from the potentiometer
        sleep_ms(50);

        // while the current value is not the same as the new value it will create a new time limit
        while (value1 != value) {
            value1 = value;
            printf("New Time Limit: %d\n", value1 + 2);
            lettertime = 0;
        }

        // if the time to enter a letter exceeds the set value, an error is displayed
        if (lettertime > ((value1 + 2 ) * 20)){
            printf("Error - time out.\n");
            playError();
            colour(250,0,0);
            sleep_ms(500);
            colour(0,0,0);
            seven_segment_show(8);
            sleep_ms(500);
            seven_segment_off();
            lettertime = 0;
        }

        // checks if 4 correct letters have been entered
        if (lettercount == 4) {
            printf("%s\n", Word);
            timer_hw->dbgpause = 0;
            buzzer_init();
            playSong();
            buzzer_disable();
            Word[0] = '\0';  // Reset the word string for the next character
            lettercount = 0;
            tocontinue();
        }

        bool pressed = gpio_get(BUTTON_PIN); // gets value of the button
        tnotpressed++;

        if (pressed) { // checks if the button is pressed and records the input length
            buzzer_init();
            buzzer_enable(392);
            tpressed++;
            sleep_ms(50);
            tnotpressed = 0;

        // records whether the input was a dot or dash depending on the duration of the button press
        // concatenates the input to the Letter string
        } else if (!pressed && tnotpressed < 8) { 
            buzzer_disable();
            if (tpressed > 0 && tpressed < 5) {
                strcat(Letter, ".");
                // playDot();
                timer_hw->dbgpause = 0;
            } else if (5 <= tpressed && tpressed < 14) {
                strcat(Letter, "-");
                // playDash();
                timer_hw->dbgpause = 0;
            // if the input is longer than 700ms then an error is displayed
            } else if (tpressed >= 14) {
                colour(250,0,0);
                printf("Error Input - input too long.\n");
                playError();
                colour(0,0,0);
                seven_segment_show(8);
                sleep_ms(500);
                seven_segment_off();
                lettertime = 0;
            }
            // resets the time the button is pressed for
            tpressed = 0;
            sleep_ms(50);

        // if the button is not pressed for 400ms it will recognise that the letter input is complete
       } else if (tnotpressed > 8){
            if (strlen(Letter) > 0) {
                check(Letter);  // checks the morse code for a match
                Letter[0] = '\0';  // reset the letter string for the next character
                colour(0,0,0);
            }
       }
  }
}