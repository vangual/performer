#include "LaunchpadProDevice.h"

//         +---+---+---+---+---+---+---+---+
//         | 91| 92| 93| 94| 95| 96| 97| 98| < CC messages
//         +---+---+---+---+---+---+---+---+
//
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 80|  | 81|...|   |   |   |   |   | 88|  | 89|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 70|  | 71|...|   |   |   |   |   | 78|  | 79|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 60|  | 61|...|   |   |   |   |   | 68|  | 69|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 50|  | 51|...|   |   |   |   |   | 58|  | 59|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 40|  | 41|...|   |   |   |   |   | 48|  | 49|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 30|  | 31|...|   |   |   |   |   | 38|  | 39|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 20|  | 21|...|   |   |   |   |   | 28|  | 29|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 10|  | 11|...|   |   |   |   |   | 18|  | 19|
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//
//         +---+---+---+---+---+---+---+---+
//         |  1|  2|  3|  4|  5|  6|  7|  8| < CC messages
//         +---+---+---+---+---+---+---+---+


LaunchpadProDevice::LaunchpadProDevice() :
    LaunchpadDevice()
{
}

void LaunchpadProDevice::recvMidi(const MidiMessage &message) {
    if (message.isNoteOn()) {
        int index = message.note();
        int row = 7 - (index - 11) / 10;
        int col = (index - 11) % 10;
        if (row >= 0 && row < Rows && col >= 0 && col < Cols) {
            setButtonState(row, col, message.velocity() != 0);
        } else if (row >= 0 && row < Rows && col == Cols) {
            setButtonState(SceneRow, row, message.velocity() != 0);
        }
    } else if (message.isControlChange()) {
        int index = message.controlNumber();
        if (index >= 91 && index <= 98) {
            setButtonState(FunctionRow, index - 91, message.controlValue() != 0);
        }
    }
}

void LaunchpadProDevice::syncLeds() {
    // grid
    for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Cols; ++col) {
            int index = row * Cols + col;
            if (_deviceLedState[index] != _ledState[index]) {
                if (sendMidi(MidiMessage::makeNoteOn(0, 11 + 10 * (7 - row) + col, _ledState[index]))) {
                    _deviceLedState[index] = _ledState[index];
                }
            }
        }
    }

    // scene
    for (int col = 0; col < Cols; ++col) {
        int index = SceneRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            if (sendMidi(MidiMessage::makeNoteOn(0, 11 + 10 * (7 - col) + 8, _ledState[index]))) {
                _deviceLedState[index] = _ledState[index];
            }
        }
    }

    // function
    for (int col = 0; col < Cols; ++col) {
        int index = FunctionRow * Cols + col;
        if (_deviceLedState[index] != _ledState[index]) {
            if (sendMidi(MidiMessage::makeControlChange(0, 91 + col, _ledState[index]))) {
                _deviceLedState[index] = _ledState[index];
            }
        }
    }
}
