#include <mbed.h>
#include <MMA7455.h>
#include <LM75B.h>
#include <display.h>

//Declare output object for LED1
DigitalOut led1(LED1);

// Initialise Joystick   
typedef enum {JLT, JRT, JUP, JDN, JCR} btnId_t;
static DigitalIn jsBtns[] = {P5_0, P5_4, P5_2, P5_1, P5_3}; // LFT, RGHT, UP, DWN, CTR
bool jsPrsdAndRlsd(btnId_t b);

//Input object for the potentiometer
AnalogIn pot(p15);
float potVal = 0.0;
  
//Object to manage the accelerometer
MMA7455 acc(P0_27, P0_28);
bool accInit(MMA7455& acc); //prototype of init routine
int32_t accVal[3];

//Object to manage temperature sensor
LM75B lm75b(P0_27, P0_28, LM75B::ADDRESS_1);
float tempVal = 0.0;
Display *screen = Display::theDisplay();
    //This is how you call a static method of class Display
    //Returns a pointer to an object that manages the display screen 

//Timer interrupt and handler
void timerHandler(); //prototype of handler function
int tickCt = 0;
int interval1=5+rand()%4;
int interval2=2+rand()%9;


//Drawing coordinates

//STRUCT FOR BALL
typedef struct ball1{
	int x,y;
	int dx,dy;
	int lives;
	int points;
	int scorerate;
	int total;
	int colour;
}ball;
ball ball1;


//STRUCT FOR THE OBSTACLE
typedef struct bar{
	int x,y;
	int size;
}bar1;
bar1 bar;

//STRUCT FOR THE BAT 
typedef struct batmovement{
	int x,y;
	int dx,dy;
}bat;
bat batmovement;

//ALL OF DIFFERENT METHOD CALLS
  int waitforstart();
	void intialiseDevices();
	void initialiseGames();
  void renderBall(ball *ball);
  void updateBall(ball *ball,bar1);
  void renderBat(bat);
  void adjustBat(bat);
  void initialiseBatBall(ball *ball,bat *bat,bar1);
  void updateScore(ball);
  int testbottom(ball,bar1);
  void delay();
  void magicTime(ball);
  int hitBat(ball,bat);
  int hitBar(ball,bar1);
  void renderBar(bar1);
   int waitForStart();
   void changeBar(bar1);
   void stopbat(bat);

//MAIN LOOP WHERE SOMEO FTHE MAINH METHODS ARE CALLED
int main() {
	intialiseDevices();
	initialiseGames();
	int ballinPlay=1;
	Ticker ticktock;
	 ticktock.attach(&timerHandler, 1);
	initialiseBatBall(&ball1,&batmovement,bar);
	while(ballinPlay==1){	
		renderBall(&ball1);
		updateBall(&ball1,bar);
	  renderBat(batmovement);
	  adjustBat(batmovement);
		magicTime(ball1);
		delay();
	}
}


//THIS WILL INTIALISE SOME OF THE DEVICES
	void intialiseDevices(){
		jsPrsdAndRlsd(JCR);
		jsPrsdAndRlsd(JLT);
		jsPrsdAndRlsd(JRT);
		jsPrsdAndRlsd(JUP);
		jsPrsdAndRlsd(JDN);
  // Initialise accelerometer and temperature sensor
  accInit(acc);
  lm75b.open();
}
	

	//THIS WILL INTIALISE THE GAME FROM BEING ON A START SCREEN TO  BEING ON A THE GAME WITH CLICK OF CENTRE
	void initialiseGames(){
		bool started=false;
		if(started==false){
		   screen->fillScreen(WHITE);
       screen->setTextColor(BLACK,WHITE);
		   screen->setCursor(2,100);
			 screen->setTextSize(3);
	     screen->printf("Pong Assingment");
		   screen->setCursor(2,200);
		 	screen->setTextSize(2);
		   screen->printf("by Adam Baker");
		 }
		 while(started==false){
			 if(jsPrsdAndRlsd(JCR)){
			  	started=true;
				  screen->fillScreen(WHITE);
          screen->setTextColor(BLACK,WHITE);
			 }
		 }
		 if(started==true){
		  screen->setCursor(2,2);
			screen->setTextSize(2);
	    screen->printf("No of live:5");
		  screen->setCursor(350,2);
		  screen->printf("Total:0");
	    screen->fillRect(0,20,480,2,BLACK);
		 }
   }
	
