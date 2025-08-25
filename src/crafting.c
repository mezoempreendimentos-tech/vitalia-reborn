/**************************************************************************
 * File: crafting.c                                Part of tbaMUD         *
 * Usage: Core logic for the Gathering and Crafting system.               *
 **************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "constants.h"
#include "act.h"
#include "crafting.h" /* Nosso novo header */

/* --- Variáveis Globais --- */

struct recipe_data *recipe_list = NULL;

const char *quality_names[NUM_QUALITIES] = {
    "§w[Comum]§n",
    "§g[Incomum]§n",
    "§b[Raro]§n",
    "§m[Épico]§n",
    "§Y[Lendário]§n",
    "§C[Obra Prima]§n",
    "§R[DIVINA]§n"
};


/* --- Funções de Carregamento de Receitas --- */

int list_length(struct recipe_data *list) {
    int count = 0;
    struct recipe_data *r;
    for (r = list; r; r = r->next)
        count++;
    return count;
}

void load_recipes(void) {
    FILE *fp;
    char line[256], tag[128];
    struct recipe_data *current_recipe = NULL;
    int component_index = 0;

    if (!(fp = fopen(RECIPE_FILE, "r"))) {
        log1("SYSERR: Não foi possível abrir o arquivo de receitas: %s", RECIPE_FILE); // CORRIGIDO: log1
        return;
    }

    log1("...Carregando receitas de %s", RECIPE_FILE); // CORRIGIDO: log1

    while (get_line(fp, line)) {
        tag_argument(line, tag);

        if (str_cmp(tag, "Recipe") == 0) {
            CREATE(current_recipe, struct recipe_data, 1);
            component_index = 0;
            
            current_recipe->vnum = atoi(line);
            current_recipe->station_vnum = -1;
            current_recipe->enigma_name = NULL;
            current_recipe->is_master_recipe = FALSE;
            
            current_recipe->next = recipe_list;
            recipe_list = current_recipe;

        } else if (current_recipe) {
            if (str_cmp(tag, "Name") == 0) current_recipe->name = strdup(line);
            else if (str_cmp(tag, "Enigma") == 0) current_recipe->enigma_name = strdup(line);
            else if (str_cmp(tag, "ResultVnum") == 0) current_recipe->result_vnum = atoi(line);
            else if (str_cmp(tag, "ResultQty") == 0) current_recipe->result_quantity = atoi(line);
            else if (str_cmp(tag, "Skill") == 0) current_recipe->skill_id = atoi(line);
            else if (str_cmp(tag, "Difficulty") == 0) current_recipe->difficulty = atoi(line);
            else if (str_cmp(tag, "Station") == 0) current_recipe->station_vnum = atoi(line);
            else if (str_cmp(tag, "Master") == 0) current_recipe->is_master_recipe = (atoi(line) == 1);
            else if (str_cmp(tag, "Component") == 0) {
                if (component_index < MAX_RECIPE_COMPONENTS) {
                    // CORRIGIDO: Usando %hd para o vnum (short)
                    sscanf(line, "%hd %d", 
                        &current_recipe->components[component_index].vnum, 
                        &current_recipe->components[component_index].quality_points);
                    component_index++;
                }
            }
        }
    }
    fclose(fp);
    log1("...%d receitas carregadas.", list_length(recipe_list)); // CORRIGIDO: log1
}

void free_recipes(void) {
    struct recipe_data *recipe, *next_recipe;
    for (recipe = recipe_list; recipe; recipe = next_recipe) {
        next_recipe = recipe->next;
        if (recipe->name) free(recipe->name);
        if (recipe->enigma_name) free(recipe->enigma_name);
        free(recipe);
    }
}


/* --- Funções de Lógica de Qualidade --- */

int calculate_quality(struct char_data *ch, int skill_id, int difficulty, int material_bonus) {
    if (rand_number(1, 100) == 1) {
        return QUALITY_OBRA_PRIMA;
    }

    int roll = dice(1, 20);
    int skill_bonus;
    int final_result;

    if (roll == 20) return QUALITY_LENDARIO;
    if (roll == 1) return QUALITY_COMUM;

    skill_bonus = (GET_SKILL(ch, skill_id) - difficulty) / 5;
    final_result = roll + skill_bonus + material_bonus;

    if (final_result <= 5) return QUALITY_COMUM;
    if (final_result <= 10) return QUALITY_INCOMUM;
    if (final_result <= 16) return QUALITY_RARO;
    if (final_result <= 24) return QUALITY_EPICO;
    
    return QUALITY_EPICO;
}

void apply_quality_to_obj(struct obj_data *obj, int quality) {
    char buf[MAX_STRING_LENGTH];

    if (quality < QUALITY_COMUM || quality >= NUM_QUALITIES)
        return;

    GET_OBJ_QUALITY(obj) = quality;

    snprintf(buf, sizeof(buf), "%s %s", obj->short_description, quality_names[quality]);
    if (obj->short_description) free(obj->short_description);
    obj->short_description = strdup(buf);

    snprintf(buf, sizeof(buf), "%s jaz aqui, com um brilho %s.", obj->description, quality_names[quality]);
    if (obj->description) free(obj->description);
    obj->description = strdup(buf);
}

int get_quality_value(struct obj_data *obj) {
    switch (GET_OBJ_QUALITY(obj)) {
        case QUALITY_INCOMUM: return 2;
        case QUALITY_RARO:    return 3;
        case QUALITY_EPICO:   return 5;
        case QUALITY_LENDARIO:return 8;
        case QUALITY_OBRA_PRIMA: return 12;
        case QUALITY_DIVINA:     return 20;
        case QUALITY_COMUM:
        default:
            return 1;
    }
}


/* --- Comandos do Jogador (Implementação Inicial) --- */

ACMD(do_gather) {
    send_to_char(ch, "O sistema de coleta ainda está sendo desenvolvido.\r\n");
}

ACMD(do_craft) {
    send_to_char(ch, "O sistema de produção de itens ainda está sendo desenvolvido.\r\n");
}

ACMD(do_legacy) {
    send_to_char(ch, "O sistema de Legado ainda está sendo desenvolvido.\r\n");
}
