# exo-controller
This is a release of the super loop that operates on the exo-arm I designed. Feel free to use your own VESC for similar applications -- I will attach a photo of my setup eventually.

![Screenshot 2024-01-30 162930](https://github.com/BlueCheeto/exo-controller/assets/36427628/7e7d8d79-1094-4448-afe8-0b09e3ace5d3)

The loop runs on an Arduino Uno clone utilizing SolidGeek's VescUart library (https://github.com/SolidGeek/VescUart), which allows the VESC and Uno to maintain UART communication. The Uno also has serial communication with an HM10 Bluetooth BLE module, allowing input to be sent from a user's phone. Inputs are handled by the switch statement, allowing ROM, duration, and intensity to be adjusted for physical therapy customization. This device is lacking an encoder, and thus is purely in the experimental stage.
