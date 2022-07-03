#include "GogginRenderer.h"

trippin::GogginRenderer::GogginRenderer(Goggin &goggin) : goggin(goggin) {
}

void trippin::GogginRenderer::afterTick(int) {
    goggin.render();
}

bool trippin::GogginRenderer::isExpired() {
    return goggin.isExpired();
}
