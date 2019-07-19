![alt-text][logo]

[logo]: https://github.com/smanizad/original_synth/raw/master/images/os.jpg "O R I G I N A L  S Y N T H"
# original_synth
This describes how to build and program an Arduino-Uno-style, polyphonic, pentatonic synthesizer.
Original Synth (OS) is designed to be cheap, customizable, and marginally entertaining.

## Getting Started

### Materials
#### Hardware
This project is designed for an Arduino kit plus a few accessories.

[Arduino Kit](https://www.amazon.com/ELEGOO-Project-Starter-Tutorial-Arduino/dp/B01D8KOZF4/ref=sr_1_3?keywords=elegoo+uno&qid=1563512117&s=gateway&sr=8-3) est. $35

[4.7nF Capacitor](https://www.galco.com/buy/NTE-Electronics/90310?source=googleshopping&utm_source=adwords&utm_campaign=&gclid=CjwKCAjwscDpBRBnEiwAnQ0HQP8t1jy9nY558hPhLuSPOiITvfzZlcQIXhWJYI10von2RAwYybtaGBoCgN8QAvD_BwE) est. $0.40

[Headphone Jack](https://www.adafruit.com/product/1699?gclid=CjwKCAjwscDpBRBnEiwAnQ0HQINPtg1ovczWfjwz1BZe1U7T3z_T7ydyzV1FaG56_17qFj11vlpcexoCIF8QAvD_BwE) est. $0.95


[Extra Button (x3)](https://www.adafruit.com/product/367?gclid=CjwKCAjwscDpBRBnEiwAnQ0HQFElpXXEmNPYQjYdngeUivREq31oZZNDCo4f8serohdGnUnrt-cUrBoChywQAvD_BwE)  est. $2.50 for a 20-pack

Headphones: Wherever you can find a pair, or use a set of computer speakers.  (worst case: 50 cents / pair of headphones)

**Estimated Total Cost:** $39.35


#### Software

**1) Setting up basic Arduino**

You will need the Arduino IDE (integrated development environment).
Get Arduino set up and running on your computer. Follow the instructions [here](https://www.arduino.cc/en/Guide/HomePage).

Open the IDE. Try some of the examples from the File>Examples tab first to confirm that your setup is working.

**2) Setting up the Synthesizer Library (mozzi)**

[Mozzi](https://sensorium.github.io/Mozzi/) is a fantastic synthesizer library that Original Synth relies on heavily.
In order to speed up the setup process, the Mozzi library as of 19 July 2019 is included in this repository. In order to use it, copy the *mozzi* directory and all of its contents located in this repository's *arduino_libraries* to your Arduino 'libraries' directory (the location of which depends on your installation process - look for the directory where you installed the Arduion IDE).

Once this step is completed, you should be able to view example Mozzi sketches using the File>Examples>Mozzi dropdown tabs.

The final test: open *osynth.ino* and attempt to compile it (click the checkmark in the IDE) - if it compiles without error (i.e. there's not a flood of orange text) then you've got Mozzi installed!
