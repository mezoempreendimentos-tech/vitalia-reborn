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

/* ... (O resto do arquivo, incluindo Variáveis Globais, Funções de Carregamento, etc. permanece o mesmo) ... */
/* ... A única alteração necessária é na função load_recipes e calculate_quality ... */

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
            // CORRIGIDO: A tag no arquivo de receitas agora deve ser "CrSkill"
            else if (str_cmp(tag, "CrSkill") == 0) current_recipe->crskill_id = atoi(line);
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

/* ... (free_recipes permanece o mesmo) ... */

int calculate_quality(struct char_data *ch, int crskill_id, int difficulty, int material_bonus) { // CORRIGIDO
    if (rand_number(1, 100) == 1) {
        return QUALITY_OBRA_PRIMA;
    }

    int roll = dice(1, 20);
    int skill_bonus;
    int final_result;

    if (roll == 20) return QUALITY_LENDARIO;
    if (roll == 1) return QUALITY_COMUM;

    // CORRIGIDO: Usará GET_CRAFT_SKILL (que vamos definir a seguir)
    skill_bonus = (GET_CRAFT_SKILL(ch, crskill_id) - difficulty) / 5;
    final_result = roll + skill_bonus + material_bonus;

    if (final_result <= 5) return QUALITY_COMUM;
    if (final_result <= 10) return QUALITY_INCOMUM;
    if (final_result <= 16) return QUALITY_RARO;
    if (final_result <= 24) return QUALITY_EPICO;
    
    return QUALITY_EPICO;
}

/* ... (apply_quality_to_obj e get_quality_value permanecem os mesmos) ... */

ACMD(do_craft)
{
    // ... (a lógica interna de do_craft que já fizemos) ...
    // A única linha que precisa mudar dentro de do_craft é a verificação de perícia:
    
    // LINHA A SER ALTERADA DENTRO DE do_craft:
    // if (GET_SKILL(ch, recipe->skill_id) <= 0) {
    // DEVE VIRAR:
    // if (GET_CRAFT_SKILL(ch, recipe->crskill_id) <= 0) {
    
    // O resto do código que te passei para do_craft já está correto
    // Se quiser, posso fornecer a função do_craft inteira e atualizada.
}

/* ... (o resto do arquivo) ... */
