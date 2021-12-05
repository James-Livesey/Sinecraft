#include "crafting.h"
#include "world.h"

CraftingRecipe crafting_recipes[RECIPES_COUNT];

CraftingRecipe crafting_recipeDefault(unsigned int outputType, unsigned int outputCount) {
    CraftingRecipe recipe = {.outputType = outputType, .outputCount = outputCount};

    for (unsigned int i = 0; i < 9; i++) {
        recipe.inputTypes[i] = BLOCK_TYPE_AIR;
    }

    return recipe;
}

void crafting_init() {
    crafting_recipes[0] = crafting_recipeDefault(BLOCK_TYPE_PLANK, 4);

    crafting_recipes[0].inputTypes[4] = BLOCK_TYPE_WOOD;
}