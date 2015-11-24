#pragma once

#include "global.h"

#define __CONFIG_H_STRINGIFY2(x) #x
#define __CONFIG_H_STRINGIFY(x) __CONFIG_H_STRINGIFY2(x)
#include __CONFIG_H_STRINGIFY(target/TARGET/config.h)
#include __CONFIG_H_STRINGIFY(target/TARGET/target.h)
