#include "Engine.h"

#include "core/Debug.h"

#include "core/midi/MIDIMessage.h"

Engine::Engine(ClockTimer &clockTimer, ADC &adc, DAC &dac, GateOutput &gateOutput, MIDI &midi) :
    _adc(adc),
    _dac(dac),
    _gateOutput(gateOutput),
    _midi(midi),
    _clock(clockTimer)
{}

void Engine::init() {
    _clock.init();

    // forward MIDI clock events to clock
    _midi.setRecvFilter([] (uint8_t data) {
        if (MIDIMessage::isClockMessage(data)) {
            // _clock.handleMidiMessage()
            return true;
        } else {
            return false;
        }
    });
}

void Engine::update() {
    if (_clock.checkStart()) {
        DBG("START");
        _running = true;
    }

    if (_clock.checkStop()) {
        DBG("STOP");
        _running = false;
    }

    if (_clock.checkResume()) {
        DBG("RESUME");
        _running = true;
    }

    uint32_t tick;
    while (_clock.checkTick(&tick)) {
        _tick = tick;

        for (size_t i = 0; i < _tracks.size(); ++i) {
            auto &track = _tracks[i];
            track.tick(tick);
            _gateOutput.setGate(i, track.gate());
            
        }
    }

}

void Engine::start() {
    for (auto &track : _tracks) {
        track.reset();
    }

    _clock.start();
}

void Engine::stop() {
    _clock.stop();
}

void Engine::resume() {
    _clock.resume();
}