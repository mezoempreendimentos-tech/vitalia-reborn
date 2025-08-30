#include "structs.h"
#include "utils.h"
#include "quality.h"
#include "conf.h"     // A configuração principal, geralmente inclui headers do sistema.
#include "sysdep.h"   // Dependências do sistema, essencial para tipos como ssize_t.
#include <stdio.h>    // Para a função snprintf() e o tipo FILE.
#include <string.h>   // Para as funções strdup() e strstr().
#include <stdlib.h>   // Para a função rand_number().
#include "structs.h"  // Estruturas de dados principais do MUD.
#include "utils.h"    // Utilitários gerais do MUD.

// Definição da tabela de qualidades com os valores que você especificou.
const struct quality_info quality_data[NUM_QUALITIES] = {
    // name,             multiplier, chance (de 1000)
    {"[COMUM]",        1.0f,  400}, // 40%
    {"[INCOMUM]",      1.2f,  250}, // 25%
    {"[RARO]",         1.4f,  200}, // 20%
    {"[EPICO]",        1.6f,  100}, // 10%
    {"[LENDARIO]",     1.8f,  40},  // 4%
    {"[OBRA PRIMA]",   1.9f,  10},  // 1%
    {"[DIVINO]",       2.0f,  0}    // 0%, só por Admins
};

// Função que aplica a qualidade a um objeto.
void set_object_quality(struct obj_data *obj, enum item_quality quality) {
    if (!obj) return;

    // Aplica qualidade apenas a armas e armaduras para evitar itens estranhos como "[LENDÁRIO] Pão"
    if (GET_OBJ_TYPE(obj) != ITEM_WEAPON && GET_OBJ_TYPE(obj) != ITEM_ARMOR) {
        return;
    }
    
    // Evita aplicar qualidade múltiplas vezes no mesmo item
    for (int i=0; i < NUM_QUALITIES; i++) {
        if (strstr(obj->short_description, quality_data[i].name)) {
            return;
        }
    }

    // 1. Adiciona a qualidade ao nome do item
    char new_name[MAX_STRING_LENGTH];
    snprintf(new_name, sizeof(new_name), "%s %s", quality_data[quality].name, obj->short_description);
    if (obj->short_description) free(obj->short_description);
    obj->short_description = strdup(new_name);

    char new_long_desc[MAX_STRING_LENGTH];
    snprintf(new_long_desc, sizeof(new_long_desc), "%s %s", quality_data[quality].name, obj->description);
    if (obj->description) free(obj->description);
    obj->description = strdup(new_long_desc);


    // 2. Aplica os multiplicadores de status
    float multiplier = quality_data[quality].multiplier;
    if (multiplier <= 1.0f) return; // Não faz nada para itens comuns

    for (int i = 0; i < MAX_OBJ_AFFECT; i++) {
        if (obj->affected[i].location != APPLY_NONE) {
            obj->affected[i].modifier = (int)(obj->affected[i].modifier * multiplier);
        }
    }

    if (GET_OBJ_TYPE(obj) == ITEM_WEAPON) {
        GET_OBJ_VAL(obj, 1) = (int)(GET_OBJ_VAL(obj, 1) * multiplier); // Num dados
        GET_OBJ_VAL(obj, 2) = (int)(GET_OBJ_VAL(obj, 2) * multiplier); // Tamanho dados
    }

    if (GET_OBJ_TYPE(obj) == ITEM_ARMOR) {
        GET_OBJ_VAL(obj, 0) = (int)(GET_OBJ_VAL(obj, 0) * multiplier); // AC
    }
    
    GET_OBJ_COST(obj) = (int)(GET_OBJ_COST(obj) * multiplier);
}


// Função principal que sorteia e aplica uma qualidade aleatória.
void enhance_object(struct obj_data *obj) {
    int random_roll = rand_number(1, 1000); // Rola um número de 1 a 1000
    int cumulative_chance = 0;

    for (int i = 0; i < NUM_QUALITIES; i++) {
        cumulative_chance += quality_data[i].chance;
        if (random_roll <= cumulative_chance) {
            set_object_quality(obj, (enum item_quality)i);
            return;
        }
    }
}
