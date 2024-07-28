#include <HX711.h>

// HX711 circuit wiring
extern const int LOADCELL_DOUT_PIN;
extern const int LOADCELL_SCK_PIN;
extern HX711 scale;

// Function declarations
void initScale();
float readWeight();

