
#include <Arduino.h>
#include "Adafruit_NeoKey_1x4.h"
#include "seesaw_neopixel.h"
#include "Adafruit_TinyUSB.h"

Adafruit_NeoKey_1x4 neokey;  

#if defined(LED_BUILTIN)
#endif

#if defined(INTERNAL_DS_DATA)
  #include <Adafruit_DotStar.h>
  Adafruit_DotStar builtin_dotstar(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);
#endif

// Report ID
enum
{
  RID_ZERO = 0,
  RID_KEYBOARD,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
};

// HID report descriptor using TinyUSB's template
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(RID_CONSUMER_CONTROL) )
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

// // Output report callback for LED indicator such as Caplocks
// void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
// {
//   (void) report_id;
//   (void) bufsize;

//   // LED indicator is output report with only 1 byte length
//   if ( report_type != HID_REPORT_TYPE_OUTPUT ) return;

//   // The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
//   // Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
//   uint8_t ledIndicator = buffer[0];

//   // turn on LED if capslock is set
//   // digitalWrite(LED_BUILTIN, ledIndicator & KEYBOARD_LED_CAPSLOCK);

// #ifdef PIN_NEOPIXEL
//   pixels.fill(ledIndicator & KEYBOARD_LED_CAPSLOCK ? 0xff0000 : 0x000000);
//   pixels.show();
// #endif
// }

void setup(void) 
{  
#if defined(LED_BUILTIN)
  digitalWrite(LED_BUILTIN, HIGH);
#endif

#if defined(INTERNAL_DS_DATA)
  builtin_dotstar.setPixelColor(0, 0x0000FF);
  builtin_dotstar.show();
#endif

  // This allows us to use the "1200 baud open/close" trick to put the device into firmware upload mode.
  Serial.begin(9600);
  // delay(3000);

  // Serial.println("Starting");

  // while (! Serial) delay(1);

  // TinyUSB Setup
  USBDevice.setProductDescriptor("TinyKeyboard");
  // USBDevice.setSerialDescriptor("TinyKeyboard");
  // Serial.setStringDescriptor("TK");
  usb_hid.setStringDescriptor("TinyKeyboard");
  usb_hid.begin();

  // Set up output report (on control endpoint) for Capslock indicator
  // usb_hid.setReportCallback(NULL, hid_report_callback);

  // wait until device mounted
  // while( !USBDevice.mounted() ) delay(1);

  if (!neokey.begin(0x30)) {     // begin with I2C address, default is 0x30
    // Serial.println("Could not start NeoKey, check wiring?");
  } else {
    // Serial.println("NeoKey started!");
  }

  unsigned long start_time = millis();
  do {
    // Pulse all the LEDs on to show we're working
    for (uint16_t i=0; i<neokey.pixels.numPixels(); i++) {
      neokey.pixels.setPixelColor(i, 0x808080); // make each LED white
      neokey.pixels.show();
      delay(50);
    }

    for (uint16_t i=0; i<neokey.pixels.numPixels(); i++) {
      neokey.pixels.setPixelColor(i, 0x000000);
      neokey.pixels.show();
      delay(50);
    }

    if (millis() - start_time > 1000 * 5) {
      // We've waited over 5 seconds for USB to init, we're probably only plugged into power.
      // Continue so we can at least demo the blinkenlights.
      break;
    }
  } while(!USBDevice.mounted());

  for (int32_t c=0xff; c>= 0; c--) {
    uint32_t color = c << (USBDevice.mounted()?8:16);
    for (uint16_t i=0; i<neokey.pixels.numPixels(); i++) {
      neokey.pixels.setPixelColor(i, color); // All LEDs fade from the same color
    }
    neokey.pixels.show();
#if defined(INTERNAL_DS_DATA)
    builtin_dotstar.setPixelColor(0, color);
    builtin_dotstar.show();
#endif
    delay(1);
  }

#if defined(LED_BUILTIN)
  digitalWrite(LED_BUILTIN, LOW);
#endif
}

// button bits convenience
#define button(num) (1 << num)

void loop()
{
  const int loop_hz = 30;
  const int loop_target_time = 1000000 / loop_hz;
  unsigned long loop_start_time = micros();
  static uint8_t last_buttons = 0;
  uint8_t buttons = neokey.read();
  uint8_t buttons_changed = last_buttons ^ buttons;

  // Wake up on any button press
  if ((buttons_changed != 0) && TinyUSBDevice.suspended()) 
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  // Check each button, if pressed, light the matching neopixel
  if (buttons_changed & button(0))
  {
    if (buttons & button(0)) {
      // Serial.println("Button A");
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_PLAY_PAUSE);
      neokey.pixels.setPixelColor(0, 0xFF0000); // red
    } else {
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      neokey.pixels.setPixelColor(0, 0);
    }
  }

  if (buttons_changed & button(1))
  {
    if (buttons & button(1)) {
      // Serial.println("Button B");
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_SCAN_PREVIOUS);
      neokey.pixels.setPixelColor(1, 0x00FF00); // green
    } else {
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      neokey.pixels.setPixelColor(1, 0);
    }
  }

  if (buttons_changed & button(2))
  {
    if (buttons & button(2)) {
      // Serial.println("Button C");
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_SCAN_NEXT);
      neokey.pixels.setPixelColor(2, 0x0000FF); // blue
    } else {
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      neokey.pixels.setPixelColor(2, 0);
    }
  }

  if (buttons_changed & button(3))
  {
    if (buttons & button(3)) {
      // Serial.println("Button D");
      uint8_t keycode[6] = { 0 };
      keycode[0] = HID_KEY_F15;
      usb_hid.keyboardReport(RID_KEYBOARD, 0, keycode);
      neokey.pixels.setPixelColor(3, 0xFFFFFF); // white
    } else {
      usb_hid.keyboardRelease(RID_KEYBOARD);
      neokey.pixels.setPixelColor(3, 0);
    }  
  }

  last_buttons = buttons;

  neokey.pixels.show();

  unsigned long loop_time = micros() - loop_start_time;
  if (loop_time < loop_target_time)
  {
    delayMicroseconds(loop_target_time - loop_time);
  }
}
