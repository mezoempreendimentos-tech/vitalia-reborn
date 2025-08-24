#ifndef _CRAFTING_H_
#define _CRAFTING_H_

/* --- Constantes e Definições --- */

// Definição do arquivo que conterá nossas receitas.
// O caminho é relativo à pasta raiz do MUD (onde o executável 'bin/circle' fica).
#define RECIPE_FILE     "../lib/world/recipes.rcp" 

// Número máximo de componentes que uma receita pode ter.
#define MAX_RECIPE_COMPONENTS 5
// Número máximo de receitas de legado que um jogador pode aprender.
#define MAX_LEGACY_RECIPES    3


/* --- Enumerações --- */

// Níveis de Qualidade para itens coletados e produzidos.
// ADICIONAMOS OBRA_PRIMA e DIVINA.
enum item_quality {
    QUALITY_COMUM,
    QUALITY_INCOMUM,
    QUALITY_RARO,
    QUALITY_EPICO,
    QUALITY_LENDARIO,
    QUALITY_OBRA_PRIMA, /* Chance rara de acontecer, cria um item superior. */
    QUALITY_DIVINA,     /* Apenas Imortais podem criar/designar esta qualidade. */
    NUM_QUALITIES       // Sentinela para contar o número de qualidades
};


/* --- Estruturas de Dados --- */

// Representa um único componente necessário para uma receita.
struct recipe_component_data {
    obj_vnum vnum;           // Vnum do objeto componente.
    int      quality_points; // Pontos de qualidade necessários deste componente.
};

// Representa uma receita de crafting completa.
struct recipe_data {
    int vnum;                         // Vnum único para identificar a receita.
    char *name;                       // Nome da receita (ex: "Espada Curta de Ferro").
    char *enigma_name;                // Nome do "enigma" para receitas de legado.
    obj_vnum result_vnum;             // Vnum do objeto que será criado.
    int result_quantity;              // Quantidade do item resultante.
    int skill_id;                     // ID da perícia necessária (ex: SKILL_WEAPONSMITH).
    int difficulty;                   // Dificuldade da receita (afeta a qualidade).
    obj_vnum station_vnum;            // Vnum da estação de trabalho necessária (ex: forja), -1 se nenhuma.
    bool is_master_recipe;            // É uma receita de Legado?
    
    // Array de componentes necessários para esta receita.
    struct recipe_component_data components[MAX_RECIPE_COMPONENTS];

    struct recipe_data *next;         // Ponteiro para a próxima receita na lista ligada.
};


/* --- Variáveis Globais (declaradas em crafting.c) --- */

// A lista global de todas as receitas carregadas no MUD.
extern struct recipe_data *recipe_list; 
// Array de strings com os nomes (e cores) das qualidades (ex: "§g[Raro]§n").
extern const char *quality_names[NUM_QUALITIES];


/* --- Protótipos de Funções --- */

/* Funções de Carregamento e Liberação */
void load_recipes(void);
void free_recipes(void);

/* Funções de Lógica de Qualidade */
int calculate_quality(struct char_data *ch, int skill_id, int difficulty, int material_bonus);
void apply_quality_to_obj(struct obj_data *obj, int quality);
int get_quality_value(struct obj_data *obj);

/* Comandos do Jogador (ACMDs) */
ACMD(do_gather);
ACMD(do_craft);
ACMD(do_legacy);

#endif // _CRAFTING_H_
