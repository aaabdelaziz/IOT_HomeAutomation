#include "SystemState.h"

SystemState       SharedState::_state{};
SemaphoreHandle_t SharedState::_mutex = nullptr;