/*THIS WIL DETECT THE MAGIUC TIME AND CHNAGE THE COLOUR OF THE BALL
 *if blue it will until the tick count is between 2 and 10 if red it rate until the tci kcount is between 2 and 10
   */
  void magicTime(ball){
		 screen->setCursor(200,2);
	   screen->printf("tick:%d",tickCt);
	   if(tickCt==interval1&&ball1.colour==BLUE){
		    ball1.colour=RED;
		    tickCt=0;
		    interval1=5+rand()%4;
	    }
	    else if(tickCt==interval2&&ball1.colour==RED){
	     	ball1.colour=BLUE;
	      tickCt=0;
		    interval2=2+rand()%9;
	   }
}

//THEASE WILL INTIALISE ALL OF THE BAT, BALL, AND OBSTACLE FEATURES
void initialiseBatBall(ball *ball,bat *bat,bar1){
	ball1.x=28+rand()%465;
  ball1.y=28+rand()%100;
	ball1.dx=1;
	ball1.dy=1;
	ball1.lives=5;
	ball1.points=1;
	ball1.scorerate=0;
	batmovement.x=rand()%440;
	batmovement.y=260;
	batmovement.dx=1;
	batmovement.dy=0;
	ball1.colour=BLUE;
	bar.x=29+rand()%150;
	bar.y=29+rand()%100;
	bar.size=40+rand()%159;
	screen->fillRect(bar.x,bar.y,bar.size,2,GREEN);
}
//RENDER THE WHITE BALL
void renderBall(ball *ball){
     screen->fillCircle(ball1.x,ball1.y,5,WHITE);
}

/*UPDATE THE BALL BY DX AND DY AND DETECTS COLLISIONS
 *this will update the ball in a similar way as the bat with dy and dx
 * if ball is about to collide it will make dx - which will make it go the other way.
 */

void updateBall(ball *ball,bar1){
	  ball1.x+=ball1.dx;
	  ball1.y+=ball1.dy;
	  screen->fillCircle(ball1.x,ball1.y,5,ball1.colour);
	  screen->fillRect(bar.x,bar.y,bar.size,2,GREEN);
		if(ball1.x<=4){
		   ball1.dx=-ball1.dx;
		  }
			else if(ball1.x>=476){
		   ball1.dx=-ball1.dx;
	    }
		  else if(ball1.y<=27){
				updateScore(ball1);
		  }
			else if (ball1.y>=270){
				testbottom(ball1,bar);
	  	}
			
			else if(hitBat(ball1,batmovement)==1)
		  {
		  	ball1.dy=-ball1.dy;
		  }
			else if(hitBar(ball1,bar)==1){
				ball1.dy=-ball1.dy;
			}
}

//DETECTS IF IT HAS BEEN HIT WITH THE BAT
int hitBat(ball,bat){
	if(ball1.y>batmovement.y-5&&ball1.y<270&&ball1.x>batmovement.x-5&&ball1.x<batmovement.x+46){
		return 1;
	}
	else{
		return 0;
	}
}

//DETECT HIT WITH THE BAR
int hitBar(ball,bar1){
	if(ball1.y>bar.y-5&&ball1.y<bar.y+7&&ball1.x>bar.x-5&&ball1.x<bar.x+bar.size){
		return 1;
	}
	else{
		return 0;
	}
}

//DELAYS THE BALL B
void delay(){
	wait(0.01);
}

/*TEST THE BOTTOM  IF YOU GOES PAST IT LOSES A LIFE AND EVERYTHIG IS RESET WHEN NEEDED
 *if the ball is out of play and live is greater than zero it will take a live each time the ball has went down
 *it will also reset the possition of the ball and bat
 *the live will be decucted by pne and then wrote to the screen*/

int testbottom(ball,bar1){
	bool stopped=false;
	if(ball1.y>=270){
	  if(ball1.lives >0){
	    if(jsPrsdAndRlsd(JCR)){
		    stopped=true;
	    }
	    if(stopped==true){
		    ball1.x=5+rand()%465;
        ball1.y=28+rand()%225;
				bar.x=29+rand()%150;
	      bar.y=29+rand()%100;
	      bar.size=40+rand()%159;
			  ball1.lives--;
			  screen->setCursor(2,2);
			  screen->setTextSize(2);
	      screen->printf("No of live:%d",ball1.lives);
		    ball1.scorerate=0;
		    ball1.points=1;
       }
     }
	return 0;
  }
	else{
	return 1;
	}
}

