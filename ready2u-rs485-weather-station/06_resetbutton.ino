#include <ezButton.h>

ezButton button(2);
int btnState;

void resetbuttonLoop() {
}
int CountPress = 0;
void resetbuttonSetup() {

  button.setDebounceTime(50);

  for (int i = 1; i <= 6; i++) {
    button.loop();  
    if (button.getState() == 1) CountPress++;
    delay(1000);
  }

  if (CountPress == 6) {
    SETMODE = 2;  // RESET
  } else if (CountPress > 0) {
    SETMODE = 1;  // SET
  }
}
bool resetbuttonispress() {
  if (btnState == 1)  // SET
  {
    return true;
  } else  // RUN MODE
  {
    return false;
  }
}
