# S1-RX8-AC-Display-controller
Arduino based A\C-Display control for S1 RX8. Please read the [wiki](https://github.com/TonyChatfield/S1-RX8-AC-Display-controller/wiki).

(Note that the wiki is still in the original repository, as it is still relevant and not included in forks)


This Fork is a rewrite using PlatformIO, that also aims to fix bugs and expand functionality, as the original Project seems to be abandoned. The main code is in `rx8_display`, with the optional ESP8266 Companion (used to store persistent data and as a Bluetooth/ESP-Now bridge) as a submodule in `S1-RX8-AC-Display-ESP8266-Companion`. (The main repository for this is [here](https://github.com/NES-FM/S1-RX8-AC-Display-ESP8266-Companion))