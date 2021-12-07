#ifndef CRAFTING_H_
#define CRAFTING_H_

#define RECIPES_COUNT 1

typedef struct {
    unsigned int inputTypes[9];
    unsigned int outputType;
    unsigned int outputCount;
} CraftingRecipe;

extern CraftingRecipe crafting_recipes[RECIPES_COUNT];

CraftingRecipe crafting_recipeDefault(unsigned int outputType, unsigned int outputCount);
void crafting_init();

#endif