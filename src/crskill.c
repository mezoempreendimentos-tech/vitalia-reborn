#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "crskill.h" // Nosso novo header

// Definição do array global de perícias de crafting.
struct craft_skill_data craft_skills[MAX_CRAFT_SKILLS + 1];

// Esta função preenche o array com as perícias existentes.
void assign_craft_skills(void)
{
    int i;

    // Primeiro, limpamos todo o array para evitar lixo de memória.
    for (i = 0; i <= MAX_CRAFT_SKILLS; i++) {
        craft_skills[i].name = "NOME_NAO_USADO";
    }

    /* Agora, definimos nossas perícias. O número dentro dos colchetes é o ID. */
    craft_skills[1].name = "ferraria"; //
    // Exemplo:
    craft_skills[1].name = "ferraria";
    craft_skills[2].name = "cutelaria";
    craft_skills[3].name = "armeiro";
    // Adicione novas perícias aqui, até o limite de MAX_CRAFT_SKILLS.

}
