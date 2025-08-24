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

// A cabeça da nossa lista ligada de receitas. Começa como NULL (vazia).
struct recipe_data *recipe_list = NULL;

// O array com os nomes e cores das qualidades.
// Usamos o caractere '§' como prefixo para os códigos de cor.
// O MUD substituirá §Y por amarelo, §g por verde, etc. §n reseta a cor.
const char *quality_names[NUM_QUALITIES] = {
    "§w[Comum]§n",           // Branco
    "§g[Incomum]§n",         // Verde
    "§b[Raro]§n",            // Azul
    "§m[Épico]§n",           // Magenta/Roxo
    "§Y[Lendário]§n",        // Amarelo
    "§C[Obra Prima]§n",      // Ciano Brilhante
    "§R[DIVINA]§n"           // Vermelho Brilhante
};


/* --- Funções de Carregamento de Receitas --- */

// Função auxiliar para contar quantas receitas temos.
int list_length(struct recipe_data *list) {
    int count = 0;
    struct recipe_data *r;
    for (r = list; r; r = r->next)
        count++;
    return count;
}

// Carrega todas as receitas do arquivo RECIPE_FILE para a memória.
void load_recipes(void) {
    FILE *fp;
    char line[256], tag[128];
    struct recipe_data *current_recipe = NULL;
    int component_index = 0;

    if (!(fp = fopen(RECIPE_FILE, "r"))) {
        log("SYSERR: Não foi possível abrir o arquivo de receitas: %s", RECIPE_FILE);
        return;
    }

    log("...Carregando receitas de %s", RECIPE_FILE);

    while (get_line(fp, line)) {
        // Separa a 'tag' (ex: "Name") do resto da linha.
        tag_argument(line, tag);

        if (str_cmp(tag, "Recipe") == 0) {
            // Nova receita encontrada, alocamos memória para ela.
            CREATE(current_recipe, struct recipe_data, 1);
            component_index = 0;
            
            // Inicializa valores padrão
            current_recipe->vnum = atoi(line);
            current_recipe->station_vnum = -1; // -1 significa que não precisa de estação
            current_recipe->enigma_name = NULL;
            current_recipe->is_master_recipe = FALSE;
            
            // Adiciona a nova receita no início da lista global.
            current_recipe->next = recipe_list;
            recipe_list = current_recipe;

        } else if (current_recipe) {
            // Se já estamos lendo uma receita, preenchemos seus campos.
            if (str_cmp(tag, "Name") == 0) current_recipe->name = strdup(line);
            else if (str_cmp(tag, "Enigma") == 0) current_recipe->enigma_name = strdup(line);
            else if (str_cmp(tag, "ResultVnum") == 0) current_recipe->result_vnum = atoi(line);
            else if (str_cmp(tag, "ResultQty") == 0) current_recipe->result_quantity = atoi(line);
            else if (str_cmp(tag, "Skill") == 0) current_recipe->skill_id = atoi(line);
            else if
