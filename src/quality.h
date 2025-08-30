#ifndef _QUALITY_H_
#define _QUALITY_H_

// Enumeração para os tipos de qualidade.
enum item_quality {
    QUALITY_COMUM,
    QUALITY_INCOMUM,
    QUALITY_RARO,
    QUALITY_EPICO,
    QUALITY_LENDARIO,
    QUALITY_OBRA_PRIMA,
    QUALITY_DIVINO,
    NUM_QUALITIES // Isso nos ajuda a contar quantos tipos de qualidade existem.
};

// Estrutura para armazenar as propriedades de cada qualidade.
struct quality_info {
    const char *name;       // Nome que aparecerá no item, ex: "[LENDÁRIO]"
    float multiplier;     // Multiplicador de status
    int chance;           // Chance de ser gerado (em 1000)
};

// Declaração da nossa tabela de qualidades (será definida em quality.c)
extern const struct quality_info quality_data[NUM_QUALITIES];

// Protótipos das funções que vamos criar em quality.c
void enhance_object(struct obj_data *obj);
void set_object_quality(struct obj_data *obj, enum item_quality quality);

#endif // _QUALITY_H_
