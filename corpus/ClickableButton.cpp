#include "ClickableButton.h"
#include <iostream>
using namespace std;

//#define DEBUG_GUARDIAN_CLICKS

//These assume were running 50 fps
const int ClickableButton::SINGLE_CLICK_ACTIVE_MIN = 5;
const int ClickableButton::SINGLE_CLICK_ACTIVE_MAX = 50;
const int ClickableButton::SINGLE_CLICK_INACTIVE_MIN = 4;
const int ClickableButton::SINGLE_CLICK_INACTIVE_MAX = 18;
const float ClickableButton::PUSHED = 1.0f;
const int ClickableButton::SHUTDOWN_THRESH = 3*50; //Three seconds

const int ClickableButton::NO_CLICKS = -1;


ClickableButton::ClickableButton(int _fps)
    : fps(_fps),
      buttonOnCounter(0), buttonOffCounter(0),
      lastButtonOnCounter(0),lastButtonOffCounter(0),
      buttonClicks(0), numClicks(NO_CLICKS),
      registeredClickThisTime(false)
{
    pthread_mutex_init(&button_mutex, NULL);
}

ClickableButton::~ClickableButton()
{
    pthread_mutex_destroy(&button_mutex);
}

int ClickableButton::getAndClearNumClicks() const{
    pthread_mutex_lock(&button_mutex);
    const int tempClicks = numClicks;
    numClicks = NO_CLICKS;
    pthread_mutex_unlock(&button_mutex);
    return tempClicks;
}

void ClickableButton::setNumClicks(int _numClicks){
    pthread_mutex_lock(&button_mutex);
    numClicks = _numClicks;
    pthread_mutex_unlock(&button_mutex);
}

void ClickableButton::updateFrame(float buttonValue){
    bool push_done =false;
    bool unpush_done = false;

    if(buttonValue == PUSHED){

        buttonOnCounter += 1;
        if(buttonOffCounter > 0){
            lastButtonOffCounter = buttonOffCounter;
            buttonOffCounter  = 0;
        }
        registeredClickThisTime = false;
    }else{
        if(buttonOnCounter >0){
            lastButtonOnCounter = buttonOnCounter;
            buttonOnCounter = 0;
        }
        buttonOffCounter  +=1;
    }

    //detect if a click occured sometime in the past
    if( lastButtonOnCounter >= SINGLE_CLICK_ACTIVE_MIN &&
        lastButtonOnCounter <= SINGLE_CLICK_ACTIVE_MAX &&
        buttonOffCounter >= SINGLE_CLICK_INACTIVE_MIN &&
        buttonOffCounter <= SINGLE_CLICK_INACTIVE_MAX &&
        !registeredClickThisTime){
        buttonClicks += 1;
#ifdef DEBUG_GUARDIAN_CLICKS
        cout << "Registered a click, waiting to see if there are more"<<endl;
#endif
        registeredClickThisTime = true;
   }

    if( buttonOffCounter > SINGLE_CLICK_INACTIVE_MAX &&
         buttonClicks > 0){
#ifdef DEBUG_GUARDIAN_CLICKS
        cout << "Processing " <<buttonClicks <<" clicks"<<endl;
#endif
        setNumClicks(buttonClicks);
        buttonClicks = 0;
    }

}
