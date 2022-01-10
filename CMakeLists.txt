# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(Sinecraft)

include(GenerateG1A)
include(GenerateG3A)
include(Fxconv)
find_package(Gint 2.1 REQUIRED)

set(SOURCES
    src/flags.h
    src/lut.h
    src/lut.c
    src/common.h
    src/common.c
    src/serial.h
    src/serial.c
    src/coords.h
    src/coords.c
    src/keys.h
    src/keys.c
    src/ui.h
    src/ui.c
    src/config.h
    src/config.c
    src/textures.h
    src/textures.c
    src/world.h
    src/world.c
    src/items.h
    src/items.c
    src/camera.h
    src/camera.c
    src/physics.h
    src/physics.c
    src/crafting.h
    src/crafting.c
    src/inventory.h
    src/inventory.c
    src/profiling.h
    src/profiling.c
    src/main.c
    # ...
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
    # ...
)
set(ASSETS_fx
    assets-fx/logo.png
    assets-fx/crosshair.png
    assets-fx/fnplay.png
    assets-fx/fnedit.png
    assets-fx/fnnew.png
    assets-fx/fndelete.png
    assets-fx/fnsearch.png
    assets-fx/fnpage.png
    assets-fx/slot.png
    assets-fx/slotselected.png
    assets-fx/slotsource.png
    assets-fx/slotcross.png
    assets-fx/craftarrow.png
    assets-fx/stone.png
    assets-fx/grass.png
    assets-fx/dirt.png
    assets-fx/cobblestone.png
    assets-fx/plank.png
    assets-fx/wood.png
    assets-fx/leaves.png
    assets-fx/craftingtable.png
    assets-fx/woodenaxe.png
    assets-fx/stick.png
    # ...
)
set(ASSETS_cg
    # assets-cg/logo.png
    # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(sinecraft ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(sinecraft PRIVATE -Wall -Wextra -Os)
target_link_libraries(sinecraft Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
    generate_g1a(
        TARGET sinecraft OUTPUT "Sincft.g1a"
        NAME "Sincft" ICON assets-fx/icon.png
    )
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
    generate_g3a(
        TARGET sinecraft OUTPUT "Sincft.g3a"
        NAME "Sincft" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png
    )
endif()
