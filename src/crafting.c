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

    crafting_recipes[1] = crafting_recipeDefault(BLOCK_TYPE_CRAFTING_TABLE, 1);

    crafting_recipes[1].inputTypes[0] = BLOCK_TYPE_PLANK;
    crafting_recipes[1].inputTypes[1] = BLOCK_TYPE_PLANK;
    crafting_recipes[1].inputTypes[3] = BLOCK_TYPE_PLANK;
    crafting_recipes[1].inputTypes[4] = BLOCK_TYPE_PLANK;

    crafting_recipes[2] = crafting_recipeDefault(ITEM_TYPE_STICK, 4);

    crafting_recipes[2].inputTypes[1] = BLOCK_TYPE_PLANK;
    crafting_recipes[2].inputTypes[4] = BLOCK_TYPE_PLANK;

    crafting_recipes[3] = crafting_recipeDefault(ITEM_TYPE_WOODEN_AXE, 16);

    crafting_recipes[3].inputTypes[0] = BLOCK_TYPE_PLANK;
    crafting_recipes[3].inputTypes[1] = BLOCK_TYPE_PLANK;
    crafting_recipes[3].inputTypes[3] = BLOCK_TYPE_PLANK;
    crafting_recipes[3].inputTypes[4] = ITEM_TYPE_STICK;
    crafting_recipes[3].inputTypes[7] = ITEM_TYPE_STICK;
}