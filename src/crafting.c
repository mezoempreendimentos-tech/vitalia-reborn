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
#include "crafting.h"

/* --- Variáveis Globais --- */

struct recipe_data *recipe_list = NULL;
struct craft_skill_data *craft_skill_list = NULL;

const char *quality_names[NUM_QUALITIES] = {
    "§w[Comum]§n",
    "§g[Incomum]§n",
    "§b[Raro]§n",
    "§m[Épico]§n",
    "§Y[Lendário]§n",
    "§C[Obra Prima]§n",
    "§R[DIVINA]§n"
};


/* --- Funções de Carregamento --- */

// Conta o número de receitas na lista
int list_length(struct recipe_data *list) {
    int count = 0;
    struct recipe_data *r;
    for (r = list; r; r = r->next)
        count++;
    return count;
}

// Carrega as receitas do arquivo recipes.rcp
void load_recipes(void) {
    FILE *fp;
    char line[256], tag[128];
    struct recipe_data *current_recipe = NULL;
    int component_index = 0;

    if (!(fp = fopen(RECIPE_FILE, "r"))) {
        log1("SYSERR: Não foi possível abrir o arquivo de receitas: %s", RECIPE_FILE);
        return;
    }

    log1("...Carregando receitas de %s", RECIPE_FILE);

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
            else if (str_cmp(tag, "CrSkill") == 0) current_recipe->crskill_id = atoi(line); // CORRIGIDO
            else if (str_cmp(tag, "Difficulty") == 0) current_recipe->difficulty = atoi(line);
            else if (str_cmp(tag, "Station") == 0) current_recipe->station_vnum = atoi(line);
            else if (str_cmp(tag, "Master") == 0) current_recipe->is_master_recipe = (atoi(line) == 1);
            else if (str_cmp(tag, "Component") == 0) {
                if (component_index < MAX_RECIPE_COMPONENTS) {
                    sscanf(line, "%hd %d", 
                        &current_recipe->components[component_index].vnum, 
                        &current_recipe->components[component_index].quality_points);
                    component_index++;
                }
            }
        }
    }
    fclose(fp);
    log1("...%d receitas carregadas.", list_length(recipe_list));
}

// Libera a memória das receitas
void free_recipes(void) {
    struct recipe_data *recipe, *next_recipe;
    for (recipe = recipe_list; recipe; recipe = next_recipe) {
        next_recipe = recipe->next;
        if (recipe->name) free(recipe->name);
        if (recipe->enigma_name) free(recipe->enigma_name);
        free(recipe);
    }
}

// Carrega as perícias de crafting do arquivo crskills.dat
void load_crafting_skills(void) {
    FILE *fp;
    char line[256];
    
    if (!(fp = fopen(CS_FILE, "r"))) {
        log1("SYSERR: Não foi possível abrir o arquivo de perícias de crafting: %s", CS_FILE);
        return;
    }

    log1("...Carregando perícias de crafting de %s", CS_FILE);

    while(get_line(fp, line)) {
        if (*line == '$') break;

        struct craft_skill_data *skill;
        CREATE(skill, struct craft_skill_data, 1);

        skill->id = atoi(strtok(line, "~"));
        skill->name = strdup(strtok(NULL, "~"));
        
        skill->next = craft_skill
