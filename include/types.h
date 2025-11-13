#pragma once

enum class MotorState
{
    STOPPED,
    OPENING,
    CLOSING
};

enum class Trigger
{
    NONE,
    WEB,
    MANUAL,
    AUTO_TEMP,
    AUTO_WIND
};