/*UPDATE SCORE WILL GET THE SCORE FORthe ball each time it hits the top
 * it detects the collision to the top wall and amount of times it has isa recorded
 *if the ball is blue it will add 1 to the total until five h9its has occured
 *if it is red it will adddouble the scoring rate
 *when five hits has been doone points is added by 1 and the scoreate is set to o again
 *this will all printed tro the screen*/


void updateScore(ball){
	ball1.dy=-ball1.dy;
		ball1.scorerate++;
			if(ball1.colour==BLUE){
		ball1.total=ball1.total+ball1.points;
			}
			if(ball1.colour==RED){
					ball1.total=ball1.total+(ball1.points*2);
			}
	if(ball1.scorerate==5){
		ball1.points=ball1.points+1;
		ball1.scorerate=0;
	}
  screen->setCursor(350,2);
	screen->setTextSize(2);
	screen->printf("Total:%d",ball1.total);
}

//RENDER A BAT
void renderBat(bat){
screen->fillRect(batmovement.x,batmovement.y,40,4,WHITE);
}

/* defination of the adjust bat function
 * @param bat(call for bat)
 *it then read the accel rometer and gets all there values
 * it add the dx and dy to the x and y respedcitivluy every time it goes through while loop
 * then it creates a recttangle of appriopriate colour
 *if accel romter is - or joystick is pressed right it goes right other wise if it is left and accel is + IT GOEWS TO THE LEFT */
void adjustBat(bat){
	acc.read(accVal[0], accVal[1], accVal[2]);
	batmovement.x+=batmovement.dx;
	 batmovement.y+=batmovement.dy;	
screen->fillRect(batmovement.x,batmovement.y,40,4,BLACK);
if(testbottom(ball1,bar)==0){
	batmovement.dx=0,batmovement.dy=0;
}
else if(testbottom(ball1,bar)==1){
if(0< batmovement.x&& batmovement.x<440){
    if (jsPrsdAndRlsd(JLT)||accVal[0]>=0015) { // NB response to JS L, R reversed
      batmovement.dx = 1; batmovement.dy= 0;                 //   because JS is upside-down
    } else if (jsPrsdAndRlsd(JRT)||accVal[0]<=-0015) {
     batmovement.dx = -1;   batmovement.dy= 0;
    }
   }else if(batmovement.x<=0){
			batmovement.dx=0, batmovement.dy=0;
			if(jsPrsdAndRlsd(JLT)||accVal[0]>=0015){
				batmovement.dx=1, batmovement.dy=0;
			}
			if(jsPrsdAndRlsd(JRT)||accVal[0]<=-0015){
				batmovement.dx=0, batmovement.dy=0;
			}
		}else if( batmovement.x>=440){
			batmovement.dx=0, batmovement.dy=0;
			if(jsPrsdAndRlsd(JRT)||accVal[0]<=-0015){
			batmovement.dx	=-1, batmovement.dy=0;
			}
			if(jsPrsdAndRlsd(JLT)||accVal[0]>=0015){
				batmovement.dx=0, batmovement.dy=0;
			}
		}
	}
}

//ACCELROMETER INTIALIALISED
bool accInit(MMA7455& acc) {
  bool result = true;
  if (!acc.setMode(MMA7455::ModeMeasurement)) {
    // screen->printf("Unable to set mode for MMA7455!\n");
    result = false;
  }
  if (!acc.calibrate()) {
    // screen->printf("Failed to calibrate MMA7455!\n");
    result = false;
  }
  // screen->printf("MMA7455 initialised\n");
  return result;
}

//Definition of timer interrupt handler ifthe ball is in play the ttick ct will go up by 1 if not it won't go up
void timerHandler() {
	if(ball1.y<265){
  tickCt++;
	}
	if(ball1.y>=270){
		tickCt=tickCt;
  }
}
/* Definition of Joystick press capture function
 * b is one of JLEFT, JRIGHT, JUP, JDOWN - from enum, 0, 1, 2, 3
 * Returns true if this Joystick pressed then released, false otherwise.
 *
 * If the value of the button's pin is 0 then the button is being pressed,
 * just remember this in savedState.
 * If the value of the button's pin is 1 then the button is released, so
 * if the savedState of the button is 0, then the result is true, otherwise
 * the result is false. */
bool jsPrsdAndRlsd(btnId_t b) {
	bool result = false;
	uint32_t state;
	static uint32_t savedState[4] = {1,1,1,1};
        //initially all 1s: nothing pressed
	state = jsBtns[b].read();
  if ((savedState[b] == 0) && (state == 1)) {
		result = true;
	}
	savedState[b] = state;
	return result;
}
