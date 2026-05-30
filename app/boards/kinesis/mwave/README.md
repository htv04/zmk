# Kinesis mWave

This board definition provides upstream support for the [Kinesis mWave](https://kinesis-ergo.com/keyboards/mwave/).

Kinesis offer a specific [custom configuration](https://github.com/KinesisCorporation/MWave-ZMK/) for the mWave that references [a customised version of ZMK](https://github.com/ReFil/zmk/tree/mwave) and a [companion Zephyr module](https://github.com/htv04/MWave-zephyr-module) which together implement mWave-specific functionality and changes over base ZMK. The Kinesis fork is regularly updated to bring the latest updates and changes from base ZMK however will not always be completely up to date.

When using this board definition some of the more advanced features (status indicator LEDs and the advertising-mode behavior) will not work, and Kinesis cannot provide customer service for usage of base ZMK. Likewise the ZMK community cannot provide support for either the Kinesis keymap editor, nor any usage of the Kinesis custom fork.
