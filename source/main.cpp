#include "main.hpp"
#include <limits>

void dpadToJoystick(nn::hid::NpadHandheldState *state) {
    if(state->Buttons & nn::hid::KEY_DLEFT) {
        state->Buttons &= ~nn::hid::KEY_DLEFT;
        state->LStickX = std::numeric_limits<s32>::min();
    }
    if(state->Buttons & nn::hid::KEY_DRIGHT) {
        state->Buttons &= ~nn::hid::KEY_DRIGHT;
        state->LStickX = std::numeric_limits<s32>::max();
    }
    if(state->Buttons & nn::hid::KEY_DDOWN) {
        state->Buttons &= ~nn::hid::KEY_DDOWN;
        state->LStickY = std::numeric_limits<s32>::min();
    }
    if(state->Buttons & nn::hid::KEY_DUP) {
        state->Buttons &= ~nn::hid::KEY_DUP;
        state->LStickY = std::numeric_limits<s32>::max();
    }
}

void (*originalNpadStateHandheldFunc)(nn::hid::NpadHandheldState *state, u32 const &controllerID);
void (*originalNpadStateJoyFunc)(nn::hid::NpadJoyDualState *state, u32 const &controllerID);
void (*originalNpadStateFullKeyFunc)(nn::hid::NpadFullKeyState *state, u32 const &controllerID);

void GetNpadHandheldStateHook(nn::hid::NpadHandheldState *state, u32 const &controllerID) {
    originalNpadStateHandheldFunc(state, controllerID);
    dpadToJoystick(state);
}
void GetNpadJoyStateHook(nn::hid::NpadJoyDualState *state, u32 const &controllerID) {
    originalNpadStateJoyFunc(state, controllerID);
    dpadToJoystick(state);
}
void GetNpadFullKeyStateHook(nn::hid::NpadFullKeyState *state, u32 const &controllerID) {
    originalNpadStateFullKeyFunc(state, controllerID);
    dpadToJoystick(state);
}

int main() {
    u64 GetNpadStateAddr;
    nn::ro::LookupSymbol(&GetNpadStateAddr, "_ZN2nn3hid12GetNpadStateEPNS0_17NpadHandheldStateERKj");
    A64HookFunction(
        reinterpret_cast<void*>(GetNpadStateAddr),
        reinterpret_cast<void*>(GetNpadHandheldStateHook),
        (void**) &originalNpadStateHandheldFunc
    );
    nn::ro::LookupSymbol(&GetNpadStateAddr, "_ZN2nn3hid12GetNpadStateEPNS0_16NpadJoyDualStateERKj");
    A64HookFunction(
        reinterpret_cast<void*>(GetNpadStateAddr),
        reinterpret_cast<void*>(GetNpadJoyStateHook),
        (void**) &originalNpadStateJoyFunc
    );
    nn::ro::LookupSymbol(&GetNpadStateAddr, "_ZN2nn3hid12GetNpadStateEPNS0_16NpadFullKeyStateERKj");
    A64HookFunction(
        reinterpret_cast<void*>(GetNpadStateAddr),
        reinterpret_cast<void*>(GetNpadFullKeyStateHook),
        (void**) &originalNpadStateFullKeyFunc
    );
}
